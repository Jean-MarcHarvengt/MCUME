 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * MC68000 emulation
  *
  * (c) 1995 Bernd Schmidt
  */

#include "shared.h"

#include "gensound.h"
#include "sounddep/sound.h"
#include "events.h"
#include "uae.h"
#include "machdep/m68k.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "autoconf.h"
#ifdef HAS_ERSATZ  
#include "ersatz.h"
#endif
//#include "debug.h"
#include "compiler.h"
#include "arduinoproto.h"

int areg_byteinc[] = { 1,1,1,1,1,1,1,2 };
int imm8_table[] = { 8,1,2,3,4,5,6,7 };

int movem_index1[256];
int movem_index2[256];
int movem_next[256];

int fpp_movem_index1[256];
int fpp_movem_index2[256];
int fpp_movem_next[256];

//#define GENTABLE

#ifdef GENTABLE
cpuop_func *cpufunctbl[65536];
#else
#include "cputbl.h"
#include "cpugentbl.h"
#endif


#ifndef NO_PREFETCH_BUFFER
/* Oh boy does this suck... */
void fill_prefetch(void)
{
    /* We can't use clever code on all machines. */
    memcpy (regs.prefetch, regs.pc_p, sizeof regs.prefetch);
    regs.prefetch_pos = regs.prefetch;
}

uae_u32 nextibyte(void)
{
    uae_u32 v = do_get_mem_byte (regs.prefetch_pos + 1);
    *(uae_u16 *)regs.prefetch_pos = *(uae_u16 *)(regs.pc_p + 16);
    regs.pc_p += 2;
    regs.prefetch_pos += 2;
    if (regs.prefetch_pos == regs.prefetch + 16)
	regs.prefetch_pos = regs.prefetch;
    return v;
}

uae_u32 nextiword(void)
{
    uae_u32 v = do_get_mem_word ((uae_u16 *)regs.prefetch_pos);
    *(uae_u16 *)regs.prefetch_pos = *(uae_u16 *)(regs.pc_p + 16);
    regs.pc_p += 2;
    regs.prefetch_pos += 2;
    if (regs.prefetch_pos == regs.prefetch + 16)
	regs.prefetch_pos = regs.prefetch;
    return v;
}

uae_u32 nextilong(void)
{
    uae_u32 v;
    if (regs.prefetch_pos == regs.prefetch + 14) {
	v = do_get_mem_word ((uae_u16 *)regs.prefetch_pos);
	v <<= 16;
	v |= do_get_mem_word ((uae_u16 *)regs.prefetch);
	*(uae_u16 *)regs.prefetch_pos = *(uae_u16 *)(regs.pc_p + 16);
	*(uae_u16 *)regs.prefetch = *(uae_u16 *)(regs.pc_p + 18);
	regs.pc_p += 4;
	regs.prefetch_pos = regs.prefetch + 2;
    } else {
	v = do_get_mem_long ((uae_u32 *)regs.prefetch_pos);
	*(uae_u16 *)regs.prefetch_pos = *(uae_u16 *)(regs.pc_p + 16);
	*(uae_u16 *)(regs.prefetch_pos+2) = *(uae_u16 *)(regs.pc_p + 18);	
	regs.pc_p += 4;
	regs.prefetch_pos += 4;
	if (regs.prefetch_pos == regs.prefetch + 16)
	    regs.prefetch_pos = regs.prefetch;
    }
    return v;
}
#endif

uaecptr m68k_currentpc(void)
{
    return m68k_getpc();
}



#ifdef GENTABLE

#include <stdio.h>
#include <stdlib.h>

int search(void * code)
{
	int k;
	for (k = 0; op_smalltbl[k].handler != NULL; k++) {
		if ((void*)op_smalltbl[k].handler == code)
			return op_smalltbl[k].opcode;
	}	
	return -1;
		
}
#endif


void init_m68k (void)
{
    int i,j;
    
    for (i = 0 ; i < 256 ; i++) {
		for (j = 0 ; j < 8 ; j++) {
			if (i & (1 << j)) break;
		}
		movem_index1[i] = j;
		movem_index2[i] = 7-j;
		movem_next[i] = i & (~(1 << j));
    }

    for (i = 0 ; i < 256 ; i++) {
		for (j = 7 ; j >= 0 ; j--) {
			if (i & (1 << j)) break;
		}
		fpp_movem_index1[i] = j;
		fpp_movem_index2[i] = 7-j;
		fpp_movem_next[i] = i & (~(1 << j));
    }

#ifdef GENTABLE
    long int opcode;
    write_log("Building CPU table...\n");
    read_table68k ();
    do_merges ();
    for (opcode = 0; opcode < 65536; opcode++)
	cpufunctbl[opcode] = op_illg;
    for (i = 0; op_smalltbl[i].handler != NULL; i++) {
	if (!op_smalltbl[i].specific)
	    cpufunctbl[op_smalltbl[i].opcode] = op_smalltbl[i].handler;
    }
    for (opcode = 0; opcode < 65536; opcode++) {
	cpuop_func *f;
	
	if (table68k[opcode].mnemo == i_ILLG)
	    continue;
	
	if (table68k[opcode].handler != -1) {
	    f = cpufunctbl[table68k[opcode].handler];
	    if (f == op_illg)
		abort();
	    cpufunctbl[opcode] = f;
	}
    }	
    for (i = 0; op_smalltbl[i].handler != NULL; i++) {
	if (op_smalltbl[i].specific)
	    cpufunctbl[op_smalltbl[i].opcode] = op_smalltbl[i].handler;
    }


	FILE *fp_wr = stdout;
	if ((fp_wr = fopen ("cpugentbl.h", "wb")) == NULL)
	{
		exit;
	}

	int cnt=0;
	int size = 65536;
	fprintf(fp_wr, "const cpuop_func * cpufunctbl[%d] = {\n", size);

	cnt = 0;
	for (int i = 0; i < size; i++) {
	cnt++;
	int code = search((void *)cpufunctbl[i]);
	if (cnt == 16) {
	  if (code == -1) fprintf(fp_wr, "op_illg,\n");
	  else if (code < 0x10) fprintf(fp_wr, "op_%01x,\n",code);
	  else if (code < 0x100) fprintf(fp_wr, "op_%02x,\n",code);
	  else if (code < 0x1000) fprintf(fp_wr, "op_%03x,\n",code);
	  else fprintf(fp_wr, "op_%04X,\n",code);
	}  
	else {
	  if (code == -1) fprintf(fp_wr, "op_illg,\n");
	  else if (code < 0x10) fprintf(fp_wr, "op_%01x,",code);
	  else if (code < 0x100) fprintf(fp_wr, "op_%02x,",code);
	  else if (code < 0x1000) fprintf(fp_wr, "op_%03x,",code);
	  else fprintf(fp_wr, "op_%04x,",code);
	}  
	cnt &= 15;
	}  
	fprintf(fp_wr, "};\n");
	fclose (fp_wr);
#endif
}


struct flag_struct regflags;
struct regstruct regs, lastint_regs;
int lastint_no;
int broken_in;



void MakeSR(void)
{
    regs.sr = ((regs.t1 << 15) | (regs.t0 << 14)
	       | (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
	       | (XFLG << 4) | (NFLG << 3) | (ZFLG << 2) | (VFLG << 1) 
	       |  CFLG);
}

void MakeFromSR(void)
{
    int oldm = regs.m;
    int olds = regs.s;

    regs.t1 = (regs.sr >> 15) & 1;
    regs.t0 = (regs.sr >> 14) & 1;
    regs.s = (regs.sr >> 13) & 1;
    regs.m = (regs.sr >> 12) & 1;
    regs.intmask = (regs.sr >> 8) & 7;
    XFLG = (regs.sr >> 4) & 1;
    NFLG = (regs.sr >> 3) & 1;
    ZFLG = (regs.sr >> 2) & 1;
    VFLG = (regs.sr >> 1) & 1;
    CFLG = regs.sr & 1;
    if (CPU_LEVEL >= 2) {
	if (olds != regs.s) {
 	    if (olds) {
 	        if (oldm)
 		    regs.msp = m68k_areg(regs, 7);
 	        else
 		    regs.isp = m68k_areg(regs, 7);
 	        m68k_areg(regs, 7) = regs.usp;
 	    } else {
 	        regs.usp = m68k_areg(regs, 7);
 	        m68k_areg(regs, 7) = regs.m ? regs.msp : regs.isp;
 	    }
	} else if (olds && oldm != regs.m) {
	    if (oldm) {
		regs.msp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.isp;
	    } else {
		regs.isp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.msp;
	    }
	}
    } else {
	if (olds != regs.s) {
 	    if (olds) {
 		regs.isp = m68k_areg(regs, 7);
 	        m68k_areg(regs, 7) = regs.usp;
 	    } else {
 	        regs.usp = m68k_areg(regs, 7);
 	        m68k_areg(regs, 7) = regs.isp;
 	    }
 	}
    }
    
    regs.spcflags |= SPCFLAG_INT;
    if (regs.t1 || regs.t0)
    	regs.spcflags |= SPCFLAG_TRACE;
    else
    	regs.spcflags &= ~(SPCFLAG_TRACE | SPCFLAG_DOTRACE);
}
/* Opcode of faulting instruction */
uae_u16 last_op_for_exception_3;
/* PC at fault time */
uaecptr last_addr_for_exception_3;
/* Address that generated the exception */
uaecptr last_fault_for_exception_3;

void Exception(int nr, uaecptr oldpc)
{
    compiler_flush_jsr_stack();
    MakeSR();
    if (!regs.s) {
	regs.usp = m68k_areg(regs, 7);
	if (CPU_LEVEL >= 2)
 	    m68k_areg(regs, 7) = regs.m ? regs.msp : regs.isp;
 	else
 	    m68k_areg(regs, 7) = regs.isp;
	regs.s = 1;
    }
    if (CPU_LEVEL > 0) {
	if (nr == 2 || nr == 3) {
	    int i;
	    /* @@@ this is probably wrong (?) */
	    for (i = 0 ; i < 12 ; i++) {
		m68k_areg(regs, 7) -= 2;
		put_word (m68k_areg(regs, 7), 0);
	    }
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0xa000 + nr * 4);
	} else if (nr ==5 || nr == 6 || nr == 7 || nr == 9) {
 	    m68k_areg(regs, 7) -= 4;
 	    put_long (m68k_areg(regs, 7), oldpc);
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), 0x2000 + nr * 4);
 	} else if (regs.m && nr >= 24 && nr < 32) {
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), nr * 4);
 	    m68k_areg(regs, 7) -= 4;
 	    put_long (m68k_areg(regs, 7), m68k_getpc ());
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), regs.sr);
 	    regs.sr |= (1 << 13);
 	    regs.msp = m68k_areg(regs, 7);
 	    m68k_areg(regs, 7) = regs.isp;
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), 0x1000 + nr * 4);
 	} else {
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), nr * 4);
 	}
    } else {
	if (nr == 2 || nr == 3) {
	    m68k_areg(regs, 7) -= 12;
	    /* ??????? */
	    if (nr == 3) {
		put_long (m68k_areg(regs, 7), last_fault_for_exception_3);
		put_word (m68k_areg(regs, 7)+4, last_op_for_exception_3);
		put_long (m68k_areg(regs, 7)+8, last_addr_for_exception_3);
	    }
	    write_log ("Exception!\n");
	    goto kludge_me_do;
	}
    }
    m68k_areg(regs, 7) -= 4;
    put_long (m68k_areg(regs, 7), m68k_getpc ());
kludge_me_do:
    m68k_areg(regs, 7) -= 2;
    put_word (m68k_areg(regs, 7), regs.sr);
    m68k_setpc(get_long(regs.vbr + 4*nr));
    regs.t1 = regs.t0 = regs.m = 0;
    regs.spcflags &= ~(SPCFLAG_TRACE | SPCFLAG_DOTRACE);
}

static void Interrupt(int nr)
{
    //assert(nr < 8 && nr >= 0);
    lastint_regs = regs;
    lastint_no = nr;
    Exception(nr+24, 0);
    
    regs.intmask = nr;
    regs.spcflags |= SPCFLAG_INT;
}



static __inline__ void
mul_unsigned(uae_u32 src1, uae_u32 src2, uae_u32 *dst_hi, uae_u32 *dst_lo)
{
        uae_u32 r0 = (src1 & 0xffff) * (src2 & 0xffff);
        uae_u32 r1 = ((src1 >> 16) & 0xffff) * (src2 & 0xffff);
        uae_u32 r2 = (src1 & 0xffff) * ((src2 >> 16) & 0xffff);
        uae_u32 r3 = ((src1 >> 16) & 0xffff) * ((src2 >> 16) & 0xffff);
	uae_u32 lo;

        lo = r0 + ((r1 << 16) & 0xffff0000ul);
        if (lo < r0) r3++;
        r0 = lo;
        lo = r0 + ((r2 << 16) & 0xffff0000ul);
        if (lo < r0) r3++;
        r3 += ((r1 >> 16) & 0xffff) + ((r2 >> 16) & 0xffff);
	*dst_lo = lo;
	*dst_hi = r3;
}

void m68k_mull (uae_u32 opcode, uae_u32 src, uae_u16 extra)
{

    if (extra & 0x800) {
	/* signed variant */
	uae_s32 src1,src2;
	uae_u32 dst_lo,dst_hi;
	uae_u32 sign;

	src1 = (uae_s32)src;
	src2 = (uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	sign = (src1 ^ src2);
	if (src1 < 0) src1 = -src1;
	if (src2 < 0) src2 = -src2;
	mul_unsigned((uae_u32)src1,(uae_u32)src2,&dst_hi,&dst_lo);
	if (sign & 0x80000000) {
		dst_hi = ~dst_hi;
		dst_lo = -dst_lo;
		if (dst_lo == 0) dst_hi++;
	}
	VFLG = CFLG = 0;
	ZFLG = dst_hi == 0 && dst_lo == 0;
	NFLG = ((uae_s32)dst_hi) < 0;
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if ((dst_hi != 0 || (dst_lo & 0x80000000) != 0)
		 && ((dst_hi & 0xffffffff) != 0xffffffff
		     || (dst_lo & 0x80000000) != 0x80000000)) 
	{
	    VFLG = 1;
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    } else {
	/* unsigned */
	uae_u32 dst_lo,dst_hi;

	mul_unsigned(src,(uae_u32)m68k_dreg(regs, (extra >> 12) & 7),&dst_hi,&dst_lo);
	
	VFLG = CFLG = 0;
	ZFLG = dst_hi == 0 && dst_lo == 0;
	NFLG = ((uae_s32)dst_hi) < 0;
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if (dst_hi != 0) {
	    VFLG = 1;
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    }

}

void m68k_reset(void)
{
    m68k_areg(regs, 7) = get_long(0x00f80000);
    m68k_setpc(get_long(0x00f80004));
    regs.kick_mask = 0xF80000;
    regs.s = 1;
    regs.m = 0;
    regs.stopped = 0;
    regs.t1 = 0;
    regs.t0 = 0;
    ZFLG = CFLG = NFLG = VFLG = 0;
    regs.spcflags = 0;
    regs.intmask = 7;
    regs.vbr = regs.sfc = regs.dfc = 0;
    regs.fpcr = regs.fpsr = regs.fpiar = 0;
    customreset();
}

void REGPARAM2 op_illg(uae_u32 opcode)
{
    compiler_flush_jsr_stack();
    if (opcode == 0x4E7B && get_long(0x10) == 0 
	&& (m68k_getpc() & 0xF80000) == 0xF80000) 
    {
	write_log("Your Kickstart requires a 68020 CPU. Giving up.\n");
	broken_in = 1; 
	regs.spcflags |= SPCFLAG_BRK;
	quit_program = 1;
    }
    if (opcode == 0xFF0D) {
	if ((m68k_getpc() & 0xF80000) == 0xF80000) {
	    /* This is from the dummy Kickstart replacement */
#ifdef HAS_ERSATZ  	    
	    ersatz_perform (nextiword ());
#endif	    
	    return;
	} else if ((m68k_getpc() & 0xF80000) == 0xF00000) {
	    /* User-mode STOP replacement */
	    m68k_setstopped(1);
	    return;
	}
    }
#ifdef USE_POINTER
    regs.pc_p -= 2;
#else
    regs.pc -= 2;
#endif
    fill_prefetch();
    if ((opcode & 0xF000) == 0xF000) {
	Exception(0xB,0);
	return;
    }
    if ((opcode & 0xF000) == 0xA000) {
    	Exception(0xA,0);
	return;
    }
    sprintf (warning_buffer, "Illegal instruction: %04x at %08lx\n", opcode, m68k_getpc());
    write_log (warning_buffer);
    Exception(4,0);
}

void mmu_op(uae_u32 opcode, uae_u16 extra)
{
    if ((extra & 0xB000) == 0) { /* PMOVE instruction */

    } else if ((extra & 0xF000) == 0x2000) { /* PLOAD instruction */
    } else if ((extra & 0xF000) == 0x8000) { /* PTEST instruction */
    } else
	op_illg(opcode);
}


static int caar, cacr;

void m68k_move2c (int regno, uae_u32 *regp)
{
    if (CPU_LEVEL == 1 && (regno & 0x7FF) > 1)
	op_illg (0x4E7B);
    else
	switch (regno) {
	 case 0: regs.sfc = *regp & 7; break;
	 case 1: regs.dfc = *regp & 7; break;
	 case 2: cacr = *regp & 0x3; break;	/* ignore C and CE */
	 case 0x800: regs.usp = *regp; break;
	 case 0x801: regs.vbr = *regp; break;
	 case 0x802: caar = *regp &0xfc; break;
	 case 0x803: regs.msp = *regp; if (regs.m == 1) m68k_areg(regs, 7) = regs.msp; break;
	 case 0x804: regs.isp = *regp; if (regs.m == 0) m68k_areg(regs, 7) = regs.isp; break;
	 default:
	    op_illg (0x4E7B);
	    break;
	}
}

void m68k_movec2 (int regno, uae_u32 *regp)
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
	 case 0x803: *regp = regs.m == 1 ? m68k_areg(regs, 7) : regs.msp; break;
	 case 0x804: *regp = regs.m == 0 ? m68k_areg(regs, 7) : regs.isp; break;
	 default:
	    op_illg (0x4E7A);
	    break;
	}
}

static __inline__ int
div_unsigned(uae_u32 src_hi, uae_u32 src_lo, uae_u32 div, uae_u32 *quot, uae_u32 *rem)
{
	uae_u32 q = 0, cbit = 0;
	int i;

	if (div <= src_hi) {
	    return(1);
	}
	for (i = 0 ; i < 32 ; i++) {
		cbit = src_hi & 0x80000000ul;
		src_hi <<= 1;
		if (src_lo & 0x80000000ul) src_hi++;
		src_lo <<= 1;
		q = q << 1;
		if (cbit || div <= src_hi) {
			q |= 1;
			src_hi -= div;
		}
	}
	*quot = q;
	*rem = src_hi;
	return(0);
}

void m68k_divl (uae_u32 opcode, uae_u32 src, uae_u16 extra, uaecptr oldpc)
{
#if defined(uae_s64)
    if (src == 0) {
	Exception(5,oldpc-2);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	uae_s64 a = (uae_s64)(uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_s64 quot, rem;
	
	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (uae_s64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (uae_s64)(uae_s32)src;
	quot = a / (uae_s64)(uae_s32)src;
	if ((quot & UVAL64(0xffffffff80000000)) != 0
	    && (quot & UVAL64(0xffffffff80000000)) != UVAL64(0xffffffff80000000))
	{
	    VFLG = NFLG = 1;
	    CFLG = 0;
	}
	else {
	    if (((uae_s32)rem < 0) != ((uae_s64)a < 0)) rem = -rem;
	    VFLG = CFLG = 0;
	    ZFLG = ((uae_s32)quot) == 0;
	    NFLG = ((uae_s32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	uae_u64 a = (uae_u64)(uae_u32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_u64 quot, rem;
	
	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (uae_u64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (uae_u64)src;
	quot = a / (uae_u64)src;
	if (quot > 0xffffffffu) {
	    VFLG = NFLG = 1;
	    CFLG = 0;
	}
	else {
	    VFLG = CFLG = 0;
	    ZFLG = ((uae_s32)quot) == 0;
	    NFLG = ((uae_s32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#else
    if (src == 0) {
	Exception(5,oldpc-2);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	uae_s32 lo = (uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_s32 hi = lo < 0 ? -1 : 0;
	uae_s32 save_high;
	uae_u32 quot, rem;
	uae_u32 sign;
	
	if (extra & 0x400) {
	    hi = (uae_s32)m68k_dreg(regs, extra & 7);
	}
	save_high = hi;
	sign = (hi ^ src);
	if (hi < 0) {
		hi = ~hi;
		lo = -lo;
		if (lo == 0) hi++;
	}
	if ((uae_s32)src < 0) src = -src;
	if (div_unsigned(hi, lo, src, &quot, &rem) ||
	    (sign & 0x80000000) ? quot > 0x80000000 : quot > 0x7fffffff) {
	    VFLG = NFLG = 1;
	    CFLG = 0;
	}
	else {
	    if (sign & 0x80000000) quot = -quot;
	    if (((uae_s32)rem < 0) != (save_high < 0)) rem = -rem;
	    VFLG = CFLG = 0;
	    ZFLG = ((uae_s32)quot) == 0;
	    NFLG = ((uae_s32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	uae_u32 lo = (uae_u32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_u32 hi = 0;
	uae_u32 quot, rem;
	
	if (extra & 0x400) {
	    hi = (uae_u32)m68k_dreg(regs, extra & 7);
	}
	if (div_unsigned(hi, lo, src, &quot, &rem)) {
	    VFLG = NFLG = 1;
	    CFLG = 0;
	}
	else {
	    VFLG = CFLG = 0;
	    ZFLG = ((uae_s32)quot) == 0;
	    NFLG = ((uae_s32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#endif
}


static int n_insns=0, n_spcinsns=0;

static uaecptr last_trace_ad = 0;

static __inline__ void do_trace(void)
{
    if (regs.spcflags & SPCFLAG_TRACE) {		/* 6 */
	if (regs.t0) {
	    uae_u16 opcode;
	    /* should also include TRAP, CHK, SR modification FPcc */
	    /* probably never used so why bother */
	    /* We can afford this to be inefficient... */
	    m68k_setpc(m68k_getpc());
	    opcode = get_word(regs.pc);
	    if (opcode == 0x4e72 		/* RTE */
		|| opcode == 0x4e74 		/* RTD */
		|| opcode == 0x4e75 		/* RTS */
		|| opcode == 0x4e77 		/* RTR */
		|| opcode == 0x4e76 		/* TRAPV */
		|| (opcode & 0xffc0) == 0x4e80 	/* JSR */
		|| (opcode & 0xffc0) == 0x4ec0 	/* JMP */
		|| (opcode & 0xff00) == 0x6100  /* BSR */
		|| ((opcode & 0xf000) == 0x6000	/* Bcc */
		    && cctrue((opcode >> 8) & 0xf)) 
		|| ((opcode & 0xf0f0) == 0x5050 /* DBcc */
		    && !cctrue((opcode >> 8) & 0xf) 
		    && (uae_s16)m68k_dreg(regs, opcode & 7) != 0)) 
	    {
		last_trace_ad = m68k_getpc();
		regs.spcflags &= ~SPCFLAG_TRACE;
		regs.spcflags |= SPCFLAG_DOTRACE;
	    }
	} else if (regs.t1) {
	    last_trace_ad = m68k_getpc();
	    regs.spcflags &= ~SPCFLAG_TRACE;
	    regs.spcflags |= SPCFLAG_DOTRACE;
	}
    }
}

static void m68k_run_1(void)
{
    for(;;) {
	if (quit_program > 0) {
	    if (quit_program == 1)
		break;
	    //quit_program = 0;
	    //m68k_reset();
	}    	
	uae_u32 opcode = nextiword();

	/* assert (!regs.stopped && !(regs.spcflags & SPCFLAG_STOP)); */
/*	regs_backup[backup_pointer = (backup_pointer + 1) % 16] = regs;*/
	(*cpufunctbl[opcode])(opcode);
#ifndef NO_EXCEPTION_3
	if (buserr) {
	    last_op_for_exception_3 = opcode;
	    Exception(3,0);
	    buserr = 0;
	}
#endif
	/*n_insns++;*/
	do_cycles();
	if (regs.spcflags) {
	    /*n_spcinsns++;*/
	    while (regs.spcflags & SPCFLAG_BLTNASTY) {
		do_cycles();
		if (regs.spcflags & SPCFLAG_DISK)
		    do_disk();
	    }
	    run_compiled_code();
	    if (regs.spcflags & SPCFLAG_DOTRACE) {
		Exception(9,last_trace_ad);
	    }
	    while (regs.spcflags & SPCFLAG_STOP) {
		do_cycles();
		if (regs.spcflags & SPCFLAG_DISK)
		    do_disk();
		if (regs.spcflags & (SPCFLAG_INT | SPCFLAG_DOINT)){
		    int intr = intlev();
		    regs.spcflags &= ~(SPCFLAG_INT | SPCFLAG_DOINT);
		    if (intr != -1 && intr > regs.intmask) {
			Interrupt(intr);
			regs.stopped = 0;
			regs.spcflags &= ~SPCFLAG_STOP;
		    }
		}
	    }
	    do_trace();
#ifdef WANT_SLOW_MULTIPLY
	    /* Kludge for Hardwired demo. The guys who wrote it should be
	     * mutilated. */
	    if (regs.spcflags & SPCFLAG_EXTRA_CYCLES) {
		do_cycles (); do_cycles (); do_cycles (); do_cycles ();
		regs.spcflags &= ~SPCFLAG_EXTRA_CYCLES;
	    }
#endif
	    if (regs.spcflags & SPCFLAG_DISK)
		do_disk();

	    if (regs.spcflags & SPCFLAG_DOINT) {
		int intr = intlev();
		regs.spcflags &= ~SPCFLAG_DOINT;
		if (intr != -1 && intr > regs.intmask) {
		    Interrupt(intr);
		    regs.stopped = 0;
		}	    
	    }
	    if (regs.spcflags & SPCFLAG_INT) {
		regs.spcflags &= ~SPCFLAG_INT;
		regs.spcflags |= SPCFLAG_DOINT;
	    }
	    if (regs.spcflags & (SPCFLAG_BRK|SPCFLAG_MODE_CHANGE)) {
		regs.spcflags &= ~(SPCFLAG_BRK|SPCFLAG_MODE_CHANGE);
		return;
	    }
	}
    }
}




#define m68k_run1 m68k_run_1

int in_m68k_go = 0;

void m68k_go(int may_quit)
{
    if (in_m68k_go || !may_quit) {
	write_log("Bug! m68k_go is not reentrant.\n");
	abort();
    }

    in_m68k_go++;
    for(;;) {
		if (quit_program > 0) {
		    if (quit_program == 1)
			break;
		    quit_program = 0;
		    m68k_reset();
		}
	    m68k_run1();
    }
    in_m68k_go--;
}



