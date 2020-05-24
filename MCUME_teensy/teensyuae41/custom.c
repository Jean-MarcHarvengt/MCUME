 /*
  * UAE - The Un*x Amiga Emulator
  * 
  * Custom chip emulation
  * 
  * (c) 1995 Bernd Schmidt, Alessandro Bissacco
  */

#include "shared.h"

#include <ctype.h>
#include "events.h"
#include "memory.h"
#include "custom.h"
#include "cia.h"
#include "disk.h"
#include "blit.h"
#include "xwin.h"
#include "keybuf.h"

#define DMA_AUD0      0x0001
#define DMA_AUD1      0x0002
#define DMA_AUD2      0x0004
#define DMA_AUD3      0x0008
#define DMA_DISK      0x0010
#define DMA_SPRITE    0x0020
#define DMA_BLITTER   0x0040
#define DMA_COPPER    0x0080
#define DMA_BITPLANE  0x0100
#define DMA_BLITPRI   0x0400

extern unsigned short * emu_LineBuffer(int line); 
extern void emu_DrawLine16(unsigned short *src, int width , int height, int line); 

#define MAX_PLANES 8

#define PIXEL_XPOS(HPOS) (((HPOS)*2 - 0x30))

/* These are default values for mouse calibration.
 * The first two are default values for mstepx and mstepy.
 * The second line set the orizontal and vertical offset for amiga and X 
 * pointer matching
 */
   
#define defstepx (1<<16)
#define defstepy (1<<16)
#define defxoffs 0
#define defyoffs 0
    
/* Values below define mouse auto calibration process.
 * They are not critical, change them if you want.
 * The most important is calweight, which sets mouse adjustement rate */ 

static const int docal = 60, xcaloff = 40, ycaloff = 20;
static const int calweight = 3;

static int lastsampledmx, lastsampledmy;

 /*
  * Events
  */

unsigned long int cycles, nextevent, nextev_count, specialflags;
int vpos;
UWORD lof;

struct ev eventtab[ev_max];

int copper_active;

static const int dskdelay = 2; /* FIXME: ??? */

 /* 
  * hardware register values that are visible/can be written to by someone
  */

static UWORD cregs[256];

static UWORD dmacon,intena,intreq;
UWORD adkcon; /* used by audio code */

static ULONG cop1lc,cop2lc,copcon;

/* Kludge. FIXME: How does sprite restart after vsync work? */
static int spron[8];
static CPTR sprpt[8];

static ULONG bpl1dat,bpl2dat,bpl3dat,bpl4dat,bpl5dat,bpl6dat,bpl7dat,bpl8dat;
static WORD  bpl1mod,bpl2mod;

xcolnr acolors[64];

UWORD *r_bplpt[MAX_PLANES];
static CPTR bplpt[MAX_PLANES];

/*static int blitcount[256];  blitter debug */

struct bplinfo {
#if AGA_CHIPSET == 0
    /* X86.S will break if this isn't at the beginning of the structure. */
    UWORD color_regs[32];
#else
    ULONG color_regs[256];
#endif
    UWORD bplcon0,bplcon1,bplcon2,bplcon3,bplcon4;
    UWORD diwstrt,diwstop,ddfstrt,ddfstop;

    UWORD sprdata[8], sprdatb[8], sprctl[8], sprpos[8];
    int sprarmed[8];
} bpl_info;

struct line_description
{
    int inborder;
    xcolnr bordercol;
    struct bplinfo bpl_info;
    struct mem_notify_node *mnn;
    CPTR bplpt[MAX_PLANES];
    int linedata_valid;
};

#if SMART_UPDATE == 1
/* 50 words give you 800 horizontal pixels. An A500 can't do that, so it ought
 * to be enough. */
#define MAX_WORDS_PER_LINE 50
static UWORD line_data[numscrlines * 2][MAX_PLANES][MAX_WORDS_PER_LINE];
#endif

static struct line_description * linedescr=NULL;

static UWORD bltsize;
static UWORD bltcon0,bltcon1;
static ULONG bltapt,bltbpt,bltcpt,bltdpt,bltcnxlpt,bltdnxlpt;

static ULONG dskpt;
static UWORD dsklen,dsksync;

static int joy0x, joy1x, joy0y, joy1y;
int joy0button;
UWORD joy0dir;
static int lastspr0x,lastspr0y,lastdiffx,lastdiffy,spr0pos,spr0ctl;
static int mstepx,mstepy,xoffs=defxoffs,yoffs=defyoffs;
static int sprvbfl;

static enum { normal_mouse, dont_care_mouse, follow_mouse } mousestate;

 /*
  * "hidden" hardware registers
  */

int dblpf_ind1[256], dblpf_ind2[256], dblpf_2nd1[256], dblpf_2nd2[256];
#if AGA_CHIPSET == 1
int dblpf_aga1[256], dblpf_aga2[256];
#endif
int dblpfofs[] = { 0, 2, 4, 8, 16, 32, 64, 128 };

static ULONG coplc;
static UWORD copi1,copi2;

static enum {
    COP_stop, COP_read, COP_wait, COP_move, COP_skip
} copstate;

static UWORD oldvblts,blitlpos,blinea,blineb;
static int blitline,blitfc,blitfill,blitife,blitdesc,blitsing;
static int blitonedot,blitsign;

static struct bltinfo blt_info;

static int dsklength;

static enum {
    BLT_done, BLT_init, BLT_read, BLT_work, BLT_write, BLT_next
} bltstate;

int plffirstline,plflastline,plfstrt,plfstop,plflinelen;
int diwfirstword,diwlastword;
int plfpri[3];

int max_diwstop, prev_max_diwstop;

int dskdmaen; /* used in cia.c */
static UWORD dskmfm,dskbyte,dsktime;
static int dsksynced;

int bpldelay1, bpldelay2;
int bplehb, bplham, bpldualpf, bpldualpfpri, bplplanecnt, bplhires;

static int pfield_fullline,pfield_linedone;
static int pfield_linedmaon;
static int pfield_lastpart_hpos,last_sprite;
static int slowline_nextpos, slowline_linepos, slowline_lasttoscr;

union {
    /* Let's try to align this thing. */
    double uupzuq;
    long int cruxmedo;
    unsigned char apixels[1000];
} pixdata;

char spixels[1000]; /* for sprites */
char spixstate[1000]; /* more sprites */

#if AGA_CHIPSET == 1
ULONG aga_linebuf[1000], *aga_lbufptr;
#endif

char linebuffer[400*2];
char *xlinebuffer=&linebuffer[0];
int next_lineno, linetoscreen, line_in_border;

/*
 * Statistics
 */

int bogusframe;

/*
 * helper functions
 */

#if unused
static void pfield_doline_slow(int);
#endif
static void pfield_doline(void);
static void do_sprites(int, int);
static void maybe_blit(void);

int inhibit_frame;
static int framecnt = 0;

static __inline__ void count_frame(void)
{
    if (inhibit_frame) 
    	framecnt = 1;
    else {
		framecnt++;
		if (framecnt >= framerate)
    	    framecnt = 0;
    }
}

static __inline__ void setclr(UWORD *p, UWORD val)
{
    if (val & 0x8000) {
	*p |= val & 0x7FFF;
    } else {
	*p &= ~val;
    }
}

int dmaen(UWORD dmamask)
{
    return (dmamask & dmacon) && (dmacon & 0x200);
}

static __inline__ int current_hpos(void)
{
    return cycles - eventtab[ev_hsync].oldcycles;
}

static void calcdiw(void)
{

	diwfirstword = (bpl_info.diwstrt & 0xFF) - 0x30 - 1;
	diwlastword  = (bpl_info.diwstop & 0xFF) + 0x100 - 0x30 - 1;

    if (diwfirstword < 0) diwfirstword = 0;
    if (diwlastword > max_diwstop) max_diwstop = diwlastword;
    
    plffirstline = bpl_info.diwstrt >> 8;
    plflastline = bpl_info.diwstop >> 8;
#if 0
    /* This happens far too often. */
    if (plffirstline < minfirstline) {
	emu_printf("Warning: Playfield begins before line!\n");
	emu_printi(minfirstline)
	plffirstline = minfirstline;
    }
#endif
    if ((plflastline & 0x80) == 0) plflastline |= 0x100;
#if 0 /* Turrican does this */
    if (plflastline > 313) {
	emu_printf("Warning: Playfield out of range!\n");
	plflastline = 313;
    }
#endif
    plfstrt = bpl_info.ddfstrt;
    plfstop = bpl_info.ddfstop;
    if (plfstrt < 0x18) plfstrt = 0x18;
    if (plfstop > 0xD8) plfstop = 0xD8;
    if (plfstrt > plfstop) plfstrt = plfstop;
    
    /* 
     * Prize question: What are the next lines supposed to be?
     * I can't seem to get it right.
     */
#if 0
    /* Pretty good guess, but wrong. */
    plflinelen = (plfstop-plfstrt+15) & ~7;
    plfstrt &= ~(bplhires ? 3 : 7);
    plfstop &= ~(bplhires ? 3 : 7);
#endif
    /* This actually seems to be correct now... */
    
    /* ! If the masking operation is changed, the pfield_doline code could break
     * on some systems (alignment) */
    plfstrt &= ~3;
    plfstop &= ~3;
    plflinelen = (plfstop-plfstrt+15) & ~7;
}

/*
 * Screen update macros
 */
#define LINE_TO_SCR(NAME, TYPE) \
static void NAME(int pix, int stoppos) \
{ \
    TYPE *buf = (TYPE *)xlinebuffer; \
 \
    while (pix < diwfirstword && pix < stoppos) { \
	*buf++ = acolors[0]; pix++; \
    } \
    if (bplham && bplplanecnt == 6) { \
	/* HAM */ \
	static UWORD lastcolor; \
	if (pix <= diwfirstword) \
	    lastcolor = bpl_info.color_regs[0]; \
	while (pix < diwlastword && pix < stoppos) { \
	    int pv = pixdata.apixels[pix]; \
	    switch(pv & 0x30) { \
	     case 0x00: lastcolor = bpl_info.color_regs[pv]; break; \
	     case 0x10: lastcolor &= 0xFF0; lastcolor |= (pv & 0xF); break; \
	     case 0x20: lastcolor &= 0x0FF; lastcolor |= (pv & 0xF) << 8; break; \
	     case 0x30: lastcolor &= 0xF0F; lastcolor |= (pv & 0xF) << 4; break; \
	    } \
 \
	    if (spixstate[pix]) { \
		*buf++ = acolors[spixels[pix]+16]; \
		spixels[pix] = spixstate[pix] = 0; \
	    } else { \
		*buf++ = xcolors[lastcolor]; \
	    } \
	    pix++; \
	} \
    } else { \
	if (bpldualpf) { \
	    int *lookup = bpldualpfpri ? dblpf_ind2 : dblpf_ind1; \
	    int *lookup_no = bpldualpfpri ? dblpf_2nd2 : dblpf_2nd1; \
	    /* Dual playfield */ \
	    while (pix < diwlastword && pix < stoppos) { \
		int pixcol = pixdata.apixels[pix]; \
		int plno = lookup_no[pixcol]; \
		if (spixstate[pix]) { \
		    int compare = plfpri[plno]; \
		    if (plno == 0 || spixstate[pix] < compare) \
			 *buf++ = acolors[spixels[pix]+16]; \
		    else \
			 *buf++ = acolors[lookup[pixcol]]; \
       		    spixels[pix] = spixstate[pix] = 0; \
		} else { \
		    *buf++ = acolors[lookup[pixcol]]; \
		} \
		pix++; \
	    } \
	} else { \
	    while (pix < diwlastword && pix < stoppos) { \
		ULONG pixcol = pixdata.apixels[pix]; \
		if (spixstate[pix]) { \
		    if (pixcol == 0 || spixstate[pix] < plfpri[2]) \
			*buf++ = acolors[spixels[pix]+16]; \
		    else \
			*buf++ = acolors[pixcol]; \
		    spixels[pix] = spixstate[pix] = 0; \
		} else { \
		    *buf++ = acolors[pixcol]; \
		} \
		pix++; \
	    } \
	} \
    } \
    while (pix < stoppos) { \
	pix++; \
	*buf++ = acolors[0]; \
    } \
    xlinebuffer = (char *)buf; \
}

#if AGA_CHIPSET == 1

/* WARNING: Not too much of this will work correctly yet. */

static void linetoscr_aga(int pix, int stoppos)
{
    ULONG *buf = aga_lbufptr;
    int i;
    int xor = (UBYTE)(bpl_info.bplcon4 >> 8);
    for (i = 0; i < stoppos; i++)
	pixdata.apixels[i] ^= xor;
    
    while (pix < diwfirstword && pix < stoppos) {
	*buf++ = bpl_info.color_regs[0]; pix++;
    }
    if (bplham && bplplanecnt == 6) {
	/* HAM 6 */
	static UWORD lastcolor;
	if (pix <= diwfirstword)
	    lastcolor = bpl_info.color_regs[0];
	while (pix < diwlastword && pix < stoppos) {
	    int pv = pixdata.apixels[pix];
	    switch(pv & 0x30) {
	     case 0x00: lastcolor = bpl_info.color_regs[pv]; break;
	     case 0x10: lastcolor &= 0xF0F000; lastcolor |= (pv & 0xF0); break;
	     case 0x20: lastcolor &= 0x00F0F0; lastcolor |= (pv & 0xF0) << 16; break;
	     case 0x30: lastcolor &= 0xF000F0; lastcolor |= (pv & 0xF0) << 8; break;
	    }

	    if (spixstate[pix]) {
		*buf++ = acolors[spixels[pix]+16];
		spixels[pix] = spixstate[pix] = 0;
	    } else {
		*buf++ = lastcolor;
	    }
	    pix++;
	}
    } else if (bplham && bplplanecnt == 8) {
	/* HAM 8 */
	static ULONG lastcolor;
	if (pix <= diwfirstword)
	    lastcolor = bpl_info.color_regs[0];
	while (pix < diwlastword && pix < stoppos) {
	    int pv = pixdata.apixels[pix];
	    switch(pv & 0x3) {
	     case 0x0: lastcolor = bpl_info.color_regs[pv >> 2]; break;
	     case 0x1: lastcolor &= 0xFFFF03; lastcolor |= (pv & 0xFC); break;
	     case 0x2: lastcolor &= 0x03FFFF; lastcolor |= (pv & 0xFC) << 16; break;
	     case 0x3: lastcolor &= 0xFF03FF; lastcolor |= (pv & 0xFC) << 8; break;
	    }

	    if (spixstate[pix]) {
		*buf++ = acolors[spixels[pix]+16];
		spixels[pix] = spixstate[pix] = 0;
	    } else {
		*buf++ = lastcolor;
	    }
	    pix++;
	}
    } else if (bpldualpf) {
	int *lookup = bpldualpfpri ? dblpf_aga2 : dblpf_aga1;
	int *lookup_no = bpldualpfpri ? dblpf_2nd2 : dblpf_2nd1;
	/* Dual playfield */
	while (pix < diwlastword && pix < stoppos) {
	    int pixcol = pixdata.apixels[pix];
	    int pfno = lookup_no[pixcol];
	    
	    if (spixstate[pix]) {
		ULONG spix = spixels[pix];
		spixels[pix] = spixstate[pix] = 0;
		
	        *buf++ = acolors[spixels[pix]+16];
	    } else {
		int val = lookup[pixdata.apixels[pix]];
		if (pfno == 2) 
		    val += dblpfofs[(bpl_info.bplcon2 >> 10) & 7];
		*buf++ = acolors[val];
	    }
	    pix++;
	}
    } else if (bplehb) {
	while (pix < diwlastword && pix < stoppos) {
            int pixcol = pixdata.apixels[pix];
	    if (spixstate[pix]) {
		if (pixcol == 0 || spixstate[pix] < plfpri[2])
		    *buf++ = acolors[spixels[pix]+16];
		else
		    *buf++ = bpl_info.color_regs[pixcol];
		spixels[pix] = spixstate[pix] = 0;
	    } else {
		*buf++ = bpl_info.color_regs[pixcol];
	    }
	    pix++;
	}
    } else {
	while (pix < diwlastword && pix < stoppos) {
	    int pixcol = pixdata.apixels[pix];
	    if (spixstate[pix]) {
		if (pixcol == 0 || spixstate[pix] < plfpri[2]) 
		    *buf++ = bpl_info.color_regs[spixels[pix]+16];
		else
		    *buf++ = bpl_info.color_regs[pixcol];
		spixels[pix] = spixstate[pix] = 0;
	    } else {
		*buf++ = bpl_info.color_regs[pixcol];
	    }
	    pix++;
	}
    }
    while (pix < stoppos) {
	pix++;
	*buf++ = acolors[0];
    }
    aga_lbufptr = buf;
}
#endif


#define FILL_LINE(NAME, TYPE) \
static void NAME(char *buf) \
{ \
    TYPE *b = (TYPE *)buf; \
    int i;\
    int maxpos = 400; \
    xcolnr col = acolors[0]; \
    for (i = 0; i < maxpos; i++) \
        *b++ = col; \
}


/* The following routines provide support for 2bit displays.
 * Though coded specifically for NeXTStation Mono's they
 * shoud be generically useful.
 */	
#define NX_INITPIXEL 	char bitOffset=8;\
		*b=0;
#define NX_SETPIXEL(col)\
	{\
		if(bitOffset==0)\
			{\
			b ++;\
			bitOffset=6;\
			*b = (col<<6);\
			}\
		else\
			{\
			bitOffset-=2;\
			*b |= (col<<bitOffset);\
			}\
	}



/* Portability hazard ahead... */
LINE_TO_SCR(pfield_linetoscr_16, short)
FILL_LINE(fill_line_16, short)


static __inline__ void fill_line(int y)
{
#ifdef HAS_VBUFFER
    fill_line_16(gfxvidinfo.bufmem + gfxvidinfo.rowbytes * y);
#else
    //fill_line_16(emu_LineBuffer(y));
    fill_line_16(&linebuffer[0]);
#endif    
}


static void pfield_slow_linetoscr(int start, int stop)
{
    int oldstop = stop;
    
    start = PIXEL_XPOS(start);
    if (start < 8)
		start = 8;
    stop = PIXEL_XPOS(stop);
    if (stop > 406)
		stop = 406;
    
    if (start >= stop)
		return;
    slowline_lasttoscr = oldstop;
	pfield_linetoscr_16 (start, stop);
}

/*
 * This function is called whenever a hardware register that controls the
 * screen display is modified. Usually, this routine does nothing. But in
 * some cases, e.g., when a color changes in mid-screen, as in copper-plasma
 * effects, this function switches the update method from the fast full-line
 * update to the much slower single-color-clock update.
 */
static void pfield_may_need_update(int colreg)
{
    /* Ignore, if this happened before or after the DDF window */
    if (framecnt != 0 || !pfield_linedmaon || current_hpos() <= plfstrt
	|| vpos < plffirstline || vpos < minfirstline || vpos >= plflastline)
    {	
    	return;
    }
    /* 
     * If a color reg was modified, it is only important if we are within
     * the DIW.
     */
    if (PIXEL_XPOS(current_hpos()) <= diwfirstword && colreg)
    	return;

    /*
     * If we are past the DDF window, me might as well draw the complete
     * line now.
     */
    if (current_hpos() > plfstrt + plflinelen && pfield_fullline) {
	if (!pfield_linedone)
	    pfield_doline();
	pfield_linedone = 1;
	return;
    }
    	
    do_sprites(vpos, current_hpos());
    if (pfield_fullline) {
	pfield_lastpart_hpos = 0;
	memset(pixdata.apixels, 0, sizeof(pixdata.apixels));
	pfield_fullline = 0;
	slowline_nextpos = -1;
	slowline_linepos = 0;
	slowline_lasttoscr = 0;
    } 
    //else {	
	//	assert(pfield_lastpart_hpos <= current_hpos());
    //}
#ifdef unused    
    int i;
    for (i = pfield_lastpart_hpos; i < current_hpos(); i++) {
	   pfield_doline_slow(i);
    }
#endif    
    if (colreg) {
	pfield_slow_linetoscr(slowline_lasttoscr, current_hpos());
    }
    pfield_lastpart_hpos = current_hpos();
}

/* Apparently, the DMA bit is tested by the hardware at some point,
 * presumably at the ddfstart position, to determine whether it
 * ought to draw the line.
 * This is probably not completely correct, but should not matter
 * very much.
 */
static void pfield_calclinedma(void)
{
    if (current_hpos() >= plfstrt)
    	return;
    
    pfield_linedmaon = dmaen(DMA_BITPLANE);
}

 /* 
  * register functions
  */

static UWORD DMACONR(void)
{
    return (dmacon | (bltstate==BLT_done ? 0 : 0x4000)
	    | (blt_info.blitzero ? 0x2000 : 0));
}
static UWORD INTENAR(void) { return intena; }
static UWORD INTREQR(void) { return intreq; }
static UWORD ADKCONR(void) { return adkcon; }
static UWORD VPOSR(void) 
{
#if AGA_CHIPSET == 1
    return (vpos >> 8) | lof | 0x2300;
#else
#ifdef ECS_AGNUS
    return (vpos >> 8) | lof | 0x2000;
#else
    return (vpos >> 8) | lof;
#endif
#endif
}
static void  VPOSW(UWORD v)  { lof = v & 0x8000; }
static UWORD VHPOSR(void) { return (vpos << 8) | current_hpos(); } 

static void  COP1LCH(UWORD v) { cop1lc= (cop1lc & 0xffff) | ((ULONG)v << 16); }
static void  COP1LCL(UWORD v) { cop1lc= (cop1lc & ~0xffff) | v; }
static void  COP2LCH(UWORD v) { cop2lc= (cop2lc & 0xffff) | ((ULONG)v << 16); }
static void  COP2LCL(UWORD v) { cop2lc= (cop2lc & ~0xffff) | v; }

static void  COPJMP1(UWORD a)
{
    coplc = cop1lc; copstate = COP_read; 
    eventtab[ev_copper].active = 1; eventtab[ev_copper].oldcycles = cycles;
    eventtab[ev_copper].evtime = 4; events_schedule();
    copper_active = 1;
}
static void  COPJMP2(UWORD a)
{
    coplc = cop2lc; copstate = COP_read; 
    eventtab[ev_copper].active = 1; eventtab[ev_copper].oldcycles = cycles;
    eventtab[ev_copper].evtime = 4; events_schedule();
    copper_active = 1;
}

static void  DMACON(UWORD v) 
{
    UWORD oldcon = dmacon;
    setclr(&dmacon,v); dmacon &= 0x1FFF;
    pfield_calclinedma();
    
    /* FIXME? Maybe we need to think a bit more about the master DMA enable 
     * bit in these cases. */
    if ((dmacon & DMA_COPPER) > (oldcon & DMA_COPPER)) { 
	COPJMP1(0);
    }
    if ((dmacon & DMA_SPRITE) > (oldcon & DMA_SPRITE)) {
	int i;
	for (i = 0; i < 8; i++)
	    spron[i] = 1;
    }
    if ((dmacon & DMA_BLITPRI) > (oldcon & DMA_BLITPRI) && bltstate != BLT_done) {
	static int count = 0;
	if (!count) {
	    count = 1;
	    emu_printf("warning: Program is doing blitpri hacks.\n");
	}
	specialflags |= SPCFLAG_BLTNASTY;
    }
    if (copper_active && !eventtab[ev_copper].active) {
	eventtab[ev_copper].active = 1;
	eventtab[ev_copper].oldcycles = cycles;
	eventtab[ev_copper].evtime = 1;
	events_schedule();
    }
}
static void  INTENA(UWORD v) { setclr(&intena,v); specialflags |= SPCFLAG_INT; }
static void  INTREQ(UWORD v) { setclr(&intreq,v); specialflags |= SPCFLAG_INT; }
static void  ADKCON(UWORD v) { setclr(&adkcon,v); }

static void  BPLPTH(UWORD v, int num) { bplpt[num] = (bplpt[num] & 0xffff) | ((ULONG)v << 16); }
static void  BPLPTL(UWORD v, int num) { bplpt[num] = (bplpt[num] & ~0xffff) | (v & 0xFFFE); }

/*
 * I've seen the listing of an example program that changes 
 * from lo- to hires while a line is being drawn. That's
 * awful, but we want to emulate it.
 */
static void  BPLCON0(UWORD v) 
{
    if (bpl_info.bplcon0 == v)
	return;
    pfield_may_need_update(0);
    bpl_info.bplcon0 = v;
    bplhires = (v & 0x8000) == 0x8000;
    bplplanecnt = (v & 0x7000) >> 12;
    bplham = (v & 0x800) == 0x800;
    bpldualpf = (v & 0x400) == 0x400;
    bplehb = (v & 0xFDC0) == 0x6000 && !(bpl_info.bplcon2 & 0x200); /* see below */
    calcdiw(); /* This should go away. */
}
static void  BPLCON1(UWORD v) 
{
    if (bpl_info.bplcon1 == v)
	return;
    pfield_may_need_update(0);
    bpl_info.bplcon1 = v; 
    bpldelay1 = v & 0xF; 
    bpldelay2 = (v >> 4) & 0xF; 
}
static void  BPLCON2(UWORD v) 
{
    if (bpl_info.bplcon2 == v)
	return;
    pfield_may_need_update(0); 
    bpl_info.bplcon2 = v;
    bpldualpfpri = (v & 0x40) == 0x40;
    plfpri[1] = 1 << 2*(v & 7);
    plfpri[2] = 1 << 2*((v>>3) & 7);
    bplehb = (bpl_info.bplcon0 & 0xFDC0) == 0x6000 && !(v & 0x200); /* see above */
}
static void  BPLCON3(UWORD v) 
{
    if (bpl_info.bplcon3 == v)
	return;
    pfield_may_need_update(0); 
    bpl_info.bplcon3 = v; 
}

#if AGA_CHIPSET == 1
static void  BPLCON4(UWORD v) 
{
    if (bpl_info.bplcon4 == v)
	return;
    pfield_may_need_update(0); 
    bpl_info.bplcon4 = v; 
}
#endif

static void  BPL1MOD(UWORD v)
{
    v &= ~1;
    if (bpl1mod == v)
	return;
    pfield_may_need_update(0); 
    bpl1mod = v;
}
static void  BPL2MOD(UWORD v)
{ 
    v &= ~1;
    if (bpl2mod == v)
	return;
    pfield_may_need_update(0); 
    bpl2mod = v;
}

static void  BPL1DAT(UWORD v) { bpl1dat = v; }
static void  BPL2DAT(UWORD v) { bpl2dat = v; }
static void  BPL3DAT(UWORD v) { bpl3dat = v; }
static void  BPL4DAT(UWORD v) { bpl4dat = v; }
static void  BPL5DAT(UWORD v) { bpl5dat = v; }
static void  BPL6DAT(UWORD v) { bpl6dat = v; }

/* We call pfield_may_need_update() from here. Actually, 
 * I have no idea what happens if someone changes ddf or
 * diw mid-line, and I don't really want to know. I doubt
 * that this sort of thing was ever used to create a
 * useful effect.
 */
static void  DIWSTRT(UWORD v) 
{
    if (bpl_info.diwstrt == v)
	return;
    pfield_may_need_update(0);
    bpl_info.diwstrt = v; 
    calcdiw();
}
static void  DIWSTOP(UWORD v)
{
    if (bpl_info.diwstop == v)
	return;
    pfield_may_need_update(0);
    bpl_info.diwstop = v; 
    calcdiw();
}
static void  DDFSTRT(UWORD v)
{ 
    if (bpl_info.ddfstrt == v)
	return;
    pfield_may_need_update(0);
    bpl_info.ddfstrt = v; 
    calcdiw();
}
static void  DDFSTOP(UWORD v) 
{ 
    if (bpl_info.ddfstop == v)
	return;
    pfield_may_need_update(0); 
    bpl_info.ddfstop = v;
    calcdiw();
}

static void  BLTADAT(UWORD v) 
{
    maybe_blit(); 
    blt_info.bltadat = v; 
}
static void  BLTBDAT(UWORD v)
{
    maybe_blit(); 
    blt_info.bltbdat = v; 
}
static void  BLTCDAT(UWORD v) { maybe_blit(); blt_info.bltcdat = v; }

static void  BLTAMOD(UWORD v) { maybe_blit(); blt_info.bltamod = v & 0xFFFE; }
static void  BLTBMOD(UWORD v) { maybe_blit(); blt_info.bltbmod = v & 0xFFFE; }
static void  BLTCMOD(UWORD v) { maybe_blit(); blt_info.bltcmod = v & 0xFFFE; }
static void  BLTDMOD(UWORD v) { maybe_blit(); blt_info.bltdmod = v & 0xFFFE; }

static void  BLTCON0(UWORD v) { maybe_blit(); bltcon0 = v; }
/* The next category is "Most useless hardware register". 
 * And the winner is... */
static void  BLTCON0L(UWORD v) { maybe_blit(); bltcon0 = (bltcon0 & 0xFF00) | (v & 0xFF); }
static void  BLTCON1(UWORD v) { maybe_blit(); bltcon1 = v; }

static void  BLTAFWM(UWORD v) { maybe_blit(); blt_info.bltafwm = v; }
static void  BLTALWM(UWORD v) { maybe_blit(); blt_info.bltalwm = v; }

static void  BLTAPTH(UWORD v) { maybe_blit(); bltapt= (bltapt & 0xffff) | ((ULONG)v << 16); }
static void  BLTAPTL(UWORD v) { maybe_blit(); bltapt= (bltapt & ~0xffff) | (v & 0xFFFE); }
static void  BLTBPTH(UWORD v) { maybe_blit(); bltbpt= (bltbpt & 0xffff) | ((ULONG)v << 16); }
static void  BLTBPTL(UWORD v) { maybe_blit(); bltbpt= (bltbpt & ~0xffff) | (v & 0xFFFE); }
static void  BLTCPTH(UWORD v) { maybe_blit(); bltcpt= (bltcpt & 0xffff) | ((ULONG)v << 16); }
static void  BLTCPTL(UWORD v) { maybe_blit(); bltcpt= (bltcpt & ~0xffff) | (v & 0xFFFE); }
static void  BLTDPTH(UWORD v) { maybe_blit(); bltdpt= (bltdpt & 0xffff) | ((ULONG)v << 16); }
static void  BLTDPTL(UWORD v) { maybe_blit(); bltdpt= (bltdpt & ~0xffff) | (v & 0xFFFE); }
static void  BLTSIZE(UWORD v) 
{
    bltsize = v;
    
    maybe_blit(); 

    blt_info.vblitsize = bltsize >> 6;
    blt_info.hblitsize = bltsize & 0x3F;
    if (!blt_info.vblitsize) blt_info.vblitsize = 1024;
    if (!blt_info.hblitsize) blt_info.hblitsize = 64;
    
    bltstate = BLT_init; 
    specialflags |= SPCFLAG_BLIT; 
}
static void BLTSIZV(UWORD v) 
{
    maybe_blit(); 
    oldvblts = v & 0x7FFF;
}
static void BLTSIZH(UWORD v) 
{
    maybe_blit(); 
    blt_info.hblitsize = v & 0x7FF;
    blt_info.vblitsize = oldvblts;
    if (!blt_info.vblitsize) blt_info.vblitsize = 32768;
    if (!blt_info.hblitsize) blt_info.hblitsize = 0x800;
    bltstate = BLT_init; 
    specialflags |= SPCFLAG_BLIT;
}
static void  SPRxCTL_1(UWORD v, int num)
{
    bpl_info.sprctl[num] = v;
    bpl_info.sprarmed[num] = 0;
    if (bpl_info.sprpos[num] == 0 && v == 0)
	spron[num] = 0;
    else
	spron[num] |= 2;
}
static void  SPRxPOS_1(UWORD v, int num) 
{
    bpl_info.sprpos[num] = v; 
}
static void  SPRxDATA_1(UWORD v, int num)
{
    bpl_info.sprdata[num] = v;
    bpl_info.sprarmed[num] = 1; 
}
static void  SPRxDATB_1(UWORD v, int num) 
{
    bpl_info.sprdatb[num] = v;
}
static void  SPRxCTL(UWORD v, int num) { pfield_may_need_update(0); SPRxCTL_1(v, num); }
static void  SPRxPOS(UWORD v, int num) { pfield_may_need_update(0); SPRxPOS_1(v, num); }
static void  SPRxDATA(UWORD v, int num){ pfield_may_need_update(0); SPRxDATA_1(v, num); }
static void  SPRxDATB(UWORD v, int num){ pfield_may_need_update(0); SPRxDATB_1(v, num); }
static void  SPRxPTH(UWORD v, int num)
{
    sprpt[num] &= 0xffff; 
    sprpt[num] |= (ULONG)v << 16; 
    if (!spron[num]) spron[num] = 1; 
}
static void  SPRxPTL(UWORD v, int num)
{
    sprpt[num] &= ~0xffff; 
    sprpt[num] |= v; 
    if (!spron[num]) spron[num] = 1; 
}

static void  COLOR(UWORD v, int num)
{
    v &= 0xFFF;
#if AGA_CHIPSET == 1
    int r,g,b;
    int cr,cg,cb;
    int colreg;
    {
	ULONG cval;
	colreg = ((bpl_info.bplcon3 >> 13) & 7) * 32 + num;
	r = (v & 0xF00) >> 8;
	g = (v & 0xF0) >> 4;
	b = (v & 0xF) >> 0;
	cr = bpl_info.color_regs[colreg] >> 16;
	cg = (bpl_info.color_regs[colreg] >> 8) & 0xFF;
	cb = bpl_info.color_regs[colreg] & 0xFF;

	if (bpl_info.bplcon3 & 0x200) {
	    cr = 0xF0; cr |= r;
	    cg = 0xF0; cg |= g;
	    cb = 0xF0; cb |= b;
	} else {
	    cr = r + (r << 4);
	    cg = g + (g << 4);
	    cb = b + (b << 4);
	}
	cval = (cr << 16) | (cg << 8) | cb;
	if (cval == bpl_info.color_regs[colreg])
	    return;
	bpl_info.color_regs[colreg] = cval;
	pfield_may_need_update(1);
    }
#else 
    {
	if (bpl_info.color_regs[num] == v)
	    return;
	pfield_may_need_update(1);
	bpl_info.color_regs[num] = v;
	acolors[num] = xcolors[v];
	acolors[num+32] = xcolors[(v >> 1) & 0x777];
    }
#endif
}

static void  DSKSYNC(UWORD v) { dsksync = v; }
static void  DSKDAT(UWORD v) { dskmfm = v; }
static void  DSKPTH(UWORD v) { dskpt = (dskpt & 0xffff) | ((ULONG)v << 16); }
static void  DSKPTL(UWORD v) { dskpt = (dskpt & ~0xffff) | (v); }

static void  DSKLEN(UWORD v) 
{
    if (v & 0x8000) { dskdmaen++; } else { dskdmaen = 0; }
    dsktime = dskdelay; dsksynced = 0;
    dsklen = dsklength = v; dsklength &= 0x3fff;
    if (dskdmaen == 2 && dsksync != 0x4489 && (adkcon & 0x400)) {
	emu_printf("Non-standard sync,len:\n");
	emu_printi(dsksync);
	emu_printi(dsklength);
    }
    if (dsklen & 0x4000) DISK_InitWrite();
    if (dskdmaen) specialflags |= SPCFLAG_DISK;
}

static UWORD DSKBYTR(void)
{
    UWORD v = (dsklen >> 1) & 0x6000;
    v |= dskbyte;
    dskbyte &= ~0x8000;
    if (dsksync == dskmfm) v |= 0x1000;
    return v;
}

static UWORD DSKDATR(void) { return dskmfm; }
static UWORD POTGOR(void)
{
    UWORD v = 0xFFFF;
    if (buttonstate[2])
	v &= 0xFBFF;

    if (buttonstate[1])
	v &= 0xFEFF;
    
    return v;
}
static UWORD POT0DAT(void)
{
    static UWORD cnt = 0;
    if (buttonstate[2])
	cnt = ((cnt + 1) & 0xFF) | (cnt & 0xFF00);
    if (buttonstate[1])
	cnt += 0x100;
    
    return cnt;
}
static UWORD JOY0DAT(void) { return joy0x + (joy0y << 8); }
static UWORD JOY1DAT(void)
{
    return joy0dir;
}
static void JOYTEST(UWORD v)
{
    joy0x = joy1x = v & 0xFC;
    joy0y = joy1y = (v >> 8) & 0xFC;    
}
static void AUD0LCH(UWORD v) { audlc[0] = (audlc[0] & 0xffff) | ((ULONG)v << 16); }
static void AUD0LCL(UWORD v) { audlc[0] = (audlc[0] & ~0xffff) | v; }
static void AUD1LCH(UWORD v) { audlc[1] = (audlc[1] & 0xffff) | ((ULONG)v << 16); }
static void AUD1LCL(UWORD v) { audlc[1] = (audlc[1] & ~0xffff) | v; }
static void AUD2LCH(UWORD v) { audlc[2] = (audlc[2] & 0xffff) | ((ULONG)v << 16); }
static void AUD2LCL(UWORD v) { audlc[2] = (audlc[2] & ~0xffff) | v; }
static void AUD3LCH(UWORD v) { audlc[3] = (audlc[3] & 0xffff) | ((ULONG)v << 16); }
static void AUD3LCL(UWORD v) { audlc[3] = (audlc[3] & ~0xffff) | v; }
static void AUD0PER(UWORD v) { audper[0] = v; }
static void AUD1PER(UWORD v) { audper[1] = v; }
static void AUD2PER(UWORD v) { audper[2] = v; }
static void AUD3PER(UWORD v) { audper[3] = v; }
static void AUD0VOL(UWORD v) { audvol[0] = v & 64 ? 63 : v & 63; }
static void AUD1VOL(UWORD v) { audvol[1] = v & 64 ? 63 : v & 63; }
static void AUD2VOL(UWORD v) { audvol[2] = v & 64 ? 63 : v & 63; }
static void AUD3VOL(UWORD v) { audvol[3] = v & 64 ? 63 : v & 63; }
static void AUD0LEN(UWORD v) { audlen[0] = v; }
static void AUD1LEN(UWORD v) { audlen[1] = v; }
static void AUD2LEN(UWORD v) { audlen[2] = v; }
static void AUD3LEN(UWORD v) { audlen[3] = v; }

static UWORD SERDATR(void) { return 0; }

#ifdef unused
static int copcomp(void)
{
    UWORD vp = vpos & (((copi2 >> 8) & 0x7F) | 0x80);
    UWORD hp = current_hpos() & (copi2 & 0xFE);
    UWORD vcmp = copi1 >> 8;
    UWORD hcmp = copi1 & 0xFE;
    return (vp > vcmp || (vp == vcmp && hp >= hcmp)) && ((copi2 & 0x8000) || !(DMACONR() & 0x4000));
}
#endif

/*
 * Calculate the minimum number of cycles after which the
 * copper comparison becomes true. This is quite tricky. I hope it works.
 */
static int calc_copcomp_true(int currvpos, int currhpos)
{    
    UWORD vp = currvpos & (((copi2 >> 8) & 0x7F) | 0x80);
    UWORD hp = currhpos & (copi2 & 0xFE);
    UWORD vcmp = copi1 >> 8;
    UWORD hcmp = copi1 & 0xFE;
    int copper_time_hpos;
    int cycleadd = maxhpos - currhpos;
    int coptime = 0;

    if ((vp > vcmp || (vp == vcmp && hp >= hcmp)) && ((copi2 & 0x8000) || !(DMACONR() & 0x4000)))
    	return 0;    

    try_again:

    while (vp < vcmp) {
	currvpos++;
	if (currvpos > maxvpos + 1)
	    return -1;
	currhpos = 0;
	coptime += cycleadd;
	cycleadd = maxhpos;
	vp = currvpos & (((copi2 >> 8) & 0x7F) | 0x80);
    }
    if (coptime > 0 && bplhires && bplplanecnt == 4)
	return coptime;
    copper_time_hpos = currhpos;
    hp = copper_time_hpos & (copi2 & 0xFE);
    if (!(vp > vcmp)) {	
	while (hp < hcmp-2) {
	    currhpos++;
	    /* Copper DMA is turned off in Hires 4 bitplane mode */
	    if (!bplhires || bplplanecnt < 4 || !dmaen(DMA_BITPLANE)
		|| currhpos < plfstrt-2 || currhpos > (plfstop+4))
		copper_time_hpos++;
	    
	    if (currhpos > maxhpos-4) {
		/* Now, what? There might be a good position on the
		 * next line. But it can also be the FFFF FFFE
		 * case.
		 */
		currhpos = 0;
		currvpos++;
		vp = currvpos & (((copi2 >> 8) & 0x7F) | 0x80);
		goto try_again;
	    }
	    coptime++;
	    hp = copper_time_hpos & (copi2 & 0xFE);
	}
    }
    if (coptime == 0) /* waiting for the blitter */
    	return 1;

    return coptime;
}

static void copper_read(void)
{
    if (dmaen(DMA_COPPER)){
	copi1 = chipmem_bank.wget(coplc); 
	copi2 = chipmem_bank.wget(coplc+2);
	coplc += 4;
	eventtab[ev_copper].oldcycles = cycles;
	eventtab[ev_copper].evtime = (copi1 & 1) ? (copi2 & 1) ? 10 : 8 : 4;
	copstate = (copi1 & 1) ? (copi2 & 1) ? COP_skip : COP_wait : COP_move;
    } else {
	copstate = COP_read;
	eventtab[ev_copper].active = 0;
    }
}

static void do_copper(void)
{    
    switch(copstate){
     case COP_read:
	copper_read();
	break;
     case COP_move:
	if (copi1 >= (copcon & 2 ? 0x40 : 0x80)) {
	    custom_bank.wput(copi1,copi2);
	    copper_read();
	} else {
	    copstate = COP_stop;
	    eventtab[ev_copper].active = 0;
	    copper_active = 0;
	}
	break;
     case COP_skip:
	if (calc_copcomp_true(vpos, current_hpos()) == 0)
	    coplc += 4;
	copper_read();
	break;
     case COP_wait: {	    
	 int coptime = calc_copcomp_true(vpos, current_hpos());
	 if (coptime < 0) {
	     copstate = COP_stop;
	     eventtab[ev_copper].active = 0;
	     copper_active = 0;
	 } else {	     
	     if (!coptime)
	     	 copper_read();
	     else {
		 eventtab[ev_copper].evtime = coptime;
		 eventtab[ev_copper].oldcycles = cycles;
	     }
	 }
	 break;
     }
     case COP_stop:
	eventtab[ev_copper].active = 0;
	copper_active = 0;
	break;
    }
}

static UBYTE blit_filltable[256][4][2];

static void build_filltable(void)
{
    unsigned int d, fillmask;
    int i;
    for (d = 0; d < 256; d++) {
	for (i = 0; i < 4; i++) {
	    int fc = i & 1;
	    UBYTE data = d;
	    for (fillmask = 1; fillmask != 0x100; fillmask <<= 1) {
		UWORD tmp = data;
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

static __inline__ UWORD *blit_xlateptr(CPTR bltpt, int bytecount)
{
    if (!chipmem_bank.check(bltpt,bytecount)) return NULL;
    return chipmem_bank.xlateaddr(bltpt);
}

static __inline__ UWORD *blit_xlateptr_desc(CPTR bltpt, int bytecount)
{
    if (!chipmem_bank.check(bltpt-bytecount, bytecount)) return NULL;
    return chipmem_bank.xlateaddr(bltpt);
}

static void blitter_dofast(void) 
{
    int i,j;
    UWORD *bltadatpt = 0, *bltbdatpt = 0, *bltcdatpt = 0, *bltddatpt = 0;
    UWORD blitahold, blitbhold, bltaold;
    UBYTE mt = bltcon0 & 0xFF;
    
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
        WORD bltamodw = blt_info.bltamod/2, bltbmodw = blt_info.bltbmod/2, bltcmodw = blt_info.bltcmod/2, bltdmodw = blt_info.bltdmod/2;
	/*if (!blitfill) emu_printf("minterm not present\n"); */
        for (j = 0; j < blt_info.vblitsize; j++) {
	    blitfc = !!(bltcon1 & 0x4);
	    for (i = 0; i < blt_info.hblitsize; i++) {
	        if (bltadatpt) blt_info.bltadat = *bltadatpt++;
	        if (bltbdatpt) blt_info.bltbdat = *bltbdatpt++;
		if (bltcdatpt) blt_info.bltcdat = *bltcdatpt++;
			    //if (bltcdatpt == chipmemory_end) bltcdatpt = chipmemory2;
	        bltaold = blt_info.bltadat;
	        if (i == 0) bltaold &= blt_info.bltafwm;
	        if (i== blt_info.hblitsize-1) bltaold &= blt_info.bltalwm;
	        blitahold = (((ULONG)blt_info.blitpreva << 16) | bltaold) >> blt_info.blitashift;
	        blitbhold = (((ULONG)blt_info.blitprevb << 16) | blt_info.bltbdat) >> blt_info.blitbshift;
	        blt_info.bltddat = blit_func(blitahold, blitbhold, blt_info.bltcdat, mt);
	        if (blitfill) {
		    UWORD d = blt_info.bltddat;
		    int ifemode = blitife ? 2 : 0;
		    int fc1 = blit_filltable[d & 255][ifemode + blitfc][1];
		    blt_info.bltddat = (blit_filltable[d & 255][ifemode + blitfc][0] 
					+ (blit_filltable[d >> 8][ifemode + fc1][0] << 8));
		    blitfc = blit_filltable[d >> 8][ifemode + fc1][1];
		}
	        blt_info.blitpreva = bltaold; blt_info.blitprevb = blt_info.bltbdat;
	        if (blt_info.bltddat) blt_info.blitzero = 0;
	        if (bltddatpt) *bltddatpt++ = blt_info.bltddat;
	    }
	    if (bltadatpt) bltadatpt += bltamodw;
	    if (bltbdatpt) bltbdatpt += bltbmodw;
	    if (bltcdatpt) bltcdatpt += bltcmodw;
	    if (bltddatpt) bltddatpt += bltdmodw;
        }
    }
    bltstate = BLT_done;
}

static void blitter_dofast_desc(void) 
{
    int i,j;
    UWORD *bltadatpt = 0, *bltbdatpt = 0, *bltcdatpt = 0, *bltddatpt = 0;
    UWORD blitahold, blitbhold, bltaold;
    WORD bltamodw = blt_info.bltamod/2, bltbmodw = blt_info.bltbmod/2, bltcmodw = blt_info.bltcmod/2, bltdmodw = blt_info.bltdmod/2; 
    UBYTE mt = bltcon0 & 0xFF;
    
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
        WORD bltamodw = blt_info.bltamod/2, bltbmodw = blt_info.bltbmod/2, bltcmodw = blt_info.bltcmod/2, bltdmodw = blt_info.bltdmod/2; 
/*	if (!blitfill) emu_printf("minterm not present\n");*/
        for (j = 0; j < blt_info.vblitsize; j++) {
	    blitfc = !!(bltcon1 & 0x4);
	    for (i = 0; i < blt_info.hblitsize; i++) {
	        if (bltadatpt) blt_info.bltadat = *bltadatpt--;
	        if (bltbdatpt) blt_info.bltbdat = *bltbdatpt--;
	        if (bltcdatpt) blt_info.bltcdat = *bltcdatpt--;
	        bltaold = blt_info.bltadat;
	        if (i == 0) bltaold &= blt_info.bltafwm;
	        if (i== blt_info.hblitsize-1) bltaold &= blt_info.bltalwm;
	        blitahold = (((ULONG)bltaold << 16) | blt_info.blitpreva) >> (16-blt_info.blitashift);
	        blitbhold = (((ULONG)blt_info.bltbdat << 16) | blt_info.blitprevb) >> (16-blt_info.blitbshift);
	        blt_info.bltddat = blit_func(blitahold, blitbhold, blt_info.bltcdat, mt);
	        if (blitfill) {
		    UWORD d = blt_info.bltddat;
		    int ifemode = blitife ? 2 : 0;
		    int fc1 = blit_filltable[d & 255][ifemode + blitfc][1];
		    blt_info.bltddat = (blit_filltable[d & 255][ifemode + blitfc][0]
					+ (blit_filltable[d >> 8][ifemode + fc1][0] << 8));
		    blitfc = blit_filltable[d >> 8][ifemode + fc1][1];
		}
	        blt_info.blitpreva = bltaold; blt_info.blitprevb = blt_info.bltbdat;
	        if (blt_info.bltddat) blt_info.blitzero = 0;
	        if (bltddatpt) *bltddatpt-- = blt_info.bltddat;
	    }
	    if (bltadatpt) bltadatpt -= bltamodw;
	    if (bltbdatpt) bltbdatpt -= bltbmodw;
	    if (bltcdatpt) bltcdatpt -= bltcmodw;
	    if (bltddatpt) bltddatpt -= bltdmodw;
        }
    }
    bltstate = BLT_done;
}

static int blitter_read(void)
{
    if (bltcon0 & 0xe00){
	if (!dmaen(DMA_BLITTER)) return 1; /* blitter stopped */
	if (!blitline){
	    if (bltcon0 & 0x800) blt_info.bltadat = chipmem_bank.wget(bltapt);
	    if (bltcon0 & 0x400) blt_info.bltbdat = chipmem_bank.wget(bltbpt);
	}
	if (bltcon0 & 0x200) blt_info.bltcdat = chipmem_bank.wget(bltcpt);
    }
    bltstate = BLT_work;
    return (bltcon0 & 0xE00) != 0;
}

static int blitter_write(void)
{
    if (blt_info.bltddat) blt_info.blitzero = 0;
    if ((bltcon0 & 0x100) || blitline){
	if (!dmaen(DMA_BLITTER)) return 1;
	chipmem_bank.wput(bltdpt, blt_info.bltddat);
    }
    bltstate = BLT_next;
    return (bltcon0 & 0x100) != 0;
}

#ifdef unused
static void blitter_blit(void)
{
    UWORD blitahold,blitbhold,blitchold;
    UWORD bltaold;
    
    if (blitdesc) {
	UWORD bltamask = 0xffff;
	
	if (!blitlpos) { bltamask &= blt_info.bltafwm; }
	if (blitlpos == (blt_info.hblitsize - 1)) { bltamask &= blt_info.bltalwm; }
	bltaold = blt_info.bltadat & bltamask;

	blitahold = (((ULONG)bltaold << 16) | blt_info.blitpreva) >> (16-blt_info.blitashift);
	blitbhold = (((ULONG)blt_info.bltbdat << 16) | blt_info.blitprevb) >> (16-blt_info.blitbshift);
	blitchold = blt_info.bltcdat;
    } else {
	UWORD bltamask = 0xffff;
	
	if (!blitlpos) { bltamask &= blt_info.bltafwm; }
	if (blitlpos == (blt_info.hblitsize - 1)) { bltamask &= blt_info.bltalwm; }
	bltaold = blt_info.bltadat & bltamask;

	blitahold = (((ULONG)blt_info.blitpreva << 16) | bltaold) >> blt_info.blitashift;
	blitbhold = (((ULONG)blt_info.blitprevb << 16) | blt_info.bltbdat) >> blt_info.blitbshift;
	blitchold = blt_info.bltcdat;
    }
    blt_info.bltddat = 0;
    blt_info.bltddat = blit_func(blitahold, blitbhold, blitchold, bltcon0 & 0xFF);
    if (blitfill){
	UWORD fillmask;
	for (fillmask = 1; fillmask; fillmask <<= 1){
	    UWORD tmp = blt_info.bltddat;
	    if (blitfc) {
		if (blitife)
		    blt_info.bltddat |= fillmask;
		else
		    blt_info.bltddat ^= fillmask;
	    }
	    if (tmp & fillmask) blitfc = !blitfc;
	}
    }
    bltstate = BLT_write;
    blt_info.blitpreva = bltaold; blt_info.blitprevb = blt_info.bltbdat;
}

static void blitter_nxblit(void)
{
    bltstate = BLT_read;
    if (blitdesc){
	if (++blitlpos == blt_info.hblitsize) {
	    if (--blt_info.vblitsize == 0) {
		bltstate = BLT_done;
#if FAST_BLITTER == 0
		custom_bank.wput(0xDFF09C,0x8040);
#endif
	    }
	    blitfc = bltcon1 & 0x4;

	    blitlpos = 0;
	    if (bltcon0 & 0x800) bltapt -= 2+blt_info.bltamod; 
	    if (bltcon0 & 0x400) bltbpt -= 2+blt_info.bltbmod; 
	    if (bltcon0 & 0x200) bltcpt -= 2+blt_info.bltcmod; 
	    if (bltcon0 & 0x100) bltdpt -= 2+blt_info.bltdmod;
	} else {
	    if (bltcon0 & 0x800) bltapt -= 2; 
	    if (bltcon0 & 0x400) bltbpt -= 2; 
	    if (bltcon0 & 0x200) bltcpt -= 2; 
	    if (bltcon0 & 0x100) bltdpt -= 2;	    
	}
    } else {
	if (++blitlpos == blt_info.hblitsize) {
	    if (--blt_info.vblitsize == 0) { 
		bltstate = BLT_done;
#if FAST_BLITTER == 0
		custom_bank.wput(0xDFF09C,0x8040);
#endif
	    }
	    blitlpos = 0;
	    if (bltcon0 & 0x800) bltapt += 2+blt_info.bltamod; 
	    if (bltcon0 & 0x400) bltbpt += 2+blt_info.bltbmod; 
	    if (bltcon0 & 0x200) bltcpt += 2+blt_info.bltcmod; 
	    if (bltcon0 & 0x100) bltdpt += 2+blt_info.bltdmod;
	} else {
	    if (bltcon0 & 0x800) bltapt += 2; 
	    if (bltcon0 & 0x400) bltbpt += 2; 
	    if (bltcon0 & 0x200) bltcpt += 2; 
	    if (bltcon0 & 0x100) bltdpt += 2;
	}
    }
}
#endif

static __inline__ void blitter_line_incx(void)
{
    blinea >>= 1;
    if (!blinea) {
	blinea = 0x8000;
	bltcnxlpt += 2;
	bltdnxlpt += 2;
    }
}

static __inline__ void blitter_line_decx(void)
{
    blinea <<= 1;
    if (!blinea) {
	blinea = 1;
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
    UWORD blitahold = blinea, blitbhold = blineb & 1 ? 0xFFFF : 0, blitchold = blt_info.bltcdat;
    blt_info.bltddat = 0;
    
    if (blitsing && blitonedot) blitahold = 0;
    blitonedot = 1;
    blt_info.bltddat = blit_func(blitahold, blitbhold, blitchold, bltcon0 & 0xFF);
    if (!blitsign){
	bltapt += (WORD)blt_info.bltamod;
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
	bltapt += (WORD)blt_info.bltbmod;
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
    blitsign = 0 > (WORD)bltapt;
    bltstate = BLT_write;
}

static __inline__ void blitter_nxline(void)
{
    bltcpt = bltcnxlpt;
    bltdpt = bltdnxlpt;
    blineb = (blineb << 1) | (blineb >> 15);
    if (--blt_info.vblitsize == 0) {
	bltstate = BLT_done;
#if FAST_BLITTER == 0
	custom_bank.wput(0xDFF09C,0x8040);
#endif
    } else {
	bltstate = BLT_read;
    }
}

static void blit_init(void)
{
    blitlpos = 0;
    blt_info.blitzero = 1; blt_info.blitpreva = blt_info.blitprevb = 0;
    blitline = bltcon1 & 1;
    blt_info.blitashift = bltcon0 >> 12; blt_info.blitbshift = bltcon1 >> 12;
    
    if (blitline) {
	if (blt_info.hblitsize != 2) {
	    emu_printf("weird hblitsize in linemode:\n");
		emu_printi(blt_info.hblitsize);
	}	
	bltcnxlpt = bltcpt;
	bltdnxlpt = bltdpt;
	blitsing = bltcon1 & 0x2;
	blinea = blt_info.bltadat >> blt_info.blitashift;
	blineb = (blt_info.bltbdat >> blt_info.blitbshift) | (blt_info.bltbdat << (16-blt_info.blitbshift));
#if 0
	if (blineb != 0xFFFF && blineb != 0)
	    emu_printf("%x %x %d %x\n", blineb, blt_info.bltbdat, blt_info.blitbshift, bltcon1);
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
	    	emu_printf("warning: weird fill mode (further messages suppressed)\n");
	    warn = 0;
	}
	blitdesc = bltcon1 & 0x2;
	if (blitfill && !blitdesc)
	    emu_printf("warning: blitter fill without desc\n");
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
}

#ifdef DEFERRED_INT
static void defer_int_handler(void)
{
    if (!dmaen(DMA_BLITTER)) {
	eventtab[ev_deferint].active = 1;
	eventtab[ev_deferint].oldcycles = cycles;
	eventtab[ev_deferint].evtime = 10; /* wait a little */
	return; /* gotta come back later. */
    }
    actually_do_blit();
    
    INTREQ(0x8040);
    eventtab[ev_deferint].active = 0;
    specialflags &= ~SPCFLAG_BLTNASTY;
}
#endif

void do_blitter(void)
{
#if FAST_BLITTER == 0
    /* I'm not sure all this bltstate stuff is really necessary.
     * Most programs should be OK if the blit is done as soon as BLTSIZE is
     * written to, and the BLTFINISH bit is set some time after that.
     * This code here is nowhere near exact.
     */
    do {	
	switch(bltstate) {
	 case BLT_init:
	    blit_init();
	    bltstate = BLT_read;
	    /* fall through */
	 case BLT_read:
	    if (blitter_read())
	    	break;
	    /* fall through */
	 case BLT_work:
	    if (blitline)
	    	blitter_line(); 
	    else 
	    	blitter_blit();
	    /* fall through */
	 case BLT_write:
	    if (blitter_write())
	    	break;
	    /* fall through */
	 case BLT_next:
	    if (blitline)
	    	blitter_nxline();
	    else 
	    	blitter_nxblit();
	    break;
	 case BLT_done:
	    specialflags &= ~SPCFLAG_BLIT;
	    break;
	}
    } while(bltstate != BLT_done && dmaen(DMA_BLITTER)
	    && dmaen(DMA_BLITPRI));  /* blitter nasty -> no time for CPU */
#else
    {
	long int blit_cycles = 2;

	if (!blitline) {
	    if (bltcon0 & 0x400)
		blit_cycles++;
	    if ((bltcon0 & 0x300) == 0x300)
		blit_cycles++;
	    blit_cycles *= blt_info.vblitsize * blt_info.hblitsize;
	}
	blit_init();
	
	eventtab[ev_deferint].active = 1;
	eventtab[ev_deferint].oldcycles = cycles;
	eventtab[ev_deferint].evtime = blit_cycles;
	events_schedule();

	specialflags &= ~SPCFLAG_BLIT;
	if (dmaen(DMA_BLITPRI))
	    specialflags |= SPCFLAG_BLTNASTY;
    }
#endif
}

static void maybe_blit(void)
{
    static int warned = 0;
    if (bltstate == BLT_done)
	return;
    
    if (!warned) {
	warned = 1;
	emu_printf("warning: Program does not wait for blitter (no further messages)\n");
    }
    if (!eventtab[ev_deferint].active)
	printf("FOO!!?\n");
    actually_do_blit();
    eventtab[ev_deferint].active = 0;
    specialflags &= ~SPCFLAG_BLTNASTY;
}

static int disk_int_defer = 0;

void do_disk(void)
{
    if (dskdmaen > 1 && dmaen(0x10)){
	if (--dsktime == 0) {
	    dsktime = dskdelay;
	    if (dsklen & 0x4000){
		UWORD dsksync_check;
		DISK_GetData(&dsksync_check, &dskbyte);
		dskbyte |= 0x8000;
		if (dsksynced || !(adkcon & 0x400)) {
 		    //*mfmwrite++ = chipmem_bank.wget(dskpt); 
            dskpt += 2;    
		    if (--dsklength == 0) {
			DISK_WriteData();
			custom_bank.wput(0xDFF09C, 0x8002); /*INTREQ->DSKBLK */
			dskdmaen = 0;
			specialflags &= ~SPCFLAG_DISK;
		    }
		}
		if (dsksync_check == dsksync) {
		    custom_bank.wput(0xDFF09C, 0x9000);
		    dsksynced = 1;
		}
	    } else {
#if defined(NO_FAST_DISK)
		DISK_GetData(&dskmfm, &dskbyte);
		dskbyte |= 0x8000;
		if (dsksynced || !(adkcon & 0x400)){
		    chipmem_bank.wput(dskpt, dskmfm); dskpt += 2;
		    if (--dsklength == 0) {
			custom_bank.wput(0xDFF09C, 0x8002);
			dskdmaen = 0;
			specialflags &= ~SPCFLAG_DISK;
		    }
		}
		if (dskmfm == dsksync) {
		    custom_bank.wput(0xDFF09C,0x9000);
		    dsksynced = 1;
		}
#else
		int ntries = 100000;
		do {
		    DISK_GetData(&dskmfm, &dskbyte);
		    dskbyte |= 0x8000;
		    if (dsksynced || !(adkcon & 0x400)){
			chipmem_bank.wput(dskpt, dskmfm); dskpt += 2;
			if (--dsklength == 0) {
			    disk_int_defer = 5;
			    dskdmaen = 0;
			    specialflags &= ~SPCFLAG_DISK;
			}
		    }
		    if (dskmfm == dsksync) {
			custom_bank.wput(0xDFF09C,0x9000);
			dsksynced = 1;
		    }
		} while ((specialflags & SPCFLAG_DISK) && --ntries);
#endif
	    }
	}
    }
}

#ifdef unused
static __inline__ void pfield_fetchdata(void)
{
    if (dmaen(0x100) && pfield_linedmaon) {
	switch(bplplanecnt){
	 case 8:
	    bpl8dat = chipmem_bank.wget(bplpt[7]); bplpt[7] += 2; bpl8dat <<= 7;
	 case 7:
	    bpl7dat = chipmem_bank.wget(bplpt[6]); bplpt[6] += 2; bpl7dat <<= 6;
	 case 6:
	    bpl6dat = chipmem_bank.wget(bplpt[5]); bplpt[5] += 2; bpl6dat <<= 5;
	 case 5:
	    bpl5dat = chipmem_bank.wget(bplpt[4]); bplpt[4] += 2; bpl5dat <<= 4;
	 case 4:
	    bpl4dat = chipmem_bank.wget(bplpt[3]); bplpt[3] += 2; bpl4dat <<= 3;
	 case 3:
	    bpl3dat = chipmem_bank.wget(bplpt[2]); bplpt[2] += 2; bpl3dat <<= 2;
	 case 2:
	    bpl2dat = chipmem_bank.wget(bplpt[1]); bplpt[1] += 2; bpl2dat <<= 1;
	 case 1:
	    bpl1dat = chipmem_bank.wget(bplpt[0]); bplpt[0] += 2;
	}
    }
}
#endif

static void do_sprites(int currvp, int currhp)
{   
    int i;
    int maxspr = currhp/4 -0x18/ 4;
    
    if (currvp == 0)
	return;
    if (maxspr < 0)
	return;
    if (maxspr > 7)
	maxspr = 7;
    
    for(i = last_sprite; i <= maxspr; i++) {
	int vstart = (bpl_info.sprpos[i] >> 8) | ((bpl_info.sprctl[i] << 6) & 0x100);
	int vstop = (bpl_info.sprctl[i] >> 8) | ((bpl_info.sprctl[i] << 7) & 0x100);
	if ((vstart <= currvp && vstop >= currvp) || spron[i] == 1) {
	    if (dmaen(0x20)) {
		UWORD data1 = chipmem_bank.wget(sprpt[i]);
		UWORD data2 = chipmem_bank.wget(sprpt[i]+2);
		sprpt[i] += 4;
		
		if (vstop != currvp && spron[i] != 1) {
		    /* Hack for X mouse auto-calibration */
		    if (i == 0 && !sprvbfl && ((bpl_info.sprpos[0]&0xff)<<2)>0x60) {
			int old0ctl = spr0ctl, old0pos = spr0pos;
			spr0ctl=bpl_info.sprctl[0];
			spr0pos=bpl_info.sprpos[0];
			sprvbfl=2;
		    }
		    SPRxDATB_1(data2, i);
		    SPRxDATA_1(data1, i);
		} else {
		    SPRxPOS_1(data1, i);
		    SPRxCTL_1(data2, i);
		}
	    }
	}
    }
    last_sprite = maxspr + 1;
}

static __inline__ void pfield_modulos(int add)
{
    switch(bplplanecnt){
     case 8:
	bplpt[7] += add + bpl2mod;
     case 7:
	bplpt[6] += add + bpl1mod;
     case 6:
	bplpt[5] += add + bpl2mod;
     case 5:
	bplpt[4] += add + bpl1mod;
     case 4:
	bplpt[3] += add + bpl2mod;
     case 3:
	bplpt[2] += add + bpl1mod;
     case 2:
	bplpt[1] += add + bpl2mod;
     case 1:
	bplpt[0] += add + bpl1mod;
    }
}

void pfield_sprite_l (int num, int sprxp, UWORD data, UWORD datb)
{
    int i;
    for(i = 15; i >= 0; i--) {
	int sprxpos = sprxp + i;
	if (spixstate[sprxpos] & ((1 << num)-1))
	    continue;

	if ((bpl_info.sprctl[num] & 0x80) && (num & 1)) {
	    /* Attached sprite */
	    int col = ((data << 2) & 4) + ((datb << 3) & 8);
	    if (col) {
		spixels[sprxpos] = col;
		spixstate[sprxpos] = 1 << num;
	    }
	    spixstate[sprxpos] |= 1 << (num-1);
	} else {			
	    int col = (data & 1) | ((datb << 1) & 2);
	    if (spixstate[sprxpos] & (1 << num)) {
		/* finish attached sprite */
		/* Did the upper half of the sprite have any bits set? */
		if (spixstate[sprxpos] & (1 << (num+1)))
		    col += spixels[sprxpos];
		if (!col) {
		    spixstate[sprxpos] &= ~(3 << num);
		}
	    } else {
		if (col) {
		    col |= (num & 6) << 1;
		}
	    }
	    if (col) {
		spixels[sprxpos] = col;
		spixstate[sprxpos] = 1<<num;
	    }
	}
	data >>= 1;
	datb >>= 1;
    }
}

void pfield_sprite_h (int num, int sprxp, UWORD data, UWORD datb)
{
    int i;
    for(i = 15; i >= 0; i--) {
	int sprxpos = sprxp + i*2;
	if (spixstate[sprxpos] & ((1 << num)-1))
	    continue;

	if ((bpl_info.sprctl[num] & 0x80) && (num & 1)) {
	    /* Attached sprite */
	    int col = ((data << 2) & 4) + ((datb << 3) & 8);
	    if (col) {
		spixels[sprxpos] = col;
		spixels[sprxpos+1] = col;
		spixstate[sprxpos] = 1 << num;
		spixstate[sprxpos+1] = 1 << num;
	    }
	    spixstate[sprxpos] |= 1 << (num-1);
	    spixstate[sprxpos+1] |= 1 << (num-1);
	} else {			
	    int col = (data & 1) | ((datb << 1) & 2);
	    if (spixstate[sprxpos] & (1 << num)) {
		/* finish attached sprite */
		/* Did the upper half of the sprite have any bits set? */
		if (spixstate[sprxpos] & (1 << (num+1)))
		    col += spixels[sprxpos];
		if (!col) {
		    spixstate[sprxpos] = spixstate[sprxpos+1] &= ~(3 << num);
		}
	    } else {
		if (col) {
		    col |= (num & 6) << 1;
		}
	    }
	    if (col) {
		spixels[sprxpos] = col;
		spixels[sprxpos+1] = col;
		spixstate[sprxpos] = 1<<num;
		spixstate[sprxpos+1] = 1<<num;
	    }
	}
	data >>= 1;
	datb >>= 1;
    }
}

static __inline__ UWORD *pfield_xlateptr(CPTR plpt, int bytecount)
{
    if (!chipmem_bank.check(plpt,bytecount)) {
	static int count = 0;
	if (count < 5) {
	    count++;
	    emu_printf("Warning: Bad playfield pointer");
	    if (count == 5) emu_printf(" (no further warnings)");
	    emu_printf("\n");
	}
	return NULL;
    }
    return chipmem_bank.xlateaddr(plpt);
}


#ifdef unused
static void pfield_doline_slow_h(int currhpos)
{
    int xpos = currhpos * 4 - 0x60;
	    
    if (bplhires) {
	int offs1 = xpos + 16 + bpldelay1*2;
	int offs2 = xpos + 16 + bpldelay2*2;
	
	int pix;
	for(pix = 15; pix >= 0; pix--) {
	    switch(bplplanecnt) {
	     case 8:
		pixdata.apixels[pix + offs2] |= bpl8dat & 0x80; bpl8dat >>= 1;
	     case 7:
		pixdata.apixels[pix + offs1] |= bpl7dat & 0x40; bpl7dat >>= 1;
	     case 6:
		pixdata.apixels[pix + offs2] |= bpl6dat & 0x20; bpl6dat >>= 1;
	     case 5:
		pixdata.apixels[pix + offs1] |= bpl5dat & 0x10; bpl5dat >>= 1;
	     case 4:
		pixdata.apixels[pix + offs2] |= bpl4dat & 0x8; bpl4dat >>= 1;
	     case 3:
		pixdata.apixels[pix + offs1] |= bpl3dat & 0x4; bpl3dat >>= 1;
	     case 2:
		pixdata.apixels[pix + offs2] |= bpl2dat & 0x2; bpl2dat >>= 1;
	     case 1:
		pixdata.apixels[pix + offs1] |= bpl1dat & 0x1; bpl1dat >>= 1;
	    }
	}
    } else {
	int offs1 = xpos + 32 + bpldelay1*2;
	int offs2 = xpos + 32 + bpldelay2*2;
	
	int pix;
	for(pix = 30; pix >= 0; pix -= 2) {
	    switch(bplplanecnt) {
	     case 8:
		pixdata.apixels[pix + offs2] |= bpl8dat & 0x80;
		pixdata.apixels[pix + offs2 + 1] |= bpl8dat & 0x80; bpl8dat >>= 1;
	     case 7:
		pixdata.apixels[pix + offs1] |= bpl7dat & 0x40; 
		pixdata.apixels[pix + offs1 + 1] |= bpl7dat & 0x40; bpl7dat >>= 1;
	     case 6:
		pixdata.apixels[pix + offs2] |= bpl6dat & 0x20;
		pixdata.apixels[pix + offs2 + 1] |= bpl6dat & 0x20; bpl6dat >>= 1;
	     case 5:
		pixdata.apixels[pix + offs1] |= bpl5dat & 0x10;
		pixdata.apixels[pix + offs1 + 1] |= bpl5dat & 0x10; bpl5dat >>= 1;
	     case 4:
		pixdata.apixels[pix + offs2] |= bpl4dat & 0x8;
		pixdata.apixels[pix + offs2 + 1] |= bpl4dat & 0x8; bpl4dat >>= 1;
	     case 3:
		pixdata.apixels[pix + offs1] |= bpl3dat & 0x4; 
		pixdata.apixels[pix + offs1 + 1] |= bpl3dat & 0x4; bpl3dat >>= 1;
	     case 2:
		pixdata.apixels[pix + offs2] |= bpl2dat & 0x2; 
		pixdata.apixels[pix + offs2 + 1] |= bpl2dat & 0x2; bpl2dat >>= 1;
	     case 1:
		pixdata.apixels[pix + offs1] |= bpl1dat & 0x1; 
		pixdata.apixels[pix + offs1 + 1] |= bpl1dat & 0x1; bpl1dat >>= 1;
	    }
	}
    }	
}

static void pfield_doline_slow_l(int currhpos)
{
    int xpos = currhpos * 2 - 0x30;
    
    if (bplhires) {
	int offs1 = xpos + 8 + bpldelay1;
	int offs2 = xpos + 8 + bpldelay2;
	
	int pix;
	for(pix = 7; pix >= 0; pix--) {
	    switch(bplplanecnt) {
	     case 8:
		pixdata.apixels[pix + offs2] |= bpl8dat & 0x80; bpl8dat >>= 2;
	     case 7:
		pixdata.apixels[pix + offs1] |= bpl7dat & 0x40; bpl7dat >>= 2;
	     case 6:
		pixdata.apixels[pix + offs2] |= bpl6dat & 0x20; bpl6dat >>= 2;
	     case 5:
		pixdata.apixels[pix + offs1] |= bpl5dat & 0x10; bpl5dat >>= 2;
	     case 4:
		pixdata.apixels[pix + offs2] |= bpl4dat & 0x8; bpl4dat >>= 2;
	     case 3:
		pixdata.apixels[pix + offs1] |= bpl3dat & 0x4; bpl3dat >>= 2;
	     case 2:
		pixdata.apixels[pix + offs2] |= bpl2dat & 0x2; bpl2dat >>= 2;
	     case 1:
		pixdata.apixels[pix + offs1] |= bpl1dat & 0x1; bpl1dat >>= 2;
	    }
	}
    } else {
	int offs1 = xpos + 16 + bpldelay1;
	int offs2 = xpos + 16 + bpldelay2;
	
	int pix;
	for(pix = 15; pix >= 0; pix --) {
	    switch(bplplanecnt) {
	     case 8:
		pixdata.apixels[pix + offs2] |= bpl8dat & 0x80; bpl8dat >>= 1;
	     case 7:
		pixdata.apixels[pix + offs1] |= bpl7dat & 0x40; bpl7dat >>= 1;
	     case 6:
		pixdata.apixels[pix + offs2] |= bpl6dat & 0x20; bpl6dat >>= 1;
	     case 5:
		pixdata.apixels[pix + offs1] |= bpl5dat & 0x10; bpl5dat >>= 1;
	     case 4:
		pixdata.apixels[pix + offs2] |= bpl4dat & 0x8; bpl4dat >>= 1;
	     case 3:
		pixdata.apixels[pix + offs1] |= bpl3dat & 0x4; bpl3dat >>= 1;
	     case 2:
		pixdata.apixels[pix + offs2] |= bpl2dat & 0x2; bpl2dat >>= 1;
	     case 1:
		pixdata.apixels[pix + offs1] |= bpl1dat & 0x1; bpl1dat >>= 1;
	    }
	}
    }	
}
#endif

static ULONG hirestab_l[256][1];
static ULONG lorestab_l[256][2];

static void gen_pfield_tables(void)
{
    int i;
    union {
	struct {
	    UBYTE a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
	} foo;
	struct {
	    ULONG a, b, c, d;
	} bar;
    } baz;
    
    for (i = 0; i < 256; i++) {
		/* We lose every second pixel in HiRes if UAE runs in a 320x200 screen. */
		
		baz.foo.a = i & 64 ? 1 : 0;
		baz.foo.b = i & 16 ? 1 : 0;
		baz.foo.c = i & 4 ? 1 : 0;
		baz.foo.d = i & 1 ? 1 : 0;
		hirestab_l[i][0] = baz.bar.a;
		
		baz.foo.a = i & 128 ? 1 : 0;
		baz.foo.b = i & 64 ? 1 : 0;
		baz.foo.c = i & 32 ? 1 : 0;
		baz.foo.d = i & 16 ? 1 : 0;
		baz.foo.e = i & 8 ? 1 : 0;
		baz.foo.f = i & 4 ? 1 : 0;
		baz.foo.g = i & 2 ? 1 : 0;
		baz.foo.h = i & 1 ? 1 : 0;
		lorestab_l[i][0] = baz.bar.a;
		lorestab_l[i][1] = baz.bar.b;
    }
}




static __inline__ void pfield_orword_hires_l(int data, unsigned char *dp, int bit)
{
    ULONG *pixptr = (ULONG *)dp;

    *pixptr |= hirestab_l[data >> 8][0] << bit;
    *(pixptr+1) |= hirestab_l[data & 255][0] << bit;
}

static __inline__ void pfield_orword_lores_l(int data, unsigned char *dp, int bit)
{
    ULONG *pixptr = (ULONG *)dp;

    *pixptr |= lorestab_l[data >> 8][0] << bit;
    *(pixptr+1) |= lorestab_l[data >> 8][1] << bit;
    *(pixptr+2) |= lorestab_l[data & 255][0] << bit;
    *(pixptr+3) |= lorestab_l[data & 255][1] << bit;
}

static __inline__ void pfield_setword_hires_l(int data, unsigned char *dp, int bit)
{
    ULONG *pixptr = (ULONG *)dp;

    *pixptr = hirestab_l[data >> 8][0] << bit;
    *(pixptr+1) = hirestab_l[data & 255][0] << bit;
}

static __inline__ void pfield_setword_lores_l(int data, unsigned char *dp, int bit)
{
    ULONG *pixptr = (ULONG *)dp;

    *pixptr = lorestab_l[data >> 8][0] << bit;
    *(pixptr+1) = lorestab_l[data >> 8][1] << bit;
    *(pixptr+2) = lorestab_l[data & 255][0] << bit;
    *(pixptr+3) = lorestab_l[data & 255][1] << bit;
}

#define DO_ONE_PLANE(POINTER, MULT, FUNC, DELAY, LL_SUB, P_ADD) { \
    int i; \
    unsigned int bpldat1; \
    UWORD data; \
    unsigned int bpldat2 = 0; \
    for (i = plflinelen; i > 0; i -= LL_SUB) { \
        bpldat1 = bpldat2; \
	bpldat2 = *POINTER++; \
	data = (bpldat1 << (16 - DELAY)) | (bpldat2 >> DELAY); \
	FUNC(data, app, MULT); \
	app += P_ADD; \
    } \
    data = bpldat2 << (16 - DELAY); \
    FUNC(data, app, MULT); \
}

#define DO_ONE_PLANE_B_LORES_H(NUM, FETCH) \
		if (bplplanecnt > NUM) { \
                    if (FETCH != 0) { \
			plfd[NUM] <<= 16; \
			if (FETCH == 1) \
			    plfd[NUM] |= *(r_bplpt[NUM])++ << delay[NUM & 1]; \
			d = plfd[NUM] >> 24; \
                    } else {\
			d = (plfd[NUM] >> 16) & 255; \
		    }\
		    if (NUM == 0) { \
			data1 = lorestab_h[d][0] << NUM;\
			data2 = lorestab_h[d][1] << NUM;\
			data3 = lorestab_h[d][2] << NUM;\
			data4 = lorestab_h[d][3] << NUM;\
		    } else { \
			data1 |= lorestab_h[d][0] << NUM;\
			data2 |= lorestab_h[d][1] << NUM;\
			data3 |= lorestab_h[d][2] << NUM;\
			data4 |= lorestab_h[d][3] << NUM;\
		    } \
		}



static void pfield_doline_l(void)
{
    int xpos = plfstrt * 2 - 0x30;
    int spr;
        
    if (bplhires) {
	if (bplplanecnt > 0) {
	    int xpos1 = xpos + 8 + (bpldelay1 >= 8 ? 8 : 0);
	    int xpos2 = xpos + 8 + (bpldelay2 >= 8 ? 8 : 0);
	    int delay1 = (bpldelay1 & 7) * 2;
	    int delay2 = (bpldelay2 & 7) * 2;
	    unsigned char *app = pixdata.apixels + xpos1;
	    
	    DO_ONE_PLANE(r_bplpt[0], 0, pfield_setword_hires_l, delay1, 4, 8);
	    if (bplplanecnt > 2) {
		app = pixdata.apixels + xpos1;
		DO_ONE_PLANE(r_bplpt[2], 2, pfield_orword_hires_l, delay1, 4, 8);
	    }
#if AGA_CHIPSET == 1
	    if (bplplanecnt > 4) {
		app = pixdata.apixels + xpos1;
		DO_ONE_PLANE(r_bplpt[4], 4, pfield_orword_hires_l, delay1, 4, 8);
	    }
	    if (bplplanecnt > 6) {
		app = pixdata.apixels + xpos1;
		DO_ONE_PLANE(r_bplpt[6], 6, pfield_orword_hires_l, delay1, 4, 8);
	    }
#endif
	    if (bplplanecnt > 1) {
		app = pixdata.apixels + xpos2;
		DO_ONE_PLANE(r_bplpt[1], 1, pfield_orword_hires_l, delay2, 4, 8);
	    }
	    if (bplplanecnt > 3) {
		app = pixdata.apixels + xpos2;
		DO_ONE_PLANE(r_bplpt[3], 3, pfield_orword_hires_l, delay2, 4, 8);
	    }
#if AGA_CHIPSET == 1
	    if (bplplanecnt > 5) {
		app = pixdata.apixels + xpos2;
		DO_ONE_PLANE(r_bplpt[5], 5, pfield_orword_hires_l, delay2, 4, 8);
	    }
	    if (bplplanecnt > 7) {
		app = pixdata.apixels + xpos2;
		DO_ONE_PLANE(r_bplpt[7], 7, pfield_orword_hires_l, delay2, 4, 8);
	    }
#endif
	} else {
	    memset(pixdata.apixels, 0, sizeof(pixdata.apixels));
	}
    } else {
	if (bplplanecnt > 0) {
	    int x = xpos + 16;
	    int delay1 = bpldelay1;
	    int delay2 = bpldelay2;
	    unsigned char *app = pixdata.apixels + x;
	    DO_ONE_PLANE(r_bplpt[0], 0, pfield_setword_lores_l, delay1, 8, 16);
	    if (bplplanecnt > 2) {
		app = pixdata.apixels + x;
		DO_ONE_PLANE(r_bplpt[2], 2, pfield_orword_lores_l, delay1, 8, 16);
	    }
	    if (bplplanecnt > 4) {
		app = pixdata.apixels + x;
		DO_ONE_PLANE(r_bplpt[4], 4, pfield_orword_lores_l, delay1, 8, 16);
	    }
#if AGA_CHIPSET == 1
	    if (bplplanecnt > 6) {
		app = pixdata.apixels + x;
		DO_ONE_PLANE(r_bplpt[6], 6, pfield_orword_lores_l, delay1, 8, 16);
	    }
#endif
	    if (bplplanecnt > 1) {
		app = pixdata.apixels + x;
		DO_ONE_PLANE(r_bplpt[1], 1, pfield_orword_lores_l, delay2, 8, 16);
	    }
	    if (bplplanecnt > 3) {
		app = pixdata.apixels + x;
		DO_ONE_PLANE(r_bplpt[3], 3, pfield_orword_lores_l, delay2, 8, 16);
	    }
	    if (bplplanecnt > 5) {
		app = pixdata.apixels + x;
		DO_ONE_PLANE(r_bplpt[5], 5, pfield_orword_lores_l, delay2, 8, 16);
	    }
#if AGA_CHIPSET == 1
	    if (bplplanecnt > 7) {
		app = pixdata.apixels + x;
		DO_ONE_PLANE(r_bplpt[7], 7, pfield_orword_lores_l, delay2, 8, 16);
	    }
#endif
	} else {
	    memset(pixdata.apixels, 0, sizeof(pixdata.apixels));
	}
    }

    linetoscreen = 1;

    for(spr = 7; spr >= 0; spr--) {
	if (bpl_info.sprarmed[spr]) {
	    int sprxp = ((bpl_info.sprpos[spr] & 0xFF) * 2) - 0x30 + (bpl_info.sprctl[spr] & 1);
	    int i;
	    /* Ugh. Nasty bug. Let's rather lose some sprites than trash
	     * memory. */
	    if (sprxp >= 0)
		pfield_sprite_l (spr, sprxp, bpl_info.sprdata[spr], bpl_info.sprdatb[spr]);
	}
    }
}

#ifdef unused
static int bpl_data_differs(UWORD *data, UWORD *r_addr, int nbytes)
{
    return memcmpy(data, r_addr, nbytes);
}

static __inline__ int bplinfo_differs(struct bplinfo *a, struct bplinfo *b)
{
    int ncolors;
    int i;
    if (a->bplcon0 != b->bplcon0
	|| a->bplcon1 != b->bplcon1
	|| a->bplcon2 != b->bplcon2
	|| a->bplcon3 != b->bplcon3
	|| a->bplcon4 != b->bplcon4
	|| a->diwstrt != b->diwstrt
	|| a->diwstop != b->diwstop
	|| a->ddfstrt != b->ddfstrt
	|| a->ddfstop != b->ddfstop)
	return 1;

#if 1
    /* This sometimes loses for sprite colors */
    ncolors = 1 << ((a->bplcon0 & 0x7000) >> 12);
    if (ncolors == 64)
	ncolors = (a->bplcon0 & 0x800) ? 16 : 32;
    for (i = 0; i < ncolors; i++)
	if (a->color_regs[i] != b->color_regs[i])
	    return 1;
#else
    /* ... and this will lose badly on few-color screens when we implement 
     * AGA.
     */
    if (memcmp(a->color_regs, b->color_regs, sizeof a->color_regs) != 0)
	return 1;
#endif
    for (i = 0; i < 8; i++) {
	if (a->sprarmed[i] != b->sprarmed[i])
	    return 1;
	if (a->sprarmed[i]
	    && (a->sprctl[i] != b->sprctl[i]
		|| a->sprpos[i] != b->sprpos[i]
		|| a->sprdata[i] != b->sprdata[i]
		|| a->sprdatb[i] != b->sprdatb[i]))
	    return 1;
    }
    return 0;
}
#endif

static void pfield_doline(void)
{
    int bytecount = plflinelen / (bplhires ? 4 : 8) * 2;
#if SMART_UPDATE == 1
    int drawit = 0;
#endif    
    int i;
    
    if (vpos < plffirstline || vpos >= plflastline) 
    	return;

    if (!dmaen(0x100) || !pfield_linedmaon) {
	line_in_border = 1;
	return;
    }
    
    for (i = 0; i < bplplanecnt; i++) {
	r_bplpt[i] = pfield_xlateptr(bplpt[i], bytecount);
	if (r_bplpt[i] == NULL)
	    return;
    }
    
#if SMART_UPDATE == 1
    drawit = bplinfo_differs(&bpl_info, &linedescr[next_lineno].bpl_info);
    if (drawit) {
	linedescr[next_lineno].bpl_info = bpl_info;
    }
    drawit |= !linedescr[next_lineno].linedata_valid;
	
    if (bytecount <= MAX_WORDS_PER_LINE * 2) {
	linedescr[next_lineno].linedata_valid = 1;
	for (i = 0; i < bplplanecnt; i++)
	    drawit |= bpl_data_differs(line_data[next_lineno][i],r_bplpt[i],
				       bytecount);
	
    } else {
	linedescr[next_lineno].linedata_valid = 0;
    }
#endif

    pfield_modulos(bytecount);

#if SMART_UPDATE != 0
    if (!drawit)
	return;
#endif    

	pfield_doline_l();
}


#ifdef unused
static void pfield_doline_slow(int currhp)
{
    if (vpos < plffirstline || vpos >= plflastline) 
    	return;
    
    if (currhp == plfstrt)
	slowline_nextpos = currhp;

    if (currhp == slowline_nextpos) {    
	if (slowline_linepos >= plflinelen) {
	    pfield_modulos(0);
	    slowline_nextpos = -1;
	} else {
	    slowline_nextpos += bplhires ? 4 : 8;
	    slowline_linepos += bplhires ? 4 : 8;

	    pfield_fetchdata();
		pfield_doline_slow_l(currhp);
	}
    }
    if (currhp > 48) {
	int xpos = PIXEL_XPOS(currhp);
	int spr;
	for(spr = 7; spr >= 0; spr--) {
	    if (bpl_info.sprarmed[spr] && currhp == (bpl_info.sprpos[spr] & 0xFF)) {
		    int sprxp = xpos + (bpl_info.sprctl[spr] & 1);
		    pfield_sprite_l (spr, sprxp, bpl_info.sprdata[spr], bpl_info.sprdatb[spr]);
	    }
	}
    }    
}
#endif

static void setdontcare(void)
{
    emu_printf("Don't care mouse mode set\n");
    mousestate=dont_care_mouse;
    lastspr0x=lastmx; lastspr0y=lastmy;
    mstepx=defstepx; mstepy=defstepy;
}

static void setfollow(void)
{
    emu_printf("Follow sprite mode set\n");
    mousestate=follow_mouse;
    lastdiffx=lastdiffy=0;
    sprvbfl=0;
    spr0ctl=spr0pos=0;
    mstepx=defstepx; mstepy=defstepy;
}

void togglemouse(void)
{
    switch(mousestate) {
     case dont_care_mouse: setfollow(); break;
     case follow_mouse: setdontcare(); break;
     default: break; /* Nnnnnghh! */
    }
}	    

static __inline__ int adjust(int val)
{
    if (val>127)
	return 127; 
    else if (val<-127)
	return -127;
    return val;
}

static int first_drawn_line, last_drawn_line;
static int first_block_line, last_block_line;

static void init_frame (void)
{
    int i;
    int maxpos = 400;
    
    if (max_diwstop == 0)
	max_diwstop = diwlastword;
    
    if ( (max_diwstop < 320) || (max_diwstop > maxpos) )
		prev_max_diwstop = maxpos;
    else
		prev_max_diwstop = max_diwstop;

    max_diwstop = 0;

    memset(spron, 0, sizeof spron);
    memset(bpl_info.sprpos, 0, sizeof bpl_info.sprpos);
    memset(bpl_info.sprctl, 0, sizeof bpl_info.sprctl);
    last_drawn_line = 0;
    first_drawn_line = 32767;

    first_block_line = last_block_line = -2;
}

static void do_flush_line (int lineno)
{
    if (lineno < first_drawn_line)
	first_drawn_line = lineno;
    if (lineno > last_drawn_line)
	last_drawn_line = lineno;

    if (gfxvidinfo.maxblocklines == 0)
	flush_line(lineno);
    else {
	if ((last_block_line+1) != lineno) {
	    if (first_block_line != -2)
		flush_block (first_block_line, last_block_line);
	    first_block_line = lineno;
	}
	last_block_line = lineno;
	if (last_block_line - first_block_line >= gfxvidinfo.maxblocklines) {
	    flush_block (first_block_line, last_block_line);
	    first_block_line = last_block_line = -2;
	}
    }
}

static void do_flush_screen (int start, int stop)
{
    if (gfxvidinfo.maxblocklines != 0 && first_block_line != -2) {
	flush_block (first_block_line, last_block_line);
    }
    if (start <= stop)
	flush_screen (start, stop);
}

static void vsync_handler(void)
{
    int spr0x = ((spr0pos & 0xff) << 2) | ((spr0ctl & 1) << 1);
    int spr0y = ((spr0pos >> 8) | ((spr0ctl & 4) << 6)) << 1;
    int diffx, diffy;
    UWORD dir;
    int button;

    handle_events();
    getjoystate(&joy0dir, &joy0button);

    switch (mousestate) {
     case normal_mouse:
	diffx = lastmx - lastsampledmx;
	diffy = lastmy - lastsampledmy;
	if (!newmousecounters) {	
	    if (diffx > 127) diffx = 127;
	    if (diffx < -127) diffx = -127;
	    joy0x += diffx;
	    if (diffy > 127) diffy = 127;
	    if (diffy < -127) diffy = -127;
	    joy0y += diffy;
	}
	lastsampledmx += diffx; lastsampledmy += diffy;
	break;

     case dont_care_mouse:
	diffx = adjust (((lastmx-lastspr0x) * mstepx) >> 16);
	diffy = adjust (((lastmy-lastspr0y) * mstepy) >> 16);
	lastspr0x=lastmx; lastspr0y=lastmy;
	joy0x+=diffx; joy0y+=diffy;
	break;
	
     case follow_mouse:
	if (sprvbfl && sprvbfl-- >1) {
	    int mousexpos, mouseypos;
	    
	    if ((lastdiffx > docal || lastdiffx < -docal) && lastspr0x != spr0x 
		&& spr0x > plfstrt*4+34+xcaloff && spr0x < plfstop*4-xcaloff)
	    {  
		int val = (lastdiffx << 16) / (spr0x - lastspr0x);
		if (val>=0x8000) mstepx=(mstepx*(calweight-1)+val)/calweight;
	    }
	    if ((lastdiffy > docal || lastdiffy < -docal) && lastspr0y != spr0y
		&& spr0y>plffirstline+ycaloff && spr0y<plflastline-ycaloff) 
	    { 
		int val = (lastdiffy<<16) / (spr0y-lastspr0y);
		if (val>=0x8000) mstepy=(mstepy*(calweight-1)+val)/calweight;
	    }
	    mousexpos = lastmx;
	    mouseypos = lastmy;
	    
	    if(dont_want_aspect)
	    	mouseypos *= 2;
	    mousexpos = 2 * (mousexpos + prev_max_diwstop - 328);

	    diffx = adjust ((((mousexpos + 0x70 + xoffs - spr0x) & ~1) * mstepx) >> 16);
	    diffy = adjust ((((mouseypos + yoffs - spr0y+minfirstline*2) & ~1) * mstepy) >> 16);
	    lastspr0x=spr0x; lastspr0y=spr0y;
	    lastdiffx=diffx; lastdiffy=diffy;
	    joy0x+=diffx; joy0y+=diffy; 
	}
	break;
    }
 
    INTREQ(0x8020);
    if (bpl_info.bplcon0 & 4) lof ^= 0x8000;
    COPJMP1(0);
    
    if (framecnt == 0)
    	do_flush_screen (first_drawn_line, last_drawn_line);

    count_frame();
    init_frame();
#ifdef HAVE_GETTIMEOFDAY
    {
	struct timeval tv;
	unsigned long int newtime;
	
	gettimeofday(&tv,NULL);	
	newtime = (tv.tv_sec-seconds_base) * 1000 + tv.tv_usec / 1000;
	
	if (!bogusframe) {	
	    frametime += newtime - msecs;
	    timeframes++;
	}
	msecs = newtime;
	bogusframe = 0;
    }
#endif
    CIA_vsync_handler();
    do_sound_vsync ();
}

static void hsync_handler(void)
{
    int lineno = vpos - minfirstline;
    int lineisdouble = 0;
    
    do_sprites(vpos, maxhpos);
    last_sprite = 0;
    
    if (!dont_want_aspect) {
		lineno *= 2;
		if (bpl_info.bplcon0 & 4) {
		    if(!lof) {
				lineno++;
		    }
		} 
		else {
		    lineisdouble = 1;
		}
    }
    
    if (disk_int_defer)
		if (!--disk_int_defer)
	    	INTREQ(0x8002);
    
    eventtab[ev_hsync].oldcycles = cycles;
    CIA_hsync_handler();
    
    if(produce_sound)
		do_sound ();

    if (framecnt == 0 && vpos >= minfirstline) 
    {
		if (vpos >= plffirstline && vpos < plflastline)
		{
		    /* Finish the line, if we started doing it with the slow update.
		     * Otherwise, draw it entirely. */
		    if (pfield_fullline) {	    
				if (!pfield_linedone) {
				    /* This can turn on line_in_border if DMA is off */
				    pfield_doline();
				}
				if (linetoscreen)
				    if ( (vpos >= 40) && (vpos < 240+44) ) {
				    	pfield_slow_linetoscr(0, maxhpos);
				    }		
		    }
#ifdef unused 		     
		    else 
		    {
				int i;
				for(i = pfield_lastpart_hpos; i < maxhpos; i++)
			    	pfield_doline_slow(i);
				pfield_slow_linetoscr(slowline_lasttoscr, maxhpos);
				/* The COLOR routine masks off the high nibble. This means
				 * that there will never be 0xFFFF in color_regs[0], and this
				 * means that the line will be drawn completely the next time
				 * we get into pfield_doline()
				 */				
				linedescr[lineno].bpl_info.color_regs[0] = 0xFFFF;
				linetoscreen = 1;
		    }
#endif		    
		} else
		    line_in_border = 1;
		
		if (line_in_border && 
		    (!linedescr[lineno].inborder 
		     || linedescr[lineno].bordercol != acolors[0]))
		{
		    linedescr[lineno].bordercol = acolors[0];
		    linedescr[lineno].linedata_valid = 0;
		    		
		    fill_line (lineno);
		    linetoscreen = 1;
		}
	    
		linedescr[lineno].inborder = line_in_border;

		if (linetoscreen) {
		    do_flush_line (lineno);    	    
		}

	    if ( (lineno < 240) ) {
	    	//if ( (lineno < 10) || (lineno > 225) )  {
	    	//memset(linebuffer, 0, sizeof(linebuffer)); 
        //}
	    	//else  
	    	emu_DrawLine16((unsigned short *)&(linebuffer)[144/*146*/], 320 , 1, lineno); 
	    }

#ifdef HAS_VBUFFER		
		if (lineisdouble) {
		    int drawit = 0;
		    
		    if (linedescr[lineno].inborder != linedescr[lineno+1].inborder
			|| (linedescr[lineno].inborder == 1
			    && linedescr[lineno].bordercol != linedescr[lineno+1].bordercol)) {
				drawit = 1;
			
		    } else if (linedescr[lineno].inborder == 0 
			       && (linetoscreen 
				   || (bplinfo_differs(&linedescr[lineno].bpl_info, 
						       &linedescr[lineno+1].bpl_info)))) {
				drawit = 1;
		    }
		    
		    if (drawit) {
				linedescr[lineno+1].inborder = linedescr[lineno].inborder;
				linedescr[lineno+1].bordercol = linedescr[lineno].bordercol;
#if SMART_UPDATE != 0
				linedescr[lineno+1].bpl_info = linedescr[lineno].bpl_info;
#endif
				memcpy (gfxvidinfo.bufmem + (lineno+1)*gfxvidinfo.rowbytes,
					gfxvidinfo.bufmem + lineno*gfxvidinfo.rowbytes,
					gfxvidinfo.rowbytes);
				do_flush_line (lineno+1);
		    }
		}
#endif	
	}

	pfield_calclinedma();

    if (++vpos == (maxvpos + (lof != 0))) {
		vpos = 0;
		vsync_handler();
    }
    
    if (framecnt == 0) 
    {
		lineno = vpos - minfirstline;
	
		if (!dont_want_aspect) {
	    	lineno *= 2;
	    	if ((bpl_info.bplcon0 & 4) && !lof) {
				lineno++;
	    	}
		}

#ifdef HAS_VBUFFER
		xlinebuffer = gfxvidinfo.bufmem + gfxvidinfo.rowbytes * lineno;
#else
		xlinebuffer = &linebuffer[0]; //emu_LineBuffer(lineno);
#endif	
#if 	AGA_CHIPSET == 1
		aga_lbufptr = aga_linebuf;
#endif	
    memset(linebuffer, 0, sizeof(linebuffer));
    
		next_lineno = lineno;
		linetoscreen = 0;
		line_in_border = 0;
		pfield_fullline = 1;
		pfield_linedone = 0;
		pfield_lastpart_hpos = 0;
    }

}

void customreset(void)
{
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
#endif
    inhibit_frame = 0;
#ifdef HAS_EXPANMEM    
    expamem_reset();
#endif    
    CIA_reset();
    cycles = 0; 
    specialflags = 0;
    
    last_sprite = 0;
    vpos = 0; 
    lof = 0;
    next_lineno = 0;
    max_diwstop = 0;
    
    if (needmousehack()) {
    	if (mousestate != follow_mouse) setfollow();
    } else {
	mousestate = normal_mouse;
    }

    memset(spixels, 0, sizeof(spixels));
    memset(spixstate, 0, sizeof(spixstate));
    
    /*memset(blitcount, 0, sizeof(blitcount));  blitter debug */
    
    int nblines;
    if (dont_want_aspect)
        nblines = numscrlines;
    else
        nblines = numscrlines*2;

    if (linedescr == NULL)  {
        linedescr = emu_Malloc(nblines*sizeof(struct line_description));
    }
    int i;    
    for (i = 0; i < nblines; i++) {
		linedescr[i].mnn = NULL;
		linedescr[i].linedata_valid = 0;
		linedescr[i].bpl_info.color_regs[0] = 0xFFFF;
		linedescr[i].bplpt[0] = (CPTR)-1;
    }
    
#ifdef HAS_VBUFFER
    xlinebuffer = gfxvidinfo.bufmem;
#else
    xlinebuffer = &linebuffer[0]; //emu_LineBuffer(0);
#endif    

    dmacon = intena = 0;
    bltstate = BLT_done;
    copstate = COP_stop;
    copcon = 0;
    dskdmaen = 0;
    cycles = 0;
    for(i = 0; i < ev_max; i++) {
	eventtab[i].active = 0;
	eventtab[i].oldcycles = 0;
    }
    copper_active = 0;
    eventtab[ev_cia].handler = CIA_handler;
    eventtab[ev_copper].handler = do_copper;
    eventtab[ev_hsync].handler = hsync_handler;
    eventtab[ev_hsync].evtime = maxhpos;
    eventtab[ev_hsync].active = 1;
#ifdef DEFERRED_INT
    eventtab[ev_deferint].handler = defer_int_handler;
    eventtab[ev_deferint].active = 0;
#endif
    events_schedule();
    
    init_frame();
#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv,NULL);
    seconds_base = tv.tv_sec;
    bogusframe = 1;
#endif
}

void dumpcustom(void)
{
	/*
    int i;
    fprintf(stderr, "DMACON: %x INTENA: %x INTREQ: %x VPOS: %x HPOS: %x\n", DMACONR(),
	   intena, intreq, vpos, current_hpos());
    if (timeframes) { 
	fprintf(stderr, "Average frame time: %d ms [frames: %d time: %d]\n", 
	       frametime/timeframes, timeframes, frametime);
    }
	*/
    /*for (i=0; i<256; i++) if (blitcount[i]) fprintf(stderr, "minterm %x = %d\n",i,blitcount[i]);  blitter debug */
}

int intlev(void)
{
    UWORD imask = intreq & intena;
    if (imask && (intena & 0x4000)){
	if (imask & 0x2000) return 6;
	if (imask & 0x1800) return 5;
	if (imask & 0x0780) return 4;
	if (imask & 0x0070) return 3;
	if (imask & 0x0008) return 2;
	if (imask & 0x0007) return 1;
    }
    return -1;
}

void custom_init(void)
{
    int num;
    if (needmousehack())
	setfollow();
    customreset();
    for (num = 0; num < 256; num++) {	
	int plane1 = (num & 1) | ((num >> 1) & 2) | ((num >> 2) & 4) | ((num >> 3) & 8);
	int plane2 = ((num >> 1) & 1) | ((num >> 2) & 2) | ((num >> 3) & 4) | ((num >> 4) & 8);
	dblpf_2nd1[num] = plane1 == 0 ? (plane2 == 0 ? 0 : 2) : 1;
	dblpf_2nd2[num] = plane2 == 0 ? (plane1 == 0 ? 0 : 1) : 2;
#if AGA_CHIPSET == 1	
	dblpf_aga1[num] = plane1 == 0 ? plane2 : plane1;
	dblpf_aga2[num] = plane2 == 0 ? plane1 : plane2;
#endif	
	if (plane2 > 0) plane2 += 8;
	dblpf_ind1[num] = plane1 == 0 ? plane2 : plane1;
	dblpf_ind2[num] = plane2 == 0 ? plane1 : plane2;
    }
    build_filltable();
    gen_pfield_tables();
}

/* Custom chip memory bank */

static ULONG custom_lget(CPTR) REGPARAM;
static UWORD custom_wget(CPTR) REGPARAM;
static UBYTE custom_bget(CPTR) REGPARAM;
static void  custom_lput(CPTR, ULONG) REGPARAM;
static void  custom_wput(CPTR, UWORD) REGPARAM;
static void  custom_bput(CPTR, UBYTE) REGPARAM;

addrbank custom_bank = {
    custom_lget, custom_wget, custom_bget,
    custom_lput, custom_wput, custom_bput,
    default_xlate, default_check
};

UWORD custom_wget(CPTR addr)
{
    switch(addr & 0x1FE) {
     case 0x002: return DMACONR();
     case 0x004: return VPOSR();
     case 0x006: return VHPOSR();
	
     case 0x008: return DSKDATR();
     case 0x012: return POT0DAT();
     case 0x016: return POTGOR();
     case 0x018: return SERDATR();
     case 0x01A: return DSKBYTR();
     case 0x01C: return INTENAR();
     case 0x01E: return INTREQR();
     case 0x010: return ADKCONR();
     case 0x00A: return JOY0DAT();
     case 0x00C: return JOY1DAT();
#ifdef ECS_DENISE
     case 0x07C: return 0xFC;
#endif
     default:
	custom_wput(addr,0);
	return 0xffff;
    }
}

UBYTE custom_bget(CPTR addr)
{
    return custom_wget(addr & 0xfffe) >> (addr & 1 ? 0 : 8);
}

ULONG custom_lget(CPTR addr)
{
    return ((ULONG)custom_wget(addr & 0xfffe) << 16) | custom_wget((addr+2) & 0xfffe);
}

void custom_wput(CPTR addr, UWORD value)
{
    addr &= 0x1FE;
    cregs[addr>>1] = value;
    switch(addr) {	
     case 0x020: DSKPTH(value); break;
     case 0x022: DSKPTL(value); break;
     case 0x024: DSKLEN(value); break;
     case 0x026: DSKDAT(value); break;
	
     case 0x02A: VPOSW(value); break;
	
     case 0x040: BLTCON0(value); break;
     case 0x042: BLTCON1(value); break;
	
     case 0x044: BLTAFWM(value); break;
     case 0x046: BLTALWM(value); break;
	
     case 0x050: BLTAPTH(value); break;
     case 0x052: BLTAPTL(value); break;
     case 0x04C: BLTBPTH(value); break;
     case 0x04E: BLTBPTL(value); break;
     case 0x048: BLTCPTH(value); break;
     case 0x04A: BLTCPTL(value); break;
     case 0x054: BLTDPTH(value); break;
     case 0x056: BLTDPTL(value); break;
	
     case 0x058: BLTSIZE(value); break;
	
     case 0x064: BLTAMOD(value); break;
     case 0x062: BLTBMOD(value); break;
     case 0x060: BLTCMOD(value); break;
     case 0x066: BLTDMOD(value); break;
	
     case 0x070: BLTCDAT(value); break;
     case 0x072: BLTBDAT(value); break;
     case 0x074: BLTADAT(value); break;
			
     case 0x07E: DSKSYNC(value); break;

     case 0x080: COP1LCH(value); break;
     case 0x082: COP1LCL(value); break;
     case 0x084: COP2LCH(value); break;
     case 0x086: COP2LCL(value); break;
	
     case 0x088: COPJMP1(value); break;
     case 0x08A: COPJMP2(value); break;
	
     case 0x08E: DIWSTRT(value); break;
     case 0x090: DIWSTOP(value); break;
     case 0x092: DDFSTRT(value); break;
     case 0x094: DDFSTOP(value); break;
	
     case 0x096: DMACON(value); break;
     case 0x09A: INTENA(value); break;
     case 0x09C: INTREQ(value); break;
     case 0x09E: ADKCON(value); break;
	
     case 0x0A0: AUD0LCH(value); break;
     case 0x0A2: AUD0LCL(value); break;
     case 0x0A4: AUD0LEN(value); break;
     case 0x0A6: AUD0PER(value); break;
     case 0x0A8: AUD0VOL(value); break;
	
     case 0x0B0: AUD1LCH(value); break;
     case 0x0B2: AUD1LCL(value); break;
     case 0x0B4: AUD1LEN(value); break;
     case 0x0B6: AUD1PER(value); break;
     case 0x0B8: AUD1VOL(value); break;
	
     case 0x0C0: AUD2LCH(value); break;
     case 0x0C2: AUD2LCL(value); break;
     case 0x0C4: AUD2LEN(value); break;
     case 0x0C6: AUD2PER(value); break;
     case 0x0C8: AUD2VOL(value); break;
	
     case 0x0D0: AUD3LCH(value); break;
     case 0x0D2: AUD3LCL(value); break;
     case 0x0D4: AUD3LEN(value); break;
     case 0x0D6: AUD3PER(value); break;
     case 0x0D8: AUD3VOL(value); break;
	
     case 0x0E0: BPLPTH(value, 0); break;
     case 0x0E2: BPLPTL(value, 0); break;
     case 0x0E4: BPLPTH(value, 1); break;
     case 0x0E6: BPLPTL(value, 1); break;
     case 0x0E8: BPLPTH(value, 2); break;
     case 0x0EA: BPLPTL(value, 2); break;
     case 0x0EC: BPLPTH(value, 3); break;
     case 0x0EE: BPLPTL(value, 3); break;
     case 0x0F0: BPLPTH(value, 4); break;
     case 0x0F2: BPLPTL(value, 4); break;
     case 0x0F4: BPLPTH(value, 5); break;
     case 0x0F6: BPLPTL(value, 5); break;
	
     case 0x100: BPLCON0(value); break;
     case 0x102: BPLCON1(value); break;
     case 0x104: BPLCON2(value); break;
     case 0x106: BPLCON3(value); break;
	
     case 0x108: BPL1MOD(value); break;
     case 0x10A: BPL2MOD(value); break;

     case 0x110: BPL1DAT(value); break;
     case 0x112: BPL2DAT(value); break;
     case 0x114: BPL3DAT(value); break;
     case 0x116: BPL4DAT(value); break;
     case 0x118: BPL5DAT(value); break;
     case 0x11A: BPL6DAT(value); break;
	
     case 0x180: case 0x182: case 0x184: case 0x186: case 0x188: case 0x18A:
     case 0x18C: case 0x18E: case 0x190: case 0x192: case 0x194: case 0x196:
     case 0x198: case 0x19A: case 0x19C: case 0x19E: case 0x1A0: case 0x1A2:
     case 0x1A4: case 0x1A6: case 0x1A8: case 0x1AA: case 0x1AC: case 0x1AE:
     case 0x1B0: case 0x1B2: case 0x1B4: case 0x1B6: case 0x1B8: case 0x1BA:
     case 0x1BC: case 0x1BE: 
	COLOR(value & 0xFFF, (addr & 0x3E) / 2);
	break;	
     case 0x120: case 0x124: case 0x128: case 0x12C: 
     case 0x130: case 0x134: case 0x138: case 0x13C:
	SPRxPTH(value, (addr - 0x120) / 4);
	break;
     case 0x122: case 0x126: case 0x12A: case 0x12E: 
     case 0x132: case 0x136: case 0x13A: case 0x13E:
	SPRxPTL(value, (addr - 0x122) / 4);
	break;
     case 0x140: case 0x148: case 0x150: case 0x158: 
     case 0x160: case 0x168: case 0x170: case 0x178:
	SPRxPOS(value, (addr - 0x140) / 8);
	break;
     case 0x142: case 0x14A: case 0x152: case 0x15A: 
     case 0x162: case 0x16A: case 0x172: case 0x17A:
	SPRxCTL(value, (addr - 0x142) / 8);
	break;
     case 0x144: case 0x14C: case 0x154: case 0x15C:
     case 0x164: case 0x16C: case 0x174: case 0x17C:
	SPRxDATA(value, (addr - 0x144) / 8);
	break;
     case 0x146: case 0x14E: case 0x156: case 0x15E: 
     case 0x166: case 0x16E: case 0x176: case 0x17E:
	SPRxDATB(value, (addr - 0x146) / 8);
	break;
	
     case 0x36: JOYTEST(value); break;
#if defined(ECS_AGNUS) || (AGA_CHIPSET == 1)
     case 0x5A: BLTCON0L(value); break;
     case 0x5C: BLTSIZV(value); break;
     case 0x5E: BLTSIZH(value); break;
#endif
#if AGA_CHIPSET == 1
     case 0x10C: BPLCON4(value); break;
#endif
    }
}

void custom_bput(CPTR addr, UBYTE value)
{
    /* Yes, there are programs that do this. The programmers should be shot.
     * This might actually work sometimes. */
    UWORD rval = value;
    CPTR raddr = addr & 0x1FE;
    if (addr & 1) {
	rval |= cregs[raddr >> 1] & 0xFF00;
    } else {
	rval <<= 8;
	rval |= cregs[raddr >> 1] & 0xFF;
    }
    custom_wput(raddr, rval);
}

void custom_lput(CPTR addr, ULONG value)
{
    custom_wput(addr & 0xfffe, value >> 16);
    custom_wput((addr+2) & 0xfffe, (UWORD)value);
}

