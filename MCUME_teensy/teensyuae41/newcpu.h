 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * MC68000 emulation
  *
  * Copyright 1995 Bernd Schmidt
  */

extern int areg_byteinc[];
extern int imm8_table[];

extern int movem_index1[256];
extern int movem_index2[256];
extern int movem_next[256];

extern int fpp_movem_index1[256];
extern int fpp_movem_index2[256];
extern int fpp_movem_next[256];

extern int broken_in;

typedef void cpuop_func(uae_u32) REGPARAM;

struct cputbl {
    cpuop_func *handler;
    int specific;
    uae_u16 opcode;
};

extern void op_illg(uae_u32) REGPARAM;

typedef char flagtype;

/* Arrrghh.. */
#if !defined NO_PREFETCH_BUFFER && !defined USE_POINTER
#define USE_POINTER
#endif
#if defined USE_COMPILER && !defined USE_POINTER
#define USE_POINTER
#endif

extern struct regstruct 
{
    uae_u32 regs[16];
    uaecptr  usp,isp,msp;
    uae_u16 sr;
    flagtype t1;
    flagtype t0;
    flagtype s;
    flagtype m;
    flagtype x;
    flagtype stopped;
    int intmask;
    uae_u32 pc;
#ifdef USE_POINTER
    uae_u8 *pc_p;
    uae_u8 *pc_oldp;
#endif
    
    uae_u32 vbr,sfc,dfc;

    double fp[8];
    uae_u32 fpcr,fpsr,fpiar;

    uae_u32 spcflags;
    uae_u32 kick_mask;
#if !defined NO_PREFETCH_BUFFER
    /* Fellow sources say this is 4 longwords. Trust Petter... */
    uae_u8 prefetch[16];
    uae_u8 *prefetch_pos;
#endif
} regs, lastint_regs;

#if !defined NO_PREFETCH_BUFFER
#endif

#define m68k_dreg(r,num) ((r).regs[(num)])
#define m68k_areg(r,num) ((r).regs[(num)+8])

#ifndef NO_PREFETCH_BUFFER
extern void fill_prefetch(void);
extern uae_u32 nextibyte(void);
extern uae_u32 nextiword(void);
extern uae_u32 nextilong(void);

#else

#define fill_prefetch() do { (void)0; } while (0)

#ifdef USE_POINTER
static __inline__ uae_u32 nextibyte(void)
{
    uae_u32 r = do_get_mem_byte(regs.pc_p+1);
    regs.pc_p += 2;
    return r;
}

static __inline__ uae_u32 nextiword(void)
{
    uae_u32 r = do_get_mem_word((uae_u16 *)regs.pc_p);
    regs.pc_p += 2;
    return r;
}

static __inline__ uae_u32 nextilong(void)
{
    uae_u32 r = do_get_mem_long((uae_u32 *)regs.pc_p);
    regs.pc_p += 4;
    return r;
}
#else

static __inline__ uae_u32 nextibyte(void)
{
    uae_u32 r = get_byte(regs.pc+1);
    regs.pc += 2;
    return r;
}

static __inline__ uae_u32 nextiword(void)
{
    uae_u32 r = get_word(regs.pc);
    regs.pc += 2;
    return r;
}

static __inline__ uae_u32 nextilong(void)
{
    uae_u32 r = get_long(regs.pc);
    regs.pc += 4;
    return r;
}

#endif
#endif

#ifdef USE_POINTER

#if !defined(USE_COMPILER)
static __inline__ void m68k_setpc(uaecptr newpc)
{
    regs.pc_p = regs.pc_oldp = get_real_address(newpc);
    regs.pc = newpc;
#if USER_PROGRAMS_BEHAVE > 0
    if ((newpc & 0xF80000) != regs.kick_mask)
	regs.spcflags |= SPCFLAG_MODE_CHANGE;
    regs.kick_mask = newpc & 0xF80000;
#endif
    fill_prefetch();
}
#else
extern void m68k_setpc(uaecptr newpc);
#endif

static __inline__ uaecptr m68k_getpc(void)
{
    return regs.pc + ((char *)regs.pc_p - (char *)regs.pc_oldp);
}

#else

static __inline__ void m68k_setpc(uaecptr newpc)
{
    regs.pc = newpc;
}

static __inline__ uaecptr m68k_getpc(void)
{
    return regs.pc;
}
#endif

#ifdef USE_COMPILER
extern void m68k_setpc_fast(uaecptr newpc);
extern void m68k_setpc_bcc(uaecptr newpc);
extern void m68k_setpc_rte(uaecptr newpc);
#else
#define m68k_setpc_fast m68k_setpc
#define m68k_setpc_bcc  m68k_setpc
#define m68k_setpc_rte  m68k_setpc
#endif

static __inline__ void m68k_setstopped(int stop)
{
    regs.stopped = stop;
    if (stop)
	regs.spcflags |= SPCFLAG_STOP;
}

#if CPU_LEVEL > 1
static __inline__ uae_u32 get_disp_ea (uae_u32 base)
{
    uae_u16 dp = nextiword();
    int reg = (dp >> 12) & 15;
    uae_s32 regd = regs.regs[reg];
    if ((dp & 0x800) == 0)
	regd = (uae_s32)(uae_s16)regd;
    regd <<= (dp >> 9) & 3;
    if (dp & 0x100) {
	uae_s32 outer = 0;
	if (dp & 0x80) base = 0;
	if (dp & 0x40) regd = 0;

	if ((dp & 0x30) == 0x20) base += (uae_s32)(uae_s16)nextiword();
	if ((dp & 0x30) == 0x30) base += nextilong();
	
	if ((dp & 0x3) == 0x2) outer = (uae_s32)(uae_s16)nextiword();
	if ((dp & 0x3) == 0x3) outer = nextilong();
	
	if ((dp & 0x4) == 0) base += regd;
	if (dp & 0x3) base = get_long (base);
	if (dp & 0x4) base += regd;
	
	return base + outer;
    } else {
	return base + (uae_s32)((uae_s8)dp) + regd;
    }
}
#else
static __inline__ uae_u32 get_disp_ea (uae_u32 base)
{
    uae_u16 dp = nextiword();
    int reg = (dp >> 12) & 15;
    uae_s32 regd = regs.regs[reg];
    if ((dp & 0x800) == 0)
	regd = (uae_s32)(uae_s16)regd;
    return base + (uae_s8)(dp) + regd;
}
#endif

extern uae_s32 ShowEA(int reg, amodes mode, wordsizes size, char *buf);

extern void MakeSR(void);
extern void MakeFromSR(void);
extern void Exception(int, uaecptr);
extern void dump_counts(void);
extern void m68k_move2c(int, uae_u32 *);
extern void m68k_movec2(int, uae_u32 *);
extern void m68k_divl (uae_u32, uae_u32, uae_u16, uaecptr);
extern void m68k_mull (uae_u32, uae_u32, uae_u16);
extern void init_m68k (void);
extern void m68k_go(int);
extern void m68k_dumpstate(uaecptr *);
extern void m68k_disasm(uaecptr,uaecptr *,int);
extern void m68k_reset(void);

extern void mmu_op (uae_u32, uae_u16);

extern void fpp_opp (uae_u32, uae_u16);
extern void fdbcc_opp (uae_u32, uae_u16);
extern void fscc_opp (uae_u32, uae_u16);
extern void ftrapcc_opp (uae_u32,uaecptr);
extern void fbcc_opp (uae_u32, uaecptr, uae_u32);
extern void fsave_opp (uae_u32);
extern void frestore_opp (uae_u32);

#ifdef MEMFUNCS_DIRECT_REQUESTED
#define CPU_OP_NAME(a) op_direct ## a
#else
#define CPU_OP_NAME(a) op ## a
#endif

extern struct cputbl op_smalltbl[];
extern struct cputbl op_direct_smalltbl[];

extern cpuop_func *cpufunctbl[65536] ASM_SYM_FOR_FUNC ("cpufunctbl");
#if USER_PROGRAMS_BEHAVE > 0
extern cpuop_func *cpufunctbl_behaved[65536] ASM_SYM_FOR_FUNC ("cpufunctbl_behaved");
#endif
