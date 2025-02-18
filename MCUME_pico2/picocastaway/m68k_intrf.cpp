#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void SetMemWW(unsigned long address, unsigned short value)
{  
  address &= MEMADDRMASK;
  WriteW(address, value);
  /*
  if (address<RAM1SIZE)
    WriteW(address + mem1base, value);
  else
    WriteW(address-RAM1SIZE + mem2base, value);
	*/
}

static void SetMemBB (unsigned long address, unsigned char value) {
  address &= MEMADDRMASK;
  WriteB(address, value);
  /*
  if (address<RAM1SIZE)
    WriteB(address + mem1base, value);
  else
    WriteB(address-RAM1SIZE + mem2base, value);
  */
}

static unsigned char GetMemBB(unsigned long address)
{
  address &= MEMADDRMASK;
  return ReadB(address);
  /*
  if (address<RAM1SIZE)
    return ReadB(address + mem1base);
  else  
    return ReadB(address-RAM1SIZE + mem2base);
  */
}


static unsigned short GetMemWW(unsigned long address)
{
  address &= MEMADDRMASK;
  return ReadW(address);
  /*
  if (address<RAM1SIZE)
    return ReadW(address + mem1base);
  else  
    return ReadW(address-RAM1SIZE + mem2base);
  */  
}




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

void ExceptionGroup0_execute(void)
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

void    ExceptionGroup0(
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
  {0, RAM1SIZE-1, (unsigned)0},
  {RAM1SIZE, MEMSIZE-1, (unsigned)mem2base},  
	{ROMBASE2, (IOBASE-1), (unsigned)rombase},
	{IOBASE, 0x00FFFFFF, (unsigned)rombase},
	{(unsigned)-1,(unsigned)-1,(unsigned)NULL}
};


static M68K_DATA read8[] = {
  {0, MEMSIZE-1, (void *)GetMemBB, NULL},
  {MEMSIZE, (ROMBASE2)-1, (void *)GetMemB, NULL},
  {ROMBASE2, IOBASE-1, NULL, (void *)rombase},
  {IOBASE, 0x00FFFFFF, (void *)DoIORB, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA read16[] = {
  {0, MEMSIZE-1, (void *)GetMemWW, NULL},
  {MEMSIZE, (ROMBASE2)-1, (void *)GetMemW, NULL},
  {ROMBASE2, IOBASE-1, NULL, (void *)rombase},
  {IOBASE, 0x00FFFFFF, (void *)DoIORW, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA write8[] = {
  {0, MEMSIZE-1,  (void *)SetMemBB, NULL},
  {MEMSIZE, (IOBASE)-1, (void *)SetMemB, NULL},
  {IOBASE, 0x00FFFFFF, (void *)DoIOWB, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};

static M68K_DATA write16[] = {
  {0, MEMSIZE-1, (void *)SetMemWW, NULL},
  {MEMSIZE, (IOBASE)-1, (void *)SetMemW, NULL},
  {IOBASE, 0x00FFFFFF, (void *)DoIOWW, NULL},
  {(unsigned)-1,(unsigned)-1,NULL,NULL}
};



void initialize_memmap(void)
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
#ifdef ALL_IN_RAM  
  program[0].offset= ((unsigned)mem1base) ;
#else
  program[0].offset= ((unsigned)0) ;
#endif  
  program[1].offset= ((unsigned)mem2base - RAM1SIZE) ;


//	read8[0].data=read16[0].data=write8[0].data=write16[0].data=(void *)((unsigned)membase);
	read8[2].data=read16[2].data=(void *)((unsigned)rombase);
//  read8[0].data=read16[0].data=write8[0].data=write16[0].data=(void *)((unsigned)membase);
//  read8[1].data=read16[1].data=write8[1].data=write16[1].data=(void *)((unsigned)membase2-MEM1SIZE);
//  read8[3].data=read16[3].data=(void *)((unsigned)rombase);

	m68k_init();
	m68k_set_context(&context);
	m68k_reset();
}
