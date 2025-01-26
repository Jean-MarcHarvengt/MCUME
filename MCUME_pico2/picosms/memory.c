#include "shared.h"


uint8 * cache;



void mem_init(void) {
    cache = emu_Malloc(CACHE_SIZE); 
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
