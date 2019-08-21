#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Arduino.h>

#include "dcastaway.h"
#include "mem.h"
#include "st.h"

#include "m68k_intrf.h"

#ifdef DEBUG_FAME
FILE *fame_debug_file=stdout;
unsigned fame_debug_count=0;
#ifdef DEBUG_FAME_START
int fame_debug_output=0;
#else
int fame_debug_output=1;
#endif
#endif

#ifdef DEBUG_FAME
#include "dis.h"
#endif

char GetMemB(unsigned long address);
short GetMemW(unsigned long address);
long GetMemL(unsigned long address);
void SetMemB(unsigned long address, unsigned char value);
void SetMemW(unsigned long address, unsigned short value);
void SetMemL(unsigned long address, unsigned long value);


static void SetMemWW(unsigned long address, unsigned short value)
{  
    address &= MEMADDRMASK;
  if (address<MEM1SIZE)
    WriteW(address + membase, value);
  else
    WriteW(address-MEM1SIZE + membase2, value);
}

static void SetMemBB (unsigned long address, unsigned char value) {
    address &= MEMADDRMASK;
  if (address<MEM1SIZE)
    WriteB(address + membase, value);
  else
    WriteB(address-MEM1SIZE + membase2, value);
}

static char GetMemBB(unsigned long address)
{
    address &= MEMADDRMASK;
  if (address<MEM1SIZE)
    return ReadB(address + membase);
  else  
    return ReadB(address-MEM1SIZE + membase2);
}


static short GetMemWW(unsigned long address)
{
    address &= MEMADDRMASK;
  if (address<MEM1SIZE)
    return ReadW(address + membase);
  else  
    return ReadW(address-MEM1SIZE + membase2);
}


#if defined(FAME_SINGLE_MEMORY) && defined(DEBUG_FAME)

static char MyGetMemB(unsigned long address)
{
	char ret;

	if (fame_debug_output)
		{ fprintf(fame_debug_file,"GetMemB(0x%.6X)",address); DEBUG_FAME_FFLUSH; }

	ret=GetMemB(address);

	if (fame_debug_output)
		{ fprintf(fame_debug_file," = 0x%.2X\n",ret); DEBUG_FAME_FFLUSH; }

	return ret;
}

static short MyGetMemW(unsigned long address)
{
	short ret;

	if (fame_debug_output)
		{ fprintf(fame_debug_file,"GetMemW(0x%.6X)",address); DEBUG_FAME_FFLUSH; }
	
	ret=GetMemW(address);

	if (fame_debug_output)
		{ fprintf(fame_debug_file," = 0x%.4X\n",ret); DEBUG_FAME_FFLUSH; }

	return ret;
}

static void MySetMemB(unsigned long address, unsigned value)
{
	value&=0xff;
	if (fame_debug_output)
		{ fprintf(fame_debug_file,"SetMemB(0x%.6X,0x%.2X)",address,value); DEBUG_FAME_FFLUSH; }

	SetMemB(address,value);

	if (fame_debug_output)
		{ fputs(fame_debug_file,"."); DEBUG_FAME_FFLUSH; }
}

static void MySetMemW(unsigned long address, unsigned value)
{
	value&=0xffff;
	if (fame_debug_output)
		{ fprintf(fame_debug_file,"SetMemW(0x%.6X,0x%.4X)",address,value); DEBUG_FAME_FFLUSH; }

	SetMemW(address,value);

	if (fame_debug_output)
		{ fputs(".",fame_debug_file); DEBUG_FAME_FFLUSH; }
}

#else

#define MyGetMemW GetMemW
#define MyGetMemB GetMemB
#define MySetMemW SetMemW
#define MySetMemB SetMemB

#endif

#ifndef USE_FAME_CORE
int recalc_int=1; //0;
#endif
int in_fame_core=0;
int number_exg0_fame=0;
static unsigned addr_exg0_fame=0;
static unsigned pc_exg0_fame=0;
static unsigned sr_exg0_fame=0;
static int rw_exg0_fame=0;
static unsigned long exaddress=0x12345678;

void HWReset(void)
{
	m68k_reset();
	exaddress=0x12345678;
}

static int inexec=0;

PROGMEM void ExceptionGroup0_execute(void)
{
	short context = 0;

#ifdef DEBUG_FAME
	if (fame_debug_output)
		{ fputs("ExceptionGroup0_execute",fame_debug_file); DEBUG_FAME_FFLUSH; }
#endif

	M68KCONTEXT.execinfo&=0x7F;

	if ((!((exaddress+1!=addr_exg0_fame)&&(exaddress+2!=addr_exg0_fame)&&(exaddress+3!=addr_exg0_fame))) || (!inexec))
	{
		if (!inexec)
			m68k_reset();
		return;
	}
	inexec=0;
	
	context |= 0x8;
	if (rw_exg0_fame)
		context |= 0x10;
	if (GetS())
		context |= 0x4;
	if (rw_exg0_fame && addr_exg0_fame == pc_exg0_fame)
		context |= 0x2;
	else
		context |= 0x1;

	M68KCONTEXT.sr |= (1<<13);
	M68KCONTEXT.sr &= ~(1<<15);

	M68KCONTEXT.areg[7]-=14;
	SetMemW(M68KCONTEXT.areg[7], context);
	SetMemL(M68KCONTEXT.areg[7] + 2, addr_exg0_fame);
	SetMemW(M68KCONTEXT.areg[7] + 6, GetMemW(pc_exg0_fame));
	SetMemW(M68KCONTEXT.areg[7] + 8, sr_exg0_fame);
	SetMemL(M68KCONTEXT.areg[7] + 10, pc_exg0_fame);

	M68KCONTEXT.pc=GetMemL((long)number_exg0_fame * 4);

	exaddress=addr_exg0_fame;
	number_exg0_fame=0;
}

PROGMEM void    ExceptionGroup0(
        int number,             /* trap number */
        unsigned long address,  /* fault address */
        int rw)         /* read = true, write = false */
{
#ifdef DEBUG_FAME
	if (fame_debug_output)
		{ fprintf(fame_debug_file,"ExceptionGroup0(%i,0x%X,%i)\n",number,address,rw); DEBUG_FAME_FFLUSH; }
#endif
	if ((exaddress+1!=address)&&(exaddress+2!=address)&&(exaddress+3!=address)&&(!inexec)){
		inexec=1;
		number_exg0_fame=number;
		addr_exg0_fame=address;
		rw_exg0_fame=rw;
		pc_exg0_fame=M68KCONTEXT.pc;
		sr_exg0_fame=M68KCONTEXT.sr;
		if (!in_fame_core)
			ExceptionGroup0_execute();
		else
			IO_CYCLE=0;
	}
}


static M68K_CONTEXT context;

static M68K_PROGRAM program[]= {
  {0, MEM1SIZE-1, (unsigned)membase},
  {MEM1SIZE, MEMSIZE-1, (unsigned)membase2},  
/*  
	{0, MEMSIZE-1, (unsigned)membase},
	{MEMSIZE, (MEMSIZE*2)-1, ((unsigned)membase)-MEMSIZE},
	{MEMSIZE*2, (MEMSIZE*3)-1, ((unsigned)membase)-(MEMSIZE*2)},
#if MEMSIZE*4 <= ROMBASE2
	{MEMSIZE*3, (MEMSIZE*4)-1, ((unsigned)membase)-(MEMSIZE*3)},
#if MEMSIZE*5 <= ROMBASE2
	{MEMSIZE*4, (MEMSIZE*5)-1, ((unsigned)membase)-(MEMSIZE*4)},
#if MEMSIZE*6 <= ROMBASE2
	{MEMSIZE*5, (MEMSIZE*6)-1, ((unsigned)membase)-(MEMSIZE*5)},
#if MEMSIZE*7 <= ROMBASE2
	{MEMSIZE*6, (MEMSIZE*7)-1, ((unsigned)membase)-(MEMSIZE*6)},
#if MEMSIZE*8 <= ROMBASE2
	{MEMSIZE*7, (MEMSIZE*8)-1, ((unsigned)membase)-(MEMSIZE*7)},
#if MEMSIZE*9 <= ROMBASE2
	{MEMSIZE*8, (MEMSIZE*9)-1, ((unsigned)membase)-(MEMSIZE*8)},
#if MEMSIZE*10 <= ROMBASE2
	{MEMSIZE*9, (MEMSIZE*10)-1, ((unsigned)membase)-(MEMSIZE*9)},
#if MEMSIZE*11 <= ROMBASE2
	{MEMSIZE*10, (MEMSIZE*11)-1, ((unsigned)membase)-(MEMSIZE*10)},
#if MEMSIZE*12 <= ROMBASE2
	{MEMSIZE*11, (MEMSIZE*12)-1, ((unsigned)membase)-(MEMSIZE*11)},
#if MEMSIZE*13 <= ROMBASE2
	{MEMSIZE*12, (MEMSIZE*13)-1, ((unsigned)membase)-(MEMSIZE*12)},
#if MEMSIZE*14 <= ROMBASE2
	{MEMSIZE*13, (MEMSIZE*14)-1, ((unsigned)membase)-(MEMSIZE*13)},
#if MEMSIZE*15 <= ROMBASE2
	{MEMSIZE*14, (MEMSIZE*15)-1, ((unsigned)membase)-(MEMSIZE*14)},
#if MEMSIZE*16 <= ROMBASE2
	{MEMSIZE*15, (MEMSIZE*16)-1, ((unsigned)membase)-(MEMSIZE*15)},
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
*/
	{ROMBASE2, (IOBASE-1), (unsigned)rombase},
	{IOBASE, 0x00FFFFFF, (unsigned)rombase},
	{(unsigned)-1,(unsigned)-1,(unsigned)NULL}
};

#ifndef FAME_SINGLE_MEMORY
static M68K_DATA read8[] = {
  {0, MEMSIZE-1, (void *)GetMemBB, NULL},
//  {0, MEMSIZE-1, NULL, (void *)membase},
//  {0, MEM1SIZE-1, NULL, (void *)membase},
//  {MEM1SIZE, MEMSIZE-1, NULL, (void *)membase2},
  {MEMSIZE, (ROMBASE2)-1, (void *)GetMemB, NULL},
  {ROMBASE2, IOBASE-1, NULL, (void *)rombase},
  {IOBASE, 0x00FFFFFF, (void *)DoIORB, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA read16[] = {
  {0, MEMSIZE-1, (void *)GetMemWW, NULL},
//  {0, MEMSIZE-1, NULL, (void *)membase},
//  {0, MEM1SIZE-1, NULL, (void *)membase},
//  {MEM1SIZE, MEMSIZE-1, NULL, (void *)membase2},
  {MEMSIZE, (ROMBASE2)-1, (void *)GetMemW, NULL},
  {ROMBASE2, IOBASE-1, NULL, (void *)rombase},
  {IOBASE, 0x00FFFFFF, (void *)DoIORW, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA write8[] = {
  {0, MEMSIZE-1,  (void *)SetMemBB, NULL},
//  {0, MEMSIZE-1, NULL, (void *)membase},
//  {0, MEM1SIZE-1, NULL, (void *)membase},
//  {MEM1SIZE, MEMSIZE-1, NULL, (void *)membase2},
  {MEMSIZE, (IOBASE)-1, (void *)SetMemB, NULL},
  {IOBASE, 0x00FFFFFF, (void *)DoIOWB, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA write16[] = {
  {0, MEMSIZE-1, (void *)SetMemWW, NULL},
//  {0, MEMSIZE-1, NULL, (void *)membase},
//  {0, MEM1SIZE-1, NULL, (void *)membase},
//  {MEM1SIZE, MEMSIZE-1, NULL, (void *)membase2},
  {MEMSIZE, (IOBASE)-1, (void *)SetMemW, NULL},
  {IOBASE, 0x00FFFFFF, (void *)DoIOWW, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};

#else

static M68K_DATA read8[] = {
	{ 0, 0xFFFFFF, (void *)MyGetMemB, NULL },
	{(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA read16[] = {
	{ 0, 0xFFFFFF, (void *)MyGetMemW, NULL },
	{(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA write8[] = {
	{ 0, 0xFFFFFF, (void *)MySetMemB, NULL },
	{(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA write16[] = {
	{ 0, 0xFFFFFF, (void *)MySetMemW, NULL },
	{(unsigned)-1,(unsigned)-1,NULL,NULL}
};

#endif

PROGMEM void initialize_memmap(void)
{
	int i;
	memset(&context,0,sizeof(M68K_CONTEXT));
	context.fetch=context.sv_fetch=context.user_fetch=(M68K_PROGRAM*)&program;
	context.read_byte=context.sv_read_byte=context.user_read_byte=(M68K_DATA*)&read8;
	context.read_word=context.sv_read_word=context.user_read_word=(M68K_DATA*)&read16;
	context.write_byte=context.sv_write_byte=context.user_write_byte=(M68K_DATA*)&write8;
	context.write_word=context.sv_write_word=context.user_write_word=(M68K_DATA*)&write16;
	for(i=0;program[i].low_addr != ((unsigned)-1);i++)
  {
		if (program[i].low_addr >= ROMBASE2)
			program[i].offset= (((unsigned)rombase)+ROMBASE2)-program[i].low_addr;
		//else
		//	program[i].offset= ((unsigned)membase) - (i*MEMSIZE);
  }
  program[0].offset= ((unsigned)membase) ;
  program[1].offset= ((unsigned)membase2 - MEM1SIZE) ;


#ifndef FAME_SINGLE_MEMORY
//	read8[0].data=read16[0].data=write8[0].data=write16[0].data=(void *)((unsigned)membase);
	read8[2].data=read16[2].data=(void *)((unsigned)rombase);
//  read8[0].data=read16[0].data=write8[0].data=write16[0].data=(void *)((unsigned)membase);
//  read8[1].data=read16[1].data=write8[1].data=write16[1].data=(void *)((unsigned)membase2-MEM1SIZE);
//  read8[3].data=read16[3].data=(void *)((unsigned)rombase);

#endif

	m68k_init();
	m68k_set_context(&context);
	m68k_reset();
}
