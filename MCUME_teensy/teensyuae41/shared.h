
#ifndef _SHARED_H_
#define _SHARED_H_

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
//#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
//#include <stdio.h>
#include <utime.h>
#undef O_BINARY
#define O_BINARY 0
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
#define sprintf(...) {}

//#define LSB_FIRST     1


#undef inline
#define __inline__


/* We can only rely on GNU C getting enums right. Mickeysoft VSC++ is known
 * to have problems, and it's likely that other compilers choke too. */

#define ENUMDECL typedef enum
#define ENUMNAME(name) name

#define ASM_SYM_FOR_FUNC(a)


#include "options.h"

static __inline__ char *my_strdup(const char*s)
{
    /* The casts to char * are there to shut up the compiler on HPUX */
    char *x = (char*)malloc(strlen((char *)s) + 1);
    strcpy(x, (char *)s);
    return x;
}

#define fast_memcmp memcmp


/*
 * Please note: Many things are configurable with command line parameters,
 * and you can put anything you can pass on the command line into a 
 * configuration file ~/.uaerc. Please read the documentation for more
 * information.
 * 
 * NOTE NOTE NOTE
 * Whenever you change something in this file, you have to "make clean"
 * afterwards.
 * Don't remove the '#' signs. If you want to enable something, move it out
 * of the C comment block, if you want to disable something, move it inside
 * the block.
 */

/*
 * Note to porters:
 * In an ideal world, all users can compile their own versions of UAE and
 * select the options they need. Since hardly anyone has a C compiler for
 * DOS, Windows or Macs, that's not possible. If you want to do your users
 * a favour, please distribute three different versions of UAE:
 *
 * 1. A fast one: no sound compiled in, NO_EXCEPTION_3 and NO_PREFETCH_BUFFER
 * 2. A slightly slower one: as above, but with sound compiled in
 * 3. A dog slow but compatible one: sound, exception 3 and prefetch emulated.
 *
 * None of these should be a "default" version, since all of these have
 * different problems.
 *
 * I know it's a pain, but it's your own fault if you don't use a real OS
 * that comes with a C compiler ;)
 * If you really can't be bothered, omit version 1, but please do distribute
 * versions 2 and 3.
 * 
 * M68K_SPEED should be set to 4 in all binary versions that are distributed.
 */

/*
 * CPU level: 0 = 68000, 1 = 68010, 2 = 68020, 3 = 68020/68881
 * If configured for 68020, the emulator will be a little slower.
 */
#define CPU_LEVEL 2

/*
 * When these two are enabled, a subset of the ECS features is emulated.
 * Actually, it's only the chip identification and big blits. This may be
 * enough to persuade some ECS programs to run.
 * DON'T enable SuperHires or Productivity modes. They are not emulated,
 * and will look very messy. NTSC doesn't work either.
 * OS versions 2.0+ don't like ECS_DENISE at the moment.
#define ECS_DENISE
 */
#define ECS_AGNUS

/*
 * If you don't have any sound hardware, or if you don't want to use it, then
 * this option may make the emulator a little faster. I don't really know
 * whether it's worthwhile, so if you have any results with this one, tell
 * me about it.
 * This will be overridden if you pass "--enable-sound" to configure.
 */
//#define DONT_WANT_SOUND

/*
 * With this parameter, you can tune the CPU speed vs. graphics/sound hardware
 * speed. If you set this to 1, you'll get maximum CPU speed, but demos and
 * games will run very slowly. With large values, the CPU can't execute many
 * instructions per frame, but for many demos, it doesn't have to. A good
 * compromise is setting this to 4. Higher values may produce better sound
 * output, but can make some programs crash because they don't get enough CPU
 * time.
 */
// moved to dtypes.h
//#define M68K_SPEED 4

/*
 * When USE_COMPILER is defined, a m68k->i386 instruction compiler will be
 * used. This is experimental. It has only been tested on a Linux/i386 ELF
 * machine, although it might work on other i386 Unices.
 * This is supposed to speed up application programs. It will not work very
 * well for hardware bangers like games and demos, in fact it will be much
 * slower. It can also be slower for some applications and/or benchmarks.
 * It needs a lot of tuning. Please let me know your results with this.
 * The second define, RELY_ON_LOADSEG_DETECTION, decides how the compiler 
 * tries to detect self-modifying code. If it is not set, the first bytes
 * of every compiled routine are used as checksum before executing the
 * routine. If it is set, the UAE filesystem will perform some checks to 
 * detect whether an executable is being loaded. This is less reliable
 * (it won't work if you don't use the harddisk emulation, so don't try to
 * use floppies or even the RAM disk), but much faster.
#define USE_COMPILER
#define RELY_ON_LOADSEG_DETECTION
 */

/*
 * Set USER_PROGRAMS_BEHAVE to 1 or 2 to indicate that you are only running
 * non-hardware banging programs which leave all the dirty work to the
 * Kickstart. This affects the compiler, and on Linux systems it also
 * affects the normal CPU emulation. Any program that is _not_ in the ROM
 * (i.e. everything but the Kickstart) will use faster memory access 
 * functions.
 * There is of course the problem that the Amiga doesn't really distinguish
 * between user programs and the kernel. Not all of the OS is in the ROM,
 * e.g. the parallel.device is on the disk and gets loaded into RAM at least
 * with Kickstart 1.3 (don't know about newer Kickstarts). So you probably
 * can't print, and some other stuff may also fail to work.
 * A useless option, really, given the way lots of Amiga software is written.
#define USER_PROGRAMS_BEHAVE 0
 */

/***************************************************************************
 * Operating system/machine specific options
 */

/*
 * This option enables a different planar->chunky conversion routine. It may
 * work slightly faster on some machines, it will work a lot slower on others,
 * and it will crash most RISC machines.
 * It seems to be a win on the Pentium. No idea about other x86's.
#define UNALIGNED_PROFITABLE
 */

/*
 * Define this option if you have a very fast Pentium or another x86 CPU that
 * understands the RDTSC instruction. When enabled, it will make sure that the
 * frame rate does not exceed 50Hz. This will give slightly more speed to the
 * CPU emulation. On slow systems (say, worse than a P166), you don't have to
 * worry about the frame rate being too high anyway unless you run only boring
 * WB-based stuff.
#define HAVE_RDTSC
 */

/***************************************************************************
 * Support for broken software. These options are set to default values
 * that are reasonable for most uses. You should not need to change these.
 */

/*
 * Unfortunately, there are people who think it's way k00l to play stupid
 * tricks with the MC68000, the CPU of the Amiga. Enable this option to
 * emulate the prefetch buffer. You don't have to know what a prefetch buffer
 * is, you should know however that it's needed for better compatibility and
 * that emulating it makes the emulator somewhat slower.
 */
#define NO_PREFETCH_BUFFER

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
 * NEWS FLASH! My sources tell me that "Axel" stands for accelerator. Not
 * that it really matters...
#define WANT_SLOW_MULTIPLY
 */

/*
 * This variable was introduced because a program could do a Bcc from
 * whithin chip memory to a location whitin expansion memory. With a
 * pointer variable the program counter would point to the wrong location.
 * With this variable unset the program counter is always correct, but
 * programs will run slower (about 4%).
 * Usually, you'll want to have this defined.
 * @@@ Does anyone know a program that doesn't work with this? I'd like
 * to remove this option.
 */
#define USE_POINTER


// Moved to dtypes.h
//#define HAS_EXPANMEM 1
//#define HAS_BOGOMEM 1
//#define HAS_MEMDISK  1
//#define HAS_HARDFILE 1  
//#define HAS_FILESYS 1  


#endif /* _SHARED_H_ */
