#ifndef __ATARI__
#define __ATARI__

// define globals
#define MEMORY_SIZE 0xC000 //0x10000

#define ATARI_WIDTH  384
#define ATARI_HEIGHT 240
#define TV_PAL      312
#define TV_NTSC     262

//#define DIRTYRECT 1
//#define WORDS_UNALIGNED_OK 1
//#define ALTERNATE_LOOP_COUNTERS 1
//#define NEW_CYCLE_EXACT 1
//#define USE_CURSES 1
//#define WORDS_BIGENDIAN 1
//#define SYNCHRONIZED_SOUND 1 
#define CLIP_SOUND 1 
#define STEREO_SOUND 1 
#define POKEYSND_SIGNED_SAMPLES 1
#define PAGED_MEM 1
#define SOUND 1
#define NO_SIMPLE_PAL_BLENDING 1
#define ANALOGJOY 1

#define max_ypos    tv_mode

extern int tv_mode;



#ifndef _TYPEDEF_H
#define _TYPEDEF_H

//data types
#define UBYTE unsigned char
#define UWORD unsigned short
#define ULONG unsigned int

#define SBYTE signed char
#define SWORD signed short
#define SLONG signed int

#define int8  char
#define int16 short
#define int32 int

#define uint8  unsigned int8
#define uint16 unsigned int16
#define uint32 unsigned int32
#ifndef FALSE
#define FALSE  0
#endif
#ifndef TRUE
#define TRUE   1
#endif

#endif



#endif
