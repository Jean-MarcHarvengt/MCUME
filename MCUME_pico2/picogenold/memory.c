#include "shared.h"


//uint8 lwork_ram[WORK_RAM_SIZE];    /* 68K work RAM */
//uint8 lzram[Z_RAM_SIZE];         /* Z80 work RAM */

//uint8 lvram[VRAM_SIZE];
uint8 lbg_name_dirty[BGNAMEDIRTY_SIZE];     
uint16 lbg_name_list[BGNAME_SIZE];     
//uint8 lbg_pattern_cache[BGPATTERN_CACH_SIZE];

uint8 * vram;// = &lvram[0];
uint8 * bg_name_dirty = &lbg_name_dirty[0];     
uint16 * bg_name_list = &lbg_name_list[0];
uint8 * bg_pattern_cache; // = &lbg_pattern_cache[0];


uint8 * work_ram; // = &lwork_ram[0];
uint8 * zram; // = &lzram[0];



void mem_init(void) {
    bg_pattern_cache = emu_Malloc(BGPATTERN_CACH_SIZE); 
    vram = emu_Malloc(VRAM_SIZE); 
    work_ram = emu_Malloc(WORK_RAM_SIZE); 
    zram = emu_Malloc(Z_RAM_SIZE); 
    //bg_pattern_cache = emu_Malloc(BGPATTERN_CACH_SIZE+1)&0xFFFFFFFE; 
    //vram = emu_Malloc(VRAM_SIZE+1)&0xFFFFFFFE; 
    //work_ram = emu_Malloc(WORK_RAM_SIZE+1)&0xFFFFFFFE; 
    //zram = emu_Malloc(Z_RAM_SIZE+1)&0xFFFFFFFE; 
}


int mem_test(void) {
//	for (int i=0x40000;i<OBJ_CACHE_SIZE;i++) {
//		if (obj_pattern_cache[i] != 0) return 0;;
//	}
	return 1;
}

void memcpy_rom(int dst, int src, int size) {
	while (size > 0) {   
		write_rom(dst++, read_rom(src++));
		size--;
	}
}
