#ifndef __PPU_H__
#define __PPU_H__

struct PPU_s {
	uint16_t ntx;
	uint16_t nty;
	uint16_t ntmap[4];
	uint16_t addr;
	uint16_t tempaddr;
	uint16_t addrinc;
	uint16_t addrlatch;
	uint16_t nametable;
	uint8_t r2006[2];

	uint16_t sprtable;
	uint16_t bgtable;
	uint16_t sprvisible;
	uint16_t bgvisible;
	uint16_t sprsize;
	uint16_t nmivblank;

	uint16_t xscroll;
	uint16_t yscroll;
	uint16_t scrolllatch;
	
	uint8_t greyscale;
	uint8_t bgclip;
	uint8_t sprclip;

	uint8_t regs[8];
	uint8_t sprzero;
	uint8_t sprover;
	uint8_t vblank;

	uint8_t bytebuffer;
};

struct OAM_s {
	uint8_t addr;
	uint8_t RAM[256];
	uint8_t buf[256];
	uint8_t valid[8];
};
#endif
