/* MoarNES source - mapper.c

   This open-source software is (c)2010-2013 Mike Chambers, and is released
   under the terms of the GNU GPL v2 license.

   This code handles the many video and system mapping hardware found in most
   game cartridge hardware. It doesn't handle them all by any means, but most
   major mappers are supported.
*/

#include <stdio.h>
#include "moarnes.h"
#include "emuapi.h"

extern uint16_t lastop;
uint16_t lastchange = 0;
struct map9_s *map9;
struct map4_s *map4;
struct map1_s *map1;
void *mappermemory = NULL; //this gets allocated, and a mapper struct shares it's pointer. it's for easier state saving.
uint32_t mappersize = 0;

extern struct cart_s cartridge;
extern struct PPU_s *PPU;

//extern bool running;
//extern bool paused;

void PRGswap(struct cart_s *cart, uint16_t banknum, uint16_t newbank, uint16_t banksize) {
     uint16_t tmpstart, tmplen, tmpcur, tmpcur2;
     tmpstart = (newbank*banksize)>>10;
     tmplen = banksize>>10;
     tmpcur2 = (banknum*banksize)>>10;
     for (tmpcur=0; tmpcur<tmplen; tmpcur++) {
       cart->PRGbank[tmpcur2] = &cart->PRGfull[(uint32_t)tmpstart << 10];
	   cart->PRGblock[tmpcur2++] = tmpstart++;
     }
}

void CHRswap(struct cart_s *cart, uint16_t banknum, uint16_t newbank, uint16_t banksize) {
     uint16_t tmpstart, tmplen, tmpcur, tmpcur2;
     tmpstart = (newbank*banksize)>>10;
     tmplen = banksize>>10;
     tmpcur2 = (banknum*banksize)>>10;
     for (tmpcur=0; tmpcur<tmplen; tmpcur++) {
       cart->CHRbank[tmpcur2] = &cart->CHRfull[(uint32_t)tmpstart << 10];
	   cart->CHRblock[tmpcur2++] = tmpstart++;
     }
}

extern uint8_t *VRAM;
bool mapperinit(struct cart_s *cart) {
	//uint8_t msgdata[256];

	mappersize = 0;
	if (mappermemory != NULL) free(mappermemory);
	mappermemory = NULL;
	memset(VRAM, 0, 16384);

	switch (cart->mapper) {
		case 0:
			if (cart->PRGcount == 16) {
				PRGswap(cart, 0, 0, 16384);
				PRGswap(cart, 1, 0, 16384);
			} else {
				PRGswap(cart, 0, 0, 32768);
			}
			CHRswap(cart, 0, 0, 8192);
			break;

		case 1: //MMC1
			mappermemory = emu_Malloc(sizeof(struct map1_s));
			memset(mappermemory, 0, sizeof(struct map1_s));
			mappersize = sizeof(struct map1_s);
			map1 = mappermemory;
			PRGswap(cart, 0, 0, 16384);
			PRGswap(cart, 1, (cart->PRGcount>>4)-1, 16384);
			CHRswap(cart, 0, 0, 8192);
			write6502(0x8000, 12);
			break;

		case 2: //UxROM
			PRGswap(cart, 0, 0, 16384);
			PRGswap(cart, 1, (cart->PRGcount>>4)-1, 16384);
			CHRswap(cart, 0, 0, 8192);
			break;

		case 3: //CNROM
			if (cart->PRGcount == 16) {
				PRGswap(cart, 0, 0, 16384);
				PRGswap(cart, 1, 0, 16384);
			} else {
				PRGswap(cart, 0, 0, 32768);
			}
			CHRswap(cart, 0, 0, 8192);
			break;

		case 4: //MMC3
			mappermemory = emu_Malloc(sizeof(struct map4_s));
			memset(mappermemory, 0, sizeof(struct map4_s));
			mappersize = sizeof(struct map4_s);
			map4 = mappermemory;
			PRGswap(cart, 0, 0, 16384);
			PRGswap(cart, 1, (cart->PRGcount >> 4) - 1, 16384);
			CHRswap(cart, 0, 0, 8192);
			map4->irqenable = 0;
			map4->irqcounter = 255;
			map4->irqlatch = 255;
			PPU->bgtable = 0x0000;
			PPU->sprtable = 0x1000;
			break;

		case 7: //AxROM
			PRGswap(cart, 0, 0, 32768);
			CHRswap(cart, 0, 0, 8192);
			cart->CHRcount = 0;
			PPU->ntmap[0] = 0x2000;
			PPU->ntmap[1] = 0x2000;
			PPU->ntmap[2] = 0x2000;
			PPU->ntmap[3] = 0x2000;
			break;

		case 9: //MMC2
		case 10: //MMC4
			mappermemory = emu_Malloc(sizeof(struct map9_s));
			memset(mappermemory, 0, sizeof(struct map9_s));
			mappersize = sizeof(struct map9_s);
			map9 = mappermemory;
			PRGswap(cart, 0, 0, 8192);
			PRGswap(cart, 1, (cart->PRGcount>>3)-3, 8192);
			PRGswap(cart, 2, (cart->PRGcount>>3)-2, 8192);
			PRGswap(cart, 3, (cart->PRGcount>>3)-1, 8192);
			map9->latch1 = 0xFE;
			map9->latch2 = 0xFE;
			break;

		case 11: //color dreams
			PRGswap(cart, 0, 0, 32768);
			CHRswap(cart, 0, 0, 8192);
			break;

		case 13: //CPROM
			PRGswap(cart, 0, 0, 32768);
			CHRswap(cart, 0, 0, 4096);
			CHRswap(cart, 1, 0, 4096);
			break;

		default:
			//sprintf((int8_t *)msgdata, "Sorry, mapper %u is not yet supported.", cart->mapper);
			//msgbox(msgdata, MB_ICONERROR | MB_OK);
			//running = false;
			//paused = false;
			return(false);
	}

	return(true);
}

void map4irqdecrement() {
	if (map4->irqcounter > 0) {
		map4->irqcounter--;
	} else {
		if (map4->irqenable)
			irqCPU();
		map4->irqcounter = map4->irqlatch;
	}
}

//uint8_t map1reg[4] = { 0, 0, 0, 0 };
//uint8_t map1bitpos = 0, map1accum = 0;
void map1calc() {
  switch (map1->map1reg[0]&3) {
    case 0:
         PPU->ntmap[0] = 0x2000;
         PPU->ntmap[1] = 0x2000;
         PPU->ntmap[2] = 0x2000;
         PPU->ntmap[3] = 0x2000; break;
    case 1:
         PPU->ntmap[0] = 0x2400;
         PPU->ntmap[1] = 0x2400;
         PPU->ntmap[2] = 0x2400;
         PPU->ntmap[3] = 0x2400; break;
    case 2:
         PPU->ntmap[0] = 0x2000;
         PPU->ntmap[1] = 0x2400;
         PPU->ntmap[2] = 0x2000;
         PPU->ntmap[3] = 0x2400; break;
    case 3:
         PPU->ntmap[0] = 0x2000;
         PPU->ntmap[1] = 0x2000;
         PPU->ntmap[2] = 0x2400;
         PPU->ntmap[3] = 0x2400; break;
  }
  if (map1->map1reg[0]&8) {
    if (map1->map1reg[0]&4) { PRGswap(&cartridge, 0, map1->map1reg[3]&0xF, 16384); PRGswap(&cartridge, 1, (cartridge.PRGcount>>4)-1, 16384); }
      else { PRGswap(&cartridge, 0, 0, 16384); PRGswap(&cartridge, 1, map1->map1reg[3]&0xF, 16384); }
  } else {
    PRGswap(&cartridge, 0, (map1->map1reg[3]&0xF)>>1, 32768);
  }
  if (map1->map1reg[0]&16) {
    CHRswap(&cartridge, 0, map1->map1reg[1], 4096);
    CHRswap(&cartridge, 1, map1->map1reg[2], 4096);   
  } else {
    CHRswap(&cartridge, 0, map1->map1reg[1]>>1, 8192);
  }

}

uint8_t mapperwrite(uint16_t addr, uint8_t value) {
	switch (cartridge.mapper) {
		case 1: //MMC1
              if (value&128) {
                map1->map1reg[0] = (map1->map1reg[0]&0xF3)+0xC; //bits 2, 3 set - others unchanged
                map1->map1bitpos = 0; map1->map1accum = 0;
                return(1);
              }
              map1->map1accum |= (value&1) << map1->map1bitpos;
              if (map1->map1bitpos==4) {
                if (addr>=0xE000) {
                  map1->map1reg[3] = map1->map1accum;
                  } else if (addr>=0xC000) {
                    map1->map1reg[2] = map1->map1accum;
                    } else if (addr>=0xA000) {
                      map1->map1reg[1] = map1->map1accum;
                      } else map1->map1reg[0] = map1->map1accum;
                map1calc();
                map1->map1bitpos = 0; map1->map1accum = 0;
                return(1);
              }
              map1->map1bitpos = (map1->map1bitpos + 1) % 5;
            break;

		case 2: //UxROM
			PRGswap(&cartridge, 0, value, 16384);
			return(1);

		case 3: //CNROM
			CHRswap(&cartridge, 0, value & 3, 8192);
			return(1);

		case 4: //MMC3
			if ((addr >= 0x8000) && (addr < 0xA000)) {
				if (addr & 1) {
					switch (map4->command) {
						case 0: //select two 1 KB VROM pages at PPU $0000
							CHRswap(&cartridge, ((0<<10)^map4->chraddrselect)>>10, value, 1024);
							CHRswap(&cartridge, ((1<<10)^map4->chraddrselect)>>10, value+1, 1024);
							break;
						case 1: //select two 1 KB VROM pages at PPU $0800
							CHRswap(&cartridge, ((2<<10)^map4->chraddrselect)>>10, value, 1024);
							CHRswap(&cartridge, ((3<<10)^map4->chraddrselect)>>10, value+1, 1024);
							break;
						case 2: //select 1 KB VROM page at PPU $1000
							CHRswap(&cartridge, ((4<<10)^map4->chraddrselect)>>10, value, 1024);
							break;
						case 3: //select 1 KB VROM page at PPU $1400
							CHRswap(&cartridge, ((5<<10)^map4->chraddrselect)>>10, value, 1024);
							break;
						case 4: //select 1 KB VROM page at PPU $1800
							CHRswap(&cartridge, ((6<<10)^map4->chraddrselect)>>10, value, 1024);
							break;
						case 5: //select 1 KB VROM page at PPU $1C00
							CHRswap(&cartridge, ((7<<10)^map4->chraddrselect)>>10, value, 1024);
							break;
						case 6: //select first switchable ROM page
							map4->prgswitch1 = value & ((cartridge.PRGcount>>3)-1);
							break;
						case 7: //select second switchable ROM page
							map4->prgswitch2 = value & ((cartridge.PRGcount>>3)-1);
							break;
					}
					if ((map4->command == 6) || (map4->command == 7)) {
						if (map4->prgaddr) {
							PRGswap(&cartridge, 0, (cartridge.PRGcount>>3)-2, 8192);
							PRGswap(&cartridge, 1, map4->prgswitch2, 8192);
							PRGswap(&cartridge, 2, map4->prgswitch1, 8192);
							PRGswap(&cartridge, 3, (cartridge.PRGcount>>3)-1, 8192);
						} else {
							PRGswap(&cartridge, 0, map4->prgswitch1, 8192);
							PRGswap(&cartridge, 1, map4->prgswitch2, 8192);
							PRGswap(&cartridge, 2, (cartridge.PRGcount>>3)-2, 8192);
							PRGswap(&cartridge, 3, (cartridge.PRGcount>>3)-1, 8192);
						}
						return(1);
					}
				} else {
					map4->command = value & 7;
					map4->prgaddr = value & 0x40;
					if (value & 0x80) map4->chraddrselect = 0x1000;
						else map4->chraddrselect = 0x0000;
					return(1);
				}
			} else if ((addr >= 0xA000) && (addr < 0xC000)) {
				if (addr & 1) {
					//if (value & 0x80)
				} else {
					cartridge.mirroring = value & 1;
					if (cartridge.mirroring) {
						PPU->ntmap[0] = 0x2000;
						PPU->ntmap[1] = 0x2000;
						PPU->ntmap[2] = 0x2400;
						PPU->ntmap[3] = 0x2400;
					} else {
						PPU->ntmap[0] = 0x2000;
						PPU->ntmap[1] = 0x2400;
						PPU->ntmap[2] = 0x2000;
						PPU->ntmap[3] = 0x2400;
					}
				}
			} else if ((addr >= 0xC000) && (addr < 0xE000)) {
				if (addr & 1) map4->irqcounter = 0;
					else map4->irqlatch = value;
			} else {
				if (addr & 1) map4->irqenable = 1; 
					else map4->irqenable = 0;
			}
			return(1);

		case 7: //AxROM
			PRGswap(&cartridge, 0, value & 0x0F, 32768);
			if (value & 0x10) {
				PPU->ntmap[0] = 0x2400;
				PPU->ntmap[1] = 0x2400;
				PPU->ntmap[2] = 0x2400;
				PPU->ntmap[3] = 0x2400;
			} else {
				PPU->ntmap[0] = 0x2000;
				PPU->ntmap[1] = 0x2000;
				PPU->ntmap[2] = 0x2000;
				PPU->ntmap[3] = 0x2000;
			}
			return(1);

		case 9: //MMC2
		case 10: //MMC4
			if ((addr >= 0xA000) && (addr < 0xB000)) {
				PRGswap(&cartridge, 0, value & 0x0F, 8192);
			} else if ((addr >= 0xB000) && (addr < 0xC000)) {
				map9->latch0_fd = value;
				if (map9->latch1 == 0xFD) CHRswap(&cartridge, 0, map9->latch0_fd, 4096);
			} else if ((addr >= 0xC000) && (addr < 0xD000)) {
				map9->latch0_fe = value;
				if (map9->latch1 == 0xFE) CHRswap(&cartridge, 0, map9->latch0_fe, 4096);
			} else if ((addr >= 0xD000) && (addr < 0xE000)) {
				map9->latch1_fd = value;
				if (map9->latch2 == 0xFD) CHRswap(&cartridge, 1, map9->latch1_fd, 4096);
			} else if ((addr >= 0xE000) && (addr < 0xF000)) {
				map9->latch1_fe = value;
				if (map9->latch2 == 0xFE) CHRswap(&cartridge, 1, map9->latch1_fe, 4096);
			} else if (addr >= 0xF000) {
				if (value & 1) { //horizontal
					PPU->ntmap[0] = 0x2000;
					PPU->ntmap[1] = 0x2000;
					PPU->ntmap[2] = 0x2400;
					PPU->ntmap[3] = 0x2400;
				} else { //vertical
					PPU->ntmap[0] = 0x2000;
					PPU->ntmap[1] = 0x2400;
					PPU->ntmap[2] = 0x2000;
					PPU->ntmap[3] = 0x2400;
				}
			}
			break;

		case 11: //color dreams
			PRGswap(&cartridge, 0, value & 3, 32768);
			CHRswap(&cartridge, 0, value >> 4, 8192);
			break;

		case 13: //CPROM
			CHRswap(&cartridge, 1, value & 3, 4096);
			break;
	 }
	 return(0);
}
