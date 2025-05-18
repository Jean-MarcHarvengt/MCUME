#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mmu.h"

// To see calls to unimplemented opcodes, define this:
//#define VERBOSE_CPU_ERRORS

// Macros to set negative and zero flags based on param, X, Y, whatever
#define SETNZ  { FLAG(F_N, param & 0x80); FLAG(F_Z, !param); }
#define SETNZX { FLAG(F_N, x & 0x80);     FLAG(F_Z, !x); }
#define SETNZY { FLAG(F_N, y & 0x80);     FLAG(F_Z, !y); }
#define SETNZA { FLAG(F_N, a & 0x80);     FLAG(F_Z, !a); }

#define FLAG(bit, condition) { if (condition) {flags |= bit;} else {flags &= ~bit;} }

#define writemem(addr, val) mmu->write(addr, val)
#define readmem(addr) mmu->read(addr)

enum optype { 
  O_ILLEGAL,
  O_ADC,
  O_AND,
  O_ASL,
  O_ASL_ACC,
  O_BCC,
  O_BCS,
  O_BEQ,
  O_BIT,
  O_BMI,
  O_BNE,
  O_BPL,
  O_BRA,
  O_BRK,
  O_BVC,
  O_BVS,
  O_CLC,
  O_CLD,
  O_CLI,
  O_CLV,
  O_CMP,
  O_CPX,
  O_CPY,
  O_DEC,
  O_DEC_ACC,
  O_DEX,
  O_DEY,
  O_EOR,
  O_INC,
  O_INC_ACC,
  O_INX,
  O_INY,
  O_JMP,
  O_JSR,
  O_LDA,
  O_LDX,
  O_LDY,
  O_LSR,
  O_LSR_ACC,
  O_NOP,
  O_ORA,
  O_PHA,
  O_PHP,
  O_PHX,
  O_PHY,
  O_PLA,
  O_PLP,
  O_PLX,
  O_PLY,
  O_ROL,
  O_ROL_ACC,
  O_ROR,
  O_ROR_ACC,
  O_RTI,
  O_RTS,
  O_SBC,
  O_SEC,
  O_SED,
  O_SEI,
  O_STA,
  O_STX,
  O_STY,
  O_STZ,
  O_TAX,
  O_TAY,
  O_TRB,
  O_TSB,
  O_TSX,
  O_TXA,
  O_TXS,
  O_TYA,

  // and the "illegal" opcodes (those that don't officially exist for
  // the 65c02, but have repeatable results)
  O_DCP
};

// accumulator mode is implied
#define A_ACC A_IMP

enum addrmode {
  A_ILLEGAL,
  A_IMM,
  A_ABS,
  A_ZER,
  A_IMP,
  A_REL,
  A_ABI,
  A_ZEX,
  A_ZEY,
  A_ZIND,
  A_ABX,
  A_ABY,
  A_INX,
  A_INY
};

typedef struct {
  optype op;
  addrmode mode;
  uint8_t cycles;
} optype_t;

static const optype_t opcodes[256] = {
  { O_BRK,     A_IMP,     7 }, // 0x00
  { O_ORA    , A_INX    , 6 }, // 0x01 [2]  i.e. "ORA ($44,X)"
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x02
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x03
  { O_TSB    , A_ZER    , 5 }, // 0x04 [2]
  { O_ORA    , A_ZER    , 3 }, // 0x05 [2]  i.e. "ORA $44"
  { O_ASL    , A_ZER    , 5 }, // 0x06 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x07
  { O_PHP    , A_IMP    , 3 }, // 0x08
  { O_ORA    , A_IMM    , 2 }, // 0x09      i.e. "ORA #$44"
  { O_ASL_ACC, A_ACC    , 2 }, // 0x0A
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x0B
  { O_TSB    , A_ABS    , 6 }, // 0x0C
  { O_ORA    , A_ABS    , 4 }, // 0x0D      i.e. "ORA $4400"
  { O_ASL    , A_ABS    , 6 }, // 0x0E
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x0F
  { O_BPL    , A_REL    , 2 }, // 0x10 [8]
  { O_ORA    , A_INY    , 5 }, // 0x11 [2,3] i.e. "ORA ($44),Y"
  { O_ORA    , A_ZIND   , 5 }, // 0x12 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x13
  { O_TRB    , A_ZER    , 5 }, // 0x14 [2]
  { O_ORA    , A_ZEX    , 4 }, // 0x15 [2]   i.e. "ORA $44,X"
  { O_ASL    , A_ZEX    , 6 }, // 0x16 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x17
  { O_CLC    , A_IMP    , 2 }, // 0x18
  { O_ORA    , A_ABY    , 4 }, // 0x19 [3]   i.e. "ORA $4400,Y"
  { O_INC_ACC, A_ACC    , 2 }, // 0x1A
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x1B
  { O_TRB    , A_ABS    , 6 }, // 0x1C [3]
  { O_ORA    , A_ABX    , 4 }, // 0x1D [3]   i.e. "ORA $4400,X"
  { O_ASL    , A_ABX    , 6 }, // 0x1E [6]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x1F
  { O_JSR    , A_ABS    , 6 }, // 0x20
  { O_AND    , A_INX    , 6 }, // 0x21 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x22
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x23
  { O_BIT    , A_ZER    , 3 }, // 0x24 [2]
  { O_AND    , A_ZER    , 3 }, // 0x25 [2]
  { O_ROL    , A_ZER    , 5 }, // 0x26 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x27
  { O_PLP    , A_IMP    , 4 }, // 0x28
  { O_AND    , A_IMM    , 2 }, // 0x29 
  { O_ROL_ACC, A_ACC    , 2 }, // 0x2A
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x2B
  { O_BIT    , A_ABS    , 4 }, // 0x2C 
  { O_AND    , A_ABS    , 4 }, // 0x2D 
  { O_ROL    , A_ABS    , 6 }, // 0x2E
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x2F
  { O_BMI    , A_REL    , 2 }, // 0x30 [8]
  { O_AND    , A_INY    , 5 }, // 0x31 [2,3]
  { O_AND    , A_ZIND   , 5 }, // 0x32 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x33
  { O_BIT    , A_ZEX    , 4 }, // 0x34 [2]
  { O_AND    , A_ZEX    , 4 }, // 0x35 [2]
  { O_ROL    , A_ZEX    , 6 }, // 0x36 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x37
  { O_SEC    , A_IMP    , 2 }, // 0x38
  { O_AND    , A_ABY    , 4 }, // 0x39 [3]
  { O_DEC_ACC, A_ACC    , 2 }, // 0x3A [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x3B
  { O_BIT    , A_ABX    , 4 }, // 0x3C [3]
  { O_AND    , A_ABX    , 4 }, // 0x3D [3]
  { O_ROL    , A_ABX    , 6 }, // 0x3E [6]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x3F
  { O_RTI    , A_IMP    , 6 }, // 0x40
  { O_EOR    , A_INX    , 6 }, // 0x41 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x42
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x43
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x44
  { O_EOR    , A_ZER    , 3 }, // 0x45 [2]
  { O_LSR    , A_ZER    , 5 }, // 0x46 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x47
  { O_PHA    , A_IMP    , 3 }, // 0x48 
  { O_EOR    , A_IMM    , 2 }, // 0x49 
  { O_LSR_ACC, A_ACC    , 2 }, // 0x4A
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x4B
  { O_JMP    , A_ABS    , 3 }, // 0x4C
  { O_EOR    , A_ABS    , 4 }, // 0x4D 
  { O_LSR    , A_ABS    , 6 }, // 0x4E
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x4F
  { O_BVC    , A_REL    , 2 }, // 0x50 [8]
  { O_EOR    , A_INY    , 5 }, // 0x51 [2,3]
  { O_EOR    , A_ZIND   , 5 }, // 0x52 
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x53
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x54
  { O_EOR    , A_ZEX    , 4 }, // 0x55 [2]
  { O_LSR    , A_ZEX    , 6 }, // 0x56 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x57
  { O_CLI    , A_IMP    , 2 }, // 0x58
  { O_EOR    , A_ABY    , 4 }, // 0x59 [3]
  { O_PHY    , A_IMP    , 3 }, // 0x5A
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x5B
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x5C
  { O_EOR    , A_ABX    , 4 }, // 0x5D [3]
  { O_LSR    , A_ABX    , 6 }, // 0x5E [6]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x5F
  { O_RTS    , A_IMP    , 6 }, // 0x60
  { O_ADC    , A_INX    , 6 }, // 0x61 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x62
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x63
  { O_STZ    , A_ZER    , 3 }, // 0x64 [2]
  { O_ADC    , A_ZER    , 3 }, // 0x65 [2]
  { O_ROR    , A_ZER    , 5 }, // 0x66 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x67
  { O_PLA    , A_IMP    , 4 }, // 0x68 
  { O_ADC    , A_IMM    , 2 }, // 0x69
  { O_ROR_ACC, A_ACC    , 2 }, // 0x6A [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x6B
  { O_JMP    , A_ABI    , 6 }, // 0x6C
  { O_ADC    , A_ABS    , 4 }, // 0x6D
  { O_ROR    , A_ABS    , 6 }, // 0x6E
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x6F
  { O_BVS    , A_REL    , 2 }, // 0x70 [8]
  { O_ADC    , A_INY    , 5 }, // 0x71 [2,3]
  { O_ADC    , A_ZIND   , 5 }, // 0x72 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x73
  { O_STZ    , A_ZEX    , 4 }, // 0x74 [2]
  { O_ADC    , A_ZEX    , 4 }, // 0x75 [2]
  { O_ROR    , A_ZEX    , 6 }, // 0x76 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x77
  { O_SEI    , A_IMP    , 2 }, // 0x78
  { O_ADC    , A_ABY    , 4 }, // 0x79 [3]
  { O_PLY    , A_IMP    , 4 }, // 0x7A
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x7B
  { O_JMP    , A_ABX    , 6 }, // 0x7C
  { O_ADC    , A_ABX    , 4 }, // 0x7D [3]
  { O_ROR    , A_ABX    , 6 }, // 0x7E [6]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x7F
  { O_BRA    , A_REL    , 3 }, // 0x80 [8]
  { O_STA    , A_INX    , 6 }, // 0x81 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x82
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x83
  { O_STY    , A_ZER    , 3 }, // 0x84 [2]
  { O_STA    , A_ZER    , 3 }, // 0x85 [2]
  { O_STX    , A_ZER    , 3 }, // 0x86 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x87
  { O_DEY    , A_IMP    , 2 }, // 0x88
  { O_BIT    , A_IMM    , 2 }, // 0x89 
  { O_TXA    , A_IMP    , 2 }, // 0x8A
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x8B
  { O_STY    , A_ABS    , 4 }, // 0x8C
  { O_STA    , A_ABS    , 4 }, // 0x8D 
  { O_STX    , A_ABS    , 4 }, // 0x8E
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x8F
  { O_BCC    , A_REL    , 2 }, // 0x90 [8]
  { O_STA    , A_INY    , 6 }, // 0x91 [2]
  { O_STA    , A_ZIND   , 5 }, // 0x92 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x93
  { O_STY    , A_ZEX    , 4 }, // 0x94 [2]
  { O_STA    , A_ZEX    , 4 }, // 0x95 [2]
  { O_STX    , A_ZEY    , 4 }, // 0x96 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x97
  { O_TYA    , A_IMP    , 2 }, // 0x98
  { O_STA    , A_ABY    , 5 }, // 0x99 
  { O_TXS    , A_IMP    , 2 }, // 0x9A [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x9B
  { O_STZ    , A_ABS    , 4 }, // 0x9C
  { O_STA    , A_ABX    , 5 }, // 0x9D 
  { O_STZ    , A_ABX    , 5 }, // 0x9E 
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0x9F
  { O_LDY    , A_IMM    , 2 }, // 0xA0
  { O_LDA    , A_INX    , 6 }, // 0xA1 [2]
  { O_LDX    , A_IMM    , 2 }, // 0xA2
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xA3
  { O_LDY    , A_ZER    , 3 }, // 0xA4 [2]
  { O_LDA    , A_ZER    , 3 }, // 0xA5 [2]
  { O_LDX    , A_ZER    , 3 }, // 0xA6 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xA7
  { O_TAY    , A_IMP    , 2 }, // 0xA8
  { O_LDA    , A_IMM    , 2 }, // 0xA9 
  { O_TAX    , A_IMP    , 2 }, // 0xAA
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xAB
  { O_LDY    , A_ABS    , 4 }, // 0xAC
  { O_LDA    , A_ABS    , 4 }, // 0xAD 
  { O_LDX    , A_ABS    , 4 }, // 0xAE
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xAF
  { O_BCS    , A_REL    , 2 }, // 0xB0 [8]
  { O_LDA    , A_INY    , 5 }, // 0xB1 [2,3]
  { O_LDA    , A_ZIND   , 5 }, // 0xB2 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xB3
  { O_LDY    , A_ZEX    , 4 }, // 0xB4 [2]
  { O_LDA    , A_ZEX    , 4 }, // 0xB5 [2]
  { O_LDX    , A_ZEY    , 4 }, // 0xB6 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xB7
  { O_CLV    , A_IMP    , 2 }, // 0xB8
  { O_LDA    , A_ABY    , 4 }, // 0xB9 [3]
  { O_TSX    , A_IMP    , 2 }, // 0xBA
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xBB
  { O_LDY    , A_ABX    , 4 }, // 0xBC [3]
  { O_LDA    , A_ABX    , 4 }, // 0xBD [3]
  { O_LDX    , A_ABY    , 4 }, // 0xBE [3]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xBF
  { O_CPY    , A_IMM    , 2 }, // 0xC0
  { O_CMP    , A_INX    , 6 }, // 0xC1 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xC2
  { O_DCP,     A_INX,     2 }, // 0xC3 -- fixme -- cycle count of this illegal instruction?
  { O_CPY    , A_ZER    , 3 }, // 0xC4 [2]
  { O_CMP    , A_ZER    , 3 }, // 0xC5 [2]
  { O_DEC    , A_ZER    , 5 }, // 0xC6 [2]
  { O_DCP    , A_ZER    , 2 }, // 0xC7 -- fixme -- cycle count of this illegal instruction?
  { O_INY    , A_IMP    , 2 }, // 0xC8
  { O_CMP    , A_IMM    , 2 }, // 0xC9 
  { O_DEX    , A_IMP    , 2 }, // 0xCA
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xCB
  { O_CPY    , A_ABS    , 4 }, // 0xCC
  { O_CMP    , A_ABS    , 4 }, // 0xCD
  { O_DEC    , A_ABS    , 6 }, // 0xCE
  { O_DCP    , A_ABS    , 2 }, // 0xCF -- fixme -- cycle count of this illegal instruction?
  { O_BNE    , A_REL    , 2 }, // 0xD0 [8]
  { O_CMP    , A_INY    , 5 }, // 0xD1 [2,3]
  { O_CMP    , A_ZIND   , 5 }, // 0xD2 [2]
  { O_DCP    , A_INY    , 2 }, // 0xD3 -- fixme -- cycle count of this illegal instruction?
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xD4
  { O_CMP    , A_ZEX    , 4 }, // 0xD5 [2]
  { O_DEC    , A_ZEX    , 6 }, // 0xD6 [2]
  { O_DCP    , A_ZEX    , 2 }, // 0xD7 -- fixme -- cycle count of this illegal instruction?
  { O_CLD    , A_IMP    , 2 }, // 0xD8
  { O_CMP    , A_ABY    , 4 }, // 0xD9 [3]
  { O_PHX    , A_IMP    , 3 }, // 0xDA
  { O_DCP    , A_ABY    , 2 }, // 0xDB -- fixme -- cycle count of this illegal instruction?
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xDC
  { O_CMP    , A_ABX    , 4 }, // 0xDD [3]
  { O_DEC    , A_ABX    , 6 }, // 0xDE [6]
  { O_DCP    , A_ABX    , 2 }, // 0xDF -- fixme -- cycle count of this illegal instruction?
  { O_CPX    , A_IMM    , 2 }, // 0xE0
  { O_SBC    , A_INX    , 6 }, // 0xE1 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xE2
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xE3
  { O_CPX    , A_ZER    , 3 }, // 0xE4 [2]
  { O_SBC    , A_ZER    , 3 }, // 0xE5 [2]
  { O_INC    , A_ZER    , 5 }, // 0xE6 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xE7
  { O_INX    , A_IMP    , 2 }, // 0xE8
  { O_SBC    , A_IMM    , 2 }, // 0xE9
  { O_NOP    , A_IMP    , 2 }, // 0xEA
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xEB
  { O_CPX    , A_ABS    , 4 }, // 0xEC
  { O_SBC    , A_ABS    , 4 }, // 0xED
  { O_INC    , A_ABS    , 6 }, // 0xEE
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xEF
  { O_BEQ    , A_REL    , 2 }, // 0xF0 [8]
  { O_SBC    , A_INY    , 5 }, // 0xF1 [2,3]
  { O_SBC    , A_ZIND   , 5 }, // 0xF2 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xF3
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xF4
  { O_SBC    , A_ZEX    , 4 }, // 0xF5 [2]
  { O_INC    , A_ZEX    , 6 }, // 0xF6 [2]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xF7
  { O_SED    , A_IMP    , 2 }, // 0xF8
  { O_SBC    , A_ABY    , 4 }, // 0xF9 [3]
  { O_PLX    , A_IMP    , 4 }, // 0xFA
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xFB
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xFC
  { O_SBC    , A_ABX    , 4 }, // 0xFD [3]
  { O_INC    , A_ABX    , 6 }, // 0xFE [6]
  { O_ILLEGAL, A_ILLEGAL, 2 }, // 0xFF
};
/* cycle count footnotes:
2: Add 1 cycle if low byte of Direct Page Register is non-zero 
3 Add 1 cycle if adding index crosses a page boundary 
6 Add 1 cycle if 65C02 and page boundary crossed 
8 Add 1 cycle if branch taken crosses page boundary on 6502, 65C02, or 65816's 6502 emulation mode (e=1)
*/

// Initialization sequence comes from http://www.pagetable.com/?p=410
// (the Visual 6502 project).

Cpu::Cpu()
{
  mmu = NULL;
  Reset();
}

Cpu::~Cpu()
{
  mmu = NULL;
}

void Cpu::Reset()
{
  a = 0;
  x = 0;
  y = 0;
  flags = F_Z | F_UNK; // FIXME: is that F_UNK flag right here?
  irqPending = false;

  if (mmu) {
    pc = readmem(0xFFFC) | (readmem(0xFFFD) << 8);
  } else {
    pc = 0x0000;
  }

  sp = 0xFD;

  cycles = 6; // according to the datasheet, the reset routine takes 6 clock cycles             
}

void Cpu::nmi()
{
  flags &= ~F_B; // clear break flag

  pushS16(pc);
  pushS8(flags);
  flags |= 0x20; // FIXME: what flag is this?

  pc = readmem(0xFFFA) | (readmem(0xFFFB) << 8);
  cycles += 2;
}

void Cpu::rst()
{
  // On reset, the CPU initializes the instruction register (IR) to 0.
  // That's BRK. So we spend 3 cycles breaking (0, 1, 2)?

  // Then we would pushS16(pc), but the Reset logic has the R/W line set
  // to R, so the value is discarded. Twice. That's cycles 3 and 4.
  cycles += 2; sp -= 2;

  // Then we would pushS8(flags) the status register, but again, R/W
  // is still R.  That's cycle 5.
  cycles++; sp--;

  // Note that in the real CPU, the SP register doesn't get updated
  // until now - pushS16/pushS8 wouldn't have updated it. Not sure
  // that's ever important to me.

  // In cycle 6, we read 0xFFFC; in cycle 7, we read 0xFFFD. Setting the 
  // PC takes no time.
  pc = readmem(0xFFFC) | (readmem(0xFFFD) << 8);
  cycles+=2;
  // And now we're going to go fetch and operate on the first instruction.
}

void Cpu::brk()
{
  pc++;
  pushS16(pc);
  pushS8(flags | F_B); // FIXME: does this have the missing status bit set?
  // FIXME: is setting the BRK bit a 65C02-specific thing? I think it is

  FLAG(F_D, 0); // 65c02...
  FLAG(F_I, 1);

  pc = readmem(0xFFFE) | (uint16_t)(readmem(0xFFFF) << 8);
  cycles += 2;
}

void Cpu::irq()
{
  // If interrupts are disabled, then do nothing
  if (flags & F_I)
    return;

  pushS16(pc);
  flags &= ~F_B; // clear BRK flag
  pushS8(flags);
  flags |= F_I; // set interrupt flag

  pc = readmem(0xFFFE) | (readmem(0xFFFF) << 8);
  cycles += 2;
}

uint8_t Cpu::Run(uint8_t numSteps)
{
  uint8_t runtime = 0;
  while (runtime < numSteps) {
    runtime += step();
  }
  return runtime;
}

uint8_t Cpu::step()
{
  if (irqPending) {
    irqPending = false;
//printf("irql\n");
    irq();
  }

  uint8_t m = readmem(pc++);


  optype_t opcode = opcodes[m];
//printf("op: 0x%02x\n", opcode);
  if (opcode.op == O_ILLEGAL || opcode.mode == A_ILLEGAL) {
    m = 0xEA; // substitute O_NOP...
//printf("illegal\n");
#ifdef VERBOSE_CPU_ERRORS
    fprintf(stderr, "** Illegal opcode $%.2X at address $%.4x\n",
	    m,
	    pc-1);
#endif
  }

  // Look at the addressing mode to determine the parameter
  uint16_t param = 0;
  switch (opcode.mode) {
  case A_ILLEGAL:
  default:
    // This should never happen; we're substituting NOP.
    // treat these as IMPLIED
    break;
  case A_IMP:
    // implied: nothing to do. These have a parameter that refers to a
    // specific register or particular action to a register
    break;
  case A_IMM:
    // immediate: the next byte at PC
    param = pc++;
    break;
  case A_ABS:
    // absolute: the address referred to in the next 2 bytes at PC
    param = readmem(pc) | (readmem(pc+1) << 8);
    pc += 2;
    break;
  case A_ABX:
    // absolute indexed, based on X
    param = (readmem(pc) | (readmem(pc+1) << 8)) + x;
    pc += 2;
    break;
  case A_REL:
    // relative
    param = (int8_t)readmem(pc++);
    param += pc; // do this in 2 steps b/c readmem(pc++) modifies PC
    break;
  case A_ABI:
    // absolute indirect
    {
      uint16_t addr = readmem(pc) | (readmem(pc+1) << 8);
      pc += 2;

      param = readmem(addr) | (readmem(addr+1) << 8);
    }
    break;
  case A_ZEX:
    // zero-page, indexed by X -- i.e. "ORA $44,X"
    param = (readmem(pc++) + x) & 0xFF;
    break;
  case A_ZER:
    // zero-page
    param = readmem(pc++);
    break;
  case A_ZEY:
    // zero-page, indexed by Y
    param = (readmem(pc++) + y) & 0xFF;
    break;
  case A_ABY:
    // absolute indexed, based on Y
    param = (readmem(pc) | (readmem(pc+1) << 8)) + y;
    pc += 2;
    break;
  case A_INY:
    // indirect indexed Y - refers to zero-page memory by one byte
    {
      uint8_t zpL = mmu->read(pc++);
      uint8_t zpH = zpL+1;
      param = ( mmu->read(zpL) | (mmu->read(zpH) << 8) ) + y;
    }
    break;
  case A_INX:
    {
      uint8_t zpL = mmu->read(pc++) + x;
      uint8_t zpH = zpL+1;
      param = ( mmu->read(zpL) | (mmu->read(zpH) << 8) );
    }
    break;
  }

  // initialize a counter for the number of cycles this run
  // (many opcodes have variable length)
  uint8_t cyclesThisStep = opcode.cycles;

  // Then look at the opcode type to perform the operations necessary
  switch (opcode.op) {
  case O_ILLEGAL:
  default:
    // This should never happen; we're trapping O_ILLEGAL above and substituting O_NOP.
    break;
  case O_CLD:
    FLAG(F_D, 0);
    break;
  case O_LDX:
    x = readmem(param);
    SETNZX;
    break;
  case O_TXS:
    sp = x;
    break;
  case O_LDA:
    a = readmem(param);
    SETNZA;
    break;
  case O_STA:
    writemem(param, a);
    break;
  case O_JSR:
    pushS16(pc-1);
    pc = param;
    break;
  case O_RTS:
    pc = popS16()+1;
    break;
  case O_PHA:
    pushS8(a);
    break;
  case O_INX:
    x++;
    SETNZX;
    break;
  case O_BNE:
    if (!(flags & F_Z)) {
      pc = param;
      cycles++;
    }
    break;
  case O_BVS:
    if (flags & F_V) {
      pc = param;
      cycles++;
    }
    break;
  case O_BRK:
    brk();
    break;
  case O_PHP:
    pushS8(flags | F_B);
    break;
  case O_DEY:
    y--;
    SETNZY;
    break;
  case O_CMP:
    {
      uint16_t tmp = a - readmem(param);
      FLAG(F_C, tmp < 0x100);
      FLAG(F_Z, !(tmp & 0xFF));
      FLAG(F_N, tmp & 0x80);
    }
    break;
  case O_BEQ:
    if (flags & F_Z) {
      pc = param;
      cycles++;
    }
    break;
  case O_TXA:
    a = x;
    SETNZA;
    break;
  case O_TYA:
    a = y;
    SETNZA;
    break;
  case O_ASL_ACC:
    FLAG(F_C, a & 0x80);
    a <<= 1;
    SETNZA;
    break;
  case O_ORA:
    a |= readmem(param);
    SETNZA;
    break;
  case O_JMP:
    pc = param;
    break;
  case O_DEX:
    x--;
    SETNZX;
    break;
  case O_LDY:
    y = readmem(param);
    SETNZY;
    break;
  case O_NOP:
    break;
  case O_TAX:
    x = a;
    SETNZA;
    break;
  case O_BPL:
    if (!(flags & F_N)) {
      pc = param;
      cycles++;
    }    
    break;
  case O_CLC:
    FLAG(F_C, 0);
    break;
  case O_EOR:
    a ^= readmem(param);
    SETNZA;
    break;
  case O_CPY:
    {
      uint16_t tmp = y - readmem(param);
      FLAG(F_C, tmp < 0x100);
      FLAG(F_Z, !(tmp & 0xFF));
      FLAG(F_N, tmp & 0x80);
    }
    break;
  case O_TSX:
    x = sp;
    SETNZX;
    break;
  case O_LSR_ACC:
    FLAG(F_C, a & 0x01);
    a >>= 1;
    SETNZA;
    break;
  case O_BCC:
    if (!(flags & F_C)) {
      pc = param;
      cycles++;
    }
    break;
  case O_PLA:
    a = popS8();
    SETNZA;
    break;
  case O_AND:
    a &= readmem(param);
    SETNZA;
    break;
  case O_CPX:
    {
      uint16_t tmp = x - readmem(param);
      FLAG(F_C, tmp < 0x100);
      FLAG(F_Z, !(tmp & 0xFF));
      FLAG(F_N, tmp & 0x80);
    }
    break;
  case O_BCS:
    if (flags & F_C) {
      pc = param;
      cycles++;
    }
    break;
  case O_BMI:
    if (flags & F_N) {
      pc = param;
      cycles++;
    }
    break;
  case O_TAY:
    y = a;
    SETNZY;
    break;
  case O_PLP:
    flags = popS8();
    FLAG(F_UNK, 1); // ??
    break;
  case O_BVC:
    if (!(flags & F_V)) {
      pc = param;
      cycles++;
    }
    break;
  case O_INY:
    y++;
    SETNZY;
    break;
  case O_STX:
    writemem(param, x);
    break;
  case O_RTI:
    flags = popS8();
    pc = popS16();
    break;
  case O_SEC:
    FLAG(F_C, 1);
    break;
  case O_CLI:
    FLAG(F_I, 0);
    break;
  case O_SEI:
    FLAG(F_I, 1);
    break;
  case O_SED:
    FLAG(F_D, 1);
    break;
  case O_CLV:
    FLAG(F_V, 0);
    break;
  case O_STY:
    writemem(param, y);
    break;
  case O_BIT:
    { 
      uint8_t m = readmem(param);
      uint8_t v = a & m;
      FLAG(F_N, (v & 0x80) | (m & 0x80));
      FLAG(F_Z, v == 0);
      FLAG(F_V, m & 0x40);
      //      status = (status & 0x3F) | (uint8_t)(m & 0xC0);
    }
    break;
  case O_ROL_ACC:
    {
      uint8_t v = a << 1;
      if (flags & F_C)
	v |= 0x01;
      FLAG(F_C, a & 0x80);
      a = v;
      SETNZA;
    }
    break;
  case O_ROR_ACC:
    {
      uint8_t v = a >> 1;
      if (flags & F_C)
	v |= 0x80;
      FLAG(F_C, a & 0x01);
      a = v;
      SETNZA;
    }
    break;
  case O_ASL:
    { 
      uint8_t v = readmem(param);
      FLAG(F_C, v & 0x80);
      v <<= 1;

      FLAG(F_N, v & 0x80);
      FLAG(F_Z, v == 0);
      writemem(param, v);
    }
    break;
  case O_LSR:
    {
      uint8_t v = readmem(param);
      FLAG(F_C, v & 0x01);
      v >>= 1;
      FLAG(F_N, 0);
      FLAG(F_Z, v == 0);
      writemem(param, v);
    }
    break;
  case O_ROL:
    {
      uint8_t m = readmem(param);
      uint8_t v = m << 1;
      if (flags & F_C)
	v |= 0x01;
      FLAG(F_C, m & 0x80);
      FLAG(F_N, v & 0x80);
      FLAG(F_Z, v == 0);
      writemem(param, v);
    }
    break;
  case O_ROR:
    {
      uint8_t m = readmem(param);
      uint8_t v = m >> 1;
      if (flags & F_C)
	v |= 0x80;
      FLAG(F_C, m & 0x01);
      FLAG(F_N, v & 0x80);
      FLAG(F_Z, v == 0);
      writemem(param, v);
    }
    break;
  case O_INC:
    {
      uint8_t v = mmu->read(param) + 1;
      FLAG(F_N, v & 0x80);
      FLAG(F_Z, v == 0);
      writemem(param, v);
    }
    break;
  case O_DEC:
    {
      uint8_t v = mmu->read(param) - 1;
      FLAG(F_N, v & 0x80);
      FLAG(F_Z, v == 0);
      writemem(param, v);
    }
    break;
  case O_DCP:
    // not a real opcode; one of the 65c02 side-effect "illegal" opcodes
    {
      uint8_t v = mmu->read(param) - 1;
      FLAG(F_N, v & 0x80);
      FLAG(F_Z, v == 0);
      writemem(param, v);

      uint16_t tmp = a - v;
      FLAG(F_C, tmp < 0x100);
      FLAG(F_Z, !(tmp & 0xFF));
      FLAG(F_N, tmp & 0x80);
    }
    break;
  case O_SBC:
    {
      uint8_t memTemp = readmem(param) ^ 0xFF;

      int16_t c;
      uint8_t v;
      if (flags & F_D) {
	cycles++;
	c = (a & 0x0F) + (memTemp & 0x0F) + (flags & F_C);
	if (c < 0x10)
	  c = (c - 0x06) & 0x0f;
	c += (a & 0xf0) + (memTemp & 0xf0);
	v = (c >> 1) ^ c;
	if (c < 0x100)
	  c = (c + 0xa0) & 0xff;
      } else {
	c = a + memTemp + (flags & F_C);
	v = (c ^ a) & 0x80;
      }

      if (((a ^ memTemp) & 0x80) != 0) {
	v = 0;
      }

      FLAG(F_C, c > 0xFF);
      FLAG(F_V, v);

      a = c & 0xFF;
      SETNZA;
    }
    break;

  case O_ADC:
    {
      uint8_t memTemp = readmem(param);

      int16_t c;
      uint8_t v;
      if (flags & F_D) {
	cycles++;
	c = (a & 0x0F) + (memTemp & 0x0F) + (flags & F_C);
	if (c > 0x09)
	  c = (c - 0x0a) | 0x10;
	c += (a & 0xf0) + (memTemp & 0xf0);
	v = (c >> 1) ^ c;
	if (c > 0x99)
	  c += 0x60;
      } else {
	c = a + memTemp + (flags & F_C);
	v = (c ^ a) & 0x80;
      }

      if (((a ^ memTemp) & 0x80) != 0) {
	v = 0;
      }

      FLAG(F_C, c > 0xFF);
      FLAG(F_V, v);

      a = c & 0xFF;
      SETNZA;
    }
    break;
  case O_PHX:
    pushS8(x);
    break;
  case O_PHY:
    pushS8(y);
    break;
  case O_PLY:
    y = popS8();
    SETNZY;
    break;
  case O_PLX:
    x = popS8();
    SETNZX;
    break;
  case O_BRA:
    pc = param;
    break;
    // ***
  }

  // And finally update our executed cycle count with the runtime
  cycles += cyclesThisStep;

  return cyclesThisStep;
}

uint8_t Cpu::X()
{
  return x;
}

uint8_t Cpu::Y()
{
  return y;
}

uint8_t Cpu::A()
{
  return a;
}

uint16_t Cpu::PC()
{
  return pc;
}

uint8_t Cpu::SP()
{
  return sp;
}

uint8_t Cpu::P()
{
  return flags;
}


void Cpu::pushS8(uint8_t b)
{
  writemem(0x100 + sp, b);
  sp--; // may underflow (does this at startup)
}

void Cpu::pushS16(uint16_t w)
{
  pushS8((w >> 8) & 0xFF); 
  pushS8(w & 0xFF);
}

uint8_t Cpu::popS8()
{
  sp++; // may overflow
  return readmem(0x100 + sp);
}

uint16_t Cpu::popS16()
{
  uint8_t lsb = popS8();
  uint8_t msb = popS8();

  return (msb << 8) | lsb;
}

void Cpu::stageIRQ()
{
  irqPending = true;
}
