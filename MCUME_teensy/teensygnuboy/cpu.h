

#ifndef __CPU_H__
#define __CPU_H__



#include "defs.h"


union reg
{
	byte b[2][2];
	uint16 w[2];
	un32 d; /* padding for alignment, carry */
};

struct cpu
{
	union reg pc, sp, bc, de, hl, af;
	int ime, ima;
	int speed;
	int halt;
	int div, tim;
	int lcdc;
	int snd;
};

extern struct cpu cpu;

extern void cpu_reset();
extern int cpu_emulate(int cycles);


#endif


