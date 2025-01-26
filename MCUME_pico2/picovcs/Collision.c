/*****************************************************************************

   This file is part of x2600, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.
   
   See the file COPYING for details.
   
   $Id: collision.c,v 1.10 1997/11/22 14:27:47 ahornby Exp $
******************************************************************************/
//This file was modified from its original version for use in PocketVCS
// by Stuart Russell

/* The 2600 collision detection code */

#include <stdlib.h>

#include "types.h"
#include "address.h"
#include "vmachine.h"
#include "col_mask.h"
#include "collision.h"

#include "emuapi.h"

/*
   There are 6 different objects on the screen. Each takes one bit of the 
   collision vector.
   Bit 0: Player 0
   Bit 1: Player 1
   Bit 2: Missile 0
   Bit 3: Missile 1
   Bit 4: Ball
   Bit 5: Playfield
 */

/* The collision vector */
BYTE *colvect=0;

/* The collision lookup table */
unsigned short col_table[256];

/* The collision state */
unsigned short col_state;

/* Resets the collision registers of the tia */
__inline void
reset_collisions (void)
{
  col_state=0;
}

/* Does collision testing on the pixel b */
/* b: byte to test for collisions */
/* Used to build up the collision table */
int
set_collisions (BYTE b)
{
  int res=0;

  if ((b & ML0_MASK) && (b & PL1_MASK))
    res |= M0P1_MASK;
  if ((b & ML0_MASK) && (b & PL0_MASK))
    res |= M0P0_MASK;
  if ((b & ML1_MASK) && (b & PL0_MASK))
    res |=  M1P0_MASK;
  if ((b & ML1_MASK) && (b & PL1_MASK))
    res |=  M1P1_MASK;

  if ((b & PL0_MASK) && (b & PF_MASK))
    res |= P0PF_MASK; 
  if ((b & PL0_MASK) && (b & BL_MASK))
    res |= P0BL_MASK;
  if ((b & PL1_MASK) && (b & PF_MASK))
    res |= P1PF_MASK ;
  if ((b & PL1_MASK) && (b & BL_MASK))
    res |= P1BL_MASK;

  if ((b & ML0_MASK) && (b & PF_MASK))
    res |=  M0PF_MASK;
  if ((b & ML0_MASK) && (b & BL_MASK))
    res |= M0BL_MASK;
  if ((b & ML1_MASK) && (b & PF_MASK))
    res |=  M1PF_MASK; 
  if ((b & ML1_MASK) && (b & BL_MASK))
    res |=  M1BL_MASK;

  if ((b & BL_MASK) && (b & PF_MASK))
    res |=BLPF_MASK;
  if ((b & PL0_MASK) && (b & PL1_MASK))
    res |=P0P1_MASK ;
  if ((b & ML0_MASK) && (b & ML1_MASK))
    res |=M0M1_MASK ;

  return res;
}


void
init_collisions(void)
{
  int i;

  /* Set up the collision lookup table */ 
  for (i = 0; i < 256; i++)
    col_table[i] = set_collisions(i);

  /* Get the colvect 8 byte aligned */
  if (colvect == 0) colvect=(BYTE *)emu_Malloc(28*8);//calloc(28, 8);

  reset_collisions();
}
