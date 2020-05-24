
#ifndef _SHARED_H_
#define _SHARED_H_

#include <stdio.h>
#include <time.h>
#include "dtypes.h"

extern void emu_printf(char * text);
extern void emu_printi(int val);

extern void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride) ;
extern void emu_DrawLine(unsigned char *src, int width , int height, int line);
extern void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index);
extern int emu_LoadFile(char * lfilename, char * buf, int size);
extern int emu_FileSize(char * filename);

extern int emu_FileOpen(const char * filename);
extern int emu_FileSeek(int offset, int seek);
extern int emu_FileTell( void );
extern int emu_FileRead(char * buf, int size);
extern int emu_FileClose( void );

extern void * emu_Malloc(size_t size); 
extern void emu_Free(void * pt);

extern int emu_RefreshScreen(void);


#define printf(...) {}
#define fprintf(...) {}

//#define LSB_FIRST     1
//#define SOUND_PRESENT 1

extern int fake_joystick;
extern int produce_sound;
extern int dont_want_aspect;
extern int framerate;
extern char df0[], df1[], df2[], df3[], romfile[], prtname[]; 

extern void read_joystick(UWORD *dir, int *button);
extern void init_joystick(void);
extern void close_joystick(void);

extern CPTR audlc[4], audpt[4];
extern UWORD audvol[4], audper[4], audlen[4];

extern int init_sound (void);
extern void do_sound (void);



#undef inline
#define __inline__


/* 
  * UAE - The Un*x Amiga Emulator
  * 
  * User configuration options
  *
  * Copyright 1995, 1996 Bernd Schmidt
  */

/*
 * Please note: Many things are configurable with command line parameters,
 * and you can put anything you can pass on the command line into a 
 * configuration file ~/.uaerc. Please read the documentation for more
 * information.
 */

/*
 * Define this when you are compiling UAE for the first time. If it works, you
 * can try to undefine it to get (much) better performance. It does not seem
 * to work on all machines, though.
 */
#define DONT_WANT_SHM

/*
 * If you are running UAE over the network on a remote X server, this can
 * boost performance quite a bit. It can even boost performance on a 
 * non-networked system.
 */
//#define LOW_BANDWIDTH

/*
 * The blitter emulator contains some optimization methods that are, strictly
 * speaking, invalid, but very unlikely to break anything. 
 * You can set the optimization level from 0 (normal emulation) to 3 (max.
 * optimization). 
 * Methods 2 and 3 are really a lot faster with games and demos.
 * Careful: method 3 will break most RISC chips. You'll see bus errors.
 */
#define FAST_BLITTER 2

/*
 * Define this if you have the AF System and want sound in UAE.
 * You also have to set the right paths in the Makefile.
#define AF_SOUND
 */

/*
 * When these two are enabled, a subset of the ECS features is emulated.
 * Actually, it's only the chip identification and big blits. This may be
 * enough to persuade some ECS programs to run.
 * DON'T enable SuperHires or Productivity modes. They are not emulated,
 * and will look very messy. NTSC doesn't work either.
 */
#define ECS_AGNUS
#define ECS_DENISE

#ifdef __linux /* This line protects you if you don't use Linux */
/***************************************************************************
 * Linux specific options. Ignore these if you are using another OS.
 */

/*
 * Define if you have installed the Linux sound driver and if you have read
 * the section about sound in the README.
 * Turn off sound at run-time with the "-S" option.
#define LINUX_SOUND
 */

/*
 * Try defining this if you don't get steady sound output. 
#define LINUX_SOUND_SLOW_MACHINE
 */

#endif /* __linux */

/***************************************************************************
 * Support for broken software. These options are set to default values
 * that are reasonable for most uses. You should not need to change these.
 */

/*
 * Some STUPID programs access a longword at an odd address and expect to
 * end up at the routine given in the vector for exception 3.
 * (For example, Katakis does this). And yes, I know it's legal, but it's dumb
 * anyway.
 * If you leave this commented in, memory accesses will be faster,
 * but some programs may fail for an obscure reason.
 */
#define NO_EXCEPTION_3

/*
 * If you want to see the "Hardwired" demo, you need to define this.
 * Otherwise, it will say "This demo don't like Axel" - apparently, Axel
 * has a 68040.
#define WANT_SLOW_MULTIPLY
 */

/*#define EMULATE_AGA*/

#ifndef EMULATE_AGA
#define AGA_CHIPSET 0 // we are here
#else
#define AGA_CHIPSET 1
#endif

//#define SMART_UPDATE 1

/*
 * Disk accesses can be sped up. This isn't such a big win, though.
 * It hasn't been extensively tested and is turned off by default.
 * (It's broken currently, even if it appears to work for most programs)
 */
#define NO_FAST_DISK

/*
 * You can specify numbers from 0 to 5 here. It is possible that higher
 * numbers will make the CPU emulation slightly faster, but if the setting
 * is too high, you will run out of memory while compiling.
 * Best to leave this as it is.
 */
#define CPU_EMU_SIZE 0

/*
 * CPU level: 0 = 68000, 1 = 68010, 2 = sort of a 68020
 * If configured for 68020, the emulator will be a little slower.
 * Don't touch this: Only 0 will work for now.
 * JMH: only 0 supported as 68000 table is hardcoded!!
 */
#define CPU_LEVEL 0


#endif /* _SHARED_H_ */
