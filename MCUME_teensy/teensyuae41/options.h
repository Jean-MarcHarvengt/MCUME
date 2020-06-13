 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Stuff
  *
  * Copyright 1995, 1996 Ed Hanway
  * Copyright 1995-97 Bernd Schmidt
  */

#define UAEMAJOR 0
#define UAEMINOR 6
#define UAEURSAMINOR 9

#define MAX_FILENAME 64

typedef enum { KBD_LANG_US, KBD_LANG_DE, KBD_LANG_SE, KBD_LANG_FR, KBD_LANG_IT, KBD_LANG_ES } KbdLang;

extern int version;

struct uae_prefs {
    int framerate;
    int illegal_mem;
    int no_xhair;
    int use_serial;
    int automount_uaedev;
    
    int fake_joystick;
    KbdLang keyboard_lang;
    int allow_save;
    int emul_accuracy;
    int test_drawing_speed;

    int produce_sound;
    int sound_bits;
    int sound_freq;
    int sound_minbsiz;
    int sound_maxbsiz;

    int gfx_width;
    int gfx_height;
    int gfx_lores;
    int gfx_linedbl;
    int gfx_correct_aspect;
    int gfx_xcenter;
    int gfx_ycenter;
    int color_mode;


    int blits_32bit_enabled;
    int immediate_blits;
    char df[4][MAX_FILENAME];
    char hf0[MAX_FILENAME];
};

#define JSEM_DECODEVAL(n,v) (((v) >> (n*8)) & 255)
/* Determine how port n is configured with the value v in fake_joystick */
#define JSEM_ISJOY0(n,v) (JSEM_DECODEVAL(n,v) == 0)
#define JSEM_ISJOY1(n,v) (JSEM_DECODEVAL(n,v) == 1)
#define JSEM_ISMOUSE(n,v) (JSEM_DECODEVAL(n,v) == 2)
#define JSEM_ISNUMPAD(n,v) (JSEM_DECODEVAL(n,v) == 3)
#define JSEM_ISCURSOR(n,v) (JSEM_DECODEVAL(n,v) == 4)
#define JSEM_ISSOMEWHEREELSE(n,v) (JSEM_DECODEVAL(n,v) == 5)
/* These preferences make sense only at startup. */
extern int no_gui, use_debugger, use_gfxlib;

extern struct uae_prefs currprefs, changed_prefs;


extern void write_log (const char *);
extern void machdep_init (void);

/* AIX doesn't think it is Unix. Neither do I. */
#if defined(_ALL_SOURCE) || defined(_AIX)
#undef __unix
#define __unix
#endif

extern char romfile[], prtname[], sername[];

#define MAX_COLOR_MODES 5

extern int fast_memcmp(const void *foo, const void *bar, int len);
extern int memcmpy(void *foo, const void *bar, int len);

/*
 * You can specify numbers from 0 to 5 here. It is possible that higher
 * numbers will make the CPU emulation slightly faster, but if the setting
 * is too high, you will run out of memory while compiling.
 * Best to leave this as it is.
 */
#define CPU_EMU_SIZE 0

/* #define NEED_TO_DEBUG_BADLY */

#if !defined(USER_PROGRAMS_BEHAVE)
#define USER_PROGRAMS_BEHAVE 0
#endif

/* Some memsets which know that they can safely overwrite some more memory
 * at both ends and use that knowledge to align the pointers. */

#define QUADRUPLIFY(c) (((c) | ((c) << 8)) | (((c) | ((c) << 8)) << 16))

/* When you call this routine, bear in mind that it rounds the bounds and
 * may need some padding for the array. */

#define fuzzy_memset(p, c, o, l) fuzzy_memset_1 ((p), QUADRUPLIFY (c), (o) & ~3, ((l) + 4) >> 2)
static __inline__ void fuzzy_memset_1 (void *p, uae_u32 c, int offset, int len)
{
    uae_u32 *p2 = (uae_u32 *)((char *)p + offset);
    int a = len & 7;
    len >>= 3;
    switch (a) {
     case 7: p2--; goto l1;
     case 6: p2-=2; goto l2;
     case 5: p2-=3; goto l3;
     case 4: p2-=4; goto l4;
     case 3: p2-=5; goto l5;
     case 2: p2-=6; goto l6;
     case 1: p2-=7; goto l7;
     case 0: if (!--len) return; break;
    }

    for (;;) {
	p2[0] = c;
	l1:
	p2[1] = c;
	l2:
	p2[2] = c;
	l3:
	p2[3] = c;
	l4:
	p2[4] = c;
	l5:
	p2[5] = c;
	l6:
	p2[6] = c;
	l7:
	p2[7] = c;
	l8:
	if (!len)
	    break;
	len--;
	p2 += 8;
    }
}

#define fuzzy_memset_le32(p, c, o, l) fuzzy_memset_le32_1 ((p), QUADRUPLIFY (c), (o) & ~3, ((l) + 7) >> 2)
static __inline__ void fuzzy_memset_le32_1 (void *p, uae_u32 c, int offset, int len)
{
    uae_u32 *p2 = (uae_u32 *)((char *)p + offset);

    switch (len) {
     case 9: p2[0] = c; p2[1] = c; p2[2] = c; p2[3] = c; p2[4] = c; p2[5] = c; p2[6] = c; p2[7] = c; p2[8] = c; break;
     case 8: p2[0] = c; p2[1] = c; p2[2] = c; p2[3] = c; p2[4] = c; p2[5] = c; p2[6] = c; p2[7] = c; break;
     case 7: p2[0] = c; p2[1] = c; p2[2] = c; p2[3] = c; p2[4] = c; p2[5] = c; p2[6] = c; break;
     case 6: p2[0] = c; p2[1] = c; p2[2] = c; p2[3] = c; p2[4] = c; p2[5] = c; break;
     case 5: p2[0] = c; p2[1] = c; p2[2] = c; p2[3] = c; p2[4] = c; break;
     case 4: p2[0] = c; p2[1] = c; p2[2] = c; p2[3] = c; break;
     case 3: p2[0] = c; p2[1] = c; p2[2] = c; break;
     case 2: p2[0] = c; p2[1] = c; break;
     case 1: p2[0] = c; break;
     case 0: break;
     default: printf("Hit the programmer.\n"); break;
    }
}

/*
 * Frame rate hack. Currently only implemented for Pentium (P6?) CPUs.
 */
#if !defined __GNUC_MINOR__
#undef HAVE_RDTSC
#endif

#undef HAVE_INLINE_RPT
#ifdef HAVE_RDTSC
#define FRAME_RATE_HACK

typedef unsigned long frame_time_t;
static inline frame_time_t read_processor_time (void)
{
    frame_time_t foo;
    /* Don't assume the assembler knows rdtsc */
    __asm__ (".byte 0x0f,0x31" : "=a" (foo) :);
    return foo;
}
#define HAVE_INLINE_RPT
#else
typedef int frame_time_t;
#endif

#if defined FRAME_RATE_HACK && !defined HAVE_INLINE_RPT
extern frame_time_t read_processor_time (void);
#endif
extern frame_time_t vsynctime, vsyncmintime;

#if defined(AMIGA) && defined(__GNUC__)
#include "md-amiga/amiga-kludges.h"
#endif

