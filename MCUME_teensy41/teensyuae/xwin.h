 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Interface to the graphics system (X, SVGAlib)
  * 
  * Copyright 1995-1997 Bernd Schmidt
  */

#include "platform_config.h"  
#ifdef HAS_T4_VGA
typedef unsigned char xcolnr;
#else
typedef unsigned short xcolnr;
#endif



typedef int (*allocfunc_type)(int, int, int, xcolnr *);

extern xcolnr xcolors[4096];

extern int buttonstate[3];
extern int newmousecounters;
extern int lastmx, lastmy;

extern int graphics_setup(void);
extern int graphics_init(void);
extern void graphics_leave(void);
extern void handle_events(void);
extern void setup_brkhandler(void);

extern void flush_line(int);
extern void flush_block(int, int);
extern void flush_screen(int, int);

extern int debuggable(void);
extern int needmousehack(void);
extern void togglemouse(void);
extern void LED(int);

extern unsigned long doMask(int p, int bits, int shift);
extern void setup_maxcol(int);
extern void alloc_colors256(int (*)(int, int, int, xcolnr *));
extern void alloc_colors64k(int, int, int, int, int, int);
extern void setup_greydither(int bits, allocfunc_type allocfunc);
extern void setup_greydither_maxcol(int maxcol, allocfunc_type allocfunc);
extern void setup_dither(int bits, allocfunc_type allocfunc);
//extern void DitherLine(uae_u8 *l, uae_u16 *r4g4b4, int x, int y, uae_s16 len, int bits) ASM_SYM_FOR_FUNC("DitherLine");

/* Sorry for the silly field names, these used to be used slightly
 * differently. */
struct vidbuf_description
{
    char *bufmem; /* Pointer to either the video memory or an area as large which is used as a buffer. */
    char *linemem; /* Pointer to a single line in memory to draw into. If NULL, use bufmem. */
    int rowbytes; /* Bytes per row in the memory pointed at by bufmem. */
    int pixbytes; /* Bytes per pixel. */
    int maxlinetoscr; /* Width of the screen */
    int maxline; /* Number of lines in the area pointed to by bufmem (= height of the screen). */
    int maxblocklines; /* Set to 0 if you want calls to flush_line after each drawn line, or the number of
			* lines that flush_block wants to/can handle (it isn't really useful to use another
			* value than maxline here). */
    int can_double; /* Set if the high part of each entry in xcolors contains the same value
		     * as the low part, so that two pixels can be drawn at once. */
    int width;
    int height;
};

extern struct vidbuf_description gfxvidinfo;

/* For ports using tui.c, this should be built by graphics_setup(). */
extern struct bstring *video_mode_menu;
extern void vidmode_menu_selected(int);


