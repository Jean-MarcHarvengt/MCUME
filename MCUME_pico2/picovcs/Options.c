/*****************************************************************************

   This file is part of x2600, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.
   
   See the file COPYING for details.
   
   $Id: options.c,v 1.7 1996/11/24 16:55:40 ahornby Exp $
******************************************************************************/
//This file was modified from its original version for use in PocketVCS
// by Stuart Russell

/* Command Line Option Parser */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

/* *INDENT-OFF* */
/* Options common to all ports of x2600 */
struct BaseOptions {
//  int rr;// unused
  int tvtype;
  int lcon;
  int rcon;
  int bank;
  int magstep;
  char filename[80];
  int sound;
  int swap;
  int realjoy;
  int limit;
  int mousey;
  int mitshm;
  int dbg_level;
} base_opts={0,1,1,1,1,"",1,1,1,1,1,0,0};

char *argv[64];
int argc;

// was written as "(int argc, char **argv)"
void parse_options(void)
{  
//  base_opts.lcon = 1;
}

