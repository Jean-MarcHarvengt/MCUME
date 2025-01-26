/*****************************************************************************

   This file is part of x2600, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.
   
   See the file COPYING for details.
   
   $Id: vmachine.c,v 2.22 1997/11/22 14:27:47 ahornby Exp $
******************************************************************************/
//This file was modified from its original version for use in PocketVCS
// by Stuart Russell

/* 
   The virtual machine. Contains the RIOT timer code, and hardware 
   initialisation.
 */

#include <stdio.h>
#include "types.h"
#include "address.h"
#include "options.h"
#include "display.h"
#include "raster.h"
#include "cpu.h"
#include "collision.h"
#include "sound.h"
#include "vmachine.h"
#include "tiasound.h"

#include "emuapi.h"

#define snd 1

extern void BlitScreen(void);
extern int nOptions_SoundBufSize;
extern int nOptions_SoundOn;
int Touchpadx=0;
int Touchpady=0;
//extern int Touchpadx;
//extern int Touchpady;
extern int nOptions_Landscape;
extern int nOptions_SkipFrames;



/* The Rom define might need altering for paged carts */
/* Enough for 16k carts */
int rom_size;

/* Used as a simple file buffer to store the data */
// JMH
//BYTE theCart[16384];
//BYTE theCart[4096];
BYTE * theCart=0;

/* Scratch area for those banking types that require memcpy() */
//BYTE cartScratch[4096];
BYTE * cartScratch=0;
/* Area for those carts containing RAM */
//BYTE cartRam[1024];
BYTE * cartRam=0;

/* Pointer to start of ROM data */
BYTE *theRom;

BYTE theRam[128];
BYTE tiaRead[0x0e];
BYTE tiaWrite[0x2d];
BYTE keypad[2][4];

/* These don't strictly need so much space */
BYTE riotRead[0x298];
BYTE riotWrite[0x298];

/* 
   Hardware addresses not programmer accessible 
 */

/* Set if whole emulator is reset */
int reset_flag = 0;

/* The timer resolution, can be 1,8,64,1024 */
int timer_res = 32;
int timer_count = 0;
int timer_clks = 0;
extern CLOCK clk;
extern int beamadj;

/* Electron beam position */
int ebeamx, ebeamy, sbeamx;

/* The state of the electron beam */
#define VSYNCSTATE 1
#define VBLANKSTATE 2
#define HSYNCSTATE 4
#define DRAWSTATE 8
#define OVERSTATE 16
int vbeam_state;		/* 1 2 8 or 16 */
int hbeam_state;		/* 4 8 or 16 */

/* The tv size, varies with PAL/NTSC */
int tv_width, tv_height, tv_vsync, tv_vblank, tv_overscan, tv_frame, tv_hertz,
  tv_hsync;


PlayField pf[2];  

Paddle paddle[4];

Player pl[2];

Missile ml[3];



#define MAXLIST 80
/* The various display lists */
struct RasterChange pl_change[2][MAXLIST], pf_change[1][MAXLIST], unified[MAXLIST];

/* The display list counters */
int pl_change_count[2], pf_change_count[1], unified_count;


/***************************************************************************
                           Let the functions begin!
****************************************************************************/
void
init_machine (void)
{
  if (theCart == 0) theCart = (BYTE *)emu_Malloc(16384);
  if (cartScratch == 0) cartScratch = (BYTE *)emu_Malloc(4096);
  if (cartRam == 0) cartRam = (BYTE *)emu_Malloc(1024);
}

/* Device independent screen initialisations */
void
init_screen (void)
{
  
  /* Set the electron beam to the top left */
  ebeamx = -tv_hsync;
  ebeamy = 0;
  sbeamx = 0;
  vbeam_state = VSYNCSTATE;
  hbeam_state = OVERSTATE;

  tv_vsync = 3;
  tv_hsync = 68;
  switch (base_opts.tvtype) 
    {
    case NTSC:      
      tv_width = 160;
      tv_height = 192;
      tv_vblank = 40;
      tv_overscan = 30;
      tv_frame = 262;
      tv_hertz = 60;
    break;
    case PAL:
    case SECAM:
      tv_width = 160;
      tv_height = 228;
      tv_vblank = 48;
      tv_overscan = 36;
      tv_frame = 312;
      tv_hertz = 50;
      break;
    }

}

/* Displays the tv screen */
void tv_display (void)
{
  /* Only display if the frame is a valid one. */
  //if ( (tv_counter % nOptions_SkipFrames) == 0)
  //{
	  emu_DrawScreenPal16(VBuf, tv_width, tv_height, tv_width);
    emu_DrawVsync();
  //}
  //tv_counter++;
}

/* Initialise the RIOT (also known as PIA) */
void
init_riot (void)
{
  int i;

  /* Reset the arrays */
  for(i=0; i< 0x298;i++)
    {
      riotRead[i]=0;
      riotWrite[i]=0;
    }

  /* Wipe the RAM */
  for (i = 0; i < 0x80; i++)
    theRam[i] = 0;

  /* Set the timer to zero */
  riotRead[INTIM] = 0;

  /* Set the joysticks and switches to input */
  riotWrite[SWACNT] = 0;
  riotWrite[SWBCNT] = 0;

  /* Centre the joysticks */
  riotRead[SWCHA] = 0xff;
  riotRead[SWCHB] = 0x0b;

  /* Set the counter resolution */
  timer_res = 32;
  timer_count = 0;
  timer_clks = 0;
}

/* Initialise the television interface adaptor (TIA) */
void
init_tia (void)
{
  int i;
  for(i=0; i< 0x2d;i++)
    {
      tiaWrite[i]=0;
    }
  for(i=0; i< 0x0e;i++)
    {
      tiaRead[i]=0;
    }

  tiaWrite[CTRLPF] = 0x00;
  for (i = 0; i < 2; i++)
    {
      pl[i].hmm = 0x0;
      pl[i].x = 0x0;
      pl[i].nusize = 0;
      pl[i].grp = 0;
      pl[i].vdel = 0;
      pl[i].vdel_flag = 0;
      pl_change_count[i] = 0;
    }

  pl[0].mask = PL0_MASK;
  pl[1].mask = PL1_MASK;
  ml[0].mask = ML0_MASK;
  ml[1].mask = ML1_MASK;
  reset_collisions ();

  pf_change_count[0] = 0;
  unified_count = 0;
  for (i = 0; i < 3; i++)
    {
      ml[i].x = 0;
      ml[i].hmm = 0;
      ml[i].enabled = 0;
      ml[i].locked = 0;
      ml[i].width = 0;
      ml[i].vdel = 0;
      ml[i].vdel_flag = 0;
    }
  
  tiaWrite[VBLANK] = 0;
  tiaRead[INPT4] = 0x80;
  tiaRead[INPT5] = 0x80;

  /* Set up the colour table */
  colour_table[P0M0_COLOUR]= 0;
  colour_table[P1M1_COLOUR]= 0;
  colour_table[PFBL_COLOUR] = 0;
  colour_table[BK_COLOUR] = 0;
}

void
init_memory(void)
{
  int i;
  for(i=0;i<1024; i++)
    cartRam[i]=0;
  for(i=0; i<128;i++)
    theRam[i]=0;
}

void
init_banking (void)
{
  /* Set to the first bank */
  //dbg_message(DBG_NORMAL, "rom_size is set at %d bytes\n", rom_size);
  if (rom_size == 2048)
    theRom = &theCart[rom_size - 2048];
  else
    theRom = &theCart[rom_size - 4096];
  //JMH
  //switch(base_opts.bank)
  //  {
  //  case 3:
  //    /* Parker Brothers 8k E0 */
  //    memcpy(&cartScratch[0xc00],&theCart[0x1c00],1024);
  //    memcpy(&cartScratch[0],&theCart[0],3072);
  //    theRom=cartScratch;
  //    break;
  //  default:
  //    break;
  //  }
}

extern void init_cpu( ADDRESS addr);

/* Main hardware startup */
void
init_hardware (void)
{
//  dbg_message(DBG_NORMAL,"Setting Up hardware\n");
  init_screen ();
  init_riot ();
  init_tia ();
  init_raster ();
  init_memory();
  init_banking();
  init_cpu (0xfffc);
}

/* Do a raster change */
__inline void
do_raster_change (int i, int type, int val, struct RasterChange *rc)
{
  rc->x = ebeamx + beamadj;
  rc->type = type;
  rc->val = val;
}

/* Do a raster change on the unified list */
/* type: type of change */
/* val: value of change */
__inline void
do_unified_change (int type, int val)
{
  if (unified_count < MAXLIST)
    {
      unified[unified_count].x = ebeamx + beamadj;
      unified[unified_count].type = type;
      unified[unified_count].val = val;
      unified_count++;
    }
}

/* Do a player raster change */
/* i: player to change. 0 or 1 */
/* type: type of change */
/* val: value of change */
__inline void
do_plraster_change (int i, int type, int val)
{
  int plc = pl_change_count[i];
  /*printf("Raster change i=%d, x=%d, type=%d, val=%d\n", i, x, type, val); */
  if (plc < MAXLIST)
    {
      do_raster_change (i, type, val, &pl_change[i][plc]);
      if (type == 1)
	pl_change[i][plc].x -= 3;
      pl_change_count[i]++;
    }
}

/* Do a playfield raster change */
/* i: playfield to change. Depreciated, as 0 is now only one used */
/* type: type of change */
/* val: value of change */
__inline void
do_pfraster_change (int i, int type, int val)
{
  int pfc = pf_change_count[i];
  /*  
     if(ebeamy>=100) {
     printf("Raster change i=%d, x=%d, type=%d, val=%d\n", 
     i, ebeamx+beamadj, type, val);
     //show();
     }
   */
  if (pfc < MAXLIST)
    {
      do_raster_change (i, type, val, &pf_change[i][pfc]);
      pf_change_count[i]++;
    }
}


/* Use a unified change */
/* rc: unified change structure to use */
__inline void
use_unified_change (struct RasterChange *rc)
{
  switch (rc->type)
    {
    case 0:
      /* P0MO colour */
      colour_table[P0M0_COLOUR] = rc->val;
      break;
    case 1:
      /* POM0 colour */
      colour_table[P1M1_COLOUR] = rc->val;
      break;
    case 2:
      /* PFBL colour */
      colour_table[PFBL_COLOUR] = rc->val;
      break;
    case 3:
      /* BK colour */
      colour_table[BK_COLOUR] = rc->val;
      break;
    case 4:
      /* Priority change Normal */
      if(rc->val)
	norm_val=1;
      else
	norm_val=0;
      colour_lookup=colour_ptrs[norm_val][scores_val];
      break;
    case 5:
      /* Priority change Scores */
      if(rc->val)
	{
	  if(rc->x < 80)
	    scores_val=1;
	  else
	    scores_val=2;
	}
      else
	scores_val=0;
      colour_lookup=colour_ptrs[norm_val][scores_val];
      break;
    }
}

/* Use a playfield change */
/* pl: playfield to change */
/* rc: change to make */
__inline void
use_pfraster_change (PlayField *pl, struct RasterChange *rc)
{
  switch (rc->type)
    {
    case 0:
      /* PF0 */
      pl->pf0 = rc->val;
      break;
    case 1:
      /* PF1 */
      pl->pf1 = rc->val;
      break;
    case 2:
      /* PF2 */
      pl->pf2 = rc->val;
      break;
    case 3:
      /* Reflection */
      pl->ref = rc->val;
      break;
    }
}

/* Use a player change */
/* pl: player to change */
/* rc: change to make */
__inline void
use_plraster_change ( Player *pl, struct RasterChange *rc)
{
  switch (rc->type)
    {
    case 0:
      /* GRP */
      pl->grp = rc->val;
      break;
      /* Vertical delay */
    case 1:
      pl->vdel = pl->grp;
      break;
    }
}


int
do_paddle (int padnum)
{
  int res = 0x00;
  int x=0;
  if ((tiaWrite[VBLANK] & 0x80) == 0)
    {
	if (!nOptions_Landscape){
		x=240-Touchpadx;
		x=x*66;
		if (paddle[padnum].val > x) 
		res=0x80;
		
		}else{
		x=320-Touchpady;
		x=x*50;
		if (paddle[padnum].val > x) 
		res=0x80;
		}
		
    if (x > clk)
	res = 0x00;
    }
  return res;
}

/* Calculate the keypad rows */
/* i.e. when reading from INPTx we don't know the row */
BYTE
do_keypad (int pad, int col)
{
  BYTE res= 0x80;

//  read_keypad(pad);
  
  /* Bottom row */
  if(pad==0) {
  if( (riotWrite[SWCHA] & 0x80) && keypad[pad][col]==3)
    res=0x00;
  /* Third row */
  if( (riotWrite[SWCHA] & 0x40) && keypad[pad][col]==2)
    res=0x00;
  if( (riotWrite[SWCHA] & 0x20) && keypad[pad][col]==1)
    res=0x00;
  if( (riotWrite[SWCHA] & 0x10) && keypad[pad][col]==0)
    res=0x00;
  } 
  else {
  /* Bottom row */
  if( (riotWrite[SWCHA] & 0x80) && keypad[pad][col]==3)
    res=0x00;
  /* Third row */
  if( (riotWrite[SWCHA] & 0x40) && keypad[pad][col]==2)
    res=0x00;
  if( (riotWrite[SWCHA] & 0x20) && keypad[pad][col]==1)
    res=0x00;
  if( (riotWrite[SWCHA] & 0x10) && keypad[pad][col]==0)
    res=0x00;
  }
  return res;
}


/* 
   Called when the timer is set .
   Note that res is the bit shift, not absolute value.
   Assumes that any timer interval set will last longer than the instruction
   setting it.
 */
/* res: timer interval resolution as a bit shift value */
/* count: the number of intervals to set */
/* clkadj: the number of CPU cycles into the current instruction  */
void
set_timer (int res, int count, int clkadj)
{
  timer_count = count << res;
  timer_clks = clk + clkadj;
  timer_res = res;
}

/* New timer code, now only called on a read of INTIM */
/* clkadj: the number of CPU cycles into the current instruction  */
/* returns: the current timer value */
BYTE
do_timer (int clkadj)
{
  BYTE result;
  int delta;
  int value;

  delta = clk - timer_clks;
  value = delta >> timer_res;
  if (delta <= timer_count)
    {				/* Timer is still going down in res intervals */
      result = value;
    }
  else
    {
      if (value == 0)
	/* Timer is in holding period */
	result = 0;
      else
	{
	  /* Timer is descending from 0xff in clock intervals */
	  set_timer (0, 0xff, clkadj);
	  result = 0;
	}
    }

  /*  printf("Timer result=%d\n", result); */
  return result;
}

#ifdef snd
//extern unsigned char *sounddata;
//#define SoundBufSize 256
//unsigned char sounddata[SoundBufSize];
#endif

/* Do the screen related part of a write to VBLANK */
/* b: the byte written */
void
do_vblank (BYTE b)
{
	
  if (b & 0x02)
    {
      /* Start vertical blank */
      vbeam_state = VBLANKSTATE;
#ifdef snd
//	  Tia_process(sounddata, SoundBufSize);
//      CESound_play_sample(sounddata, nOptions_SoundBufSize);
#endif
      /* Also means we can update screen */
      tv_display ();
    }
  else
    {
      /* End vblank, and start first hsync drawing */
      int i;

      vbeam_state = DRAWSTATE;
      hbeam_state = HSYNCSTATE;
      /* Set up the screen */
      for (i = 0; i < unified_count; i++)
	use_unified_change (&unified[i]);
      /* Hope for a WSYNC, but just in case */
      ebeamx = -tv_hsync;
      ebeamy = 0;
    }
}


//int count = 1;

/* do a horizontal sync */
void
do_hsync (void)
{
  /* Only perform heavy stuff if electron beam is in correct position */
  if (vbeam_state == DRAWSTATE && (ebeamx > -tv_hsync))
    {
//      if (!(count--))
//      {
//        count = 2;
      tv_raster (ebeamy);
        
//      }
      /* Fix the clock value */
      clk += (ebeamx - tv_width) / 3;
      ebeamy++;
    }
  hbeam_state = HSYNCSTATE;
  ebeamx = -tv_hsync;
  sbeamx = 0;
}

/* Main screen logic */
/* clks: CPU clock length of last instruction */
__inline void
do_screen (int clks)
{
  switch (vbeam_state)
    {
    case VSYNCSTATE:
    case VBLANKSTATE:
      switch (hbeam_state)
	{
	case HSYNCSTATE:
	  ebeamx += clks * 3;
	  if (ebeamx >= 0)
	    {
	      hbeam_state = DRAWSTATE;
	    }
	  break;
	case DRAWSTATE:
	  ebeamx += clks * 3;
	  if (ebeamx >= tv_width)
	    {
	      ebeamx -= (tv_hsync + tv_width);
	      /* Insert hsync stuff here */
	      sbeamx = ebeamx;
	      hbeam_state = HSYNCSTATE;
	    }
	  break;
	case OVERSTATE:
	  break;
	}
      break;
    case DRAWSTATE:
      switch (hbeam_state)
	{
	case HSYNCSTATE:
	  ebeamx += clks * 3;
	  if (ebeamx >= 0)
	    {
	      hbeam_state = DRAWSTATE;
	    }
	  break;
	case DRAWSTATE:
	  ebeamx += clks * 3;
	  if (ebeamx >= tv_width)
	    {
	      /* Insert hsync stuff here */
	      sbeamx = ebeamx;
	      ebeamx -= (tv_hsync + tv_width);
	      tv_raster (ebeamy);
	      ebeamy++;
	      hbeam_state = HSYNCSTATE;
	    }
	  if (ebeamy >= tv_height + tv_overscan)
	    {
	      vbeam_state = OVERSTATE;
	      ebeamy = 0;
	    }
	  break;
	case OVERSTATE:
	  break;
	}
      break;
    case OVERSTATE:
      break;
    }
}

