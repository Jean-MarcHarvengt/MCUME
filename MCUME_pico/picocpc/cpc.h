#ifndef CPC_H_
#define CPC_H_

#ifndef PICO_H_
#include "pico.h"
#include "pico/stdlib.h"
#endif

extern uint8_t RAM[0x10000];
extern unsigned char* bitstream;
extern struct GAConfig ga_config;
extern bool vsync_wait;

#define VGA_RGB(r,g,b)   ( (((r>>5)&0x07)<<5) | (((g>>5)&0x07)<<2) | (((b>>6)&0x3)<<0) )

extern void write_to_bitstream(char pixel);
extern void draw_vsync();
extern void cpc_Init(void);
extern void cpc_Step(void);
extern void cpc_Start(char* filename);
extern void cpc_Input(int bClick);

#endif