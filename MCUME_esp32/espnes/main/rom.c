/* MoarNES source - rom.c

   This open-source software is (c)2010-2013 Mike Chambers, and is released
   under the terms of the GNU GPL v2 license.

   This file contains the code to load NES ROM files.
*/

#include <stdio.h>
#include "moarnes.h"
#include "emuapi.h"

extern bool ROMisNSF;
struct cart_s cartridge;
extern struct PPU_s *PPU;
static const uint8_t validhdr[4] = { 'N', 'E', 'S', 0x1A };

bool loadROM(char *filename) {
	static struct hdr_s header;
	uint16_t setptr;

  emu_printf("Loading ROM...");
  
	emu_LoadFileSeek(filename, (char*)&header, sizeof(header), 0);
	if (memcmp((void *)&header.match, (void *)&validhdr[0], 4)) return(false);	
	emu_printf("ROM header read!");

	//parse through the header data
	if (header.flags[1] == 'D') header.flags[1] = 0; //DISKDUDE fix
	cartridge.mapper = ((header.flags[0] >> 4) | (header.flags[1] & 0xF0));
	cartridge.hasSRAM = 1; //(header.flags[0] & 2) >> 1;
	if (header.flags[0] & 8) cartridge.mirroring = MIR_FOURSCREEN;
		else if (header.flags[0] & 1) cartridge.mirroring = MIR_VERTICAL;
			else cartridge.mirroring = MIR_HORIZONTAL;
	if (header.flags[0] & 4) cartridge.hasTrainer = true;
		else cartridge.hasTrainer = false;
	cartridge.PRGcount = (uint16_t)header.PRGsize << 4; //header gives PRG size in 16 KB units, we want it in 1 KB
	cartridge.CHRcount = (uint16_t)header.CHRsize << 3; //and CHR size is in 8 KB units

	switch (cartridge.mirroring) {
		case 0:
			PPU->ntmap[0] = 0x2000;
			PPU->ntmap[1] = 0x2000;
			PPU->ntmap[2] = 0x2400;
			PPU->ntmap[3] = 0x2400;
			break;
		case 1:
			PPU->ntmap[0] = 0x2000;
			PPU->ntmap[1] = 0x2400;
			PPU->ntmap[2] = 0x2000;
			PPU->ntmap[3] = 0x2400;
			break;
		case 2:
			PPU->ntmap[0] = 0x2000;
			PPU->ntmap[1] = 0x2400;
			PPU->ntmap[2] = 0x2600;
			PPU->ntmap[3] = 0x2800;
			break;
		default:
			break;
	}

	cartridge.PRGfull = emu_Malloc(cartridge.PRGcount << 10);
	if (cartridge.PRGfull == NULL) fatalerr("Unable to allocate for PRG-ROM data!");
	if (cartridge.CHRcount > 0) {
		cartridge.CHRfull = emu_Malloc(cartridge.CHRcount << 10);
		if (cartridge.CHRfull == NULL) fatalerr("Unable to allocate for CHR-ROM data!");
	} else {
		cartridge.CHRfull = emu_Malloc(1024 << 10);
	}

  emu_LoadFileSeek(filename, (char*)cartridge.PRGfull, (int)cartridge.PRGcount << 10, 16);
  emu_LoadFileSeek(filename, (char*)cartridge.CHRfull, (int)cartridge.CHRcount << 10, 16+((int)cartridge.PRGcount << 10));

	//first init all PRG/CHR bank pointers to the beginning of PRG/CHR memory
	//just in case, to avoid possible crashes from bad pointers
	for (setptr=0; setptr<1024; setptr++) {
		cartridge.CHRbank[setptr] = cartridge.CHRfull;
		cartridge.PRGbank[setptr] = cartridge.PRGfull;
	}

	if (mapperinit(&cartridge) == false) return(false);
  emu_printf("ROM read!");

	return(true);
}
