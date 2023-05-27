/*****************************************************************************\
     Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
                This file is licensed under the Snes9x License.
   For further information, consult the LICENSE file in the root directory.
\*****************************************************************************/

#ifndef _PORT_H_
#define _PORT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#include "emuapi.h"



//#include <Arduino.h>
//#define PROGMEM
#define IRAM_ATTR

//#define printf (void)

#ifndef PIXEL_FORMAT
#define PIXEL_FORMAT RGB565
#endif

// typedef unsigned char    bool8;
typedef bool        bool8;
typedef intptr_t      pint;
typedef int8_t        int8;
typedef uint8_t       uint8;
typedef int16_t       int16;
typedef uint16_t      uint16;
typedef int32_t       int32;
typedef uint32_t      uint32;
typedef int64_t       int64;
typedef uint64_t      uint64;

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

//#define GFX16    1
//typedef uint16 SNESPixel;
typedef uint8 SNESPixel;


#if defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__x86_64__) || defined(__alpha__) || defined(__MIPSEL__) || defined(_M_IX86) || defined(_M_X64) || defined(_XBOX1) || defined(__arm__) || defined(ANDROID) || defined(__aarch64__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__) || defined(IS_LITTLE_ENDIAN)
#define LSB_FIRST
#define FAST_LSB_WORD_ACCESS
#else
#define MSB_FIRST
#endif

#ifdef FAST_LSB_WORD_ACCESS
#define READ_WORD(s)		(*(uint16 *) (s))
#define READ_3WORD(s)		(*(uint32 *) (s) & 0x00ffffff)
#define READ_DWORD(s)		(*(uint32 *) (s))
#define WRITE_WORD(s, d)	*(uint16 *) (s) = (d)
#define WRITE_3WORD(s, d)	*(uint16 *) (s) = (uint16) (d), *((uint8 *) (s) + 2) = (uint8) ((d) >> 16)
#define WRITE_DWORD(s, d)	*(uint32 *) (s) = (d)
#else
#define READ_WORD(s)		(*(uint8 *) (s) | (*((uint8 *) (s) + 1) << 8))
#define READ_3WORD(s)		(*(uint8 *) (s) | (*((uint8 *) (s) + 1) << 8) | (*((uint8 *) (s) + 2) << 16))
#define READ_DWORD(s)		(*(uint8 *) (s) | (*((uint8 *) (s) + 1) << 8) | (*((uint8 *) (s) + 2) << 16) | (*((uint8 *) (s) + 3) << 24))
#define WRITE_WORD(s, d)	*(uint8 *) (s) = (uint8) (d), *((uint8 *) (s) + 1) = (uint8) ((d) >> 8)
#define WRITE_3WORD(s, d)	*(uint8 *) (s) = (uint8) (d), *((uint8 *) (s) + 1) = (uint8) ((d) >> 8), *((uint8 *) (s) + 2) = (uint8) ((d) >> 16)
#define WRITE_DWORD(s, d)	*(uint8 *) (s) = (uint8) (d), *((uint8 *) (s) + 1) = (uint8) ((d) >> 8), *((uint8 *) (s) + 2) = (uint8) ((d) >> 16), *((uint8 *) (s) + 3) = (uint8) ((d) >> 24)
#endif

#define SWAP_WORD(s)		(s) = (((s) & 0xff) <<  8) | (((s) & 0xff00) >> 8)
#define SWAP_DWORD(s)		(s) = (((s) & 0xff) << 24) | (((s) & 0xff00) << 8) | (((s) & 0xff0000) >> 8) | (((s) & 0xff000000) >> 24)


#ifndef _PIXFORM_H_
#define _PIXFORM_H_

#ifdef GFX16
/* RGB565 format */
#define RED_SHIFT_BITS     11
#define GREEN_SHIFT_BITS   6
#define RED_LOW_BIT_MASK   0x0800
#define GREEN_LOW_BIT_MASK 0x0020
#define BLUE_LOW_BIT_MASK  0x0001
#define RED_HI_BIT_MASK    0x8000
#define GREEN_HI_BIT_MASK  0x0400
#define BLUE_HI_BIT_MASK   0x0010
#define FIRST_COLOR_MASK   0xF800
#define SECOND_COLOR_MASK  0x07E0
#define THIRD_COLOR_MASK   0x001F

// RGB are 5,5,5 => 0x00-0x1F values

//#define BUILD_PIXEL(R, G, B)     (((SNESPixel)(R) << 11) | ((SNESPixel)(G) << 5) | (((SNESPixel)(G) & 0x10) << 1) | (int)(B))
#define BUILD_PIXEL(R, G, B)     (((SNESPixel)(R) << 11) | ((SNESPixel)(G) << 6)  | (SNESPixel)(B))

#define RGB_LOW_BITS_MASK          (RED_LOW_BIT_MASK | GREEN_LOW_BIT_MASK | BLUE_LOW_BIT_MASK)
#define RGB_HI_BITS_MASKx2         ((RED_HI_BIT_MASK | GREEN_HI_BIT_MASK | BLUE_HI_BIT_MASK) << 1)
#define RGB_REMOVE_LOW_BITS_MASK   (~RGB_LOW_BITS_MASK)

#else

/*
#define RED_SHIFT_BITS     10
#define GREEN_SHIFT_BITS   5
//7C00   3E0   1F
//RRRRRGGGGGBBBBB
#define RED_LOW_BIT_MASK   0x0800
#define RED_HI_BIT_MASK    0x4000
#define GREEN_LOW_BIT_MASK 0x0020
#define GREEN_HI_BIT_MASK  0x0200
#define BLUE_LOW_BIT_MASK  0x0001
#define BLUE_HI_BIT_MASK   0x0010
#define FIRST_COLOR_MASK   0x7C00
#define SECOND_COLOR_MASK  0x03E0
#define THIRD_COLOR_MASK   0x001F
*/

#define RED_SHIFT_BITS     11
#define GREEN_SHIFT_BITS   6
#define RED_LOW_BIT_MASK   0x0800
#define GREEN_LOW_BIT_MASK 0x0020
#define BLUE_LOW_BIT_MASK  0x0001
#define RED_HI_BIT_MASK    0x8000
#define GREEN_HI_BIT_MASK  0x0400
#define BLUE_HI_BIT_MASK   0x0010
#define FIRST_COLOR_MASK   0xF800
#define SECOND_COLOR_MASK  0x07E0
#define THIRD_COLOR_MASK   0x001F
/* RGB332 format */
//#define RED_SHIFT_BITS     5
//#define GREEN_SHIFT_BITS   2
//842 184 21 
//000 000 00
/*
#define RED_LOW_BIT_MASK   0x20
#define GREEN_LOW_BIT_MASK 0x04
#define BLUE_LOW_BIT_MASK  0x01

#define RED_HI_BIT_MASK    0x80
#define GREEN_HI_BIT_MASK  0x10
#define BLUE_HI_BIT_MASK   0x02

#define FIRST_COLOR_MASK   0xE0
#define SECOND_COLOR_MASK  0x1C
#define THIRD_COLOR_MASK   0x03
*/


#define BUILD_PIXEL(R, G, B)     (((SNESPixel)(R&0x1C) << 3) | ((SNESPixel)(G&0x1C))  | (SNESPixel)(B>>3) )

#define RGB_LOW_BITS_MASK          (RED_LOW_BIT_MASK | GREEN_LOW_BIT_MASK | BLUE_LOW_BIT_MASK)
#define RGB_HI_BITS_MASKx2         ((RED_HI_BIT_MASK | GREEN_HI_BIT_MASK | BLUE_HI_BIT_MASK) << 1)
#define RGB_REMOVE_LOW_BITS_MASK   (~RGB_LOW_BITS_MASK)

#endif

#endif // _PIXFORM_H_

#endif
