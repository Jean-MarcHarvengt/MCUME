 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * MC68000 emulation
  *
  * Copyright 1995 Bernd Schmidt
  */

#ifndef NEWCPU_H
#define NEWCPU_H


extern const int areg_byteinc[];
extern const int imm8_table[];

extern int broken_in;

typedef void cpuop_func(ULONG) REGPARAM;

struct cputbl {
    cpuop_func *handler;
    int specific;
    UWORD opcode;
};


extern cpuop_func *cpufunctbl[65536];
extern void op_illg(ULONG) REGPARAM;

typedef char flagtype; 

union flagu {
    struct {
	/* v must be at the start so that the x86 seto instruction
	 * changes the V flag. C must follow after V. */
	char v;
	char c;
	char n;
	char z;
    } flags;
    ULONG longflags;
};

extern struct regstruct 
{
    ULONG d[8];
    CPTR  a[8],usp;
    UWORD sr;
    flagtype t;
    flagtype s;
    flagtype x;
    flagtype stopped;
    int intmask;
    ULONG pc;
    UWORD *pc_p;
    UWORD *pc_oldp;
    
    ULONG vbr,sfc,dfc;
} regs;

#ifdef INTEL_FLAG_OPT
extern union flagu intel_flag_lookup[256] __asm__ ("intel_flag_lookup");
extern union flagu regflags __asm__ ("regflags");
#else
extern union flagu regflags;
#endif

#define ZFLG (regflags.flags.z)
#define NFLG (regflags.flags.n)
#define CFLG (regflags.flags.c)
#define VFLG (regflags.flags.v)

//extern void emu_printi(int val);
//extern void emu_printf(char *);

extern void MC68000_oldstep(UWORD opcode);

extern UWORD *pc_pref;
extern int cnt;
static __inline__ UWORD nextiword(void)
{
    //emu_printi((int)regs.pc);
    //emu_printi(regs.pc + (regs.pc_p-pc_pref)*2);
    //delay(1);
    //regs.pc_p = get_real_address(regs.pc + (regs.pc_p-pc_pref)*2);
    UWORD r = *regs.pc_p++;
    //emu_printf("w:");
    //emu_printi(cnt++);
    //emu_printi(r);
    return r;
}

static __inline__ ULONG nextilong(void)
{
    //emu_printi((int)regs.pc);
    //regs.pc_p = get_real_address(regs.pc + (regs.pc_p-pc_pref)*2);
    ULONG r = *regs.pc_p++;
    //regs.pc_p = get_real_address(regs.pc + (regs.pc_p-pc_pref)*2);
    r = (r << 16) + *regs.pc_p++;
    //emu_printf("l:");
    //emu_printi(cnt++);    
    //emu_printi(r);    
    return r;
}

static __inline__ void m68k_setpc(CPTR newpc)
{
    regs.pc = newpc;
    regs.pc_p = regs.pc_oldp = get_real_address(newpc);
    pc_pref = regs.pc_p;
    //emu_printf("setpc");
    //emu_printi(regs.pc + (regs.pc_p-pc_pref)*2);
}

static __inline__ CPTR m68k_getpc(void)
{
    return regs.pc + ((char *)regs.pc_p - (char *)regs.pc_oldp);
}

static __inline__ void m68k_setstopped(int stop)
{
    regs.stopped = stop;
    if (stop)
	specialflags |= SPCFLAG_STOP;
}

static __inline__ int cctrue(const int cc)
{
    switch(cc){
     case 0: return 1;                       /* T */
     case 1: return 0;                       /* F */
     case 2: return !CFLG && !ZFLG;          /* HI */
     case 3: return CFLG || ZFLG;            /* LS */
     case 4: return !CFLG;                   /* CC */
     case 5: return CFLG;                    /* CS */
     case 6: return !ZFLG;                   /* NE */
     case 7: return ZFLG;                    /* EQ */
     case 8: return !VFLG;                   /* VC */
     case 9: return VFLG;                    /* VS */
     case 10:return !NFLG;                   /* PL */
     case 11:return NFLG;                    /* MI */
     case 12:return NFLG == VFLG;            /* GE */
     case 13:return NFLG != VFLG;            /* LT */
     case 14:return !ZFLG && (NFLG == VFLG); /* GT */
     case 15:return ZFLG || (NFLG != VFLG);  /* LE */
    }
    abort();
    return 0;
}

#if CPU_LEVEL > 1
extern ULONG get_disp_ea (ULONG, UWORD);

#else
static __inline__ ULONG get_disp_ea (ULONG base, UWORD dp)
{
    int reg = (dp >> 12) & 7;
    LONG regd;
    
    if (dp & 0x8000)
	regd = regs.a[reg];
    else
	regd = regs.d[reg];
    if (!(dp & 0x800))
	regd = (LONG)(WORD)regd;
    return base + (BYTE)(dp) + regd;
}
#endif

extern void MakeSR(void);
extern void MakeFromSR(void);
extern void Exception(int);
extern void m68k_move2c(int, ULONG *);
extern void m68k_movec2(int, ULONG *);
extern void m68k_divl (UWORD, ULONG, UWORD);
extern void m68k_mull (UWORD, ULONG, UWORD);
extern void init_m68k (void);
extern void MC68000_step(void);
extern void MC68000_run(void);
extern void MC68000_skip(CPTR);
extern void MC68000_dumpstate(CPTR *);
extern void MC68000_disasm(CPTR,CPTR *,int);
extern void MC68000_reset(void);

#endif
