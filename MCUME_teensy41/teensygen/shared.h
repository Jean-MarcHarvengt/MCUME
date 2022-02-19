
#ifndef _SHARED_H_
#define _SHARED_H_


#define LSB_FIRST     1
#define SOUND_PRESENT 1
//#define ALIGN_LONG 1
#define GENESIS_HACKS 1


typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;

typedef signed char int8;
typedef signed short int int16;
typedef signed long int int32;

typedef unsigned char UINT8;
typedef unsigned short int UINT16;
typedef unsigned long int UINT32;

typedef signed char INT8;
typedef signed short int INT16;
typedef signed long int INT32;



#include <stdio.h>
#include "macros.h"
#include "m68k.h"
#include "z80.h"
#include "genesis.h"
#include "vdp.h"
#include "render.h"
#include "mem68k.h"
#include "memz80.h"
#include "membnk.h"
#include "memvdp.h"
#include "system.h"
#include "io.h"
#include "sound.h"
#include "fm.h"
#include "sn76496.h"
#include "memory.h"

#define __inline__
#define error(...)



#endif /* _SHARED_H_ */
