
#ifndef _SHARED_H_
#define _SHARED_H_

#define VERSION     "0.9.3"

//#define ALIGN_DWORD 1
#define LSB_FIRST 1

/* Data types */

typedef unsigned char 						byte;

typedef unsigned char						UINT8;
typedef unsigned short					UINT16;
typedef unsigned int						UINT32;
typedef signed char 						INT8;
typedef signed short						INT16;
typedef signed int							INT32;

typedef unsigned char						uint8;
typedef unsigned short					uint16;
typedef unsigned int						uint32;
typedef signed char 						int8;
typedef signed short						int16;
typedef signed int							int32;

#define NULL 0

typedef union {
#ifdef LSB_FIRST
	struct { UINT8 l,h,h2,h3; } b;
	struct { UINT16 l,h; } w;
#else
	struct { UINT8 h3,h2,h,l; } b;
	struct { UINT16 h,l; } w;
#endif
	UINT32 d;
}	PAIR;

#define __inline__

/* To keep the MAME code happy */
#define HAS_YM3812  1
typedef signed short int FMSAMPLE;


#include "memory.h"
#include "z80.h"
#include "sms.h"
#include "vdp.h"
#include "render.h"
#include "sn76496.h"
#include "fmopl.h"
#include "ym2413.h"
#include "system.h"

#endif /* _SHARED_H_ */
