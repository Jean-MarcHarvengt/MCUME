 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * MC68000 emulation
  *
  * (c) 1995 Bernd Schmidt
  */

#include "shared.h"

#include "events.h"
#include "gui.h"
#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#ifdef HAS_ERSATZ  
#include "ersatz.h"
#endif

#include "arduinoproto.h"

const int areg_byteinc[] = { 1,1,1,1,1,1,1,2 };
const int imm8_table[] = { 8,1,2,3,4,5,6,7 };
int broken_in;
#ifdef INTEL_FLAG_OPT
union flagu intel_flag_lookup[256];
#endif
struct regstruct regs;
union flagu regflags;

UWORD *pc_pref;
int cnt=0;
#include "cputbl.h"
#include "cputable.h"


void MakeSR(void)
{
#if 0
    assert((NFLG & 1) == NFLG);
    assert((regs.s & 1) == regs.s);
    assert((regs.x & 1) == regs.x);
    assert((CFLG & 1) == CFLG);
    assert((VFLG & 1) == VFLG);
    assert((ZFLG & 1) == ZFLG);
#endif
    regs.sr = ((regs.t << 15) | (regs.s << 13) | (regs.intmask << 8)
	       | (regs.x << 4) | (NFLG << 3) | (ZFLG << 2) | (VFLG << 1) 
	       |  CFLG);
}

void MakeFromSR(void)
{
    int olds = regs.s;

    regs.t = (regs.sr >> 15) & 1;
    regs.s = (regs.sr >> 13) & 1;
    regs.intmask = (regs.sr >> 8) & 7;
    regs.x = (regs.sr >> 4) & 1;
    NFLG = (regs.sr >> 3) & 1;
    ZFLG = (regs.sr >> 2) & 1;
    VFLG = (regs.sr >> 1) & 1;
    CFLG = regs.sr & 1;
    if (olds != regs.s) {
	CPTR temp = regs.usp;
	regs.usp = regs.a[7];
	regs.a[7] = temp;
    }
    specialflags |= SPCFLAG_INT;
    if (regs.t)
    	specialflags |= SPCFLAG_TRACE;
    else
    	specialflags &= ~(SPCFLAG_TRACE | SPCFLAG_DOTRACE);
}

void Exception(int nr)
{
  	//emu_printf("exception");
  
    MakeSR();
    if (!regs.s) {
	CPTR temp = regs.usp;
	regs.usp = regs.a[7];
	regs.a[7] = temp;
	regs.s = 1;
    }
    if (CPU_LEVEL > 0) {
	regs.a[7] -= 2;
	put_word (regs.a[7], nr * 4);
    }
    regs.a[7] -= 4;
    put_long (regs.a[7], m68k_getpc ());
    regs.a[7] -= 2;
    put_word (regs.a[7], regs.sr);
    m68k_setpc(get_long(regs.vbr + 4*nr));
    regs.t = 0;
    specialflags &= ~(SPCFLAG_TRACE | SPCFLAG_DOTRACE);
}

static void Interrupt(int nr)
{
    //assert(nr < 8 && nr >= 0);
    Exception(nr+24);
    
    regs.intmask = nr;
    specialflags |= SPCFLAG_INT;
}

static int caar, cacr;

void m68k_move2c (int regno, ULONG *regp)
{
    if (CPU_LEVEL == 1 && (regno & 0x7FF) > 1)
	op_illg (0x4E7B);
    else
	switch (regno) {
	 case 0: regs.sfc = *regp; break;
	 case 1: regs.dfc = *regp; break;
	 case 2: cacr = *regp & 0xFF; break;
	 case 0x800: regs.usp = *regp; break;
	 case 0x801: regs.vbr = *regp; break;
	 case 0x802: caar = *regp & 0xFF; break;
	 default:
	    op_illg (0x4E7B);
	    break;
	}
}

void m68k_movec2 (int regno, ULONG *regp)
{
    if (CPU_LEVEL == 1 && (regno & 0x7FF) > 1)
	op_illg (0x4E7A);
    else
	switch (regno) {
	 case 0: *regp = regs.sfc; break;
	 case 1: *regp = regs.dfc; break;
	 case 2: *regp = cacr; break;
	 case 0x800: *regp = regs.usp; break;
	 case 0x801: *regp = regs.vbr; break;
	 case 0x802: *regp = caar; break;
	 default:
	    op_illg (0x4E7A);
	    break;
	}
}

extern void m68k_divl (UWORD opcode, ULONG src, UWORD extra)
{
    if (src == 0)
	return;
#ifdef INT_64BIT
    if (extra & 0x800) {
	/* signed variant */
	INT_64BIT a = regs.d[(extra >> 12) & 7];
	INT_64BIT quot, rem;
	
	if (extra & 0x400)
	    a |= (INT_64BIT)regs.d[extra & 7] << 32;
	rem = a % src;
	quot = a / src;
	if ((extra & 0x400) && (extra & 7) != ((extra >> 12) & 7))
	    regs.d[extra & 7] = rem;
	regs.d[(extra >> 12) & 7] = quot;
    } else {
	/* unsigned */
	unsigned INT_64BIT a = regs.d[(extra >> 12) & 7];
	unsigned INT_64BIT quot, rem;
	
	if (extra & 0x400)
	    a |= (INT_64BIT)regs.d[extra & 7] << 32;
	rem = a % src;
	quot = a / src;
	if ((extra & 0x400) && (extra & 7) != ((extra >> 12) & 7))
	    regs.d[extra & 7] = rem;
	regs.d[(extra >> 12) & 7] = quot;
    }
#endif
}

extern void m68k_mull (UWORD opcode, ULONG src, UWORD extra)
{
#ifdef INT_64BIT
    if (extra & 0x800) {
	/* signed variant */
	INT_64BIT a = (LONG)regs.d[(extra >> 12) & 7];

	a *= (LONG)src;
	if ((extra & 0x400) && (extra & 7) != ((extra >> 12) & 7))
	    regs.d[extra & 7] = a >> 32;
	regs.d[(extra >> 12) & 7] = (ULONG)a;
    } else {
	/* unsigned */
	unsigned INT_64BIT a = (ULONG)regs.d[(extra >> 12) & 7];
	unsigned INT_64BIT quot, rem;
	
	a *= src;
	if ((extra & 0x400) && (extra & 7) != ((extra >> 12) & 7))
	    regs.d[extra & 7] = a >> 32;
	regs.d[(extra >> 12) & 7] = (ULONG)a;
    }
#endif
}


void MC68000_reset(void)
{
    emu_printf("reset");
    regs.a[7] = get_long(0x00f80000);
    m68k_setpc(get_long(0x00f80004));
    regs.s = 1;
    regs.stopped = 0;
    regs.t = 0;
    specialflags = 0;
    regs.intmask = 7;
    regs.vbr = regs.sfc = regs.dfc = 0;
    customreset();
}

void op_illg(ULONG opcode)
{
    if (opcode == 0x4E7B && get_long(0x10) == 0 
	&& (m68k_getpc() & 0xF80000) == 0xF80000) 
    {
	fprintf(stderr, "Your Kickstart requires a 68020 CPU. Giving up.\n");
	broken_in = 1; 
	specialflags |= SPCFLAG_BRK;
	quit_program = 1;
    }
#ifdef HAS_ERSATZ    
    if (opcode == 0xF00D && ((m68k_getpc() & 0xF80000) == 0xF80000)) {
		/* This is from the dummy Kickstart replacement */
		ersatz_perform (nextiword ());
		return;
    }
#endif    
    regs.pc_p--;
    if ((opcode & 0xF000) == 0xF000) {
	if ((opcode & 0xE00) == 0x200)
	    Exception(0xB);
	else 
	    switch (opcode & 0x1FF) {
	     case 0x17:
		regs.pc_p+=2;
		break;
	     default:
		regs.pc_p++;
	    }
	return;
    }
    if ((opcode & 0xF000) == 0xA000) {
    	Exception(0xA);
	return;
    }
    fprintf(stderr, "Illegal instruction: %04x\n", opcode);
    Exception(4);
}

//static int n_insns=0, n_spcinsns=0;

static __inline__ void do_hardware(void)
{
  //emu_printf("do_hardware");
    if (specialflags & SPCFLAG_BLIT) {
	do_blitter();
#ifdef NO_FAST_BLITTER
	do_blitter();
	do_blitter();
	do_blitter();
#endif
    }
    if (specialflags & SPCFLAG_DISK) {
	do_disk(); /* This is not critical. Four calls make disk */
	do_disk(); /* loading quite fast. */
	do_disk();
	do_disk();
    }
}

void MC68000_run(void)
{
    for(;;) {

    if (quit_program) {
		break;
	}

	UWORD opcode;
	/* assert (!regs.stopped && !(specialflags & SPCFLAG_STOP)); */
  //emu_printi(regs.pc + (regs.pc_p-pc_pref)*2);
	opcode = nextiword();
  //emu_printf("o");
  //emu_printi(opcode);  	
#ifdef COUNT_INSTRS
	instrcount[opcode]++;
#endif
	(*cpufunctbl[opcode])(opcode);
#ifndef NO_EXCEPTION_3
	if (buserr) {
	    Exception(3);
	    buserr = 0;
	}
#endif
	/*n_insns++;*/
	do_cycles();	
	if (specialflags) {
	    /*n_spcinsns++;*/
	    while (specialflags & SPCFLAG_BLTNASTY) {
		do_cycles();
		do_hardware();
	    }
	    if (specialflags & SPCFLAG_DOTRACE) {
		Exception(9);
	    }
	    while (specialflags & SPCFLAG_STOP) {
		do_cycles();
		do_hardware();
		if (specialflags & (SPCFLAG_INT | SPCFLAG_DOINT)){
		    int intr = intlev();
		    specialflags &= ~(SPCFLAG_INT | SPCFLAG_DOINT);
		    specialflags &= ~(SPCFLAG_INT | SPCFLAG_DOINT);
		    if (intr != -1 && intr > regs.intmask) {
			Interrupt(intr);
			regs.stopped = 0;
			specialflags &= ~SPCFLAG_STOP;
		    }	    
		}		
	    }
	    if (specialflags & SPCFLAG_TRACE) {
		specialflags &= ~SPCFLAG_TRACE;
		specialflags |= SPCFLAG_DOTRACE;
	    }
#ifdef WANT_SLOW_MULTIPLY
	    /* Kludge for Hardwired demo. The guys who wrote it should be
	     * mutilated. */
	    if (specialflags & SPCFLAG_EXTRA_CYCLES) {
		do_cycles ();
		do_cycles ();
		do_cycles ();
		do_cycles ();
		specialflags &= ~SPCFLAG_EXTRA_CYCLES;
	    }
#endif
	    do_hardware();
	    
	    if (specialflags & SPCFLAG_DOINT) {
		int intr = intlev();
		specialflags &= ~(SPCFLAG_INT | SPCFLAG_DOINT);
		if (intr != -1 && intr > regs.intmask) {
		    Interrupt(intr);
		    regs.stopped = 0;
		}	    
	    }
	    if (specialflags & SPCFLAG_INT) {
		specialflags &= ~SPCFLAG_INT;
		specialflags |= SPCFLAG_DOINT;
	    }
	    if (specialflags & SPCFLAG_BRK) {		
		specialflags &= ~SPCFLAG_BRK;
		return;		
	    }
	}
    }
}

void MC68000_step(void)
{
    specialflags |= SPCFLAG_BRK;
    MC68000_run();
}

void MC68000_skip(CPTR nextpc)
{
    broken_in = 0;
    specialflags |= SPCFLAG_BRK;
    do {
	MC68000_step();
    } while (nextpc != m68k_getpc() && !broken_in);
}

