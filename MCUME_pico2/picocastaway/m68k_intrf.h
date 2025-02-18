#include <stdio.h>
#include <stdlib.h>
#include "dcastaway.h"

extern int waitstate;

#ifndef USE_FAME_CORE

#include "castaway/68000.h"
#include "castaway/op68k.h"

extern unsigned IO_CYCLE;

static __inline__ unsigned long cpu_loop(unsigned slice) {

extern unsigned long (*jmp_table[8192])(operin);

#define initialize_memmap()
    //printf("%8x =>%8x\n",pc,myinst);

#define cpuinst \
	address = pc&MEMADDRMASK; \
	myinst=biginst=ReadSL(address); \
	else { HWReset(); return slice-IO_CYCLE; } \
	pc+=2; \
	IO_CYCLE-=(*jmp_table[(myinst<<16)>>19])(reg);

	register unsigned long *p_reg=(unsigned long *)&reg[0];
	register unsigned long address;
	register uint32 myinst;
	IO_CYCLE=slice;
	//Execute 10 instructions
	cpuinst
	cpuinst
	cpuinst
	cpuinst
	cpuinst
	cpuinst
	cpuinst
	cpuinst
	cpuinst
	cpuinst
	return slice-IO_CYCLE;
#undef cpuinst
}


#else

#include "fame.h"


#if defined(DREAMCAST) || defined(USE_FAME_CORE_C)
#define M68KCONTEXT m68kcontext
#define IO_CYCLE io_cycle_counter
#else
#define M68KCONTEXT _m68kcontext
#define IO_CYCLE __io_cycle_counter
#endif

extern unsigned IO_CYCLE;

//extern struct M68K_CONTEXT M68KCONTEXT;
extern M68K_CONTEXT M68KCONTEXT;
extern int recalc_int;
extern void SetMemW(unsigned long address, unsigned short value);
extern void HWReset(void);

#define GetS() ((M68KCONTEXT.sr >> 13) & 1)
#define GetFC2() GetS()
#define GetI() ((M68KCONTEXT.sr >> 8) & 7)
#ifdef DEBUG_FAME
#define Interrupt(NUM,LEV) \
{ \
	extern int fame_debug_output; \
	extern FILE *fame_debug_file; \
	if (fame_debug_output) \
		{ fprintf(fame_debug_file,"Interrupt(%i,%i)\n",(NUM),(LEV)); DEBUG_FAME_FFLUSH; } \
	m68k_lower_irq((LEV)); \
	m68k_raise_irq((LEV),(NUM)); \
}
#else
#define Interrupt(NUM,LEV) \
{ \
	M68KCONTEXT.interrupts[0] |= (1 << (LEV)); \
	M68KCONTEXT.interrupts[(LEV)]=(NUM); \
	M68KCONTEXT.execinfo &= 0x7F; \
}
#endif

#define ClearInterrupt(LEV) M68KCONTEXT.interrupts[0] &= ~(1 << (LEV))
	

#define MEMADDRMASK 0x00ffffffl
#define MEMADDRMASKS 0x00fffffel
#define MEMIDXSHIFT 11
#define MEMADDRSIZE (MEMADDRMASK + 1)
#define AUTOINT2        26
#define AUTOINT4        28
#define BUSERR          2
#define ADDRESSERR      3


extern char GetMemB(unsigned long address);
extern short GetMemW(unsigned long address);
extern long GetMemL(unsigned long address);
extern void SetMemB(unsigned long address, unsigned char value);
extern void SetMemW(unsigned long address, unsigned short value);
extern void SetMemL(unsigned long address, unsigned long value);

void initialize_memmap(void);
void ExceptionGroup0( int, unsigned long, int);
void ExceptionGroup0_execute(void);


#ifdef DEBUG_FAME
static char dis_msg[96];
static unsigned short dis_buf[10];
void    disassemble68k(char *buf, unsigned short *inst_stream);
static __inline__ unsigned long cpu_loop(unsigned slice)
{
	unsigned i;
	extern int number_exg0_fame;
	extern int in_fame_core;
	extern unsigned fame_debug_count;
	extern int fame_debug_output;
	extern FILE *fame_debug_file;

#ifdef DEBUG_FAME_START
	if (fame_debug_count>=((unsigned)(DEBUG_FAME_START)))
		fame_debug_output=1;
	else {
		unsigned long cycles_actual=M68KCONTEXT.cycles_counter;
		short lastint=M68KCONTEXT.sr&0x700;

		in_fame_core=1;
		m68k_emulate(slice);
		in_fame_core=0;

		fprintf(fame_debug_file,"SLICE(%u) %u (%u) ret=%u (%x %x)\n",slice,fame_debug_count,slice,M68KCONTEXT.cycles_counter-cycles_actual,M68KCONTEXT.execinfo&0x80,M68KCONTEXT.interrupts[0]);
		fame_debug_count++;

		if (number_exg0_fame)
			ExceptionGroup0_execute();

		if ((M68KCONTEXT.execinfo&0x80)&&(lastint!=(M68KCONTEXT.sr&0x700)))
			M68KCONTEXT.execinfo&=0x7f;

		return (M68KCONTEXT.cycles_counter-cycles_actual);
	}

	if (fame_debug_output)
		fprintf(fame_debug_file,"SLICE(%u) %u (%u)\n",slice,fame_debug_count,slice);
#endif

	in_fame_core=1;
	unsigned long cycles_actual=M68KCONTEXT.cycles_counter;
	short lastint=M68KCONTEXT.sr&0x700;
	while(M68KCONTEXT.cycles_counter-cycles_actual<slice)
	{
		if (fame_debug_output)
		{
			dis_msg[0]= 0;
			dis_buf[0]= m68k_fetch(m68k_get_pc(),0);
			dis_buf[1]= m68k_fetch(m68k_get_pc()+2,0);
			dis_buf[2]= m68k_fetch(m68k_get_pc()+4,0);
			dis_buf[3]= m68k_fetch(m68k_get_pc()+6,0);
			dis_buf[4]= m68k_fetch(m68k_get_pc()+8,0);
			dis_buf[5]= m68k_fetch(m68k_get_pc()+10,0);
			dis_buf[6]= m68k_fetch(m68k_get_pc()+12,0);
			dis_buf[7]= m68k_fetch(m68k_get_pc()+14,0);
			disassemble68k(dis_msg,dis_buf);
			fprintf(fame_debug_file,"- PC=%.8X (%s) SR=%.2X - INT=%.2X STAT=%.4X E0=%i\n",m68k_get_pc(), dis_msg, M68KCONTEXT.sr,M68KCONTEXT.interrupts[0],M68KCONTEXT.execinfo,number_exg0_fame);
			fprintf(fame_debug_file,"\tD0=%.8X D1=%.8X D2=%.8X D3=%.8X\n",M68KCONTEXT.dreg[0],M68KCONTEXT.dreg[1],M68KCONTEXT.dreg[2],M68KCONTEXT.dreg[3]);
			fprintf(fame_debug_file,"\tD4=%.8X D5=%.8X D6=%.8X D7=%.8X\n",M68KCONTEXT.dreg[4],M68KCONTEXT.dreg[5],M68KCONTEXT.dreg[6],M68KCONTEXT.dreg[7]);
			fprintf(fame_debug_file,"\tA0=%.8X A1=%.8X A2=%.8X A3=%.8X\n",M68KCONTEXT.areg[0],M68KCONTEXT.areg[1],M68KCONTEXT.areg[2],M68KCONTEXT.areg[3]);
			fprintf(fame_debug_file,"\tA4=%.8X A5=%.8X A6=%.8X A7=%.8X\n",M68KCONTEXT.areg[4],M68KCONTEXT.areg[5],M68KCONTEXT.areg[6],M68KCONTEXT.areg[7]);DEBUG_FAME_FFLUSH;
		}
		m68k_emulate(1);
		if (fame_debug_output)
			{ fputs(".",fame_debug_file); DEBUG_FAME_FFLUSH; }
		if (M68KCONTEXT.execinfo&0x80)
			break;
	}
	in_fame_core=0;

	fame_debug_count++;

#ifdef DEBUG_FAME_STOP
	if (fame_debug_count>((unsigned)(DEBUG_FAME_STOP)))
		{
#ifdef DEBUG_FAME_FILE
	extern FILE *fame_debug_file;
	if (fame_debug_file)
		fclose(fame_debug_file);
	fame_debug_file=NULL;
#endif
		       	free(malloc(24*1024*1024)); exit(0); }
#endif
	if (number_exg0_fame)
	{
		ExceptionGroup0_execute();
		number_exg0_fame=0;
	}
	if ((M68KCONTEXT.execinfo&0x80)&&(lastint!=(M68KCONTEXT.sr&0x700)))
		M68KCONTEXT.execinfo&=0x7f;
	return (M68KCONTEXT.cycles_counter-cycles_actual);
}

#else
static __inline__ unsigned long cpu_loop(unsigned slice)
{
	extern int in_fame_core;
	extern int number_exg0_fame;

	unsigned long cycles_actual=M68KCONTEXT.cycles_counter;
	short lastint=M68KCONTEXT.sr&0x700;

	in_fame_core=1;
	m68k_emulate(slice);
	in_fame_core=0;

	if (number_exg0_fame)
		ExceptionGroup0_execute();

	if ((M68KCONTEXT.execinfo&0x80)&&(lastint!=(M68KCONTEXT.sr&0x700)))
		M68KCONTEXT.execinfo&=0x7f;

	return (M68KCONTEXT.cycles_counter-cycles_actual);
}
#endif


#endif
