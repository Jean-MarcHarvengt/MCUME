#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "shared.h"

#define CACHE_SIZE 0x20000


extern uint8 * cache;


extern void  mem_init(void);
extern int mem_test(void);
extern uint8 rom_version(void);
extern uint8 readb_rom(int address);
extern uint8 readb_swap_rom(int address);
extern uint16 readw_swap_rom(int address);
extern void  write_rom(int address, uint8 val);

extern void memcpy_rom(int dst, int src, int size);

#endif
