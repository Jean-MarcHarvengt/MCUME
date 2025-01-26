/*
 * sio.c - Serial I/O emulation
 *
 * Copyright (C) 1995-1998 David Firth
 * Copyright (C) 1998-2010 Atari800 development team (see DOC/CREDITS)
 *
 * This file is part of the Atari800 emulator project which emulates
 * the Atari 400, 800, 800XL, 130XE, and 5200 8-bit computers.
 *
 * Atari800 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Atari800 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Atari800; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define _POSIX_C_SOURCE 200112L /* for snprintf */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "antic.h"  /* ANTIC_ypos */
#include "atari.h"
#include "cpu.h"
#include "memory.h"
#include "pokey.h"
#include "pokeysnd.h"
#include "sio.h"




/* Put a byte that comes out of POKEY. So get it here... */
void SIO_PutByte(int byte)
{
}

/* Get a byte from the floppy to the pokey. */
int SIO_GetByte(void)
{
	int byte = 0;

	return byte;
}

