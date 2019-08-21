/* MoarNES source - PPU.c

   This open-source software is (c)2010-2012 Mike Chambers, and is released
   under the terms of the GNU GPL v2 license.

   This is my implementation of the Picture Processing Unit (PPU) of the
   Nintendo Entertainment System. It is not perfect at this point, but the
   vast majority games will run correctly.

   This is arguably the most difficult aspect of a NES emulator to make
   perfect!
*/

//#include <stdio.h>
//#include <stdlib.h>
//#include "stdint.h"
//#include <string.h>

#include "moarnes.h"

#include "PPU.h"
#include "rom.h"
#include "prototypes.h"

#include "emuapi.h"


struct PPU_s *PPU;
struct OAM_s *OAM;
uint8_t *VRAM;
uint8_t * outputNES;

extern struct cart_s cartridge;
extern struct map9_s *map9;

uint8_t readPPU(uint16_t addr) {
   uint8_t tempbyte, oldbyte;
   addr &= 0x3FFF;
   if (addr>=0x3F00) addr = 0x3F00 | (addr & 0x1F);
   if (addr>0x2FFF && addr<0x3F00) addr = 0x3000 | (addr & 0xEFF);
   if (addr>=0x2000 && addr<0x2400) addr = (addr - 0x2000) + PPU->ntmap[0];
      else if (addr>=0x2400 && addr<0x2800) addr = (addr - 0x2400) + PPU->ntmap[1];
      else if (addr>=0x2800 && addr<0x2C00) addr = (addr - 0x2800) + PPU->ntmap[2];
      else if (addr>=0x2C00 && addr<0x3000) addr = (addr - 0x2C00) + PPU->ntmap[3];
   if (addr<0x2000) {
		tempbyte = cartridge.CHRbank[addr>>10][addr&1023];
		if (cartridge.mapper == 9) {
			switch (addr & 0x3FF0) {
				case 0x0FD0: map9->latch1 = 0xFD; CHRswap(&cartridge, 0, map9->latch0_fd, 4096); break;
				case 0x0FE0: map9->latch1 = 0xFE; CHRswap(&cartridge, 0, map9->latch0_fe, 4096); break;
				case 0x1FD0: map9->latch2 = 0xFD; CHRswap(&cartridge, 1, map9->latch1_fd, 4096); break;
				case 0x1FE0: map9->latch2 = 0xFE; CHRswap(&cartridge, 1, map9->latch1_fe, 4096); break;
			}
		}
		oldbyte = PPU->bytebuffer;
		PPU->bytebuffer = tempbyte;
		return oldbyte;
   }
   if (addr >= 0x3F00) tempbyte = VRAM[addr];
      else tempbyte = PPU->bytebuffer;
   PPU->bytebuffer = VRAM[addr];
   return(tempbyte);
} 

void writePPU(uint16_t addr, uint8_t value){
	addr &= 0x3FFF;
	if (addr>=0x3F00) addr = 0x3F00 | (addr & 0x1F);
	if (addr==0x3F00 || addr==0x3F10) { VRAM[0x3F00] = value; VRAM[0x3F10] = value; return; }
	if (addr==0x3F04 || addr==0x3F14) { VRAM[0x3F04] = value; VRAM[0x3F14] = value; return; }
	if (addr==0x3F08 || addr==0x3F18) { VRAM[0x3F08] = value; VRAM[0x3F18] = value; return; }
	if (addr==0x3F0C || addr==0x3F1C) { VRAM[0x3F0C] = value; VRAM[0x3F1C] = value; return; }
	if (addr>0x2FFF && addr<0x3F00) addr = 0x3000 | (addr & 0xEFF);
	if (addr>=0x2000 && addr<0x2400) addr = (addr - 0x2000) + PPU->ntmap[0];
		else if (addr>=0x2400 && addr<0x2800) addr = (addr - 0x2400) + PPU->ntmap[1];
		else if (addr>=0x2800 && addr<0x2C00) addr = (addr - 0x2800) + PPU->ntmap[2];
		else if (addr>=0x2C00 && addr<0x3000) addr = (addr - 0x2C00) + PPU->ntmap[3];
	if (addr>=0x2000) {
//emu_printf("v");

		VRAM[addr] = value;
		if (cartridge.mapper == 9) {
			switch (addr & 0x3FF0) {
				case 0x0FD0: map9->latch1 = 0xFD; CHRswap(&cartridge, 0, map9->latch0_fd, 4096); break;
				case 0x0FE0: map9->latch1 = 0xFE; CHRswap(&cartridge, 0, map9->latch0_fe, 4096); break;
				case 0x1FD0: map9->latch2 = 0xFD; CHRswap(&cartridge, 1, map9->latch1_fd, 4096); break;
				case 0x1FE0: map9->latch2 = 0xFE; CHRswap(&cartridge, 1, map9->latch1_fe, 4096); break;
			}
		}
	} else if (cartridge.CHRcount==0) cartridge.CHRbank[addr>>10][addr&1023] = value;
}

uint8_t lastwritten;
uint8_t readPPUregs(uint16_t addr) {
     uint8_t tempbyte;
     switch (addr) {
       case 0x2002:
		    tempbyte = (PPU->vblank << 7) | (PPU->sprzero << 6) | (PPU->sprover << 5) | (lastwritten & 0x1F);
            PPU->vblank = 0;
            PPU->addrlatch = 0;
            PPU->scrolllatch = 0;
            return(tempbyte);
       case 0x2004:
            return(OAM->RAM[OAM->addr]);
       case 0x2007:
			tempbyte = readPPU(PPU->addr);
            PPU->addr = (PPU->addr + PPU->addrinc) & 0x3FFF;
            return(tempbyte);
     }
     return(0);
}

void writePPUregs(uint16_t addr, uint8_t value) {
     PPU->regs[addr & 7] = value;
	 lastwritten = value;
     switch (addr) {
       case 0x2000:
            if (value&128) PPU->nmivblank = 1; else PPU->nmivblank = 0;
            if (value&32) PPU->sprsize = 16; else PPU->sprsize = 8;
            if (value&16) PPU->bgtable = 0x1000; else PPU->bgtable = 0x0000;
            if (value&8) PPU->sprtable = 0x1000; else PPU->sprtable = 0x0000;
            if (value&4) PPU->addrinc = 32; else PPU->addrinc = 1;
            PPU->nametable = value&3;
			PPU->tempaddr = (PPU->tempaddr & 0xF3FF) | (((uint16_t)value & 3) << 10);
            break;
       case 0x2001:
            if (value&16) PPU->sprvisible = 1; else PPU->sprvisible = 0;
            if (value&8) {
              PPU->bgvisible = 1;
        //emu_printf("a");
                             
            }
            else PPU->bgvisible = 0;
            if (value&4) PPU->sprclip = 0; else PPU->sprclip = 1;
            if (value&2) PPU->bgclip = 0; else PPU->bgclip = 1;
            break;
       case 0x2003:
            OAM->addr = value;
            break;
       case 0x2004:
            OAM->RAM[OAM->addr++] = value;
            break;
       case 0x2005:
            if (PPU->addrlatch == 0) {
              PPU->xscroll = value & 7;
			  PPU->tempaddr = (PPU->tempaddr & 0xFFE0) | ((uint16_t)value >> 3);
              PPU->addrlatch = 1;
            } else {
              PPU->yscroll = value & 7;
			  PPU->tempaddr = (PPU->tempaddr & 0xFC1F) | (((uint16_t)value & 0xF8) << 2);
              PPU->addrlatch = 0;
            }
			break;
       case 0x2006:
            if (PPU->addrlatch == 0) {
				PPU->tempaddr = (PPU->tempaddr & 0x00FF) | (((uint16_t)value & 0x3F) << 8);
				PPU->addrlatch = 1;
            } else {
				PPU->tempaddr = (PPU->tempaddr & 0xFF00) | (uint16_t)value;
				PPU->addr = PPU->tempaddr;
				PPU->addrlatch = 0;
            }
			break;
       case 0x2007:
            writePPU(PPU->addr, value);
            PPU->addr = (PPU->addr + PPU->addrinc) & 0x3FFF;
            break;
     }
}

uint16_t backgnd[256], sprfront[256], sprback[256], sprzero[256];
uint8_t frontidx[256], backidx[256];
extern uint16_t sprtablesave;

void rendersprites(uint16_t scanline) {
	int16_t OAMptr;
	uint16_t attr, spry, sprx, table, tile, flipx, flipy, x, startx, plotx, calcx, calcy, patoffset;
	uint8_t curpixel, palette, priority;
	//uint8_t valid[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	//uint8_t spr0idx = 255;
	uint8_t drawcount = 0;

	if (scanline == 0) return;
	else scanline--;

	memset(backidx, 255, sizeof(backidx));
	memset(frontidx, 255, sizeof(frontidx));
	OAMptr = 0;

    if (PPU->sprclip) startx = 8;
		else startx = 0;
	if (PPU->sprsize == 8) table = PPU->sprtable;

	for (OAMptr=252; OAMptr>=0; OAMptr-=4) {
		if ((scanline >= OAM->buf[OAMptr]) && (scanline < (OAM->buf[OAMptr]  + PPU->sprsize))) {
			spry = OAM->buf[OAMptr];
			spry = scanline - spry;
			tile = OAM->buf[OAMptr+1];
			attr = OAM->buf[OAMptr+2];
			sprx = OAM->buf[OAMptr+3];
			palette = (attr & 3) << 2;
			priority = (attr >> 5) & 1;
			flipx = (attr >> 6) & 1;
			flipy = (attr >> 7) & 1;

			if (++drawcount > 8) {
				PPU->sprover = 1;
				break;
			}

			if (PPU->sprsize == 16) {
				table = (tile & 1) << 12;
				tile &= 0xFE;
				if (flipy) calcy = (~spry & 15);
					else calcy = spry & 15;
				if (calcy > 7) tile++;
				calcy &= 7;
			} else {
				if (flipy) calcy = (~spry & 7);
					else calcy = spry & 7;
			}
			
			for (x=0; x<8; x++) {
				if (flipx) calcx = ~x & 7;
					else calcx = x;
				plotx = sprx + x;
				if ((plotx >= startx) && (plotx < 255)) {
					patoffset = table + (tile << 4) + calcy;
					curpixel = (cartridge.CHRbank[patoffset>>10][patoffset&1023] >> (~calcx & 7)) & 1; patoffset += 8;
					curpixel |= ((cartridge.CHRbank[patoffset>>10][patoffset&1023] >> (~calcx & 7)) & 1) << 1;
					if (curpixel > 0) {
						if (OAMptr == 0) if (backgnd[plotx] > 0) PPU->sprzero = 1;
						curpixel |= palette;
						if (priority) { sprback[plotx] = 0x3F10 + (uint16_t)curpixel; backidx[plotx] = (uint8_t)OAMptr; }
						else { sprfront[plotx] = 0x3F10 + (uint16_t)curpixel; frontidx[plotx] = (uint8_t)OAMptr; }
					}
				}
			}

		}
	}
}

void renderbackground(uint16_t scanline) {
	uint16_t x, startx, calcx, calcy, patoffset, usent, ntbase, tile, tempval, patbank;
	uint8_t curpixel, curattrib;
	uint16_t lastx= 0xFFFF;
	//uint16_t lasty = lastx = 0xFFFF;

	if (PPU->bgclip) startx = 8; //if background clipping enabled, don't draw leftmost 8 pixels
  else startx = 0; //otherwise do



  for (x=0; x<264; x++) {
    calcx = ((PPU->addr << 3) & 0xFF) | PPU->xscroll;
    calcy = ((PPU->addr >> 2) & 0xF8) | PPU->yscroll;
    usent = (PPU->addr >> 10) & 3;
    ntbase = PPU->ntmap[usent]; //nametable base address
    tile = VRAM[ntbase + ((calcy & 248) << 2) + (calcx >> 3)]; //calculate tile offset based on X,Y coords
	
    patoffset = PPU->bgtable + (tile << 4) + (calcy & 7); //then turn that into the byte offset in the nametable array
    curattrib = (VRAM[ntbase + 0x3C0 + ((calcy & 224) >> 2) + (calcx >> 5)] >> (((calcx & 16) >> 3) | ((calcy & 16) >> 2)) & 3) << 2;
    patbank = patoffset >> 10;

    curpixel = (cartridge.CHRbank[patoffset>>10][patoffset&1023] >> (~calcx & 7)) & 1; //used to go fetch the two
    curpixel |= ((cartridge.CHRbank[patoffset>>10][(patoffset+8)&1023] >> (~calcx & 7)) & 1) << 1; //lower bits of the pixel's palette index

    if ((curpixel > 0) && (x >= startx) && (x < 256)) { //if those are not zero, then this is a visible pixel
      backgnd[x] = 0x3F00 + (curpixel | curattrib); //stick it in the buffer to be drawn
    }
/*
			if (cartridge.mapper == 9) {
				if (tile == 0xFD) {
					if (patbank > 3) CHRswap(&cartridge, 1, map9->latch1_fd, 4096);
						else CHRswap(&cartridge, 0, map9->latch0_fd, 4096);
				} else if (tile == 0xFE) {
					if (patbank > 3) CHRswap(&cartridge, 1, map9->latch1_fe, 4096);
						else CHRswap(&cartridge, 0, map9->latch0_fe, 4096);
				}
			}
*/
    PPU->xscroll++;
    if (PPU->xscroll == 8) {
      PPU->xscroll = 0;
      tempval = (PPU->addr & 0x1F) + 1;
			if (tempval == 32) PPU->addr ^= 1024;
			PPU->addr = (PPU->addr & 0xFFE0) | (tempval & 0x1F);
		}
  }
  
	PPU->yscroll++;
	if (PPU->yscroll == 8) 
	{
	  PPU->yscroll = 0;
		tempval = ((PPU->addr >> 5) & 0x1F) + 1;
		if (tempval == 30) 
		{
			tempval = 0;
			PPU->addr ^= 2048;
		}
		PPU->addr = (PPU->addr & 0xFC1F) | ((tempval & 0x1F) << 5);
	}
}

void renderscanline(uint16_t scanline) {
  uint16_t tmpx;
  
  PPU->sprover = 0;
  
  memset(&backgnd[0], 0, sizeof(backgnd));
  memset(&sprback[0], 0, sizeof(sprback));
  memset(&sprfront[0], 0, sizeof(sprfront));
  memset(&sprzero[0], 0, sizeof(sprzero));


  if (PPU->bgvisible) renderbackground(scanline);
  else if ((scanline%3)==0) execCPU(86); else execCPU(85);
  memcpy(&OAM->buf[0], &OAM->RAM[0], sizeof(OAM->RAM));
  if (PPU->sprvisible) rendersprites(scanline);
  
  uint8_t *dst=outputNES;
  for (tmpx=0; tmpx<256; tmpx++) {
    if ((sprfront[tmpx]>0) && (frontidx[tmpx] < backidx[tmpx])) 
      *dst++ = VRAM[sprfront[tmpx]] & 0x3F;
    else {
      if (backgnd[tmpx]>0) {
        *dst++ = VRAM[backgnd[tmpx]] & 0x3F;
        if (sprzero[tmpx]) PPU->sprzero = 1;
      }
      else {
        if (sprback[tmpx]==0) 
          *dst++ = VRAM[0x3F00] & 0x3F;
        else 
          *dst++ = VRAM[sprback[tmpx]] & 0x3F;      
      }        
    }          
  }
}

void initPPU() {
  outputNES = emu_Malloc(256);
	PPU = emu_Malloc(sizeof(struct PPU_s));
	VRAM = emu_Malloc(16384);
	if ((PPU == NULL) || (VRAM == NULL)) fatalerr("Unable to allocate memory for PPU!");
	OAM = emu_Malloc(sizeof(struct OAM_s));
	if (OAM == NULL) fatalerr("Unable to allocate memory for OAM!");

	memset(PPU, 0, sizeof(*PPU));
	memset(OAM, 0, sizeof(*OAM));
	memset(VRAM, 0, 16384);
	PPU->sprsize = 8;
}

void killPPU() {
	if (PPU != NULL) free(PPU);
	if (VRAM != NULL) free(VRAM);
	if (OAM != NULL) free(OAM);
	if (outputNES != NULL) free(outputNES);
}
