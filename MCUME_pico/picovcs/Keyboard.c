/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Daniel Boris. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

******************************************************************************/
/*
  This module has been completely re-written since X2600
  This has been hacked for the E-125 by Stuart Russell
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "keyboard.h"
#include "types.h"
#include "address.h"
#include "vmachine.h"
#include "extern.h"
#include "memory.h"
#include "display.h"
#include "resource.h"

#include "emuapi.h"

// this returns a byte from the key state buffer. 
// example: KEY(0) will be expanded to: keys[SCANCODE_0]
//#define KEY(__a) keys[SCANCODE_##__a]

extern char *KeyboardGetstate(void);
extern int pausing;
extern int nOptions_Color;
extern int nOptions_P1Diff;
extern int nOptions_P2Diff;


void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}

static int k = 0;
static int hk = 0;

void vcs_Input(int key) {
  k = emu_GetPad();
  hk = emu_ReadI2CKeyboard() & 0xF;  
}

void keyjoy(void) {
	BYTE v1,v2;

	v1=v2=0x0f;
	// read the keyboard state. The return value (in keys) is a pointer to a
	// 256 byte buffer which holds the state of all the keyboard keys. If a 
	// byte's upper bit is set to 1, the key is pressed.
	
	int key = 0;
	if ( !(k & MASK_OSKB) ) {
		key = k;
	}	
	if (key & MASK_JOY2_UP)  v1 &= 0x0E;
	if (key & MASK_JOY2_DOWN) v1 &= 0x0D;
	if (key & MASK_JOY2_RIGHT)  v1 &= 0x0B;
	if (key & MASK_JOY2_LEFT)  v1 &= 0x07;
	
	riotRead[0x280]=(v1 << 4) | v2;
}

void keycons(void) {
//---------------------------------------------------------
// This function reads the state of the joysticks (buttons)
//---------------------------------------------------------
	int key = 0;
	if ( !(k & MASK_OSKB) ) {
		key = k;
	}	

  int kswitches = 0;
	if (hk) { 	 
	  if (hk == 1) {
	  	if (kswitches & 0x01)
	  		kswitches &= ~0x01;
	  	else	
	  		kswitches |= 0x01;
	  }
	  else if (hk == 2) {
	  	if (kswitches & 0x02)
	  		kswitches &= ~0x02;
	  	else	
	  		kswitches |= 0x02;
	  }
	  else if (hk == 3) {
	  	if (kswitches & 0x04)
	  		kswitches &= ~0x04;
	  	else	
	  		kswitches |= 0x04;
	  }
	  else if (hk == 4) {
	  	if (kswitches & 0x08)
	  		kswitches &= ~0x08;
	  	else	
	  		kswitches |= 0x08;
	  }
  }


	// read the keyboard state. The return value (in keys) is a pointer to a
	// 256 byte buffer which holds the state of all the keyboard keys. If a 
	// byte's upper bit is set to 1, the key is pressed.
  
	riotRead[SWCHB] |= 0x03;
	
	if ( (key & MASK_KEY_USER3) || (kswitches & 0x04)  )
    nOptions_Color = !nOptions_Color;
    
	if (!nOptions_Color) 
	  riotRead[SWCHB] &= 0xF7;		/* BW */
	else 
		riotRead[SWCHB] |= 0x08;	/* Color */


	if (   (kswitches & 0x02)
#if (defined(ILI9341) || defined(ST7789)) && defined(USE_VGA)
#else  
		     || (key & MASK_KEY_USER1)
#endif		
		 )
		riotRead[SWCHB] &= 0xFE;	/* Reset */
	if (   (kswitches & 0x01)
#if (defined(ILI9341) || defined(ST7789)) && defined(USE_VGA)
#else  
		     || (key & MASK_KEY_USER2)
#endif
		 )  
		riotRead[SWCHB] &= 0xFD;	/* Select */

	if (nOptions_P1Diff) riotRead[SWCHB] &= 0xBF; 	/* P0 amateur */
	else riotRead[SWCHB] |= 0x40;   /* P0 pro */

	if (nOptions_P2Diff) riotRead[SWCHB] &= 0x7f;    /* P1 amateur */
	else riotRead[SWCHB] |= 0x80;	/* P1 pro */
}

void keytrig(void) {
	// read the keyboard state. The return value (in keys) is a pointer to a
	// 256 byte buffer which holds the state of all the keyboard keys. If a 
	// byte's upper bit is set to 1, the key is pressed.
	
	int key = 0;
	if ( !(k & MASK_OSKB) ) {
		key = k;
	}	
	if (!(tiaWrite[VBLANK] & 0x40)) {
		tiaRead[INPT5]=0x80;
		if (key & MASK_JOY2_BTN)
			tiaRead[INPT4]=0x00;
		else
			tiaRead[INPT4]=0x80;
	}
}

