/* MoarNES source - NES.c

   This open-source software is (c)2010-2013 Mike Chambers, and is released
   under the terms of the GNU GPL v2 license.

   This file contains the code to simulate the internal NES architecture,
   such as memory mapping, redirecting read/writes to I/O registers, etc.
*/

#include <stdio.h>
#include "moarnes.h"
#include "emuapi.h"

const uint32_t nesRgb[] =
{ 0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
  0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
  0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
  0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
  0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
  0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
  0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
  0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000 };

/*
   Nintendo Entertainment System: CPU memory map

   $0000-$07FF = 2 KB internal RAM
   $0800-$1FFF = Mirrors of $0000-$07FF
   $2000-$2007 = PPU registers
   $2008-$3FFF = Mirrors of $2000-$2007
   $4000-$4017 = APU and I/O registers
   $4018-$FFFF = Cartridge PRG ROM, cartridge PRG RAM, and mapper registers

   $FFFA = NMI vector
   $FFFC = Reset vector
   $FFFE = IRQ/BRK vector

   In some cartridges, $6000-$7FFF is SRAM.

*/

uint8_t NESRAM[0x800]; //2 KB internal RAM
uint16_t sprtablesave;
extern struct cart_s cartridge; //struct is declared in rom.c
extern struct PPU_s *PPU;
extern struct OAM_s *OAM;
extern struct map4_s *map4;

extern uint8_t * outputNES;

static uint8_t m_strobe;       // Joypad strobe latch.
static unsigned int m_keyStates;

static uint8_t joypad_state(uint8_t joy) {
  int val = emu_ReadKeys();  
  int but = emu_GetPad();
  int hbut = emu_ReadI2CKeyboard();    
  uint8_t retval=0x00;
  
  if (joy == 0) 
  {
    if (val & MASK_JOY2_BTN)   retval |= 0x01; //A
    if (val & MASK_KEY_USER1)  retval |= 0x02; //B
    if ( (val & MASK_KEY_USER2) || (but == 2) || (hbut == 2) )   retval |= 0x04; //Select
    if ( (val & MASK_KEY_USER3) || (but == 3) || (hbut == 3) )  retval |= 0x08; //Start
    if (val & MASK_JOY2_UP)    retval |= 0x10; //Up
    if (val & MASK_JOY2_DOWN)  retval |= 0x20; //Down
    if (val & MASK_JOY2_RIGHT) retval |= 0x40; //Right    
    if (val & MASK_JOY2_LEFT)  retval |= 0x80; //Left
  }
  
  return retval;  
}

static void joystrobe(uint8_t b)
{
    m_strobe = (b & 1);
    if (!m_strobe)
    {
        m_keyStates = joypad_state(0);
    }
}

static uint8_t joyread(uint8_t n)
{
    uint8_t ret;
    if (m_strobe)
        ret = (joypad_state(0) & 1);
    else
    {
        ret = (m_keyStates & 1);
        m_keyStates >>= 1;
        m_keyStates |= 0x80;
    }
    return ret | 0x40;
}


#define readAPU(x) 0
#define writeAPU(x,y) {}


uint8_t read6502(uint16_t addr) {
	if (addr < 0x2000) return (NESRAM[addr & 0x7FF]);
	else if (addr < 0x4000) return (readPPUregs(0x2000 | (addr & 7)));
	else if (addr < 0x4018) {
		switch (addr) {
			case 0x4016: return (joyread(0)); break;
			//case 0x4017: return (joyread(1)); break;
			default: return (readAPU(addr)); break;
		}
	}
	else if (addr < 0x8000) {
		return ((cartridge.hasSRAM == true)?cartridge.SRAM[addr & 0x1FFF]:0);
	} else {
		//if we get to this point, it's a read from the cartridge
		addr &= 0x7FFF;
		return (cartridge.PRGbank[addr>>10][addr&1023]);
	}
}

void write6502(uint16_t addr, uint8_t value) {
	uint16_t tempsprite;
  
	if ((cartridge.mapper>0) && (addr>0x7FFF)) if (mapperwrite(addr, value)) return;
	if (addr < 0x2000) { NESRAM[addr & 0x7FF] = value; return; }
	if (addr < 0x4000) { writePPUregs(0x2000 | (addr & 7), value); return; }
	if (addr < 0x4018) {
		if (addr == 0x4014) {
      addr = (value<<8);
			for (tempsprite=0; tempsprite<64; tempsprite++) {
				OAM->RAM[OAM->addr++] = NESRAM[addr++];
        OAM->RAM[OAM->addr++] = NESRAM[addr++];
        OAM->RAM[OAM->addr++] = NESRAM[addr++];
        OAM->RAM[OAM->addr++] = NESRAM[addr++];
			}
      //for (tempsprite=0; tempsprite<256; tempsprite++) {
      //  OAM->RAM[(OAM->addr+tempsprite)&255] = read6502(value*256+tempsprite);
      //}
			return;
		}
		if (addr == 0x4016) joystrobe(value);
	  else writeAPU(addr, value);
		return;
	}

	if (addr < 0x8000) {
		if (cartridge.hasSRAM == true) {
			cartridge.SRAM[addr & 0x1FFF] = value;
		} else return; //open data bus if no cartidge SRAM
	}

	//if we get to this point, it's an attempted write to the cartridge.
	//obviously, being ROM, nothing would actually get written anywhere.
}


void execframe(bool skiprender) {
	uint16_t scanline;

	if (cartridge.mapper == 4) map4->irqcounter = map4->irqlatch;
	PPU->vblank = 0;
	PPU->sprzero = 0;
	PPU->sprover = 0;
	sprtablesave = PPU->sprtable;
	if (PPU->bgvisible) {
		execCPU(101);
		PPU->addr = PPU->tempaddr;
		execCPU(13);
	} else execCPU(114);
	for (scanline=0; scanline<240; scanline++) {
		if (cartridge.mapper == 4) {
			if (map4->irqenable && map4->irqcounter == 0) irqCPU();
			if (map4->irqcounter == 0) map4->irqcounter = map4->irqlatch;
			if (PPU->bgvisible || PPU->sprvisible) {
				map4irqdecrement();
				//if (map4->irqcounter > 0) map4->irqcounter--;
				//	else map4->irqcounter = map4->irqlatch;
			}
		}

#ifdef PIXEL_ACCURACY
		if ((scanline%3)==0) execCPU(8);
#else
		if ((scanline%3)==0) execCPU(86);
			else execCPU(85);
#endif

    if (skiprender == false) {
      renderscanline(scanline);
      emu_DrawLine(outputNES, 256, 240, scanline);      
    }

		if (PPU->bgvisible) PPU->addr = (PPU->addr & 0xFBE0) | (PPU->tempaddr & 0x041F);

		execCPU(28);
	}

	execCPU(340);

	PPU->vblank = 1;
	if (PPU->nmivblank) nmiCPU();
  emu_DrawVsync();
  
	execCPU(1930);
}


void execNES() {
  resetCPU();  
	memset(&NESRAM[0], 0, 2048);

	int i;
  for (i=0; i<64; i++) {
    emu_SetPaletteEntry((nesRgb[i]>>16)&0xff,(nesRgb[i]>>8)&0xff,(nesRgb[i])&0xff, i);
  }
  
}

