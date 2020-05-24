 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * custom chip support
  *
  * (c) 1995 Bernd Schmidt
  */

extern void custom_init(void);
extern void customreset(void);
extern int intlev(void);
extern int dmaen(UWORD dmamask);
extern void dumpcustom(void);

extern void do_disk(void);
extern void do_blitter(void);

extern int inhibit_frame;
extern int bogusframe;

extern unsigned long specialflags;

#define SPCFLAG_BLIT 1
#define SPCFLAG_STOP 2
#define SPCFLAG_DISK 4
#define SPCFLAG_INT  8
#define SPCFLAG_BRK  16
#define SPCFLAG_EXTRA_CYCLES 32
#define SPCFLAG_TRACE 64
#define SPCFLAG_DOTRACE 128
#define SPCFLAG_DOINT 256
#define SPCFLAG_BLTNASTY 512

extern int dskdmaen;
extern UWORD adkcon;

extern int max_diwstop, prev_max_diwstop;

extern UWORD joy0dir;
extern int joy0button;

extern void pfield_sprite_l (int, int, UWORD, UWORD);
extern void pfield_sprite_h (int, int, UWORD, UWORD);

struct bltinfo {
    int blitzero;
    UWORD blitpreva,blitprevb,blitashift,blitbshift;
    UWORD bltadat,bltbdat,bltcdat,bltddat,bltafwm,bltalwm;
    UWORD vblitsize,hblitsize;
    WORD  bltamod,bltbmod,bltcmod,bltdmod;
};

typedef void blitter_func(UWORD *, UWORD *, UWORD *, UWORD *, struct bltinfo *);

extern blitter_func *blitfunc_dofast[256];
extern blitter_func *blitfunc_dofast_desc[256];

#define maxhpos 227
#define maxvpos 312
#define minfirstline 29
#define numscrlines (maxvpos+1-minfirstline+1)

