#ifndef __ROM_H__
#define __ROM_H__

#define MIR_HORIZONTAL 0
#define MIR_VERTICAL   1
#define MIR_FOURSCREEN 2

struct cart_s {
//	uint8_t title[MAX_PATH];
	uint8_t mapper;
	uint8_t mirroring;
	uint16_t PRGcount;
	uint16_t CHRcount;
	bool hasSRAM;
	bool hasCHRRAM;
	bool hasTrainer;
	uint8_t *PRGfull;
	uint8_t *CHRfull;
	uint8_t *PRGbank[1024];
	uint8_t *CHRbank[1024];
	uint16_t PRGblock[1024]; //this and CHRblock aren't used for memory access, but they are used to
	uint16_t CHRblock[1024]; //store save states since pointers can change between executions
	uint8_t SRAM[8192];
	//uint8_t trainer[512];
};

struct hdr_s {
	uint8_t match[4];
	uint8_t PRGsize;
	uint8_t CHRsize;
	uint8_t flags[4]; //flags[2] is actually size of PRG RAM in 8 KB units
	uint8_t zerobytes[5];
};

struct map9_s {
	uint8_t latch0_fd;
	uint8_t latch0_fe;
	uint8_t latch1_fd;
	uint8_t latch1_fe;
	uint8_t latch1;
	uint8_t latch2;
};

struct map4_s {
	/*uint8_t prgmode; //0 = $8000-$9FFF swappable, $C000-$DFFF fixed to second-last bank. 1 = reversed.
	uint8_t chrmode; //0 = two 2 KB at $0000-$0FFF, four 1 KB at $1000-$1FFF. 1 = reversed.
	uint8_t bankselect;
	uint8_t irqlatch;
	uint8_t irqenable;*/
	uint8_t command;
	uint8_t vromsize;
	uint16_t chraddrselect;
	uint16_t irqcounter;
	uint16_t irqlatch;
	uint8_t irqenable;
	uint8_t swap;
	uint16_t prgswitch1;
	uint16_t prgswitch2;
	uint8_t prgaddr;
};

struct map1_s {
	uint8_t map1reg[4];
	uint8_t map1bitpos;
	uint8_t map1accum;
};

struct cheat_s {
	uint8_t replace;
	uint8_t compare;
	uint8_t docompare;
	uint8_t valid;
	uint16_t addr;
};
#endif
