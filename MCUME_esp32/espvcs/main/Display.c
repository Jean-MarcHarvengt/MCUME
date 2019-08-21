/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.
   Modified 1996 by Daniel Boris
   Modified 2001 by Stuart Russell

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

   $Id: display.c,v 1.23 1996/03/21 15:52:38 alex Exp $
******************************************************************************/
//This file was modified from its original version for use in PocketVCS
// by Stuart Russell

/*
   Display handling code.
*/

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "btypes.h"
#include "vmachine.h"
#include "address.h"
#include "colours.h"
#include "resource.h"
#include "display.h"
#include "emuapi.h"


int vwidth,vheight,theight;
uint8 * VBuf=0; //[(160)*192+8];
char coltable[256];

/* The refresh skipping counter */
int tv_counter=0;

/* Set up the colormap - original is 24bit!
//------------------------------------------------------------------------------------
// we use a 16 bit color palette (rrrrrggggggbbbbb). This should change based
// on the PDA device (E-125).
//------------------------------------------------------------------------------------*/
static void create_cmap(void)
{
	int i;
	unsigned char red,green,blue;

	/* Initialise parts of the colors array */
	for(i=0; i< 256; i++)
	{
		red  = (unsigned char)((colortable[i] & 0xff0000) >> 16);
		green = (unsigned char)((colortable[i] & 0x00ff00) >> 8);
		blue = (unsigned char)(colortable[i] & 0x0000ff);
    emu_SetPaletteEntry(red,green,blue,i);
	}
}


/* Create the main application shell */
static void create_window()
{
	int i;

	theight = tv_height;
	vheight = tv_height;
	vwidth  = tv_width;
	create_cmap();
	for(i=0; i<256; i++) coltable[i]=1;
	coltable[0]=coltable[1]=coltable[2]=coltable[4]=0;
	coltable[8]=coltable[16]=coltable[32]=coltable[64]=0;
	coltable[128]=0;
}


/* The main initialiser for the X stuff */
int tv_on()
{
   /* Get the basic colors */
	unsigned long m;

  if (VBuf == 0) VBuf = (uint8 *)emu_Malloc((160)*192+8);

	create_window();

	// if pointer size is 4 bytes, make sure the
	// buffer is aligned on an 8 byte boundary.
	if(sizeof(uint8*)==4)
	{
		m=(unsigned long)VBuf;
		m+=8;
		m&=0xFFFFFFF8;
		VBuf=(uint8*)m;
	} 
	memset(VBuf,128,160*192);
	return(1);
}







