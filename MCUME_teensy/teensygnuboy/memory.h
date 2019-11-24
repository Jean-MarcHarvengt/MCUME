#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "shared.h"

//extern uint8 * vram;
//extern byte ***patpix;


extern void  mem_init(void);
extern int mem_test(void);
extern uint8 read_rom(int address);
extern void  write_rom(int address, uint8 val);
extern void memcpy_rom(int dst, int src, int size);

#endif