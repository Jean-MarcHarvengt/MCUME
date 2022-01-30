 /*
  * UAE - The Un*x Amiga Emulator
  * 
  * Custom chip emulation
  * 
  * (c) 1995 Bernd Schmidt, Alessandro Bissacco
  */

#include "shared.h"

#include "gensound.h"
#include "sounddep/sound.h"
#include "events.h"
#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "blitter.h"
#include "blit.h"

uae_u16 bltsize, oldvblts;
uae_u16 bltcon0,bltcon1;
uae_u32 bltapt,bltbpt,bltcpt,bltdpt;

int blinea_shift;
static uae_u16 blitlpos, blinea, blineb;
static uaecptr bltcnxlpt,bltdnxlpt;
static int blitline,blitfc,blitfill,blitife,blitdesc,blitsing;
static int blitonedot,blitsign;
static long int bltwait;

struct bltinfo blt_info;

static uae_u8 blit_filltable[256][4][2];
uae_u32 blit_masktable[BLITTER_MAX_WORDS];
static uae_u16 blit_trashtable[BLITTER_MAX_WORDS];
enum blitter_states bltstate;

void build_blitfilltable(void)
{
    unsigned int d, fillmask;
    int i;
    
    for (i = 0; i < BLITTER_MAX_WORDS; i++)
	blit_masktable[i] = 0xFFFF;
    
    for (d = 0; d < 256; d++) {
	for (i = 0; i < 4; i++) {
	    int fc = i & 1;
	    uae_u8 data = d;
	    for (fillmask = 1; fillmask != 0x100; fillmask <<= 1) {
		uae_u16 tmp = data;
		if (fc) {
		    if (i & 2)
			data |= fillmask;
		    else
			data ^= fillmask;
		}
		if (tmp & fillmask) fc = !fc;
	    }
	    blit_filltable[d][i][0] = data;
	    blit_filltable[d][i][1] = fc;
	}
    }
}

static __inline__ uae_u8 *blit_xlateptr(uaecptr bltpt, int bytecount)
{
    if (!chipmem_bank.check(bltpt,bytecount)) return NULL;
    return chipmem_bank.xlateaddr(bltpt);
}

static __inline__ uae_u8 *blit_xlateptr_desc(uaecptr bltpt, int bytecount)
{
    if (!chipmem_bank.check(bltpt-bytecount, bytecount)) return NULL;
    return chipmem_bank.xlateaddr(bltpt);
}

static void blitter_dofast(void) 
{
    int i,j;
    uae_u8 *bltadatpt = 0, *bltbdatpt = 0, *bltcdatpt = 0, *bltddatpt = 0;
    uae_u8 mt = bltcon0 & 0xFF;
    
    blit_masktable[0] = blt_info.bltafwm;
    blit_masktable[blt_info.hblitsize - 1] &= blt_info.bltalwm;
    
    if (bltcon0 & 0x800) {
	bltadatpt = blit_xlateptr(bltapt, (blt_info.hblitsize*2+blt_info.bltamod)*blt_info.vblitsize);
	bltapt += (blt_info.hblitsize*2+blt_info.bltamod)*blt_info.vblitsize;
    }
    if (bltcon0 & 0x400) {
	bltbdatpt = blit_xlateptr(bltbpt, (blt_info.hblitsize*2+blt_info.bltbmod)*blt_info.vblitsize);
	bltbpt += (blt_info.hblitsize*2+blt_info.bltbmod)*blt_info.vblitsize;
    }
    if (bltcon0 & 0x200) {
	bltcdatpt = blit_xlateptr(bltcpt, (blt_info.hblitsize*2+blt_info.bltcmod)*blt_info.vblitsize);
	bltcpt += (blt_info.hblitsize*2+blt_info.bltcmod)*blt_info.vblitsize;
    }
    if (bltcon0 & 0x100) {
	bltddatpt = blit_xlateptr(bltdpt, (blt_info.hblitsize*2+blt_info.bltdmod)*blt_info.vblitsize);
	bltdpt += (blt_info.hblitsize*2+blt_info.bltdmod)*blt_info.vblitsize;
    }
    
    if (blitfunc_dofast[mt] && !blitfill) 
        (*blitfunc_dofast[mt])(bltadatpt,bltbdatpt,bltcdatpt,bltddatpt,&blt_info);
    else {
	uae_u32 blitbhold = blt_info.bltbhold;
	uae_u32 preva = 0, prevb = 0;

	/*if (!blitfill) fprintf(stderr, "minterm %x not present\n",mt); */
        for (j = 0; j < blt_info.vblitsize; j++) {
	    blitfc = !!(bltcon1 & 0x4);
	    for (i = 0; i < blt_info.hblitsize; i++) {
		uae_u32 bltadat, blitahold;
	        if (bltadatpt) {
		    bltadat = do_get_mem_word((uae_u16 *)bltadatpt); bltadatpt += 2;
		} else
		    bltadat = blt_info.bltadat;
		bltadat &= blit_masktable[i];
		blitahold = (((uae_u32)preva << 16) | bltadat) >> blt_info.blitashift;
		preva = bltadat;

	        if (bltbdatpt) {
		    uae_u16 bltbdat = do_get_mem_word((uae_u16 *)bltbdatpt); bltbdatpt += 2;
		    blitbhold = (((uae_u32)prevb << 16) | bltbdat) >> blt_info.blitbshift;
		    prevb = bltbdat;
		}
		if (bltcdatpt) {
		    blt_info.bltcdat = do_get_mem_word((uae_u16 *)bltcdatpt); bltcdatpt += 2; 
		}
	        blt_info.bltddat = blit_func(blitahold, blitbhold, blt_info.bltcdat, mt) & 0xFFFF;
	        if (blitfill) {
		    uae_u16 d = blt_info.bltddat;
		    int ifemode = blitife ? 2 : 0;
		    int fc1 = blit_filltable[d & 255][ifemode + blitfc][1];
		    blt_info.bltddat = (blit_filltable[d & 255][ifemode + blitfc][0] 
					+ (blit_filltable[d >> 8][ifemode + fc1][0] << 8));
		    blitfc = blit_filltable[d >> 8][ifemode + fc1][1];
		}
	        if (blt_info.bltddat) blt_info.blitzero = 0;
	        if (bltddatpt) {
		    do_put_mem_word((uae_u16 *)bltddatpt, blt_info.bltddat);
		    bltddatpt += 2;
		}
	    }
	    if (bltadatpt) bltadatpt += blt_info.bltamod;
	    if (bltbdatpt) bltbdatpt += blt_info.bltbmod;
	    if (bltcdatpt) bltcdatpt += blt_info.bltcmod;
	    if (bltddatpt) bltddatpt += blt_info.bltdmod;
        }
	blt_info.bltbhold = blitbhold;
    }
    blit_masktable[0] = 0xFFFF;
    blit_masktable[blt_info.hblitsize - 1] = 0xFFFF;
    
    bltstate = BLT_done;
}

static void blitter_dofast_desc(void) 
{
    int i,j;
    uae_u8 *bltadatpt = 0, *bltbdatpt = 0, *bltcdatpt = 0, *bltddatpt = 0;
    uae_u8 mt = bltcon0 & 0xFF;
    
    blit_masktable[0] = blt_info.bltafwm;
    blit_masktable[blt_info.hblitsize - 1] &= blt_info.bltalwm;
    
    if (bltcon0 & 0x800) {
	bltadatpt = blit_xlateptr_desc(bltapt, (blt_info.hblitsize*2+blt_info.bltamod)*blt_info.vblitsize);
	bltapt -= (blt_info.hblitsize*2+blt_info.bltamod)*blt_info.vblitsize;
    }
    if (bltcon0 & 0x400) {
	bltbdatpt = blit_xlateptr_desc(bltbpt, (blt_info.hblitsize*2+blt_info.bltbmod)*blt_info.vblitsize);
	bltbpt -= (blt_info.hblitsize*2+blt_info.bltbmod)*blt_info.vblitsize;
    }
    if (bltcon0 & 0x200) {
	bltcdatpt = blit_xlateptr_desc(bltcpt, (blt_info.hblitsize*2+blt_info.bltcmod)*blt_info.vblitsize);
	bltcpt -= (blt_info.hblitsize*2+blt_info.bltcmod)*blt_info.vblitsize;
    }
    if (bltcon0 & 0x100) {
	bltddatpt = blit_xlateptr_desc(bltdpt, (blt_info.hblitsize*2+blt_info.bltdmod)*blt_info.vblitsize);
	bltdpt -= (blt_info.hblitsize*2+blt_info.bltdmod)*blt_info.vblitsize;
    }
    if (blitfunc_dofast_desc[mt] && !blitfill) 
	(*blitfunc_dofast_desc[mt])(bltadatpt,bltbdatpt,bltcdatpt,bltddatpt,&blt_info);
    else {
	uae_u32 blitbhold = blt_info.bltbhold;
	uae_u32 preva = 0, prevb = 0;

/*	if (!blitfill) fprintf(stderr, "minterm %x not present\n",mt);*/
        for (j = 0; j < blt_info.vblitsize; j++) {
	    blitfc = !!(bltcon1 & 0x4);
	    for (i = 0; i < blt_info.hblitsize; i++) {
		uae_u32 bltadat, blitahold;
	        if (bltadatpt) {
		    bltadat = do_get_mem_word((uae_u16 *)bltadatpt); bltadatpt -= 2;
		} else
		    bltadat = blt_info.bltadat;
		bltadat &= blit_masktable[i];
		blitahold = (((uae_u32)bltadat << 16) | preva) >> blt_info.blitdownashift;
		preva = bltadat;
	        if (bltbdatpt) {
		    uae_u16 bltbdat = do_get_mem_word((uae_u16 *)bltbdatpt); bltbdatpt -= 2;
		    blitbhold = (((uae_u32)bltbdat << 16) | prevb) >> blt_info.blitdownbshift;
		    prevb = bltbdat;
		}
		if (bltcdatpt) {
		    blt_info.bltcdat = do_get_mem_word((uae_u16 *)bltcdatpt); bltcdatpt -= 2; 
		}
	        blt_info.bltddat = blit_func(blitahold, blitbhold, blt_info.bltcdat, mt) & 0xFFFF;
	        if (blitfill) {
		    uae_u16 d = blt_info.bltddat;
		    int ifemode = blitife ? 2 : 0;
		    int fc1 = blit_filltable[d & 255][ifemode + blitfc][1];
		    blt_info.bltddat = (blit_filltable[d & 255][ifemode + blitfc][0]
					+ (blit_filltable[d >> 8][ifemode + fc1][0] << 8));
		    blitfc = blit_filltable[d >> 8][ifemode + fc1][1];
		}
	        if (blt_info.bltddat) blt_info.blitzero = 0;
	        if (bltddatpt) {
		    do_put_mem_word((uae_u16 *)bltddatpt, blt_info.bltddat);
		    bltddatpt -= 2;
		}
	    }
	    if (bltadatpt) bltadatpt -= blt_info.bltamod;
	    if (bltbdatpt) bltbdatpt -= blt_info.bltbmod;
	    if (bltcdatpt) bltcdatpt -= blt_info.bltcmod;
	    if (bltddatpt) bltddatpt -= blt_info.bltdmod;
        }
	blt_info.bltbhold = blitbhold;
    }
    blit_masktable[0] = 0xFFFF;
    blit_masktable[blt_info.hblitsize - 1] = 0xFFFF;

    bltstate = BLT_done;
}

static __inline__ int blitter_read(void)
{
    if (bltcon0 & 0xe00){
	if (!dmaen(DMA_BLITTER)) return 1; /* blitter stopped */
	if (bltcon0 & 0x200) blt_info.bltcdat = chipmem_bank.wget(bltcpt);
    }
    bltstate = BLT_work;
    return (bltcon0 & 0xE00) != 0;
}

static __inline__ int blitter_write(void)
{
    if (blt_info.bltddat) blt_info.blitzero = 0;
    if ((bltcon0 & 0x100) || blitline){
	if (!dmaen(DMA_BLITTER)) return 1;
	chipmem_bank.wput(bltdpt, blt_info.bltddat);
    }
    bltstate = BLT_next;
    return (bltcon0 & 0x100) != 0;
}

static __inline__ void blitter_line_incx(void)
{
    if (++blinea_shift == 16) {
	blinea_shift = 0;
	bltcnxlpt += 2;
	bltdnxlpt += 2;
    }
}

static __inline__ void blitter_line_decx(void)
{
    if (blinea_shift-- == 0) {
	blinea_shift = 15;
	bltcnxlpt -= 2;
	bltdnxlpt -= 2;
    }
}

static __inline__ void blitter_line_decy(void)
{
    bltcnxlpt -= blt_info.bltcmod;
    bltdnxlpt -= blt_info.bltcmod; /* ??? am I wrong or doesn't KS1.3 set bltdmod? */
    blitonedot = 0;
}

static __inline__ void blitter_line_incy(void)
{
    bltcnxlpt += blt_info.bltcmod;
    bltdnxlpt += blt_info.bltcmod; /* ??? */
    blitonedot = 0;
}

static void blitter_line(void)
{
    uae_u16 blitahold = blinea >> blinea_shift, blitbhold = blineb & 1 ? 0xFFFF : 0, blitchold = blt_info.bltcdat;
    blt_info.bltddat = 0;
    
    if (blitsing && blitonedot) blitahold = 0;
    blitonedot = 1;
    blt_info.bltddat = blit_func(blitahold, blitbhold, blitchold, bltcon0 & 0xFF);
    if (!blitsign){
	bltapt += (uae_s16)blt_info.bltamod;
	if (bltcon1 & 0x10){
	    if (bltcon1 & 0x8)
	    	blitter_line_decy();
	    else
	    	blitter_line_incy();
	} else {
	    if (bltcon1 & 0x8)
	    	blitter_line_decx();
	    else 
	    	blitter_line_incx();
	}
    } else {
	bltapt += (uae_s16)blt_info.bltbmod;
    }
    if (bltcon1 & 0x10){
	if (bltcon1 & 0x4)
	    blitter_line_decx();
	else
	    blitter_line_incx();
    } else {
	if (bltcon1 & 0x4)
	    blitter_line_decy();
	else
	    blitter_line_incy();
    }
    blitsign = 0 > (uae_s16)bltapt;
    bltstate = BLT_write;
}

static __inline__ void blitter_nxline(void)
{
    bltcpt = bltcnxlpt;
    bltdpt = bltdnxlpt;
    blineb = (blineb << 1) | (blineb >> 15);
    if (--blt_info.vblitsize == 0) {
	bltstate = BLT_done;
    } else {
	bltstate = BLT_read;
    }
}

static void blit_init(void)
{
    blitlpos = 0;
    blt_info.blitzero = 1; 
    blitline = bltcon1 & 1;
    blt_info.blitashift = bltcon0 >> 12; 
    blt_info.blitdownashift = 16 - blt_info.blitashift;
    blt_info.blitbshift = bltcon1 >> 12;
    blt_info.blitdownbshift = 16 - blt_info.blitbshift;
    
    if (blitline) {
	if (blt_info.hblitsize != 2) {
	    //sprintf (warning_buffer, "weird hblitsize in linemode: %d\n", blt_info.hblitsize);
	    //write_log (warning_buffer);
	}
	bltcnxlpt = bltcpt;
	bltdnxlpt = bltdpt;
	blitsing = bltcon1 & 0x2;
	blinea = blt_info.bltadat;
	blineb = (blt_info.bltbdat >> blt_info.blitbshift) | (blt_info.bltbdat << (16-blt_info.blitbshift));
#if 0
	if (blineb != 0xFFFF && blineb != 0)
	    fprintf(stderr, "%x %x %d %x\n", blineb, blt_info.bltbdat, blt_info.blitbshift, bltcon1);
#endif
	blitsign = bltcon1 & 0x40; 
	blitonedot = 0;
    } else {
	blitfc = !!(bltcon1 & 0x4);
	blitife = bltcon1 & 0x8;
	blitfill = bltcon1 & 0x18;
	if ((bltcon1 & 0x18) == 0x18) {
	    /* Digital "Trash" demo does this; others too. Apparently, no
	     * negative effects. */
	    static int warn = 1;
	    if (warn)
	    	write_log ("warning: weird fill mode (further messages suppressed)\n");
	    warn = 0;
	}
	blitdesc = bltcon1 & 0x2;
	if (blitfill && !blitdesc)
	    write_log ("warning: blitter fill without desc\n");
    }
}

static void actually_do_blit(void)
{
    if (blitline) {
	do {
	    blitter_read();
	    blitter_line();
	    blitter_write();
	    blitter_nxline();
	} while (bltstate != BLT_done);
    } else {
	/*blitcount[bltcon0 & 0xff]++;  blitter debug */
	if (blitdesc) blitter_dofast_desc();
	else blitter_dofast();
    }
    blitter_done_notify ();
}

void blitter_handler(void)
{
    if (!dmaen(DMA_BLITTER)) {
	eventtab[ev_blitter].active = 1;
	eventtab[ev_blitter].oldcycles = cycles;
	eventtab[ev_blitter].evtime = 10 + cycles; /* wait a little */
	return; /* gotta come back later. */
    }
    actually_do_blit();
    
    INTREQ(0x8040);
    eventtab[ev_blitter].active = 0;
    regs.spcflags &= ~SPCFLAG_BLTNASTY;
}

void do_blitter(void)
{
    long int blit_cycles;
    if (!currprefs.immediate_blits) {
	
	blit_cycles = 2;

	if (!blitline) {
	    if (bltcon0 & 0x400)
		blit_cycles++;
	    if ((bltcon0 & 0x300) == 0x300)
		blit_cycles++;
	    blit_cycles *= blt_info.vblitsize * blt_info.hblitsize;
	}
    } else	
	blit_cycles = 1;

    blit_init();
	
    eventtab[ev_blitter].active = 1;
    eventtab[ev_blitter].oldcycles = cycles;
    eventtab[ev_blitter].evtime = blit_cycles + cycles;
    events_schedule();

    if (dmaen(DMA_BLITPRI))
	regs.spcflags |= SPCFLAG_BLTNASTY;
}

void maybe_blit(void)
{
    static int warned = 0;
    if (bltstate == BLT_done)
	return;
    
    if (!warned) {
		warned = 1;
		write_log ("warning: Program does not wait for blitter\n");
    }
    if (!eventtab[ev_blitter].active) {
		emu_printf("FOO!!?\n");
	}
    actually_do_blit();
    eventtab[ev_blitter].active = 0;
    regs.spcflags &= ~SPCFLAG_BLTNASTY;
}

