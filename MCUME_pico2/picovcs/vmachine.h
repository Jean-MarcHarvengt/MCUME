/*****************************************************************************

   This file is part of x2600, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.
   
   See the file COPYING for details.
   
   $Id: vmachine.h,v 2.13 1996/11/24 16:55:40 ahornby Exp $
******************************************************************************/


/*
   External definitions of the 2600 virtual machine.
*/


#ifndef VMACHINE_H
#define VMACHINE_H
//#define BYTE char

extern int rom_size;
//JMH
extern BYTE *theCart;
//extern BYTE theCart[4096];
extern BYTE *theRom;
extern BYTE *cartScratch;
extern BYTE *cartRam;
extern BYTE theRam[];
extern BYTE riotRead[];
extern BYTE riotWrite[];
extern BYTE tiaWrite[];
extern BYTE tiaRead[];
extern BYTE keypad[2][4];

extern int reset_flag;

extern int ebeamx, ebeamy, sbeamx;

#define VSYNCSTATE 1
#define VBLANKSTATE 2
#define HSYNCSTATE 4
#define DRAWSTATE 8
#define OVERSTATE 16 

extern int vbeam_state, hbeam_state;

extern int tv_width, tv_height, tv_vsync, tv_vblank,
    tv_overscan, tv_frame, tv_hertz, tv_hsync;

extern int timer_res, timer_count, timer_clks; 

typedef struct  {
    BYTE pf0,pf1,pf2;
    BYTE ref;
} PlayField;  

extern PlayField pf[2];  

typedef struct  {
    int pos;
    int val;
} Paddle;

extern Paddle paddle[4];
typedef struct {
  int x;
  BYTE grp;
  BYTE hmm;
  BYTE vdel;
  BYTE vdel_flag;
  BYTE nusize;
  BYTE reflect;
  BYTE mask;
} Player;

extern Player pl[2];

struct RasterChange {
  int x;     /* Position at which change happened */
  int type;  /* Type of change */
  int val;   /* Value of change */
} ;

extern struct RasterChange pl_change[2][80], pf_change[1][80], unified[80];

extern int pl_change_count[2], pf_change_count[1], unified_count;

/* The missile and ball positions */
typedef struct {
    int x;
    BYTE hmm;
    BYTE locked;
    BYTE enabled;
    BYTE width;
    BYTE vdel;
    BYTE vdel_flag;
    BYTE mask;
} Missile;

extern Missile ml[3];

/****************************************************************************
                              functions.
*****************************************************************************/

extern __inline void 
do_plraster_change(int i, int type, int val);

extern __inline void
do_unified_change(int type, int val);

extern __inline void
use_unified_change( struct RasterChange *rc);

extern __inline void 
use_plraster_change( Player *pl, struct RasterChange *rc);

extern __inline void
do_pfraster_change(int i, int type, int val);

extern __inline void
use_pfraster_change( PlayField *pf, struct RasterChange *rc);

void init_machine(void);
void init_hardware(void);
void init_banking(void);

extern __inline void set_timer(int res, int count, int clkadj);
extern __inline BYTE do_timer(int clkadj);
extern __inline void do_vblank(BYTE b);
extern __inline void do_hsync(void);

int  do_paddle(int padnum);
BYTE do_keypad(int pad, int col);
extern __inline void do_screen(int clks);

#endif






