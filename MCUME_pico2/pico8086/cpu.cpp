#include <stdio.h>
#include <stdint.h>
#include "intf.h"
#include "rom.h"

extern void portout(uint16_t portnum, uint16_t value);
extern uint16_t portin(uint16_t portnum);

extern void readdisk(uint8_t drivenum, uint16_t dstseg, uint16_t dstoff, uint16_t cyl, uint16_t sect, uint16_t head, uint16_t sectcount);
extern void insertdisk();

extern void doirq(uint8_t irqnum);
extern uint8_t nextintr();
extern struct structpic {
  uint8_t imr; //mask register
  uint8_t irr; //request register
  uint8_t isr; //service register
  uint8_t icwstep; //used during initialization to keep track of which ICW we're at
  uint8_t icw[5];
  uint8_t intoffset; //interrupt vector offset
  uint8_t priority; //which IRQ has highest priority
  uint8_t autoeoi; //automatic EOI mode
  uint8_t readmode; //remember what to return on read register from OCW3
  uint8_t enabled;
} i8259;

extern uint8_t curkey;
void intcall86(uint8_t intnum);

uint64_t curtimer, lasttimer, timerfreq;

char *biosfile = NULL;
uint8_t byteregtable[8] = { regal, regcl, regdl, regbl, regah, regch, regdh, regbh };

uint8_t parity[0x100];

_bytewordregs_ regs;

uint16_t segregs[6];
uint8_t opcode, segoverride, reptype, bootdrive, hdcount = 0;
uint16_t savecs, saveip, ip, useseg, oldsp;
uint8_t tempcf, oldcf, cf, pf, af, zf, sf, tf, ifl, df, of, nt, iopriv, mode, reg, rm, msw = 0;
uint16_t oper1, oper2, res16, disp16, temp16, dummy, stacksize, frametemp;
uint8_t oper1b, oper2b, res8, disp8, temp8, nestlev, addrbyte;
uint16_t cr0 = 0, cr1 = 0, cr2 = 0, cr3 = 0;
uint32_t ldtr = 0, gdtr = 0, gdtlimit = 0, idtr = 0, idtlimit = 0;
uint32_t temp1, temp2, temp3, temp4, temp5, temp32, tempaddr32, ea;
int32_t result, speed = 0;
uint32_t totalexec;
uint32_t ips[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t *tempwordptr;

uint8_t vidmode, cgabg, blankattr, vidgfxmode, vidcolor;
uint16_t cursx, cursy, cols, rows, vgapage, cursorposition, cursorvisible;
uint8_t updatedscreen, port3da, port6, portout16;
//uint16_t VGA_SC[0xFF], VGA_CRTC[0xFF], VGA_ATTR[0xFF], VGA_GC[0xFF];
uint32_t videobase, textbase, x, y;

uint8_t debugmode, showcsip, verbose, mouseemu;

extern "C" unsigned char read_ram(int address);
extern "C" void  write_ram(int address, unsigned char val);

extern uint8_t * LORAM;

void write86(uint32_t addr32, uint8_t value) {
  if (addr32 < RAM_SIZE) {
    write_ram(addr32, value);
    return;
  } 
  else if ((addr32 >= 0xB8000) && (addr32 < 0xC0000)) {
    VRAM_write(addr32 - 0xB8000UL, value);
  }
}

#define writew86(addr32,value) {write86((addr32),(uint8_t)(value));write86((addr32)+1,(uint8_t)((uint16_t)(value)>>8));}

uint8_t read86(uint32_t addr32) {
  if (addr32 < NATIVE_RAM) {
    switch (addr32) { //some hardcoded values for the BIOS data area
      case 0x410: //0040:0010 is the equipment word
#ifdef VGA
        return (0x41); //video type (0x41 is VGA/EGA, 0x61 is CGA, 0x31 = MDA)
#else
        return (0x61); //video type (0x41 is VGA/EGA, 0x61 is CGA, 0x31 = MDA)
#endif
      case 0x475: //hard drive count
        return (hdcount);
      default:
        return LORAM[addr32];
    }    
  } 
  else if (addr32 < RAM_SIZE) {
    return read_ram(addr32);
  } 
  else if ((addr32 >= 0xB8000) && (addr32 < 0xC0000)) {
    addr32 -= 0xB8000UL;
    return VRAM_read(addr32);
  }
  else if (addr32 >= 0xFE000UL) {
    addr32 -= 0xFE000UL;
    return ROM_READ(BIOS, addr32); //BIOS[addr32];
  }
  else if ((addr32 >= 0xD0000) && (addr32 < 0xD0640)) {
    return net_read_ram(addr32 - 0xD0000);
  }
  else if ((addr32 >= 0xE0000) && (addr32 < 0xE0006)) {
    return net_mac[addr32 - 0xE0000];
  }
#ifdef INCLUDE_ROM_BASIC
  else if ((addr32 >= 0xF6000UL) && (addr32 < 0xFA000UL)) {
    addr32 -= 0xF6000UL;
    return ROM_READ(BASICL, addr32); //BASICL[addr32];
  } else if ((addr32 >= 0xFA000UL) && (addr32 < 0xFE000UL)) {
    addr32 -= 0xFA000UL;
    return ROM_READ(BASICH, addr32); //BASICH[addr32];
  }
#endif
  else return 0;
}



#define readw86(addr32) ((uint16_t)read86((addr32))|((uint16_t)read86((addr32)+1)<<8))

//inline void flag_szp8(uint8_t value) {
#define flag_szp8(value) {\
  if (!(value)) zf = 1; else zf = 0;\
  if ((value) & 0x80) sf = 1; else sf = 0;\
  pf = parity[value];\
}

//inline void flag_szp16(uint16_t value) {
#define flag_szp16(value) {\
  if (!(value)) zf = 1; else zf = 0;\
  if (value & 0x8000) sf = 1; else sf = 0;\
  pf = parity[(uint8_t)value];\
}

//inline void flag_log8(uint8_t value) {
#define flag_log8(value) {\
  flag_szp8(value);\
  cf = 0; of = 0;\
}

//inline void flag_log16(uint16_t value) {
#define flag_log16(value) {\
  flag_szp16(value);\
  cf = 0; of = 0;\
}

//inline void flag_adc8(uint8_t v1, uint8_t v2, uint8_t v3) { //v1 = destination operand, v2 = source operand, v3 = carry flag
#define flag_adc8(v1, v2, v3) {\
  uint16_t dst;\
  dst = (uint16_t)(v1) + (uint16_t)(v2) + (uint16_t)(v3);\
  flag_szp8((uint8_t)dst);\
  if (((dst ^ (v1)) & (dst ^ (v2)) & 0x80) == 0x80) of = 1; else of = 0;\
  if (dst & 0xFF00) cf = 1; else cf = 0;\
  if ((((v1) ^ (v2) ^ dst) & 0x10) == 0x10) af = 1; else af = 0;\
}

//inline void flag_adc16(uint16_t v1, uint16_t v2, uint16_t v3) { //v1 = destination operand, v2 = source operand, v3 = carry flag
#define flag_adc16(v1, v2, v3) {\
  uint32_t dst;\
  dst = (uint32_t)(v1) + (uint32_t)(v2) + (uint32_t)(v3);\
  flag_szp16((uint16_t)dst);\
  if ((((dst ^ (v1)) & (dst ^ (v2))) & 0x8000) == 0x8000) of = 1; else of = 0;\
  if (dst & 0xFFFF0000UL) cf = 1; else cf = 0;\
  if ((((v1) ^ (v2) ^ dst) & 0x10) == 0x10) af = 1; else af = 0;\
}

//inline void flag_add8(uint8_t v1, uint8_t v2) { //v1 = destination operand, v2 = source operand
#define flag_add8(v1, v2) {\
  uint16_t dst;\
  dst = (uint16_t)(v1) + (uint16_t)(v2);\
  flag_szp8((uint8_t)dst);\
  if (dst & 0xFF00) cf = 1; else cf = 0;\
  if (((dst ^ (v1)) & (dst ^ (v2)) & 0x80) == 0x80) of = 1; else of = 0;\
  if ((((v1) ^ (v2) ^ dst) & 0x10) == 0x10) af = 1; else af = 0;\
}

//inline void flag_add16(uint16_t v1, uint16_t v2) { //v1 = destination operand, v2 = source operand
#define flag_add16(v1, v2) {\
  uint32_t dst;\
  dst = (uint32_t)(v1) + (uint32_t)(v2);\
  flag_szp16((uint16_t)dst);\
  if (dst & 0xFFFF0000UL) cf = 1; else cf = 0;\
  if (((dst ^ (v1)) & (dst ^ (v2)) & 0x8000) == 0x8000) of = 1; else of = 0;\
  if ((((v1) ^ (v2) ^ dst) & 0x10) == 0x10) af = 1; else af = 0;\
}

//inline void flag_sbb8(uint8_t v1, uint8_t v2, uint8_t v3) { //v1 = destination operand, v2 = source operand, v3 = carry flag
#define flag_sbb8(v1, v2, v3) {\
  uint16_t dst;\
  uint16_t newv2;\
  newv2 = (uint16_t)(v2) + (uint16_t)(v3);\
  dst = (uint16_t)(v1) - (uint16_t)newv2;\
  flag_szp8((uint8_t)dst);\
  if (dst & 0xFF00) cf = 1; else cf = 0;\
  if ((dst ^ (v1)) & ((v1) ^ newv2) & 0x80) of = 1; else of = 0;\
  if (((v1) ^ newv2 ^ dst) & 0x10) af = 1; else af = 0;\
}

//inline void flag_sbb16(uint16_t v1, uint16_t v2, uint16_t v3) { //v1 = destination operand, v2 = source operand, v3 = carry flag
#define flag_sbb16(v1, v2, v3){\
  uint32_t dst;\
  uint32_t newv2;\
  newv2 = (uint32_t)(v2) + (uint32_t)(v3);\
  dst = (uint32_t)v1 - newv2;\
  flag_szp16((uint16_t)dst);\
  if (dst & 0xFFFF0000UL) cf = 1; else cf = 0;\
  if ((dst ^ (v1)) & (v1 ^ newv2) & 0x8000) of = 1; else of = 0;\
  if (((v1) ^ newv2 ^ dst) & 0x10) af = 1; else af = 0;\
}

//inline void flag_sub8(uint8_t v1, uint8_t v2) { //v1 = destination operand, v2 = source operand
#define flag_sub8(v1, v2) {\
  uint16_t dst;\
  dst = (uint16_t)(v1) - (uint16_t)(v2);\
  flag_szp8((uint8_t)dst);\
  if (dst & 0xFF00) cf = 1; else cf = 0;\
  if ((dst ^ (v1)) & ((v1) ^ (v2)) & 0x80) of = 1; else of = 0;\
  if (((v1) ^ (v2) ^ dst) & 0x10) af = 1; else af = 0;\
}

//inline void flag_sub16(uint16_t v1, uint16_t v2) { //v1 = destination operand, v2 = source operand
#define flag_sub16(v1, v2) {\
  uint32_t dst;\
  dst = (uint32_t)(v1) - (uint32_t)(v2);\
  flag_szp16((uint16_t)dst);\
  if (dst & 0xFFFF0000UL) cf = 1; else cf = 0;\
  if ((dst ^ (v1)) & ((v1) ^ (v2)) & 0x8000) of = 1; else of = 0;\
  if (((v1) ^ (v2) ^ dst) & 0x10) af = 1; else af = 0;\
}

//inline void op_adc8() {
#define op_adc8() {\
  res8 = oper1b + oper2b + cf;\
  flag_adc8(oper1b, oper2b, cf);\
}

//inline void op_adc16() {
#define op_adc16() {\
  res16 = oper1 + oper2 + cf;\
  flag_adc16(oper1, oper2, cf);\
}

//inline void op_add8() {
#define op_add8() {\
  res8 = oper1b + oper2b;\
  flag_add8(oper1b, oper2b);\
}

//inline void op_add16() {
#define op_add16() {\
  res16 = oper1 + oper2;\
  flag_add16(oper1, oper2);\
}

//inline void op_and8() {
#define op_and8() {\
  res8 = oper1b & oper2b;\
  flag_log8(res8);\
}

//inline void op_and16() {
#define op_and16() {\
  res16 = oper1 & oper2;\
  flag_log16(res16);\
}

//inline void op_or8() {
#define op_or8() {\
  res8 = oper1b | oper2b;\
  flag_log8(res8);\
}

//inline void op_or16() {
#define op_or16() {\
  res16 = oper1 | oper2;\
  flag_log16(res16);\
}

//inline void op_xor8() {
#define op_xor8() {\
  res8 = oper1b ^ oper2b;\
  flag_log8(res8);\
}

//inline void op_xor16() {
#define op_xor16() {\
  res16 = oper1 ^ oper2;\
  flag_log16(res16);\
}

//inline void op_sub8() {
#define op_sub8() {\
  res8 = oper1b - oper2b;\
  flag_sub8(oper1b, oper2b);\
}

//inline void op_sub16() {
#define op_sub16() {\
  res16 = oper1 - oper2;\
  flag_sub16(oper1, oper2);\
}

//inline void op_sbb8() {
#define op_sbb8() {\
  res8 = oper1b - (oper2b + cf);\
  flag_sbb8(oper1b, oper2b, cf);\
}

//inline void op_sbb16() {
#define op_sbb16() {\
  res16 = oper1 - (oper2 + cf);\
  flag_sbb16(oper1, oper2, cf);\
}

//inline void modregrm() {
#define modregrm() {\
  addrbyte = getmem8(segregs[regcs], ip); StepIP(1);\
  mode = addrbyte >> 6;\
  reg = (addrbyte >> 3) & 7;\
  rm = addrbyte & 7;\
  switch (mode) {\
    case 0:\
      if (rm == 6) {\
        disp16 = getmem16(segregs[regcs], ip);\
        StepIP(2);\
      }\
      if (((rm == 2) || (rm == 3)) && !segoverride) useseg = segregs[regss]; break;\
    case 1:\
      disp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);\
      if (((rm == 2) || (rm == 3) || (rm == 6)) && !segoverride) useseg = segregs[regss]; break;\
    case 2:\
      disp16 = getmem16(segregs[regcs], ip); StepIP(2);\
      if (((rm == 2) || (rm == 3) || (rm == 6)) && !segoverride) useseg = segregs[regss]; break;\
    default:\
      disp8 = 0; disp16 = 0;\
  }\
  if (mode < 3) getea(rm);\
}

//inline void getea(uint8_t rmval) {
#define getea(rmval) {\
  uint32_t tempea;\
  tempea = 0;\
  switch (mode) {\
    case 0:\
      switch (rmval) {\
        case 0: tempea = regs.wordregs[regbx] + regs.wordregs[regsi]; break;\
        case 1: tempea = regs.wordregs[regbx] + regs.wordregs[regdi]; break;\
        case 2: tempea = regs.wordregs[regbp] + regs.wordregs[regsi]; break;\
        case 3: tempea = regs.wordregs[regbp] + regs.wordregs[regdi]; break;\
        case 4: tempea = regs.wordregs[regsi]; break;\
        case 5: tempea = regs.wordregs[regdi]; break;\
        case 6: tempea = disp16; break;\
        case 7: tempea = regs.wordregs[regbx]; break;\
      } break;\
    case 1: case 2:\
      switch (rmval) {\
        case 0: tempea = regs.wordregs[regbx] + regs.wordregs[regsi] + disp16; break;\
        case 1: tempea = regs.wordregs[regbx] + regs.wordregs[regdi] + disp16; break;\
        case 2: tempea = regs.wordregs[regbp] + regs.wordregs[regsi] + disp16; break;\
        case 3: tempea = regs.wordregs[regbp] + regs.wordregs[regdi] + disp16; break;\
        case 4: tempea = regs.wordregs[regsi] + disp16; break;\
        case 5: tempea = regs.wordregs[regdi] + disp16; break;\
        case 6: tempea = regs.wordregs[regbp] + disp16; break;\
        case 7: tempea = regs.wordregs[regbx] + disp16; break;\
      } break;\
  }\
  ea = useseg;\
  ea <<= 4;\
  ea += (tempea & 0xFFFF);\
}

inline void push(uint16_t pushval) {
  putreg16(regsp, getreg16(regsp) - 2);
  //printf("  %04X:%04X PUSH\n", segregs[regss], getreg16(regsp));
  putmem16(segregs[regss], getreg16(regsp), pushval);
}

inline uint16_t pop() {
  uint16_t tempval;
  //printf("  %04X:%04X POP\n", segregs[regss], getreg16(regsp));
  tempval = getmem16(segregs[regss], getreg16(regsp));
  putreg16(regsp, getreg16(regsp) + 2);
  return (tempval);
}

void reset86() {
  uint16_t i, cnt, bitcount;
  segregs[regcs] = 0xFFFF;
  ip = 0x0000;
  segregs[regss] = 0x0000;
  regs.wordregs[regsp] = 0xFFFE;

  //generate parity lookup table
  for (i = 0; i < 256; i++) {
    bitcount = 0;
    for (cnt = 0; cnt < 8; cnt++)
      bitcount += ((i >> cnt) & 1);
    if (bitcount & 1) parity[i] = 0; else parity[i] = 1;
  }
}

/*inline uint16_t readrm16(uint8_t rmval) {
  if (mode < 3) {
    getea(rmval);
    return (read86(ea) | ((uint16_t)read86(ea + 1) << 8));
  } else {
    return (getreg16(rmval));
  }
}*/

#define readrm16(rmval) ( (mode < 3) ? (read86(ea) | ((uint16_t)read86(ea + 1) << 8)) : (getreg16(rmval)) )

/*inline uint8_t readrm8(uint8_t rmval) {
  if (mode < 3) {
    //getea(rmval);
    return (read86(ea));
  } else {
    return (getreg8(rmval));
  }
}*/

#define readrm8(rmval) ( (mode < 3) ? read86(ea) : getreg8(rmval) )

inline void writerm16(uint8_t rmval, uint16_t value) {
//#define writerm16(rmval, value) {
  if (mode < 3) {\
    /*getea(rmval);*/\
    write86(ea, value & 0xFF);\
    write86(ea + 1, value >> 8);\
  } else {\
    putreg16(rmval, value);\
  }\
}

inline void writerm8(uint8_t rmval, uint8_t value) {
//#define writerm8(rmval, value) {
  if (mode < 3) {\
    /*getea(rmval);*/\
    write86(ea, value);\
  } else {\
    putreg8(rmval, value);\
  }\
}

inline uint8_t op_grp2_8(uint8_t cnt) {
  uint16_t s, oldcf, msb;
  uint8_t shift;
  s = oper1b;
  oldcf = cf;
  switch (reg) {
    case 0: //ROL r/m8
      for (shift = 1; shift <= cnt; shift++) {
        if (s & 0x80) cf = 1; else cf = 0;
        s = s << 1;
        s = s | cf;
      }
      if (cnt == 1) of = cf ^ ((s >> 7) & 1);
      break;

    case 1: //ROR r/m8
      for (shift = 1; shift <= cnt; shift++) {
        cf = s & 1;
        s = (s >> 1) | (cf << 7);
      }
      if (cnt == 1) of = (s >> 7) ^ ((s >> 6) & 1);
      break;

    case 2: //RCL r/m8
      for (shift = 1; shift <= cnt; shift++) {
        oldcf = cf;
        if (s & 0x80) cf = 1; else cf = 0;
        s = s << 1;
        s = s | oldcf;
      }
      if (cnt == 1) of = cf ^ ((s >> 7) & 1);
      break;

    case 3: //RCR r/m8
      for (shift = 1; shift <= cnt; shift++) {
        oldcf = cf;
        cf = s & 1;
        s = (s >> 1) | (oldcf << 7);
      }
      if (cnt == 1) of = (s >> 7) ^ ((s >> 6) & 1);
      break;

    case 4: case 6: //SHL r/m8
      for (shift = 1; shift <= cnt; shift++) {
        if (s & 0x80) cf = 1; else cf = 0;
        s = (s << 1) & 0xFF;
      }
      if ((cnt == 1) && (cf == (s >> 7))) of = 0; else of = 1;
      flag_szp8((uint8_t)s); break;

    case 5: //SHR r/m8
      if ((cnt == 1) && (s & 0x80)) of = 1; else of = 0;
      for (shift = 1; shift <= cnt; shift++) {
        cf = s & 1;
        s = s >> 1;
      }
      flag_szp8((uint8_t)s); break;

    case 7: //SAR r/m8
      for (shift = 1; shift <= cnt; shift++) {
        msb = s & 0x80;
        cf = s & 1;
        s = (s >> 1) | msb;
      }
      of = 0;
      flag_szp8((uint8_t)s); break;

  }
  return (s & 0xFF);
}

inline uint16_t op_grp2_16(uint8_t cnt) {
  uint32_t s, oldcf, msb;
  uint8_t shift;
  s = oper1;
  oldcf = cf;
  switch (reg) {
    case 0: //ROL r/m8
      for (shift = 1; shift <= cnt; shift++) {
        if (s & 0x8000) cf = 1; else cf = 0;
        s = s << 1;
        s = s | cf;
      }
      if (cnt == 1) of = cf ^ ((s >> 15) & 1);
      break;

    case 1: //ROR r/m8
      for (shift = 1; shift <= cnt; shift++) {
        cf = s & 1;
        s = (s >> 1) | (cf << 15);
      }
      if (cnt == 1) of = (s >> 15) ^ ((s >> 14) & 1);
      break;

    case 2: //RCL r/m8
      for (shift = 1; shift <= cnt; shift++) {
        oldcf = cf;
        if (s & 0x8000) cf = 1; else cf = 0;
        s = s << 1;
        s = s | oldcf;
      }
      if (cnt == 1) of = cf ^ ((s >> 15) & 1);
      break;

    case 3: //RCR r/m8
      for (shift = 1; shift <= cnt; shift++) {
        oldcf = cf;
        cf = s & 1;
        s = (s >> 1) | (oldcf << 15);
      }
      if (cnt == 1) of = (s >> 15) ^ ((s >> 14) & 1);
      break;

    case 4: case 6: //SHL r/m8
      for (shift = 1; shift <= cnt; shift++) {
        if (s & 0x8000) cf = 1; else cf = 0;
        s = (uint16_t)(s << 1);
      }
      if ((cnt == 1) && (cf == (s >> 15))) of = 0; else of = 1;
      flag_szp16((uint16_t)s); break;

    case 5: //SHR r/m8
      if ((cnt == 1) && (s & 0x8000)) of = 1; else of = 0;
      for (shift = 1; shift <= cnt; shift++) {
        cf = s & 1;
        s = s >> 1;
      }
      flag_szp16((uint16_t)s); break;

    case 7: //SAR r/m8
      for (shift = 1; shift <= cnt; shift++) {
        msb = s & 0x8000;
        cf = s & 1;
        s = (s >> 1) | msb;
      }
      of = 0;
      flag_szp16((uint16_t)s); break;

  }
  return ((uint16_t)s);
}

inline void op_div8(uint16_t valdiv, uint8_t divisor) {
  if (divisor == 0) {
    intcall86(0);
    return;
  }
  if ((valdiv / (uint16_t)divisor) > 0xFF) {
    intcall86(0);
    return;
  }
  regs.byteregs[regah] = valdiv % (uint16_t)divisor;
  regs.byteregs[regal] = valdiv / (uint16_t)divisor;
}

inline void op_idiv8(uint16_t valdiv, uint8_t divisor) {
  uint16_t s1, s2, d1, d2, sign;
  if (divisor == 0) {
    intcall86(0);
    return;
  }
  s1 = valdiv;
  s2 = divisor;
  sign = (((s1 ^ s2) & 0x8000) != 0);
  s1 = (s1 < 0x8000) ? s1 : (uint16_t)(~s1 + 1);
  s2 = (s2 < 0x8000) ? s2 : (uint16_t)(~s2 + 1);
  d1 = s1 / s2;
  d2 = s1 % s2;
  if (d1 & 0xFF00) {
    intcall86(0);
    return;
  }
  if (sign) {
    d1 = (~d1 + 1) & 0xff;
    d2 = (~d2 + 1) & 0xff;
  }
  regs.byteregs[regah] = d2;
  regs.byteregs[regal] = d1;
}

inline void op_grp3_8() {
  oper1 = signext(oper1b); oper2 = signext(oper2b);
  switch (reg) {
    case 0: case 1: //TEST
      flag_log8(oper1b & getmem8(segregs[regcs], ip)); StepIP(1);
      break;

    case 2: //NOT
      res8 = ~oper1b; break;

    case 3: //NEG
      res8 = (~oper1b) + 1;
      flag_sub8(0, oper1b);
      if (res8 == 0) cf = 0; else cf = 1;
      break;

    case 4: //MUL
      temp1 = (uint32_t)oper1b * (uint32_t)regs.byteregs[regal];
      putreg16(regax, (uint16_t)temp1);
      flag_szp8((uint8_t)temp1);
      if (regs.byteregs[regah]) {
        cf = 1;
        of = 1;
      } else {
        cf = 0;
        of = 0;
      }
      break;

    case 5: //IMUL
      oper1 = signext(oper1b);
      temp1 = signext(regs.byteregs[regal]);
      temp2 = oper1;
      if ((temp1 & 0x80) == 0x80) temp1 = temp1 | 0xFFFFFF00UL;
      if ((temp2 & 0x80) == 0x80) temp2 = temp2 | 0xFFFFFF00UL;
      temp3 = (uint16_t)(temp1 * temp2);
      putreg16(regax, (uint16_t)temp3);
      if (regs.byteregs[regah]) {
        cf = 1;
        of = 1;
      } else {
        cf = 0;
        of = 0;
      }
      break;

    case 6: //DIV
      op_div8(getreg16(regax), oper1b);
      break;

    case 7: //IDIV
      op_idiv8(getreg16(regax), oper1b);
      break;
  }
}

void op_div16(uint32_t valdiv, uint16_t divisor) {
  if (divisor == 0) {
    intcall86(0);
    return;
  }
  if ((valdiv / (uint32_t)divisor) > 0xFFFF) {
    intcall86(0);
    return;
  }
  putreg16(regdx, valdiv % (uint32_t)divisor);
  putreg16(regax, valdiv / (uint32_t)divisor);
}

void op_idiv16(uint32_t valdiv, uint16_t divisor) {
  uint32_t d1, d2, s1, s2, sign;
  if (divisor == 0) {
    intcall86(0);
    return;
  }
  s1 = valdiv;
  s2 = divisor;
  s2 = (s2 & 0x8000) ? (s2 | 0xffff0000UL) : s2;
  sign = (((s1 ^ s2) & 0x80000000UL) != 0);
  s1 = (s1 < 0x80000000UL) ? s1 : ((~s1 + 1) & 0xffffffffUL);
  s2 = (s2 < 0x80000000UL) ? s2 : ((~s2 + 1) & 0xffffffffUL);
  d1 = s1 / s2;
  d2 = s1 % s2;
  if (d1 & 0xFFFF0000UL) {
    intcall86(0);
    return;
  }
  if (sign) {
    d1 = (uint16_t)(~d1 + 1);
    d2 = (uint16_t)(~d2 + 1);
  }
  putreg16(regax, d1);
  putreg16(regdx, d2);
}

inline void op_grp3_16() {
  switch (reg) {
    case 0: case 1: //TEST
      flag_log16(oper1 & getmem16(segregs[regcs], ip)); StepIP(2); break;
    case 2: //NOT
      res16 = ~oper1; break;
    case 3: //NEG
      res16 = (~oper1) + 1;
      flag_sub16(0, oper1);
      if (res16) cf = 1; else cf = 0;
      break;
    case 4: //MUL
      temp1 = (uint32_t)oper1 * (uint32_t)getreg16(regax);
      putreg16(regax, (uint16_t)temp1);
      putreg16(regdx, temp1 >> 16);
      flag_szp16((uint16_t)temp1);
      if (getreg16(regdx)) {
        cf = 1;
        of = 1;
      } else {
        cf = 0;
        of = 0;
      }
      break;
    case 5: //IMUL
      temp1 = getreg16(regax);
      temp2 = oper1;
      if (temp1 & 0x8000) temp1 |= 0xFFFF0000UL;
      if (temp2 & 0x8000) temp2 |= 0xFFFF0000UL;
      temp3 = temp1 * temp2;
      putreg16(regax, (uint16_t)temp3); //into register ax
      putreg16(regdx, temp3 >> 16); //into register dx
      if (getreg16(regdx)) {
        cf = 1;
        of = 1;
      } else {
        cf = 0;
        of = 0;
      }
      break;
    case 6: //DIV
      op_div16(((uint32_t)getreg16(regdx) << 16) + (uint32_t)getreg16(regax), oper1); break;
    case 7: //DIV
      op_idiv16(((uint32_t)getreg16(regdx) << 16) + (uint32_t)getreg16(regax), oper1); break;
  }
}

//inline void op_grp5() {
#define op_grp5() {\
  switch (reg) {\
    case 0: /*INC Ev*/\
      oper2 = 1;\
      tempcf = cf;\
      op_add16();\
      cf = tempcf;\
      writerm16(rm, res16); break;\
    case 1: /*DEC Ev*/\
      oper2 = 1;\
      tempcf = cf;\
      op_sub16();\
      cf = tempcf;\
      writerm16(rm, res16); break;\
    case 2: /*CALL Ev*/\
      push(ip);\
      ip = oper1; break;\
    case 3: /*CALL Mp*/\
      push(segregs[regcs]); push(ip);\
      /*getea(rm);*/\
      ip = (uint16_t)read86(ea) + ((uint16_t)read86(ea + 1) << 8);\
      segregs[regcs] = (uint16_t)read86(ea + 2) + ((uint16_t)read86(ea + 3) << 8); break;\
    case 4: /*JMP Ev*/\
      ip = oper1; break;\
    case 5: /*JMP Mp*/\
      /*getea(rm);*/\
      ip = (uint16_t)read86(ea) + ((uint16_t)read86(ea + 1) << 8);\
      segregs[regcs] = (uint16_t)read86(ea + 2) + ((uint16_t)read86(ea + 3) << 8); break;\
    case 6: /*PUSH Ev*/\
      push(oper1); break;\
  }\
}

uint8_t didintr = 0;

void intcall86(uint8_t intnum) {
  didintr = 1;

  switch (intnum) {
    case 0x10: //video services
      if (regs.byteregs[regah] == 0) { //video mode set
#ifdef ADVANCED_CLIENT
      Serial.write(0xFF);
      Serial.write(0x02);
      Serial.write(regs.byteregs[regal]);
      Serial.write(regs.byteregs[regal]); //duplicate for checksum
      Serial.write(0xFE);
      Serial.write(0x02);
#endif
#ifdef USE_DISPLAY
        if (vidmode != regs.byteregs[regal]) clear_display();
        palettereset();
#endif
        vidmode = regs.byteregs[regal];
        //Serial.print("vidmode = "); Serial.println(vidmode);
      }
      break;
    case 0x13: //disk services
      diskhandler();
      return;
    case 0x19: //bootstrap
      //Serial.println("Bootstrap!");
      if (bootdrive < 255) { //read first sector of boot drive into 07C0:0000 and execute it
        regs.byteregs[regdl] = bootdrive;
        readdisk((bootdrive & 0x80) ? bootdrive - 126 : bootdrive, 0x07C0, 0x0000, 0, 1, 0, 1);
        segregs[regcs] = 0x0000; ip = 0x7C00;
      } else {
        segregs[regcs] = 0xF600;  //start ROM BASIC at bootstrap if requested
        ip = 0x0000;
      }
      return;
    case 0xFC:
      net_handler();
      return;
    default:
      break;
  }

  push(makeflagsword());
  push(segregs[regcs]);
  push(ip);
  segregs[regcs] = getmem16(0, ((uint16_t)intnum << 2) + 2);
  ip = getmem16(0, (uint16_t)intnum << 2);
  ifl = 0;
  tf = 0;
}

uint64_t frametimer = 0, didwhen = 0, didticks = 0;
uint32_t makeupticks = 0;
extern float timercomp;
uint64_t timerticks = 0, realticks = 0;
uint64_t lastcountertimer = 0, counterticks = 10000;

#ifdef PROFILING
uint32_t startmicros, endmicros;
uint32_t instrtime[0x100];
#endif

#ifdef PS2_KEYBOARD
extern uint8_t kbloop;
#endif

extern volatile uint8_t timerTick;

void exec86(uint32_t execloops) {
  uint32_t loopcount;
  uint8_t docontinue;
  static uint16_t firstip, trap_toggle = 0;

  for (loopcount = 0; loopcount < execloops; loopcount++) {
    /*Serial.print(segregs[regcs], HEX);
    Serial.write(':');
    Serial.println(ip, HEX);*/
    #ifdef PS2_KEYBOARD
    if (kbloop) {
      uint32_t msnow;
      msnow = micros();
      while ((micros() - msnow) < 20000) { }
      kbloop = 0;
      ps2poll();
    }
    #endif

    if (timerTick) {
      //printf("isr\n");
      timerTick = 0;
      doirq(0);
    }
    
    if (trap_toggle) intcall86(1);
    if (tf)  trap_toggle = 1;
    else trap_toggle = 0;
    if (!trap_toggle && (ifl && (i8259.irr & (~i8259.imr)))) intcall86(nextintr()); //get next interrupt from the i8259, if any
    reptype = 0; segoverride = 0;
    useseg = segregs[regds]; docontinue = 0;
    firstip = ip;
    while (!docontinue) {
      segregs[regcs] = segregs[regcs] & 0xFFFF; ip = ip & 0xFFFF;
      savecs = segregs[regcs]; saveip = ip;
      opcode = getmem8(segregs[regcs], ip); StepIP(1);

      switch (opcode) {
        //segment prefix check
        case 0x2E: //segment segregs[regcs]
          useseg = segregs[regcs]; segoverride = 1; break;
        case 0x3E: //segment segregs[regds]
          useseg = segregs[regds]; segoverride = 1; break;
        case 0x26: //segment segregs[reges]
          useseg = segregs[reges]; segoverride = 1; break;
        case 0x36: //segment segregs[regss]
          useseg = segregs[regss]; segoverride = 1; break;

        //repetition prefix check
        case 0xF3: //REP/REPE/REPZ
          reptype = 1; break;
        case 0xF2: //REPNE/REPNZ
          reptype = 2; break;
        default:
          docontinue = 1;
          break;
      }
    }
    totalexec++;
    //printf("%04X:%04X %02X\n", segregs[regcs], ip, opcode);
    //if ((segregs[regcs]==0xF000) && (ip < 0xE000)) exit(0);

#ifdef PROFILING
    startmicros = micros();
#endif
    switch (opcode) {
      case 0x0: //00 ADD Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        op_add8();
        writerm8(rm, res8);
        break;
      case 0x1: //01 ADD Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        op_add16();
        writerm16(rm, res16);
        break;
      case 0x2: //02 ADD Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        op_add8();
        putreg8(reg, res8);
        break;
      case 0x3: //03 ADD Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        op_add16();
        putreg16(reg, res16);
        break;
      case 0x4: //04 ADD regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        op_add8();
        regs.byteregs[regal] = res8;
        break;
      case 0x5: //05 ADD eAX Iv
        oper1 = (getreg16(regax)); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        op_add16();
        putreg16(regax, res16);
        break;
      case 0x6: //06 PUSH segregs[reges]
        push(segregs[reges]);
        break;
      case 0x7: //07 POP segregs[reges]
        segregs[reges] = pop();
        break;
      case 0x8: //08 OR Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        op_or8();
        writerm8(rm, res8);
        break;
      case 0x9: //09 OR Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        op_or16();
        writerm16(rm, res16);
        break;
      case 0xA: //0A OR Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        op_or8();
        putreg8(reg, res8);
        break;
      case 0xB: //0B OR Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        op_or16();
        if ((oper1 == 0xF802) && (oper2 == 0xF802)) sf = 0; //cheap hack to make Wolf 3D think we're a 286 so it plays
        putreg16(reg, res16);
        break;
      case 0xC: //0C OR regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        op_or8();
        regs.byteregs[regal] = res8;
        break;
      case 0xD: //0D OR eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        op_or16();
        putreg16(regax, res16);
        break;
      case 0xE: //0E PUSH segregs[regcs]
        push(segregs[regcs]);
        break;
      case 0xF: //0F POP CS
        segregs[regcs] = pop();
        break;
      /*case 0xF: //0F 80286+ extended opcodes
          segregs[regcs] = segregs[regcs] & 0xFFFF; ip = ip & 0xFFFF;
          savecs = segregs[regcs]; saveip = ip;
          opcode = getmem8(segregs[regcs], ip); StepIP(1);
          op_286();
          break;*/
      case 0x10: //10 ADC Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        op_adc8();
        writerm8(rm, res8);
        break;
      case 0x11: //11 ADC Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        op_adc16();
        writerm16(rm, res16);
        break;
      case 0x12: //12 ADC Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        op_adc8();
        putreg8(reg, res8);
        break;
      case 0x13: //13 ADC Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        op_adc16();
        putreg16(reg, res16);
        break;
      case 0x14: //14 ADC regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        op_adc8();
        regs.byteregs[regal] = res8;
        break;
      case 0x15: //15 ADC eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        op_adc16();
        putreg16(regax, res16);
        break;
      case 0x16: //16 PUSH segregs[regss]
        push(segregs[regss]);
        break;
      case 0x17: //17 POP segregs[regss]
        segregs[regss] = pop();
        break;
      case 0x18: //18 SBB Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        op_sbb8();
        writerm8(rm, res8);
        break;
      case 0x19: //19 SBB Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        op_sbb16();
        writerm16(rm, res16);
        break;
      case 0x1A: //1A SBB Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        op_sbb8();
        putreg8(reg, res8);
        break;
      case 0x1B: //1B SBB Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        op_sbb16();
        putreg16(reg, res16);
        break;
      case 0x1C: //1C SBB regs.byteregs[regal] Ib;
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        op_sbb8();
        regs.byteregs[regal] = res8;
        break;
      case 0x1D: //1D SBB eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        op_sbb16();
        putreg16(regax, res16);
        break;
      case 0x1E: //1E PUSH segregs[regds]
        push(segregs[regds]);
        break;
      case 0x1F: //1F POP segregs[regds]
        segregs[regds] = pop();
        break;
      case 0x20: //20 AND Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        op_and8();
        writerm8(rm, res8);
        break;
      case 0x21: //21 AND Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        op_and16();
        writerm16(rm, res16);
        break;
      case 0x22: //22 AND Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        op_and8();
        putreg8(reg, res8);
        break;
      case 0x23: //23 AND Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        op_and16();
        putreg16(reg, res16);
        break;
      case 0x24: //24 AND regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        op_and8();
        regs.byteregs[regal] = res8;
        break;
      case 0x25: //25 AND eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        op_and16();
        putreg16(regax, res16);
        break;
      case 0x27: //27 DAA
        if (((regs.byteregs[regal] & 0xF) > 9) || (af == 1)) {
          oper1 = regs.byteregs[regal] + 6;
          regs.byteregs[regal] = oper1 & 255;
          if (oper1 & 0xFF00) cf = 1; else cf = 0;
          af = 1;
        } else af = 0;
        if (((regs.byteregs[regal] & 0xF0) > 0x90) || (cf == 1)) {
          regs.byteregs[regal] = regs.byteregs[regal] + 0x60;
          cf = 1;
        } else cf = 0;
        regs.byteregs[regal] = regs.byteregs[regal] & 255;
        flag_szp8(regs.byteregs[regal]);
        break;
      case 0x28: //28 SUB Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        op_sub8();
        writerm8(rm, res8);
        break;
      case 0x29: //29 SUB Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        op_sub16();
        writerm16(rm, res16);
        break;
      case 0x2A: //2A SUB Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        op_sub8();
        putreg8(reg, res8);
        break;
      case 0x2B: //2B SUB Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        op_sub16();
        putreg16(reg, res16);
        break;
      case 0x2C: //2C SUB regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        op_sub8();
        regs.byteregs[regal] = res8;
        break;
      case 0x2D: //2D SUB eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        op_sub16();
        putreg16(regax, res16);
        break;
      case 0x2F: //2F DAS
        if (((regs.byteregs[regal] & 15) > 9) || (af == 1)) {
          oper1 = regs.byteregs[regal] - 6;
          regs.byteregs[regal] = oper1 & 255;
          if (oper1 & 0xFF00) cf = 1; else cf = 0;
          af = 1;
        } else af = 0;
        if (((regs.byteregs[regal] & 0xF0) > 0x90) || (cf == 1)) {
          regs.byteregs[regal] = regs.byteregs[regal] - 0x60;
          cf = 1;
        } else cf = 0;
        flag_szp8(regs.byteregs[regal]);
        break;
      case 0x30: //30 XOR Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        op_xor8();
        writerm8(rm, res8);
        break;
      case 0x31: //31 XOR Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        op_xor16();
        writerm16(rm, res16);
        break;
      case 0x32: //32 XOR Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        op_xor8();
        putreg8(reg, res8);
        break;
      case 0x33: //33 XOR Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        op_xor16();
        putreg16(reg, res16);
        break;
      case 0x34: //34 XOR regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        op_xor8();
        regs.byteregs[regal] = res8;
        break;
      case 0x35: //35 XOR eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        op_xor16();
        putreg16(regax, res16);
        break;
      case 0x37: //37 AAA ASCII
        if (((regs.byteregs[regal] & 0xF) > 9) || (af == 1)) {
          regs.byteregs[regal] = regs.byteregs[regal] + 6;
          regs.byteregs[regah] = regs.byteregs[regah] + 1;
          af = 1;
          cf = 1;
        } else {
          af = 0;
          cf = 0;
        }
        regs.byteregs[regal] = regs.byteregs[regal] & 0xF;
        break;
      case 0x38: //38 CMP Eb Gb
        modregrm();
        oper1b = readrm8(rm); oper2b = getreg8(reg);
        flag_sub8(oper1b, oper2b);
        break;
      case 0x39: //39 CMP Ev Gv
        modregrm();
        oper1 = readrm16(rm); oper2 = getreg16(reg);
        flag_sub16(oper1, oper2);
        break;
      case 0x3A: //3A CMP Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        flag_sub8(oper1b, oper2b);
        break;
      case 0x3B: //3B CMP Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        flag_sub16(oper1, oper2);
        break;
      case 0x3C: //3C CMP regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        flag_sub8(oper1b, oper2b);
        break;
      case 0x3D: //3D CMP eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        flag_sub16(oper1, oper2);
        break;
      case 0x3F: //3F AAS ASCII
        if (((regs.byteregs[regal] & 0xF) > 9) || (af == 1)) {
          regs.byteregs[regal] = regs.byteregs[regal] - 6;
          regs.byteregs[regah] = regs.byteregs[regah] - 1;
          af = 1;
          cf = 1;
        } else {
          af = 0;
          cf = 0;
        }
        regs.byteregs[regal] = regs.byteregs[regal] & 0xF;
        break;
      case 0x40: //40 INC eAX
        oldcf = cf;
        oper1 = getreg16(regax); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regax, res16);
        break;
      case 0x41: //41 INC eCX
        oldcf = cf;
        oper1 = getreg16(regcx); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regcx, res16);
        break;
      case 0x42: //42 INC eDX
        oldcf = cf;
        oper1 = getreg16(regdx); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regdx, res16);
        break;
      case 0x43: //43 INC eBX
        oldcf = cf;
        oper1 = getreg16(regbx); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regbx, res16);
        break;
      case 0x44: //44 INC eSP
        oldcf = cf;
        oper1 = getreg16(regsp); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regsp, res16);
        break;
      case 0x45: //45 INC eBP
        oldcf = cf;
        oper1 = getreg16(regbp); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regbp, res16);
        break;
      case 0x46: //46 INC eSI
        oldcf = cf;
        oper1 = getreg16(regsi); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regsi, res16);
        break;
      case 0x47: //47 INC eDI
        oldcf = cf;
        oper1 = getreg16(regdi); oper2 = 1;
        op_add16();
        cf = oldcf;
        putreg16(regdi, res16);
        break;
      case 0x48: //48 DEC eAX
        oldcf = cf;
        oper1 = getreg16(regax); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regax, res16);
        break;
      case 0x49: //49 DEC eCX
        oldcf = cf;
        oper1 = getreg16(regcx); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regcx, res16);
        break;
      case 0x4A: //4A DEC eDX
        oldcf = cf;
        oper1 = getreg16(regdx); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regdx, res16);
        break;
      case 0x4B: //4B DEC eBX
        oldcf = cf;
        oper1 = getreg16(regbx); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regbx, res16);
        break;
      case 0x4C: //4C DEC eSP
        oldcf = cf;
        oper1 = getreg16(regsp); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regsp, res16);
        break;
      case 0x4D: //4D DEC eBP
        oldcf = cf;
        oper1 = getreg16(regbp); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regbp, res16);
        break;
      case 0x4E: //4E DEC eSI
        oldcf = cf;
        oper1 = getreg16(regsi); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regsi, res16);
        break;
      case 0x4F: //4F DEC eDI
        oldcf = cf;
        oper1 = getreg16(regdi); oper2 = 1;
        op_sub16();
        cf = oldcf;
        putreg16(regdi, res16);
        break;
      case 0x50: //50 PUSH eAX
        push (getreg16(regax));
        break;
      case 0x51: //51 PUSH eCX
        push (getreg16(regcx));
        break;
      case 0x52: //52 PUSH eDX
        push (getreg16(regdx));
        break;
      case 0x53: //53 PUSH eBX
        push (getreg16(regbx));
        break;
      case 0x54: //54 PUSH eSP
        push (getreg16(regsp) - 2);
        break;
      case 0x55: //55 PUSH eBP
        push (getreg16(regbp));
        break;
      case 0x56: //56 PUSH eSI
        push (getreg16(regsi));
        break;
      case 0x57: //57 PUSH eDI
        push (getreg16(regdi));
        break;
      case 0x58: //58 POP eAX
        putreg16(regax, pop());
        break;
      case 0x59: //59 POP eCX
        putreg16(regcx, pop());
        break;
      case 0x5A: //5A POP eDX
        putreg16(regdx, pop());
        break;
      case 0x5B: //5B POP eBX
        putreg16(regbx, pop());
        break;
      case 0x5C: //5C POP eSP
        putreg16(regsp, pop());
        break;
      case 0x5D: //5D POP eBP
        putreg16(regbp, pop());
        break;
      case 0x5E: //5E POP eSI
        putreg16(regsi, pop());
        break;
      case 0x5F: //5F POP eDI
        putreg16(regdi, pop());
        break;
      case 0x60: //60 PUSHA (80186+)
        oldsp = getreg16(regsp);
        push(getreg16(regax));
        push(getreg16(regcx));
        push(getreg16(regdx));
        push(getreg16(regbx));
        push(oldsp);
        push(getreg16(regbp));
        push(getreg16(regsi));
        push(getreg16(regdi));
        break;
      case 0x61: //61 POPA (80186+)
        putreg16(regdi, pop());
        putreg16(regsi, pop());
        putreg16(regbp, pop());
        dummy = pop();
        putreg16(regbx, pop());
        putreg16(regdx, pop());
        putreg16(regcx, pop());
        putreg16(regax, pop());
        break;
      case 0x68: //68 PUSH Iv (80186+)
        push(getmem16(segregs[regcs], ip)); StepIP(2);
        break;
      case 0x69: //69 IMUL Gv Ev Iv (80186+)
        //print("WE HIT 69h IMUL\r\n");
        modregrm();
        temp1 = readrm16(rm);
        temp2 = getmem16(segregs[regcs], ip); StepIP(2);
        if ((temp1 & 0x8000L) == 0x8000L) temp1 = temp1 | 0xFFFF0000L;
        if ((temp2 & 0x8000L) == 0x8000L) temp2 = temp2 | 0xFFFF0000L;
        temp3 = temp1 * temp2;
        putreg16(reg, temp3 & 0xFFFFL);
        if (temp3 & 0xFFFF0000L) {
          cf = 1;
          of = 1;
        } else {
          cf = 0;
          of = 0;
        }
        break;
      case 0x6A: //6A PUSH Ib (80186+)
        push(getmem8(segregs[regcs], ip)); StepIP(1);
        break;
      case 0x6B: //6B IMUL Gv Eb Ib (80186+)
        //print("WE HIT 6Bh IMUL\r\n");
        modregrm();
        temp1 = readrm16(rm);
        temp2 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if ((temp1 & 0x8000L) == 0x8000L) temp1 = temp1 | 0xFFFF0000L;
        if ((temp2 & 0x8000L) == 0x8000L) temp2 = temp2 | 0xFFFF0000L;
        temp3 = temp1 * temp2;
        putreg16(reg, temp3 & 0xFFFFL);
        if (temp3 & 0xFFFF0000L) {
          cf = 1;
          of = 1;
        } else {
          cf = 0;
          of = 0;
        }
        break;
      //case 0x6C ... 0x6F: //80186 port operations, just act as if they//re NOPs for now...
      //    StepIP(1); //they have a modregrm(); byte we must skip... i may properly emulate these later.
      //    break;
      case 0x6E: //6E OUTSB
        if (reptype && (getreg16(regcx) == 0)) break;
        portout16 = 0;
        portout(regs.wordregs[regdx], getmem16(useseg, getreg16(regsi)));
        if (df) {
          putreg16(regsi, getreg16(regsi) - 1);
          putreg16(regdi, getreg16(regdi) - 1);
        }
        else {
          putreg16(regsi, getreg16(regsi) + 1);
          putreg16(regdi, getreg16(regdi) + 1);
        }
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;
      case 0x6F: //6F OUTSW
        if (reptype && (getreg16(regcx) == 0)) break;
        portout16 = 1;
        portout(regs.wordregs[regdx], getmem16(useseg, getreg16(regsi)));
        if (df) {
          putreg16(regsi, getreg16(regsi) - 2);
          putreg16(regdi, getreg16(regdi) - 2);
        }
        else {
          putreg16(regsi, getreg16(regsi) + 2);
          putreg16(regdi, getreg16(regdi) + 2);
        }
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;

      case 0x70: //70 JO Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (of) ip = ip + temp16;
        break;
      case 0x71: //71 JNO Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!of) ip = ip + temp16;
        break;
      case 0x72: //72 JB Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (cf) ip = ip + temp16;
        break;
      case 0x73: //73 JNB Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!cf) ip = ip + temp16;
        break;
      case 0x74: //74 JZ Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (zf) ip = ip + temp16;
        break;
      case 0x75: //75 JNZ Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!zf) ip = ip + temp16;
        break;
      case 0x76: //76 JBE Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (cf || zf) ip = ip + temp16;
        break;
      case 0x77: //77 JA Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!cf && !zf) ip = ip + temp16;
        break;
      case 0x78: //78 JS Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (sf) ip = ip + temp16;
        break;
      case 0x79: //79 JNS Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!sf) ip = ip + temp16;
        break;
      case 0x7A: //7A JPE Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (pf) ip = ip + temp16;
        break;
      case 0x7B: //7B JPO Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!pf) ip = ip + temp16;
        break;
      case 0x7C: //7C JL Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (sf != of) ip = ip + temp16;
        break;
      case 0x7D: //7D JGE Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (sf == of) ip = ip + temp16;
        break;
      case 0x7E: //7E JLE Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if ((sf != of) || zf) ip = ip + temp16;
        break;
      case 0x7F: //7F JG Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!zf && (sf == of)) ip = ip + temp16;
        break;
      case 0x80: case 0x82: //80/82 GRP1 Eb Ib
        modregrm();
        oper1b = readrm8(rm);
        oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        switch (reg) {
          case 0: op_add8(); break;
          case 1: op_or8(); break;
          case 2: op_adc8(); break;
          case 3: op_sbb8(); break;
          case 4: op_and8(); break;
          case 5: op_sub8(); break;
          case 6: op_xor8(); break;
          case 7: flag_sub8(oper1b, oper2b); break;
          default: break; //to avoid compiler warnings
        }
        if (reg < 7) writerm8(rm, res8);
        break;
      case 0x81: //81 GRP1 Ev Iv
      case 0x83: //83 GRP1 Ev Ib
        modregrm();
        oper1 = readrm16(rm);
        if (opcode == 0x81) {
          oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        } else {
          oper2 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        }
        switch (reg) {
          case 0: op_add16(); break;
          case 1: op_or16(); break;
          case 2: op_adc16(); break;
          case 3: op_sbb16(); break;
          case 4: op_and16(); break;
          case 5: op_sub16(); break;
          case 6: op_xor16(); break;
          case 7: flag_sub16(oper1, oper2); break;
          default: break; //to avoid compiler warnings
        }
        if (reg < 7) writerm16(rm, res16);
        break;
      case 0x84: //84 TEST Gb Eb
        modregrm();
        oper1b = getreg8(reg); oper2b = readrm8(rm);
        flag_log8(oper1b & oper2b);
        break;
      case 0x85: //85 TEST Gv Ev
        modregrm();
        oper1 = getreg16(reg); oper2 = readrm16(rm);
        flag_log16(oper1 & oper2);
        break;
      case 0x86: //86 XCHG Gb Eb
        modregrm();
        oper1b = getreg8(reg);
        putreg8(reg, readrm8(rm));
        writerm8(rm, oper1b);
        break;
      case 0x87: //87 XCHG Gv Ev
        modregrm();
        oper1 = getreg16(reg);
        putreg16(reg, readrm16(rm));
        writerm16(rm, oper1);
        break;
      case 0x88: //88 MOV Eb Gb
        modregrm();
        writerm8(rm, getreg8(reg));
        break;
      case 0x89: //89 MOV Ev Gv
        modregrm();
        writerm16(rm, getreg16(reg));
        break;
      case 0x8A: //8A MOV Gb Eb
        modregrm();
        putreg8(reg, readrm8(rm));
        break;
      case 0x8B: //8B MOV Gv Ev
        modregrm();
        putreg16(reg, readrm16(rm));
        break;
      case 0x8C: //8C MOV Ew Sw
        modregrm();
        writerm16(rm, getsegreg(reg));
        break;
      case 0x8D: //8D LEA Gv M
        modregrm();
        //getea(rm);
        putreg16(reg, ea - segbase(useseg));
        break;
      case 0x8E: //8E MOV Sw Ew
        modregrm();
        putsegreg(reg, readrm16(rm));
        break;
      case 0x8F: //8F POP Ev
        modregrm();
        writerm16(rm, pop());
        break;
      case 0x90: //90 NOP
        break;
      case 0x91: //91 XCHG eCX eAX
        oper1 = getreg16(regcx);
        putreg16(regcx, getreg16(regax));
        putreg16(regax, oper1);
        break;
      case 0x92: //92 XCHG eDX eAX
        oper1 = getreg16(regdx);
        putreg16(regdx, getreg16(regax));
        putreg16(regax, oper1);
        break;
      case 0x93: //93 XCHG eBX eAX
        oper1 = getreg16(regbx);
        putreg16(regbx, getreg16(regax));
        putreg16(regax, oper1);
        break;
      case 0x94: //94 XCHG eSP eAX
        oper1 = getreg16(regsp);
        putreg16(regsp, getreg16(regax));
        putreg16(regax, oper1);
        break;
      case 0x95: //95 XCHG eBP eAX
        oper1 = getreg16(regbp);
        putreg16(regbp, getreg16(regax));
        putreg16(regax, oper1);
        break;
      case 0x96: //96 XCHG eSI eAX
        oper1 = getreg16(regsi);
        putreg16(regsi, getreg16(regax));
        putreg16(regax, oper1);
        break;
      case 0x97: //97 XCHG eDI eAX
        oper1 = getreg16(regdi);
        putreg16(regdi, getreg16(regax));
        putreg16(regax, oper1);
        break;
      case 0x98: //98 CBW
        if ((regs.byteregs[regal] & 0x80) == 0x80) regs.byteregs[regah] = 0xFF; else regs.byteregs[regah] = 0;
        break;
      case 0x99: //99 CWD
        if ((regs.byteregs[regah] & 0x80) == 0x80) putreg16(regdx, 0xFFFF); else putreg16(regdx, 0);
        break;
      case 0x9A: //9A CALL Ap
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        push(segregs[regcs]); push(ip); ip = oper1; segregs[regcs] = oper2;
        break;
      case 0x9B: //9B WAIT
        break;
      case 0x9C: //9C PUSHF
        push(makeflagsword() | 0xF800);
        break;
      case 0x9D: //9D POPF
        temp16 = pop();
        decodeflagsword(temp16);
        break;
      case 0x9E: //9E SAHF
        decodeflagsword ((makeflagsword() & 0xFF00) | regs.byteregs[regah]);
        break;
      case 0x9F: //9F LAHF
        regs.byteregs[regah] = makeflagsword() & 0xFF;
        break;
      case 0xA0: //A0 MOV regs.byteregs[regal] Ob
        regs.byteregs[regal] = getmem8(useseg, getmem16(segregs[regcs], ip)); StepIP(2);
        break;
      case 0xA1: //A1 MOV eAX Ov
        oper1 = getmem16(useseg, getmem16(segregs[regcs], ip)); StepIP(2);
        putreg16(regax, oper1);
        break;
      case 0xA2: //A2 MOV Ob regs.byteregs[regal]
        putmem8(useseg, getmem16(segregs[regcs], ip), regs.byteregs[regal]); StepIP(2);
        break;
      case 0xA3: //A3 MOV Ov eAX
        putmem16(useseg, getmem16(segregs[regcs], ip), getreg16(regax)); StepIP(2);
        break;
      case 0xA4: //A4 MOVSB
        if (reptype && (getreg16(regcx) == 0)) break;
        putmem8(segregs[reges], getreg16(regdi), getmem8(useseg, getreg16(regsi)));
        if (df) {
          putreg16(regsi, getreg16(regsi) - 1);
          putreg16(regdi, getreg16(regdi) - 1);
        }
        else {
          putreg16(regsi, getreg16(regsi) + 1);
          putreg16(regdi, getreg16(regdi) + 1);
        }
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xA5: //A5 MOVSW
        if (reptype && (getreg16(regcx) == 0)) break;
        putmem16(segregs[reges], getreg16(regdi), getmem16(useseg, getreg16(regsi)));
        if (df) {
          putreg16(regsi, getreg16(regsi) - 2);
          putreg16(regdi, getreg16(regdi) - 2);
        }
        else {
          putreg16(regsi, getreg16(regsi) + 2);
          putreg16(regdi, getreg16(regdi) + 2);
        }
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xA6: //A6 CMPSB
        if (reptype && (getreg16(regcx) == 0)) break;
        oper1b = getmem8(useseg, getreg16(regsi)); oper2b = getmem8(segregs[reges], getreg16(regdi));
        if (df) {
          putreg16(regsi, getreg16(regsi) - 1);
          putreg16(regdi, getreg16(regdi) - 1);
        }
        else {
          putreg16(regsi, getreg16(regsi) + 1);
          putreg16(regdi, getreg16(regdi) + 1);
        }
        flag_sub8(oper1b, oper2b);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if ((reptype == 1) && !zf) break;
        else if ((reptype == 2) && (zf == 1)) break;
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xA7: //A7 CMPSW
        if (reptype && (getreg16(regcx) == 0)) break;
        oper1 = getmem16(useseg, getreg16(regsi)); oper2 = getmem16(segregs[reges], getreg16(regdi));
        if (df) {
          putreg16(regsi, getreg16(regsi) - 2);
          putreg16(regdi, getreg16(regdi) - 2);
        }
        else {
          putreg16(regsi, getreg16(regsi) + 2);
          putreg16(regdi, getreg16(regdi) + 2);
        }
        flag_sub16(oper1, oper2);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if ((reptype == 1) && !zf) break;
        if ((reptype == 2) && (zf == 1)) break;
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xA8: //A8 TEST regs.byteregs[regal] Ib
        oper1b = regs.byteregs[regal]; oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        flag_log8(oper1b & oper2b);
        break;
      case 0xA9: //A9 TEST eAX Iv
        oper1 = getreg16(regax); oper2 = getmem16(segregs[regcs], ip); StepIP(2);
        flag_log16(oper1 & oper2);
        break;
      case 0xAA: //AA STOSB
        if (reptype && (getreg16(regcx) == 0)) break;
        putmem8(segregs[reges], getreg16(regdi), regs.byteregs[regal]);
        if (df) putreg16(regdi, getreg16(regdi) - 1);
        else putreg16(regdi, getreg16(regdi) + 1);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xAB: //AB STOSW
        if (reptype && (getreg16(regcx) == 0)) break;
        putmem16(segregs[reges], getreg16(regdi), getreg16(regax));
        if (df) putreg16(regdi, getreg16(regdi) - 2);
        else putreg16(regdi, getreg16(regdi) + 2);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xAC: //AC LODSB
        if (reptype && (getreg16(regcx) == 0)) break;
        regs.byteregs[regal] = getmem8(useseg, getreg16(regsi));
        if (df) putreg16(regsi, getreg16(regsi) - 1);
        else putreg16(regsi, getreg16(regsi) + 1);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xAD: //AD LODSW
        if (reptype && (getreg16(regcx) == 0)) break;
        oper1 = getmem16(useseg, getreg16(regsi));
        putreg16(regax, oper1);
        if (df) putreg16(regsi, getreg16(regsi) - 2);
        else putreg16(regsi, getreg16(regsi) + 2);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xAE: //AE SCASB
        if (reptype && (getreg16(regcx) == 0)) break;
        oper1b = getmem8(segregs[reges], getreg16(regdi)); oper2b = regs.byteregs[regal];
        flag_sub8(oper1b, oper2b);
        if (df) putreg16(regdi, getreg16(regdi) - 1);
        else putreg16(regdi, getreg16(regdi) + 1);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if ((reptype == 1) && !zf) break;
        else if ((reptype == 2) && (zf == 1)) break;
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xAF: //AF SCASW
        if (reptype && (getreg16(regcx) == 0)) break;
        oper1 = getmem16(segregs[reges], getreg16(regdi)); oper2 = getreg16(regax);
        flag_sub16(oper1, oper2);
        if (df) putreg16(regdi, getreg16(regdi) - 2);
        else putreg16(regdi, getreg16(regdi) + 2);
        if (reptype) putreg16(regcx, getreg16(regcx) - 1);
        if ((reptype == 1) && !zf) break;
        else if ((reptype == 2) & (zf == 1)) break;
        if (!reptype) break;
        ip = firstip;
        break;
      case 0xB0: //B0 MOV regs.byteregs[regal] Ib
        regs.byteregs[regal] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB1: //B1 MOV regs.byteregs[regcl] Ib
        regs.byteregs[regcl] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB2: //B2 MOV regs.byteregs[regdl] Ib
        regs.byteregs[regdl] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB3: //B3 MOV regs.byteregs[regbl] Ib
        regs.byteregs[regbl] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB4: //B4 MOV regs.byteregs[regah] Ib
        regs.byteregs[regah] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB5: //B5 MOV regs.byteregs[regch] Ib
        regs.byteregs[regch] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB6: //B6 MOV regs.byteregs[regdh] Ib
        regs.byteregs[regdh] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB7: //B7 MOV regs.byteregs[regbh] Ib
        regs.byteregs[regbh] = getmem8(segregs[regcs], ip); StepIP(1);
        break;
      case 0xB8: //B8 MOV eAX Iv
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        putreg16(regax, oper1);
        break;
      case 0xB9: //B9 MOV eCX Iv
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        putreg16(regcx, oper1);
        break;
      case 0xBA: //BA MOV eDX Iv
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        putreg16(regdx, oper1);
        break;
      case 0xBB: //BB MOV eBX Iv
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        putreg16(regbx, oper1);
        break;
      case 0xBC: //BC MOV eSP Iv
        putreg16(regsp, getmem16(segregs[regcs], ip)); StepIP(2);
        break;
      case 0xBD: //BD MOV eBP Iv
        putreg16(regbp, getmem16(segregs[regcs], ip)); StepIP(2);
        break;
      case 0xBE: //BE MOV eSI Iv
        putreg16(regsi, getmem16(segregs[regcs], ip)); StepIP(2);
        break;
      case 0xBF: //BF MOV eDI Iv
        putreg16(regdi, getmem16(segregs[regcs], ip)); StepIP(2);
        break;
      case 0xC0: //C0 GRP2 byte imm8 (80186+)
        modregrm();
        oper1b = readrm8(rm);
        oper2b = getmem8(segregs[regcs], ip); StepIP(1);
        writerm8(rm, op_grp2_8(oper2b));
        break;
      case 0xC1: //C1 GRP2 word imm8 (80186+)
        modregrm();
        oper1 = readrm16(rm);
        oper2 = getmem8(segregs[regcs], ip); StepIP(1);
        writerm16(rm, op_grp2_16(oper2));
        break;
      case 0xC2: //C2 RET Iw
        oper1 = getmem16(segregs[regcs], ip);
        ip = pop();
        putreg16(regsp, getreg16(regsp) + oper1);
        break;
      case 0xC3: //C3 RET
        ip = pop();
        break;
      case 0xC4: //C4 LES Gv Mp
        modregrm();
        //getea(rm);
        putreg16(reg, read86(ea) + ((uint16_t)read86(ea + 1) << 8));
        segregs[reges] = read86(ea + 2) + ((uint16_t)read86(ea + 3) << 8);
        break;
      case 0xC5: //C5 LDS Gv Mp
        modregrm();
        //getea(rm);
        putreg16(reg, read86(ea) + ((uint16_t)read86(ea + 1) << 8));
        segregs[regds] = read86(ea + 2) + ((uint16_t)read86(ea + 3) << 8);
        break;
      case 0xC6: //C6 MOV Eb Ib
        modregrm();
        writerm8(rm, getmem8(segregs[regcs], ip)); StepIP(1);
        break;
      case 0xC7: //C7 MOV Ev Iv
        modregrm();
        writerm16(rm, getmem16(segregs[regcs], ip)); StepIP(2);
        break;
      case 0xC8: //C8 ENTER (80186+)
        stacksize = getmem16(segregs[regcs], ip); StepIP(2);
        nestlev = getmem8(segregs[regcs], ip); StepIP(1);
        push(getreg16(regbp));
        frametemp = getreg16(regsp);
        if (nestlev) {
          for (temp16 = 1; temp16 < nestlev; temp16++) {
            putreg16(regbp, getreg16(regbp) - 2);
            push(getreg16(regbp));
          }
          push(getreg16(regsp));
        }
        putreg16(regbp, frametemp);
        putreg16(regsp, getreg16(regbp) - stacksize);

        break;
      case 0xC9: //C9 LEAVE (80186+)
        putreg16(regsp, getreg16(regbp));
        putreg16(regbp, pop());

        break;
      case 0xCA: //CA RETF Iw
        oper1 = getmem16(segregs[regcs], ip);
        ip = pop(); segregs[regcs] = pop();
        putreg16(regsp, getreg16(regsp) + oper1);
        break;
      case 0xCB: //CB RETF
        ip = pop();; segregs[regcs] = pop();
        break;
      case 0xCC: //CC INT 3
        intcall86(3);
        break;
      case 0xCD: //CD INT Ib
        oper1 = getmem8(segregs[regcs], ip); StepIP(1);
        intcall86(oper1);
        break;
      case 0xCE: //CE INTO
        if (of) intcall86(4);
        break;
      case 0xCF: //CF IRET
        ip = pop(); segregs[regcs] = pop();
        decodeflagsword(pop());
        //if (net.enabled) net.canrecv = 1;
        break;
      case 0xD0: //D0 GRP2 Eb 1
        modregrm();
        oper1b = readrm8(rm);
        writerm8(rm, op_grp2_8(1));
        break;
      case 0xD1: //D1 GRP2 Ev 1
        modregrm();
        oper1 = readrm16(rm);
        writerm16(rm, op_grp2_16(1));
        break;
      case 0xD2: //D2 GRP2 Eb regs.byteregs[regcl]
        modregrm();
        oper1b = readrm8(rm);
        writerm8(rm, op_grp2_8(regs.byteregs[regcl]));
        break;
      case 0xD3: //D3 GRP2 Ev regs.byteregs[regcl]
        modregrm();
        oper1 = readrm16(rm);
        writerm16(rm, op_grp2_16(regs.byteregs[regcl]));
        break;
      case 0xD4: //D4 AAM I0
        oper1 = getmem8(segregs[regcs], ip); StepIP(1);
        if (!oper1) {
          intcall86(0);  //division by zero
          return;
        }
        regs.byteregs[regah] = (regs.byteregs[regal] / oper1) & 255;
        regs.byteregs[regal] = (regs.byteregs[regal] % oper1) & 255;
        flag_szp16 (getreg16(regax));
        break;
      case 0xD5: //D5 AAD I0
        oper1 = getmem8(segregs[regcs], ip); StepIP(1);
        regs.byteregs[regal] = (regs.byteregs[regah] * oper1 + regs.byteregs[regal]) & 255;
        regs.byteregs[regah] = 0;
        flag_szp16(regs.byteregs[regah] * oper1 + regs.byteregs[regal]);
        sf = 0;
        break;
      case 0xD6: //D6 XLAT on V20/V30, SALC on 8086/8088
        regs.byteregs[regal] = cf;
        break;
      case 0xD7: //D7 XLAT
        putreg8(regal, read86(segbase(useseg) + (uint32_t)getreg16(regbx) + (uint32_t)getreg8(regal)));
        break;
      case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDE: case 0xDD: case 0xDF: //escape
        StepIP(1);
        break;
      case 0xE0: //E0 LOOPNZ Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        putreg16(regcx, getreg16(regcx) - 1);
        if ((getreg16(regcx)) && !zf) ip = ip + temp16;
        break;
      case 0xE1: //E1 LOOPZ Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        putreg16(regcx, (getreg16(regcx)) - 1);
        if ((getreg16(regcx)) && (zf == 1)) ip = ip + temp16;
        break;
      case 0xE2: //E2 LOOP Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        putreg16(regcx, (getreg16(regcx)) - 1);
        if (getreg16(regcx)) ip = ip + temp16;
        break;
      case 0xE3: //E3 JCXZ Jb
        temp16 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        if (!(getreg16(regcx))) ip = ip + temp16;
        break;
      case 0xE4: //E4 IN regs.byteregs[regal] Ib
        oper1b = getmem8(segregs[regcs], ip);
        StepIP(1);
        regs.byteregs[regal] = portin(oper1b);
        break;
      case 0xE5: //E5 IN eAX Ib
        oper1b = getmem8(segregs[regcs], ip);
        StepIP(1);
        putreg16(regax, portin(oper1b));
        break;
      case 0xE6: //E6 OUT Ib regs.byteregs[regal]
        oper1b = getmem8(segregs[regcs], ip);
        StepIP(1);
        portout16 = 0;
        portout(oper1b, regs.byteregs[regal]);
        break;
      case 0xE7: //E7 OUT Ib eAX
        oper1b = getmem8(segregs[regcs], ip);
        StepIP(1);
        portout16 = 1;
        portout(oper1b, (getreg16(regax)));
        break;
      case 0xE8: //E8 CALL Jv
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        push(ip);
        ip = ip + oper1;
        break;
      case 0xE9: //E9 JMP Jv
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        ip = ip + oper1;
        break;
      case 0xEA: //EA JMP Ap
        oper1 = getmem16(segregs[regcs], ip); StepIP(2);
        oper2 = getmem16(segregs[regcs], ip);
        ip = oper1; segregs[regcs] = oper2;
        break;
      case 0xEB: //EB JMP Jb
        oper1 = signext(getmem8(segregs[regcs], ip)); StepIP(1);
        ip = ip + oper1;
        break;
      case 0xEC: //EC IN regs.byteregs[regal] regdx
        oper1 = (getreg16(regdx));
        regs.byteregs[regal] = portin(oper1);
        break;
      case 0xED: //ED IN eAX regdx
        oper1 = (getreg16(regdx));
        putreg16(regax, portin(oper1));
        break;
      case 0xEE: //EE OUT regdx regs.byteregs[regal]
        oper1 = (getreg16(regdx));
        portout16 = 0;
        portout(oper1, regs.byteregs[regal]);
        break;
      case 0xEF: //EF OUT regdx eAX
        oper1 = (getreg16(regdx));
        portout16 = 1;
        portout(oper1, (getreg16(regax)));
        break;
      case 0xF0: //F0 LOCK
      case 0xF4: //F4 HLT
        break;
      case 0xF5: //F5 CMC
        if (!cf) cf = 1; else cf = 0;
        break;
      case 0xF6: //F6 GRP3a Eb
        modregrm();
        oper1b = readrm8(rm);
        op_grp3_8();
        if ((reg > 1) && (reg < 4))     writerm8(rm, res8);

        break;
      case 0xF7: //F7 GRP3b Ev
        modregrm();
        oper1 = readrm16(rm);
        op_grp3_16();
        if ((reg > 1) && (reg < 4)) writerm16(rm, res16);
        break;
      case 0xF8: //F8 CLC
        cf = 0;
        break;
      case 0xF9: //F9 STC
        cf = 1;
        break;
      case 0xFA: //FA CLI
        ifl = 0;
        break;
      case 0xFB: //FB STI
        ifl = 1;
        break;
      case 0xFC: //FC CLD
        df = 0;
        break;
      case 0xFD: //FD STD
        df = 1;
        break;
      case 0xFE: //FE GRP4 Eb
        modregrm();
        oper1b = readrm8(rm); oper2b = 1;
        if (!reg) {
          tempcf = cf;
          res8 = oper1b + oper2b;
          flag_add8(oper1b, oper2b);
          cf = tempcf; writerm8(rm, res8);
        } else {
          tempcf = cf;
          res8 = oper1b - oper2b;
          flag_sub8(oper1b, oper2b);
          cf = tempcf; writerm8(rm, res8);
        }
        break;
      case 0xFF: //FF GRP5 Ev
        modregrm();
        oper1 = readrm16(rm);
        op_grp5();
        break;
      default:
        intcall86(6); //trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs
        if (verbose) {
          //if (opcode==0xF) sprintf(msg, "Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
          //else
          //sprintf(msg, "Illegal opcode: %02X %02X @ %04X:%04X\n", opcode, getmem8(savecs, saveip + 1), savecs, saveip);
        }

        break;
    }
#ifdef PROFILING
    endmicros = micros();
    if (instrtime[opcode]) {
      instrtime[opcode] = (instrtime[opcode] + (endmicros - startmicros)) >> 1;
    } else {
      instrtime[opcode] = endmicros - startmicros;
    }
#endif
    //if (!running) return;
  }
}

void testmem() {

}

