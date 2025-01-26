#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "shared.h"

#define WORK_RAM_SIZE 0x10000
#define Z_RAM_SIZE 0x2000

#define VRAM_SIZE 0x10000
#define BGNAMEDIRTY_SIZE 0x800
#define BGNAME_SIZE 0x800
#define BGPATTERN_CACH_SIZE 0x40000


//extern uint8 * cart_rom;
extern uint8 * work_ram;    /* 68K work RAM */
extern uint8 * zram;        /* Z80 work RAM */

extern uint8 * vram;
extern uint8 * bg_name_dirty;     
extern uint16 * bg_name_list;     
extern uint8 * bg_pattern_cache;


extern void  mem_init(void);
extern int mem_test(void);
extern uint8 rom_version(void);
extern uint8 readb_rom(int address);
extern uint8 readb_swap_rom(int address);
extern uint16 readw_swap_rom(int address);
extern void  write_rom(int address, uint8 val);

extern void memcpy_rom(int dst, int src, int size);

#endif
