/*****************************************************************************

   This file is part of x2600, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.
   
   See the file COPYING for details.
   
   $Id: cpu.c,v 1.11.1.11 1997/11/22 14:27:47 ahornby Exp $
******************************************************************************/
/* 
 * 6507 CPU emulation
 *
 * Originally from X64
 * Modified by Alex Hornby for use in x2600
 * See COPYING for license terms
 */
//This file was modified from its original version for use in PocketVCS
// by Stuart Russell

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "cpu.h"
#include "macro.h"
#include "vmachine.h"
#include "memory.h"
#include "display.h"
#include "exmacro.h"
#include "exmacro.h"
#include "Atari2600EmulatorGlobals.h"

extern int pausing;

/* CPU internals */
ADDRESS program_counter;
BYTE x_register, y_register, stack_pointer;
BYTE accumulator, status_register;
int zero_flag;
int sign_flag;
int overflow_flag;
int break_flag;
int decimal_flag;
int interrupt_flag;
int carry_flag;

/* CPU Clock counts */
/* Aggregate */
CLOCK clk;
/* Current instruction */
CLOCK clkcount = 0;

/* Electron beam adjustment values for nine and twelve colour clock ins. */
#define BEAMNINE 5
#define BEAMTWELVE 9
int beamadj;

/* Used in undocumented 6507 instructions */
/* These are unusual and have no discernable use, but here it is. */
/* val: register to be acted upon */
/* address: address to take high byte from */
/* index: index to add to calculated address */
#ifndef NO_UNDOC_CMDS
void
u_stoshr (unsigned int val, ADDRESS address, BYTE index)
{
  val &= (((address >> 8) + 1) & 0xff);
  if (((address & 0xff) + index) > 0xff)
    address = val;
  STORE ((address + index), val);
}
#endif /* UNDOC */

/* Initialise the CPU */
/* addr: reset vector address */
/* Called from init_hardware() */
void
init_cpu (ADDRESS addr)
{
  SET_SR (0x20);
  AC=0;
  XR=0;
  YR=0;	
  SP = 0xff;
  pc6507 = LOAD_ADDR (addr);
  clk = 0;
  clkcount = 0;
}

/* The main emulation loop, performs the CPU emulation */
void
mainloop (void)
{
  register BYTE b;
//  init_hardware ();

//  int i=6000;
  int i=15200/2;

while (i--) 
{

  do_screen (clkcount);
  b = LOADEXEC (pc6507);
  beamadj = 0;

  switch (b)
	    {
	    case 0:
	      {
		pc6507++;
		/* Force break. */

		SET_BREAK (1);	/* Set break status. */
		PUSH (UPPER (pc6507));
		PUSH (LOWER (pc6507));	/* Push return address into the stack. */
		PUSH (GET_SR ());	/* Push status register into the stack. */
		SET_INTERRUPT (1);	/* Disable interrupts. */
		pc6507 = LOAD_ADDR ((ADDRESS) 65534);	/* Jump using BRK vector (0xfffe). */
		clkcount = 7;
	      }
	      break;

	    case 1:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 6;
	      }

	      break;

	    case 2:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 3:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		/* src = asl(src); AC = ora(src); */

		SET_CARRY (src & 0x80);		/* ASL+ORA */
		src <<= 1;
		src |= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (LOAD_ZERO_ADDR (p1 + XR), (src));
		clkcount = 8;
	      }

	      break;

	    case 4:
	      {
		pc6507 += 2;

		clkcount = 3;
	      }

	      break;

	    case 5:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 3;
	      }

	      break;

	    case 6:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		SET_CARRY (src & 0x80);
		src <<= 1;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 7:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		/* src = asl(src); AC = ora(src); */

		SET_CARRY (src & 0x80);		/* ASL+ORA */
		src <<= 1;
		src |= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 8:
	      {
		register unsigned src = GET_SR ();
		pc6507++;

		PUSH (src);
		/* First push item onto stack and then decrement SP. */
		clkcount = 3;
	      }

	      break;

	    case 9:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 10:
	      {
		register unsigned src = AC;
		pc6507++;

		SET_CARRY (src & 0x80);
		src <<= 1;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 11:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & p1);
		pc6507 += 2;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 12:
	      {
		pc6507 += 3;

		clkcount = 4;
	      }

	      break;

	    case 13:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 4;
	      }

	      break;

	    case 14:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		SET_CARRY (src & 0x80);
		src <<= 1;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 15:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		/* src = asl(src); AC = ora(src); */

		SET_CARRY (src & 0x80);		/* ASL+ORA */
		src <<= 1;
		src |= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 16:
/*  BPL,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register BYTE hb;
		pc6507 += 2;
		/* Branch if sign flag is clear. */
		if (!IF_SIGN ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 17:
/* ORA,    INDIRECT_Y */
	      {
		register p1 = LOAD (pc6507 + 1);
		register ADDRESS p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 2;

		/* If low byte of address  + index reg is > 0xff then extra cycle */
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;
		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 18:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 19:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1) + YR);
		pc6507 += 2;

		/* src = asl(src); AC = ora(src); */

		SET_CARRY (src & 0x80);		/* ASL+ORA */
		src <<= 1;
		src |= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (LOAD_ZERO_ADDR (p1) + YR, (src));
		clkcount = 8;
	      }

	      break;

	    case 20:
	      {
		pc6507 += 2;

		clkcount = 4;
	      }

	      break;

	    case 21:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 4;
	      }

	      break;

	    case 22:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		SET_CARRY (src & 0x80);
		src <<= 1;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 23:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		/* src = asl(src); AC = ora(src); */

		SET_CARRY (src & 0x80);		/* ASL+ORA */
		src <<= 1;
		src |= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 24:
	      {
		pc6507++;


		SET_CARRY ((0));
		clkcount = 2;
	      }

	      break;

	    case 25:
/*  ORA,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;

		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 26:
	      {
		pc6507++;

		clkcount = 2;
	      }

	      break;

	    case 27:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;

		/* src = asl(src); AC = ora(src); */

		SET_CARRY (src & 0x80);		/* ASL+ORA */
		src <<= 1;
		src |= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + YR, (src));
		clkcount = 7;
	      }

	      break;

	    case 28:
	      {
		pc6507 += 3;

		clkcount = 4;
	      }

	      break;

	    case 29:
/*   ORA,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		src |= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 30:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		SET_CARRY (src & 0x80);
		src <<= 1;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 31:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		/* src = asl(src); AC = ora(src); */

		SET_CARRY (src & 0x80);		/* ASL+ORA */
		src <<= 1;
		src |= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 32:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = p2;
		pc6507 += 3;
		/* Jump to subroutine. */
		pc6507--;
		PUSH ((pc6507 >> 8) & 0xff);	/* Push return address onto the stack. */
		PUSH (pc6507 & 0xff);

		pc6507 = (src);
		clkcount = 6;
	      }

	      break;

	    case 33:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 6;
	      }

	      break;

	    case 34:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 35:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		/* src = rol(src); AC = and(src); */

		src <<= 1;
		if (IF_CARRY ())	/* ROL+AND */
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;

		AC &= src;
		SET_SIGN (AC);
		SET_ZERO (AC);
		/* rotated, not disturbed */
		STORE (LOAD_ZERO_ADDR (p1 + XR), (src));
		clkcount = 8;
	      }

	      break;

	    case 36:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		SET_SIGN (src);
		SET_OVERFLOW (0x40 & src);	/* Copy bit 6 to OVERFLOW flag. */
		SET_ZERO (src & AC);
		clkcount = 3;
	      }

	      break;

	    case 37:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 3;
	      }

	      break;

	    case 38:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src <<= 1;
		if (IF_CARRY ())
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 39:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		/* src = rol(src); AC = and(src); */

		src <<= 1;
		if (IF_CARRY ())	/* ROL+AND */
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;

		AC &= src;
		SET_SIGN (AC);
		SET_ZERO (AC);
		/* rotated, not disturbed */
		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 40:
	      {
		register unsigned src;
		pc6507++;

		/* First increment stack pointer and then pull item from stack. */
		src = PULL ();

		SET_SR ((src));
		clkcount = 4;
	      }

	      break;

	    case 41:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 42:
	      {
		register unsigned src = AC;
		pc6507++;

		src <<= 1;
		if (IF_CARRY ())
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 43:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & p1);
		pc6507 += 2;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 44:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		SET_SIGN (src);
		SET_OVERFLOW (0x40 & src);	/* Copy bit 6 to OVERFLOW flag. */
		SET_ZERO (src & AC);
		clkcount = 4;
	      }

	      break;

	    case 45:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 4;
	      }

	      break;

	    case 46:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src <<= 1;
		if (IF_CARRY ())
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 47:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		/* src = rol(src); AC = and(src); */

		src <<= 1;
		if (IF_CARRY ())	/* ROL+AND */
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;

		AC &= src;
		SET_SIGN (AC);
		SET_ZERO (AC);
		/* rotated, not disturbed */
		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 48:
/*   BMI,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register BYTE hb;
		pc6507 += 2;
		/* Branch if sign flag is set. */
		if (IF_SIGN ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 49:
/*  AND,    INDIRECT_Y */
	      {
		register p1 = LOAD (pc6507 + 1);
		register ADDRESS p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 2;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 50:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 51:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1) + YR);
		pc6507 += 2;

		/* src = rol(src); AC = and(src); */

		src <<= 1;
		if (IF_CARRY ())	/* ROL+AND */
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;

		AC &= src;
		SET_SIGN (AC);
		SET_ZERO (AC);
		/* rotated, not disturbed */
		STORE (LOAD_ZERO_ADDR (p1) + YR, (src));
		clkcount = 8;
	      }

	      break;

	    case 52:
	      {
		pc6507 += 2;

		clkcount = 4;
	      }

	      break;

	    case 53:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 4;
	      }

	      break;

	    case 54:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		src <<= 1;
		if (IF_CARRY ())
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 55:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		/* src = rol(src); AC = and(src); */

		src <<= 1;
		if (IF_CARRY ())	/* ROL+AND */
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;

		AC &= src;
		SET_SIGN (AC);
		SET_ZERO (AC);
		/* rotated, not disturbed */
		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 56:
	      {
		pc6507++;


		SET_CARRY ((1));
		clkcount = 2;
	      }

	      break;

	    case 57:
/*  AND,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;
		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 58:
	      {
		pc6507++;

		clkcount = 2;
	      }

	      break;

	    case 59:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;

		/* src = rol(src); AC = and(src); */

		src <<= 1;
		if (IF_CARRY ())	/* ROL+AND */
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;

		AC &= src;
		SET_SIGN (AC);
		SET_ZERO (AC);
		/* rotated, not disturbed */
		STORE (p2 + YR, (src));
		clkcount = 7;
	      }

	      break;

	    case 60:
	      {
		pc6507 += 3;

		clkcount = 4;
	      }

	      break;

	    case 61:
/*   AND,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;
		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;
		src &= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 62:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		src <<= 1;
		if (IF_CARRY ())
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 63:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		/* src = rol(src); AC = and(src); */

		src <<= 1;
		if (IF_CARRY ())	/* ROL+AND */
		  src |= 0x1;
		SET_CARRY (src > 0xff);
		src &= 0xff;

		AC &= src;
		SET_SIGN (AC);
		SET_ZERO (AC);
		/* rotated, not disturbed */
		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 64:
	      {
		register unsigned src;
		pc6507++;
		/* Return from interrupt. */
		/* Load program status and program counter from stack. */
		src = PULL ();
		SET_SR (src);
		src = PULL ();
		src |= (PULL () << 8);	/* Load return address from stack. */

		pc6507 = (src);
		clkcount = 6;
	      }

	      break;

	    case 65:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
		clkcount = 6;
	      }

	      break;

	    case 66:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 67:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		/* src = lsr(src); AC = eor(src); */

		SET_CARRY (src & 0x01);		/* LSR+EOR */
		src >>= 1;
		src ^= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);


		STORE (LOAD_ZERO_ADDR (p1 + XR), (src));
		clkcount = 8;
	      }

	      break;

	    case 68:
	      {
		pc6507 += 2;

		clkcount = 3;
	      }

	      break;

	    case 69:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
		clkcount = 3;
	      }

	      break;

	    case 70:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 71:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		/* src = lsr(src); AC = eor(src); */

		SET_CARRY (src & 0x01);		/* LSR+EOR */
		src >>= 1;
		src ^= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);


		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 72:
/*   PHA,    IMPLIED */
	      {
		register unsigned src = AC;
		pc6507++;

		beamadj = BEAMNINE;
		PUSH (src);
		/* First push item onto stack and then decrement SP. */
		clkcount = 3;
	      }

	      break;

	    case 73:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
		clkcount = 2;
	      }

	      break;

	    case 74:
	      {
		register unsigned src = AC;
		pc6507++;

		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 75:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & p1);
		pc6507 += 2;

		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 76:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = p2;
		pc6507 += 3;


		pc6507 = (src);
		clkcount = 3;
	      }

	      break;

	    case 77:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
		clkcount = 4;
	      }

	      break;

	    case 78:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 79:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		/* src = lsr(src); AC = eor(src); */

		SET_CARRY (src & 0x01);		/* LSR+EOR */
		src >>= 1;
		src ^= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);


		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 80:
/*   BVC,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register BYTE hb;
		pc6507 += 2;
		/* Branch if overflow flag is clear. */
		if (!IF_OVERFLOW ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 81:
/* EOR,    INDIRECT_Y */
	      {
		register p1 = LOAD (pc6507 + 1);
		register p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 2;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
	      }

	      break;

	    case 82:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 83:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1) + YR);
		pc6507 += 2;

		/* src = lsr(src); AC = eor(src); */

		SET_CARRY (src & 0x01);		/* LSR+EOR */
		src >>= 1;
		src ^= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);


		STORE (LOAD_ZERO_ADDR (p1) + YR, (src));
		clkcount = 8;
	      }

	      break;

	    case 84:
	      {
		pc6507 += 2;

		clkcount = 4;
	      }

	      break;

	    case 85:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
		clkcount = 4;
	      }

	      break;

	    case 86:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 87:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		/* src = lsr(src); AC = eor(src); */

		SET_CARRY (src & 0x01);		/* LSR+EOR */
		src >>= 1;
		src ^= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);


		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 88:
	      {
		pc6507++;


		SET_INTERRUPT ((0));
		clkcount = 2;
	      }

	      break;

	    case 89:
/*  EOR,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
	      }

	      break;

	    case 90:
	      {
		pc6507++;

		clkcount = 2;
	      }

	      break;

	    case 91:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;

		/* src = lsr(src); AC = eor(src); */

		SET_CARRY (src & 0x01);		/* LSR+EOR */
		src >>= 1;
		src ^= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);


		STORE (p2 + YR, (src));
		clkcount = 7;
	      }

	      break;

	    case 92:
	      {
		pc6507 += 3;

		clkcount = 4;
	      }

	      break;

	    case 93:
/*  EOR,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;
		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		src ^= AC;
		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src & 0xff);
	      }

	      break;

	    case 94:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 95:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		/* src = lsr(src); AC = eor(src); */

		SET_CARRY (src & 0x01);		/* LSR+EOR */
		src >>= 1;
		src ^= AC;
		src &= 0xff;
		SET_SIGN (src);
		SET_ZERO (src);


		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 96:
	      {
		register unsigned src;
		pc6507++;
		/* Return from subroutine. */
		src = PULL ();
		src += ((PULL ()) << 8) + 1;	/* Load return address from stack and add 1. */

		pc6507 = (src);
		clkcount = 6;
	      }

	      break;

	    case 97:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 2;


		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
		clkcount = 6;
	      }

	      break;

	    case 98:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 99:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		register unsigned int temp;
		pc6507 += 2;


		/* src = ror(src); AC = adc(src);
		 * the real operation discovered by msmakela
		 * ADC only does the BCD fixup */

		temp = src >> 1;	/* ROR+ADC */
		if (IF_CARRY ())
		  temp |= 0x80;

		SET_SIGN (temp);
		SET_ZERO (temp);	/* Set flags like ROR does */

		SET_OVERFLOW ((temp ^ src) & 0x40);	/* ROR causes this ? */

		if (IF_DECIMAL ())
		  {
		    if ((src & 0x0f) > 4)	/* half of the normal */
		      temp = (temp & 0xf0) | ((temp + 6) & 0xf);
		    if (src > 0x4f)
		      temp += 96;

		    SET_CARRY (src > 0x4f);
		  }
		else
		  {
		    SET_CARRY (src & 0x80);	/* 8502 behaviour */
		  }


		STORE (LOAD_ZERO_ADDR (p1 + XR), ((BYTE) temp));
		clkcount = 8;
	      }

	      break;

	    case 100:
	      {
		pc6507 += 2;

		clkcount = 3;
	      }

	      break;

	    case 101:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 2;


		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
		clkcount = 3;
	      }

	      break;

	    case 102:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		if (IF_CARRY ())
		  src |= 0x100;
		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 103:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		register unsigned int temp;
		pc6507 += 2;


		/* src = ror(src); AC = adc(src);
		 * the real operation discovered by msmakela
		 * ADC only does the BCD fixup */

		temp = src >> 1;	/* ROR+ADC */
		if (IF_CARRY ())
		  temp |= 0x80;

		SET_SIGN (temp);
		SET_ZERO (temp);	/* Set flags like ROR does */

		SET_OVERFLOW ((temp ^ src) & 0x40);	/* ROR causes this ? */

		if (IF_DECIMAL ())
		  {
		    if ((src & 0x0f) > 4)	/* half of the normal */
		      temp = (temp & 0xf0) | ((temp + 6) & 0xf);
		    if (src > 0x4f)
		      temp += 96;

		    SET_CARRY (src > 0x4f);
		  }
		else
		  {
		    SET_CARRY (src & 0x80);	/* 8502 behaviour */
		  }


		STORE_ZERO (p1, ((BYTE) temp));
		clkcount = 5;
	      }

	      break;

	    case 104:
/*   PLA,    IMPLIED */
	      {
		register unsigned src;
		pc6507++;

		clkcount = 4;
		/* First increment stack pointer and then pull item from stack. */
		src = PULL ();
		SET_SIGN (src);	/* Change sign and zero flag accordingly. */
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 105:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 2;


		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
		clkcount = 2;
	      }

	      break;

	    case 106:
	      {
		register unsigned src = AC;
		pc6507++;

		if (IF_CARRY ())
		  src |= 0x100;
		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 107:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & p1);
		pc6507 += 2;

		if (IF_CARRY ())
		  src |= 0x100;
		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 108:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD_ADDR (p2);
		pc6507 += 3;


		pc6507 = (src);
		clkcount = 5;
	      }

	      break;

	    case 109:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 3;


		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
		clkcount = 4;
	      }

	      break;

	    case 110:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		if (IF_CARRY ())
		  src |= 0x100;
		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 111:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		register unsigned int temp;
		pc6507 += 3;


		/* src = ror(src); AC = adc(src);
		 * the real operation discovered by msmakela
		 * ADC only does the BCD fixup */

		temp = src >> 1;	/* ROR+ADC */
		if (IF_CARRY ())
		  temp |= 0x80;

		SET_SIGN (temp);
		SET_ZERO (temp);	/* Set flags like ROR does */

		SET_OVERFLOW ((temp ^ src) & 0x40);	/* ROR causes this ? */

		if (IF_DECIMAL ())
		  {
		    if ((src & 0x0f) > 4)	/* half of the normal */
		      temp = (temp & 0xf0) | ((temp + 6) & 0xf);
		    if (src > 0x4f)
		      temp += 96;

		    SET_CARRY (src > 0x4f);
		  }
		else
		  {
		    SET_CARRY (src & 0x80);	/* 8502 behaviour */
		  }


		STORE (p2, ((BYTE) temp));
		clkcount = 6;
	      }

	      break;

	    case 112:
/*   BVS,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register BYTE hb;
		pc6507 += 2;
		/* Branch if overflow flag is set. */
		if (IF_OVERFLOW ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 113:
	      {
		register p1 = LOAD (pc6507 + 1);
		register ADDRESS p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 2;

		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;

		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
	      }

	      break;

	    case 114:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 115:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1) + YR);
		register unsigned int temp;
		pc6507 += 2;


		/* src = ror(src); AC = adc(src);
		 * the real operation discovered by msmakela
		 * ADC only does the BCD fixup */

		temp = src >> 1;	/* ROR+ADC */
		if (IF_CARRY ())
		  temp |= 0x80;

		SET_SIGN (temp);
		SET_ZERO (temp);	/* Set flags like ROR does */

		SET_OVERFLOW ((temp ^ src) & 0x40);	/* ROR causes this ? */

		if (IF_DECIMAL ())
		  {
		    if ((src & 0x0f) > 4)	/* half of the normal */
		      temp = (temp & 0xf0) | ((temp + 6) & 0xf);
		    if (src > 0x4f)
		      temp += 96;

		    SET_CARRY (src > 0x4f);
		  }
		else
		  {
		    SET_CARRY (src & 0x80);	/* 8502 behaviour */
		  }


		STORE (LOAD_ZERO_ADDR (p1) + YR, ((BYTE) temp));
		clkcount = 8;
	      }

	      break;

	    case 116:
	      {
		pc6507 += 2;

		clkcount = 4;
	      }

	      break;

	    case 117:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 2;


		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
		clkcount = 4;
	      }

	      break;

	    case 118:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		if (IF_CARRY ())
		  src |= 0x100;
		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 119:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		register unsigned int temp;
		pc6507 += 2;


		/* src = ror(src); AC = adc(src);
		 * the real operation discovered by msmakela
		 * ADC only does the BCD fixup */

		temp = src >> 1;	/* ROR+ADC */
		if (IF_CARRY ())
		  temp |= 0x80;

		SET_SIGN (temp);
		SET_ZERO (temp);	/* Set flags like ROR does */

		SET_OVERFLOW ((temp ^ src) & 0x40);	/* ROR causes this ? */

		if (IF_DECIMAL ())
		  {
		    if ((src & 0x0f) > 4)	/* half of the normal */
		      temp = (temp & 0xf0) | ((temp + 6) & 0xf);
		    if (src > 0x4f)
		      temp += 96;

		    SET_CARRY (src > 0x4f);
		  }
		else
		  {
		    SET_CARRY (src & 0x80);	/* 8502 behaviour */
		  }


		STORE_ZERO (p1 + XR, ((BYTE) temp));
		clkcount = 6;
	      }

	      break;

	    case 120:
	      {
		pc6507++;


		SET_INTERRUPT ((1));
		clkcount = 2;
	      }

	      break;

	    case 121:
/*   ADC,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 3;

		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
	      }

	      break;

	    case 122:
	      {
		pc6507++;

		clkcount = 2;
	      }

	      break;

	    case 123:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		register unsigned int temp;
		pc6507 += 3;


		/* src = ror(src); AC = adc(src);
		 * the real operation discovered by msmakela
		 * ADC only does the BCD fixup */

		temp = src >> 1;	/* ROR+ADC */
		if (IF_CARRY ())
		  temp |= 0x80;

		SET_SIGN (temp);
		SET_ZERO (temp);	/* Set flags like ROR does */

		SET_OVERFLOW ((temp ^ src) & 0x40);	/* ROR causes this ? */

		if (IF_DECIMAL ())
		  {
		    if ((src & 0x0f) > 4)	/* half of the normal */
		      temp = (temp & 0xf0) | ((temp + 6) & 0xf);
		    if (src > 0x4f)
		      temp += 96;

		    SET_CARRY (src > 0x4f);
		  }
		else
		  {
		    SET_CARRY (src & 0x80);	/* 8502 behaviour */
		  }


		STORE (p2 + YR, ((BYTE) temp));
		clkcount = 7;
	      }

	      break;

	    case 124:
	      {
		pc6507 += 3;

		clkcount = 4;
	      }

	      break;

	    case 125:
/*  ADC,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		register unsigned int temp = src + AC + (IF_CARRY ()? 1 : 0);
		pc6507 += 3;

		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		SET_ZERO (temp & 0xff);		/* This is not valid in decimal mode */

		/*
		 * In decimal mode this is "correct" but not exact emulation. However,
		 * the probability of different result when using undefined BCD codes is
		 * less than 6%
		 * Sign and Overflow are set between BCD fixup of the two digits  -MSM
		 */

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (src & 0xf) + (IF_CARRY ()? 1 : 0)) > 9)
		      temp += 6;

		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));

		    if (temp > 0x99)
		      temp += 96;

		    SET_CARRY (temp > 0x99);
		  }
		else
		  {
		    SET_SIGN (temp);
		    SET_OVERFLOW (!((AC ^ src) & 0x80) && ((AC ^ temp) & 0x80));
		    SET_CARRY (temp > 0xff);
		  }

		AC = ((BYTE) temp);
	      }

	      break;

	    case 126:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		if (IF_CARRY ())
		  src |= 0x100;
		SET_CARRY (src & 0x01);
		src >>= 1;

		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 127:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		register unsigned int temp;
		pc6507 += 3;


		/* src = ror(src); AC = adc(src);
		 * the real operation discovered by msmakela
		 * ADC only does the BCD fixup */

		temp = src >> 1;	/* ROR+ADC */
		if (IF_CARRY ())
		  temp |= 0x80;

		SET_SIGN (temp);
		SET_ZERO (temp);	/* Set flags like ROR does */

		SET_OVERFLOW ((temp ^ src) & 0x40);	/* ROR causes this ? */

		if (IF_DECIMAL ())
		  {
		    if ((src & 0x0f) > 4)	/* half of the normal */
		      temp = (temp & 0xf0) | ((temp + 6) & 0xf);
		    if (src > 0x4f)
		      temp += 96;

		    SET_CARRY (src > 0x4f);
		  }
		else
		  {
		    SET_CARRY (src & 0x80);	/* 8502 behaviour */
		  }


		STORE (p2 + XR, ((BYTE) temp));
		clkcount = 7;
	      }

	      break;

	    case 128:
	      {
		pc6507 += 2;

		clkcount = 2;
	      }

	      break;

	    case 129:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = AC;
		pc6507 += 2;


		STORE (LOAD_ZERO_ADDR (p1 + XR), (src));
		clkcount = 6;
	      }

	      break;

	    case 130:
	      {
		pc6507 += 2;

		clkcount = 2;
	      }

	      break;

	    case 131:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & XR);
		pc6507 += 2;


		STORE (LOAD_ZERO_ADDR (p1 + XR), (src));
		clkcount = 6;
	      }

	      break;

	    case 132:
/*  STY,    ZERO_PAGE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = YR;

		clkcount = 3;
		pc6507 += 2;

		beamadj = BEAMNINE;
		STORE_ZERO (p1, (src));
	      }

	      break;

	    case 133:
/*   STA,    ZERO_PAGE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = AC;

		clkcount = 3;
		pc6507 += 2;

		beamadj = BEAMNINE;
		STORE_ZERO (p1, (src));
	      }

	      break;

	    case 134:
/*  STX,    ZERO_PAGE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = XR;

		clkcount = 3;
		pc6507 += 2;

		beamadj = BEAMNINE;
		STORE_ZERO (p1, (src));
	      }

	      break;

	    case 135:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & XR);
		pc6507 += 2;


		STORE_ZERO (p1, (src));
		clkcount = 3;
	      }

	      break;

	    case 136:
	      {
		register unsigned src = YR;
		pc6507++;

		src = (src - 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
		clkcount = 2;
	      }

	      break;

	    case 137:
	      {
		pc6507 += 2;

		clkcount = 2;
	      }

	      break;

	    case 138:
	      {
		register unsigned src = XR;
		pc6507++;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 139:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = ((AC | 0xee) & XR & p1);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 140:
/*  STY,    ABSOLUTE */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = YR;
		pc6507 += 3;

		beamadj = BEAMTWELVE;
		STORE (p2, (src));
		clkcount = 4;
	      }

	      break;

	    case 141:
/*  STA,    ABSOLUTE */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = AC;
		pc6507 += 3;

		beamadj = BEAMTWELVE;
		STORE (p2, (src));
		clkcount = 4;
	      }

	      break;

	    case 142:
/*  STA,    ABSOLUTE */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = XR;
		pc6507 += 3;

		beamadj = BEAMTWELVE;
		STORE (p2, (src));
		clkcount = 4;
	      }

	      break;

	    case 143:
/* STX,    ABSOLUTE */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = (AC & XR);
		pc6507 += 3;

		beamadj = BEAMTWELVE;
		STORE (p2, (src));
		clkcount = 4;
	      }

	      break;

	    case 144:
/*  BCC,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register hb;
		register unsigned src = p1;
		pc6507 += 2;
		/* Branch if carry flag is clear. */
		if (!IF_CARRY ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 145:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = AC;
		pc6507 += 2;


		STORE (LOAD_ZERO_ADDR (p1) + YR, (src));
		clkcount = 6;
	      }

	      break;

	    case 146:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 147:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & XR);
		pc6507 += 2;


		u_stoshr (src, LOAD_ZERO_ADDR (p1), YR);
		clkcount = 6;
	      }

	      break;

	    case 148:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = YR;
		clkcount = 4;
		pc6507 += 2;

		beamadj = BEAMTWELVE;
		STORE_ZERO (p1 + XR, (src));
	      }

	      break;

	    case 149:
/*  STA,    ZERO_PAGE_X */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = AC;
		clkcount = 4;
		pc6507 += 2;

		beamadj = BEAMTWELVE;
		STORE_ZERO (p1 + XR, (src));
	      }

	      break;

	    case 150:
/* STX,    ZERO_PAGE_Y */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = XR;
		clkcount = 4;
		pc6507 += 2;

		beamadj = BEAMTWELVE;
		STORE_ZERO (p1 + YR, (src));
	      }

	      break;

	    case 151:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & XR);
		pc6507 += 2;


		STORE_ZERO (p1 + YR, (src));
		clkcount = 4;
	      }

	      break;

	    case 152:
	      {
		register unsigned src = YR;
		pc6507++;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 153:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = AC;
		pc6507 += 3;


		STORE (p2 + YR, (src));
		clkcount = 5;
	      }

	      break;

	    case 154:
	      {
		register unsigned src = XR;
		pc6507++;


		SP = (src);
		clkcount = 2;
	      }

	      break;

	    case 155:
	      {
		register unsigned src = (AC & XR);
		pc6507 += 3;


		SP = src; /* SHS */ ;
		clkcount = 5;
	      }

	      break;

	    case 156:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = YR;
		pc6507 += 3;


		u_stoshr (src, p2, XR);
		clkcount = 5;
	      }

	      break;

	    case 157:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = AC;
		pc6507 += 3;


		STORE (p2 + XR, (src));
		clkcount = 5;
	      }

	      break;

	    case 158:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = XR;
		pc6507 += 3;


		u_stoshr (src, p2, YR);
		clkcount = 5;
	      }

	      break;

	    case 159:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = (AC & XR);
		pc6507 += 3;


		u_stoshr (src, p2, YR);
		clkcount = 5;
	      }

	      break;

	    case 160:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
		clkcount = 2;
	      }

	      break;

	    case 161:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 6;
	      }

	      break;

	    case 162:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 2;
	      }

	      break;

	    case 163:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = (src);
		clkcount = 6;
	      }

	      break;

	    case 164:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
		clkcount = 3;
	      }

	      break;

	    case 165:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 3;
	      }

	      break;

	    case 166:
/*      LDX,   ZERO_PAGE */
	      {
		register p1 = LOADEXEC (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);

		clkcount = 3;
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
	      }

	      break;

	    case 167:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = (src);
		clkcount = 3;
	      }

	      break;

	    case 168:
	      {
		register unsigned src = AC;
		pc6507++;

		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
		clkcount = 2;
	      }

	      break;

	    case 169:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 2;
	      }

	      break;

	    case 170:
	      {
		register unsigned src = AC;
		pc6507++;

		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 2;
	      }

	      break;

	    case 171:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = (AC & p1);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = (src);
		clkcount = 2;
	      }

	      break;

	    case 172:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
		clkcount = 4;
	      }

	      break;

	    case 173:		/* LDA absolute */
	      {
		register p2 = load_abs_addr ();
		register unsigned src;
		clkcount = 4;
		src = LOAD (p2);
		pc6507 += 3;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 174:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 4;
	      }

	      break;

	    case 175:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = (src);
		clkcount = 4;
	      }

	      break;

	    case 176:
/*   BCS,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register BYTE hb;
		pc6507 += 2;
		/* Branch if carry flag is set. */
		if (IF_CARRY ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 177:
/*   LDA,    INDIRECT_Y */
	      {
		register p1 = LOAD (pc6507 + 1);
		register ADDRESS p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 2;

		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 178:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 179:
/*   LAX,    INDIRECT_Y */
	      {
		register p1 = LOAD (pc6507 + 1);
		register ADDRESS p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 2;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = (src);
	      }

	      break;

	    case 180:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
		clkcount = 4;
	      }

	      break;

	    case 181:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
		clkcount = 4;
	      }

	      break;

	    case 182:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + YR);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 4;
	      }

	      break;

	    case 183:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + YR);
		pc6507 += 2;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = (src);
		clkcount = 4;
	      }

	      break;

	    case 184:
	      {
		pc6507++;


		SET_OVERFLOW ((0));
		clkcount = 2;
	      }

	      break;

	    case 185:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 186:
	      {
		register unsigned src = SP;
		pc6507++;

		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 2;
	      }

	      break;

	    case 187:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = (SP & LOAD (p2 + YR));
		pc6507 += 3;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = SP = (src);
	      }

	      break;

	    case 188:
/*   LDY,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;
		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;


		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
	      }

	      break;

	    case 189:
/*  LDA,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = (src);
	      }

	      break;

	    case 190:
/*   LDX,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
	      }

	      break;

	    case 191:
/*   LAX,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		SET_SIGN (src);
		SET_ZERO (src);

		AC = XR = (src);
	      }

	      break;

	    case 192:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		src = YR - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 2;
	      }

	      break;

	    case 193:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 6;
	      }

	      break;

	    case 194:
	      {
		pc6507 += 2;

		clkcount = 2;
	      }

	      break;

	    case 195:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		pc6507 += 2;

		/* cmp(--src & 0xff));  */

		src = (src - 1) & 0xff;		/* DEC+CMP */
		SET_CARRY (AC >= src);
		SET_SIGN (AC - src);
		SET_ZERO (AC != src);

		STORE (LOAD_ZERO_ADDR (p1 + XR), (src));
		clkcount = 8;
	      }

	      break;

	    case 196:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src = YR - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 3;
	      }

	      break;

	    case 197:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 3;
	      }

	      break;

	    case 198:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src = (src - 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 199:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		/* cmp(--src & 0xff));  */

		src = (src - 1) & 0xff;		/* DEC+CMP */
		SET_CARRY (AC >= src);
		SET_SIGN (AC - src);
		SET_ZERO (AC != src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 200:
	      {
		register unsigned src = YR;
		pc6507++;

		src = (src + 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		YR = (src);
		clkcount = 2;
	      }

	      break;

	    case 201:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 2;
	      }

	      break;

	    case 202:
	      {
		register unsigned src = XR;
		pc6507++;

		src = (src - 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 2;
	      }

	      break;

	    case 203:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		src = (AC & XR) - src;	/* Carry is ignored (CMP) */
		/* Overflow flag may be affected */
		SET_CARRY (src < 0x100);

		src &= 0xff;	/* No decimal mode */
		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 2;
	      }

	      break;

	    case 204:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src = YR - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 4;
	      }

	      break;

	    case 205:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 4;
	      }

	      break;

	    case 206:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src = (src - 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 207:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		/* cmp(--src & 0xff));  */

		src = (src - 1) & 0xff;		/* DEC+CMP */
		SET_CARRY (AC >= src);
		SET_SIGN (AC - src);
		SET_ZERO (AC != src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 208:
/*  BNE,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register BYTE hb;
		pc6507 += 2;
		/* Branch if  zero flag is clear. */
		if (!IF_ZERO ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 209:
/*   CMP,    INDIRECT_Y */
	      {
		register p1 = LOAD (pc6507 + 1);
		register ADDRESS p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 2;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
	      }

	      break;

	    case 210:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		////mon (pc6507);
	      }

	      break;

	    case 211:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1) + YR);
		pc6507 += 2;

		/* cmp(--src & 0xff));  */

		src = (src - 1) & 0xff;		/* DEC+CMP */
		SET_CARRY (AC >= src);
		SET_SIGN (AC - src);
		SET_ZERO (AC != src);

		STORE (LOAD_ZERO_ADDR (p1) + YR, (src));
		clkcount = 8;
	      }

	      break;

	    case 212:
	      {
		pc6507 += 2;

		clkcount = 4;
	      }

	      break;

	    case 213:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 4;
	      }

	      break;

	    case 214:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		src = (src - 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 215:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		/* cmp(--src & 0xff));  */

		src = (src - 1) & 0xff;		/* DEC+CMP */
		SET_CARRY (AC >= src);
		SET_SIGN (AC - src);
		SET_ZERO (AC != src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 216:
	      {
		pc6507++;


		SET_DECIMAL ((0));
		clkcount = 2;
	      }

	      break;

	    case 217:
/*   CMP,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;
		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
	      }

	      break;

	    case 218:
	      {
		pc6507++;

		clkcount = 2;
	      }

	      break;

	    case 219:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		pc6507 += 3;

		/* cmp(--src & 0xff));  */

		src = (src - 1) & 0xff;		/* DEC+CMP */
		SET_CARRY (AC >= src);
		SET_SIGN (AC - src);
		SET_ZERO (AC != src);

		STORE (p2 + YR, (src));
		clkcount = 7;
	      }

	      break;

	    case 220:
	      {
		pc6507 += 3;

		clkcount = 4;
	      }

	      break;

	    case 221:
/*   CMP,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;
		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		src = AC - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
	      }

	      break;

	    case 222:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		src = (src - 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 223:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		/* cmp(--src & 0xff));  */

		src = (src - 1) & 0xff;		/* DEC+CMP */
		SET_CARRY (AC >= src);
		SET_SIGN (AC - src);
		SET_ZERO (AC != src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 224:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		pc6507 += 2;

		src = XR - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 2;
	      }

	      break;

	    case 225:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 2;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
		clkcount = 6;
	      }

	      break;

	    case 226:
	      {
		pc6507 += 2;

		clkcount = 2;
	      }

	      break;

	    case 227:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1 + XR));
		register unsigned int temp;
		pc6507 += 2;


		/* src = ++src & 0xff; AC = sbc(src); */

		src = ((src + 1) & 0xff);	/* INC+SBC */

		temp = AC - src - (IF_CARRY ()? 0 : 1);

		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = temp;
		/* src saved */
		STORE (LOAD_ZERO_ADDR (p1 + XR), (src));
		clkcount = 8;
	      }

	      break;

	    case 228:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src = XR - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 3;
	      }

	      break;

	    case 229:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 2;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
		clkcount = 3;
	      }

	      break;

	    case 230:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		pc6507 += 2;

		src = (src + 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 231:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1);
		register unsigned int temp;
		pc6507 += 2;


		/* src = ++src & 0xff; AC = sbc(src); */

		src = ((src + 1) & 0xff);	/* INC+SBC */

		temp = AC - src - (IF_CARRY ()? 0 : 1);

		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = temp;
		/* src saved */
		STORE_ZERO (p1, (src));
		clkcount = 5;
	      }

	      break;

	    case 232:
	      {
		register unsigned src = XR;
		pc6507++;

		src = (src + 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		XR = (src);
		clkcount = 2;
	      }

	      break;

	    case 233:
/*  SBC,    IMMEDIATE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 2;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */

		if (!IF_DECIMAL ())
		  {
		    SET_SIGN (temp);
		    SET_ZERO (temp & 0xff);	/* Sign and Zero are invalid in decimal mode */

		    SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));
		    SET_CARRY (temp < 0x100);
		    AC = (temp & 0xff);

		  }
		else
		  {
		    int bcd1, bcd2;
		    BYTE old_A;
		    int C = IF_CARRY ()? 1 : 0;

		    old_A = AC;

		    bcd1 = fromBCD (AC);
		    bcd2 = fromBCD (src);

		    bcd1 = bcd1 - bcd2 - !C;

		    if (bcd1 < 0)
		      bcd1 = 100 - (-bcd1);
		    AC = toBCD (bcd1);

		    SET_CARRY ((old_A < (src + !C)) ? 0 : 1);
		    SET_OVERFLOW ((old_A ^ AC) & 0x80);
		  }
		clkcount = 2;
	      }

	      break;

	    case 234:
	      {
		pc6507++;

		clkcount = 2;
	      }

	      break;

	    case 235:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 2;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
		clkcount = 2;
	      }

	      break;

	    case 236:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src = XR - src;
		SET_CARRY (src < 0x100);
		SET_SIGN (src);
		SET_ZERO (src &= 0xff);
		clkcount = 4;
	      }

	      break;

	    case 237:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 3;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
		clkcount = 4;
	      }

	      break;

	    case 238:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		pc6507 += 3;

		src = (src + 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 239:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2);
		register unsigned int temp;
		pc6507 += 3;


		/* src = ++src & 0xff; AC = sbc(src); */

		src = ((src + 1) & 0xff);	/* INC+SBC */

		temp = AC - src - (IF_CARRY ()? 0 : 1);

		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = temp;
		/* src saved */
		STORE (p2, (src));
		clkcount = 6;
	      }

	      break;

	    case 240:
/*   BEQ,    RELATIVE */
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = p1;
		register BYTE hb;
		pc6507 += 2;
		/* Branch if  zero flag is set. */
		if (IF_ZERO ())
		  {
		    hb = UPPER (pc6507);
		    pc6507 = REL_ADDR (pc6507, src);
		    if (brtest (hb))
		      /* Same page */
		      clkcount = 3;
		    else
		      /* Different page */
		      clkcount = 4;
		  }
		else
		  clkcount = 2;
	      }

	      break;

	    case 241:
	      {
		register p1 = LOAD (pc6507 + 1);
		register ADDRESS p2 = LOAD_ZERO_ADDR (p1);
		register unsigned src = LOAD (p2 + YR);
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 2;

		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 6;
		else
		  /* Same page */
		  clkcount = 5;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
	      }

	      break;

	    case 242:
	      {
		pc6507++; // KILL Instruction
		/* No such opcode. */
		//(void) printf ("Illegal instruction.\n");
		//verflg = 1;
		//--pc6507;
		//show();
		//mon (pc6507);
	      }

	      break;

	    case 243:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD (LOAD_ZERO_ADDR (p1) + YR);
		register unsigned int temp;
		pc6507 += 2;


		/* src = ++src & 0xff; AC = sbc(src); */

		src = ((src + 1) & 0xff);	/* INC+SBC */

		temp = AC - src - (IF_CARRY ()? 0 : 1);

		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = temp;
		/* src saved */
		STORE (LOAD_ZERO_ADDR (p1) + YR, (src));
		clkcount = 8;
	      }

	      break;

	    case 244:
	      {
		pc6507 += 2;

		clkcount = 4;
	      }

	      break;

	    case 245:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 2;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
		clkcount = 4;
	      }

	      break;

	    case 246:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		pc6507 += 2;

		src = (src + 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 247:
	      {
		register p1 = LOAD (pc6507 + 1);
		register unsigned src = LOAD_ZERO (p1 + XR);
		register unsigned int temp;
		pc6507 += 2;


		/* src = ++src & 0xff; AC = sbc(src); */

		src = ((src + 1) & 0xff);	/* INC+SBC */

		temp = AC - src - (IF_CARRY ()? 0 : 1);

		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = temp;
		/* src saved */
		STORE_ZERO (p1 + XR, (src));
		clkcount = 6;
	      }

	      break;

	    case 248:
/*  SED,    IMPLIED */
	      {
		pc6507++;

		SET_DECIMAL ((1));
		clkcount = 2;
	      }

	      break;

	    case 249:
/*  SBC,    ABSOLUTE_Y */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 3;

		if (pagetest (p2, YR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
	      }

	      break;

	    case 250:
	      {
		pc6507++;

		clkcount = 2;
	      }

	      break;

	    case 251:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + YR);
		register unsigned int temp;
		pc6507 += 3;


		/* src = ++src & 0xff; AC = sbc(src); */

		src = ((src + 1) & 0xff);	/* INC+SBC */

		temp = AC - src - (IF_CARRY ()? 0 : 1);

		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = temp;
		/* src saved */
		STORE (p2 + YR, (src));
		clkcount = 7;
	      }

	      break;

	    case 252:
	      {
		pc6507 += 3;

		clkcount = 4;
	      }

	      break;

	    case 253:
/*   SBC,    ABSOLUTE_X */
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		register unsigned int temp = AC - src - (IF_CARRY ()? 0 : 1);
		pc6507 += 3;

		if (pagetest (p2, XR))
		  /* Over page */
		  clkcount = 5;
		else
		  /* Same page */
		  clkcount = 4;

		/*
		 * SBC is not exact either. It has 6% different results too.
		 */


		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = (temp & 0xff);
	      }

	      break;

	    case 254:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		pc6507 += 3;

		src = (src + 1) & 0xff;
		SET_SIGN (src);
		SET_ZERO (src);

		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;

	    case 255:
	      {
		register p2 = load_abs_addr ();
		register unsigned src = LOAD (p2 + XR);
		register unsigned int temp;
		pc6507 += 3;


		/* src = ++src & 0xff; AC = sbc(src); */

		src = ((src + 1) & 0xff);	/* INC+SBC */

		temp = AC - src - (IF_CARRY ()? 0 : 1);

		SET_SIGN (temp);
		SET_ZERO (temp & 0xff);		/* Sign and Zero are invalid in decimal mode */

		SET_OVERFLOW (((AC ^ temp) & 0x80) && ((AC ^ src) & 0x80));

		if (IF_DECIMAL ())
		  {
		    if (((AC & 0xf) + (IF_CARRY ()? 1 : 0)) < (src & 0xf))
		      temp -= 6;
		    if (temp > 0x99)
		      temp -= 0x60;
		  }
		SET_CARRY (temp < 0x100);

		AC = temp;
		/* src saved */
		STORE (p2 + XR, (src));
		clkcount = 7;
	      }

	      break;



	    }			/* switch */

	  clk += clkcount;

  }		/* while(!pausing) */

	
bThreadRunning = 0;
pausing = 0;

}				/* mainloop */
