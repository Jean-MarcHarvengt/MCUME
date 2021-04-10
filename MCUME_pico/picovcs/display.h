/*****************************************************************************

   This file is part of x2600, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.
   
   See the file COPYING for details.
   
   $Id: display.h,v 1.13 1997/11/22 14:29:54 ahornby Exp $
******************************************************************************/

/*
  Defines for the X11 display code.
  */

#ifndef DISPLAY_H
#define DISPLAY_H

int tv_on(void);
void tv_display(void);

extern int vwidth,vheight,theight;
extern int tv_counter;
extern unsigned char * VBuf;

#endif




