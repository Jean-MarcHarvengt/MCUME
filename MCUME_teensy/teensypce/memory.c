#include "shared.h"

//static uint8 lpceram[0x8000];
static uint8 lvram[0x10000];
static uint8 lobjram[0x200];
static uint8 lbram[0x2000];

static uint8 lbg_name_dirty[0x800];
static uint16 lbg_name_list[0x800];
static uint16 lobj_name_dirty[0x200];
static uint16 lobj_name_list[0x200];
static uint8 lbg_pattern_cache[BG_CACHE_SIZE];
//static uint8 lobj_pattern_cache[OBJ_CACHE_SIZE]; 


uint8 *  vram=&lvram[0];
uint16 * vramw = (uint16 *)&lvram[0];
uint8 * objram = &lobjram[0];
uint16 *objramw = (uint16 *)&lobjram[0];
uint8 *  pceram;// = &lpceram[0];
#ifdef CDRAM    
uint8 *  cdram;
#endif
uint8 *  bram=&lbram[0];

uint8 * bg_name_dirty=&lbg_name_dirty[0];
uint16 * bg_name_list=&lbg_name_list[0];
uint16 * obj_name_dirty=&lobj_name_dirty[0];
uint16 * obj_name_list=&lobj_name_list[0];
uint8 * bg_pattern_cache;
uint8 * obj_pattern_cache; 
uint8 * bg_pattern_cache=&lbg_pattern_cache[0];
//uint8 * obj_pattern_cache=&lobj_pattern_cache[0]; 

#include "emuapi.h"

void mem_init(void) {
    //bg_pattern_cache = emu_Malloc(BG_CACHE_SIZE); 
    obj_pattern_cache = emu_Malloc(OBJ_CACHE_SIZE); //0x20200000; 
    pceram = emu_Malloc(0x8000);
#ifdef CDRAM    
    cdram = emu_Malloc(0x10000);
#endif    
    //emu_printh(obj_pattern_cache);
}


void memcpy_rom(int dst, int src, int size) {
	while (size > 0) {   
		write_rom(dst++, read_rom(src++));
		size--;
	}
}
