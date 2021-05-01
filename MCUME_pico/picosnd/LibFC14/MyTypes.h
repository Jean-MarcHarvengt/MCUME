// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#ifndef MYTYPES_H
#define MYTYPES_H

#include "Config.h"

// Wanted: 8-bit signed/unsigned.
#if SIZEOF_CHAR > 1
#error Platform unsupported.
#endif  // SIZEOF_CHAR
typedef signed char sbyte;
typedef unsigned char ubyte;

// Wanted: 16-bit signed/unsigned.
#if SIZEOF_SHORT_INT >= 2
typedef signed short int sword;
typedef unsigned short int uword;
#else
typedef signed int sword;
typedef unsigned int uword;
#endif  // SIZEOF_SHORT_INT

// Wanted: 32-bit signed/unsigned.
#if SIZEOF_INT >= 4
typedef signed int sdword;
typedef unsigned int udword;
#elif SIZEOF_LONG_INT >= 4
typedef signed long int sdword;
typedef unsigned long int udword;
#else
#error Platform not supported.
#endif  // SIZEOF_INT

#endif  // MYTYPES_H
