
/*
 *   O2EM Freeware Odyssey2 / Videopac+ Emulator
 *
 *   Created by Daniel Boris <dboris@comcast.net>  (c) 1997,1998
 *
 *   Developed by Andre de la Rocha <adlroc@users.sourceforge.net>
 *
 *   http://o2em.sourceforge.net
 *
 *
 *
 *   Main O2 machine emulation
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "audio.h"
#include "types.h"
#include "cpu.h"
#include "config.h"
#include "vdc.h" 
//#include "vpp.h"
#include "vmachine.h"

#include "emuapi.h"

static Byte x_latch,y_latch;
static Byte line_count;
static int fps=FPS_NTSC;

//static Byte snapedlines[MAXLINES+2*MAXSNAP][256][2]; //500+2*50 *256*2 600*512

int evblclk=EVBLCLK_NTSC;

struct resource app_data;
int frame=0;
Byte dbstick1,dbstick2;

int int_clk; 	/* counter for length of /INT pulses */
int master_clk;	/* Master clock */
int h_clk;   /* horizontal clock */
unsigned long clk_counter;
int last_line;
int key2vcnt=0;
int mstate;
int ccolflag=0;

int pendirq=0;
int enahirq=1;
int useforen=0;
long regionoff=0xffff;
int mxsnap=2;
int sproff=0;
int tweakedaudio=0;

Byte rom1[5120];
Byte rom2[5120];
Byte rom3[5120];
Byte rom4[5120];

Byte intRAM[64];
Byte extRAM[256];
Byte extROM[1024];
Byte VDCwrite[256];
Byte ColorVector[MAXLINES];
Byte AudioVector[MAXLINES];
Byte *rom, *orom;

int key2[128];

#define KEY_0 0
#define KEY_1 1
#define KEY_2 2
#define KEY_3 3
#define KEY_4 4
#define KEY_5 5
#define KEY_6 6
#define KEY_7 7
#define KEY_8 8
#define KEY_9 9
#define KEY_A 10
#define KEY_B 11
#define KEY_C 12
#define KEY_D 13
#define KEY_E 14
#define KEY_F 15
#define KEY_G 16
#define KEY_H 17
#define KEY_I 18
#define KEY_J 19
#define KEY_K 20
#define KEY_L 21
#define KEY_M 22
#define KEY_N 23
#define KEY_O 24
#define KEY_P 25
#define KEY_Q 26
#define KEY_R 27
#define KEY_S 28
#define KEY_T 29
#define KEY_U 30
#define KEY_V 31
#define KEY_W 32
#define KEY_X 33
#define KEY_Y 34
#define KEY_Z 35
#define KEY_PLUS 35
#define KEY_MINUS 36
#define KEY_SLASH 38
#define KEY_ASTERISK 39

#define KEY_SPACE 40
#define KEY_EQUALS 41
#define KEY_DEL 42
#define KEY_ENTER 43
#define KEY_STOP 44
#define KEY_SLASH_PAD 45
#define KEY_PLUS_PAD 46

static unsigned int key_map[6][8]= {
	{KEY_0,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7},
	{KEY_8,KEY_9,0,0,KEY_SPACE,KEY_SLASH,KEY_L,KEY_P},
	{KEY_PLUS_PAD,KEY_W,KEY_E,KEY_R,KEY_T,KEY_U,KEY_I,KEY_O},
	{KEY_Q,KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K},
	{KEY_A,KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_M,KEY_STOP},
	{KEY_MINUS,KEY_ASTERISK,KEY_SLASH_PAD,KEY_EQUALS,KEY_Y,KEY_N,KEY_DEL,KEY_ENTER}
};


static void do_kluges(void);
static void setvideomode(int t);

void run(void){
  cpu_exec();
}


void handle_vbl(void){
		update_audio();
//		update_voice();
//	}
	if (!ccolflag) {
		clear_collision();
		ccolflag=1;
	}
	draw_region();
	ext_IRQ();
	mstate = 1;
}


void handle_evbl(void){
	static long last=0;
	static int rest_cnt=0;
	int i;

	i = (15*app_data.speed/100);
	rest_cnt = (rest_cnt+1)%(i<5?5:i);
	last_line=0;
	master_clk -= evblclk;
	frame++;
	ccolflag=0;
	if (!app_data.debug) {
		finish_display();
	}

	for (i=0; i<MAXLINES; i++)  {
		ColorVector[i] = (VDCwrite[0xA3] & 0x7f) | (p1 & 0x80);
		AudioVector[i] = VDCwrite[0xAA];
	}

	if (key2vcnt++ > 10) {
		key2vcnt=0;
		for (i=0; i<128; i++) key2[i] = 0;
		dbstick1 = dbstick2 = 0;
	}
	mstate=0;	
}


void init_system(void){
	int i,j,k;

	last_line=0;
	dbstick1=0x00;
	dbstick2=0x00;
	mstate=0;
	master_clk=0;
	h_clk=0;
	line_count=0;
	itimer=0;
	clk_counter=0;
	for(i=0; i<256; i++) {
		VDCwrite[i]=0;
		extRAM[i]=0;
	}
	for(i=0; i<64; i++) {
		intRAM[i]=0;
	}
	for (i=0; i<MAXLINES; i++) AudioVector[i] = ColorVector[i] = 0;
	ccolflag=0;
/*
	for (i=0; i<MAXLINES+2*MAXSNAP; i++)
		for (j=0; j<256; j++)
			for (k=0; k<2; k++)
				snapedlines[i][j][k]=0;
*/				
	for (i=0; i<128; i++) key2[i] = 0;
	key2vcnt=0;
	if (app_data.euro)
		setvideomode(1);
	else
		setvideomode(0);
	do_kluges();
	//init_vpp();
}


Byte read_t1(void){
	if ((h_clk > 16) || (master_clk > VBLCLK))
		return 1;
	else
		return 0;
}


void write_p1(Byte d){
	if ((d & 0x80) != (p1 & 0x80)) {
		int i,l;
		//l = snapline((int)((float)master_clk/22.0+0.5), VDCwrite[0xA3], 1);
		for (i=l; i<MAXLINES; i++) ColorVector[i] = (VDCwrite[0xA3] & 0x7f) | (d & 0x80);
	}
	p1 = d;
	if (app_data.bank == 2) {
		if (p1 & 0x01)
			rom=rom2;
		else
			rom=rom1;
	}
	if (app_data.bank == 3) {
		switch(p1 & 0x03) {
			case 0:
				rom=rom1;
				break;
			case 1:
				rom=rom2;
				break;
			case 2:
				rom=rom3;
				break;
			case 3:
				rom=rom4;
				break;
		}
	}
}


Byte read_P2(void){
	int i,si,so,km;
    int k;

	k=emu_GetPad()&0x7f;
  int hk = emu_ReadI2CKeyboard()&0x7f;  
  
  int key = emu_ReadKeys();
  if (key & MASK_KEY_USER1) { // 1
    k = 1;
  }

  if (!(p1 & 0x04)) {
    si = (p2 & 7);
    so=0xff;
    if (si<6) {
      for (i=0; i<8; i++) {
        km = key_map[si][i];
                if ( (km == (k-1)) && (k) )
        {
          so = i ^ 0x07;
        }
        else if ( (km == (hk-1)) && (hk) )
        {
          so = i ^ 0x07;
        }       
//        if ((key[km] && ((!joykeystab[km]) || (key_shifts & KB_CAPSLOCK_FLAG))) || (key2[km])) {
//          so = i ^ 0x07;
//        }
      }
    }

		if (so != 0xff) {
			p2 = p2 & 0x0F;
			p2 = p2 | (so << 5);
		} else {
			p2 = p2 | 0xF0;
		}
	 } else {
		p2 = p2 | 0xF0;
	 }
	 return(p2);
	 
}


Byte ext_read(ADDRESS adr){
	Byte d;
	Byte si;
	Byte m;
	int i;

	if (!(p1 & 0x08) && !(p1 & 0x40)) {
		/* Handle VDC Read */
		switch(adr) {
			case 0xA1:
				d = VDCwrite[0xA0] & 0x02;
				if (master_clk > VBLCLK) d = d | 0x08;
				if (h_clk < (LINECNT-7)) d = d | 0x01;
				if (sound_IRQ) d = d | 0x04;
				sound_IRQ=0;
				return d;
			case 0xA2:
				si = VDCwrite[0xA2];
				m=0x01;
				d=0;
				for(i=0; i<8; i++) {
					if (si & m) {
						if (coltab[1] & m) d = d | (coltab[1] & (m ^ 0xFF));
						if (coltab[2] & m) d = d | (coltab[2] & (m ^ 0xFF));
						if (coltab[4] & m) d = d | (coltab[4] & (m ^ 0xFF));
						if (coltab[8] & m) d = d | (coltab[8] & (m ^ 0xFF));
						if (coltab[0x10] & m) d = d | (coltab[0x10] & (m ^ 0xFF));
						if (coltab[0x20] & m) d = d | (coltab[0x20] & (m ^ 0xFF));
						if (coltab[0x80] & m) d = d | (coltab[0x80] & (m ^ 0xFF));
					}
					m = m << 1;
				}
				return d;
			case 0xA5:
				if (!(VDCwrite[0xA0] & 0x02)) {
					return x_latch;
				}
				else {
					x_latch = h_clk * 12;
					return x_latch;
				}			
			case 0xA4:
				if (!(VDCwrite[0xA0] & 0x02)) {
					return y_latch;
				}
				else {
					y_latch = master_clk/22;
					if (y_latch > 241) y_latch=0xFF;
					return y_latch;
				}
			default:
				return VDCwrite[adr];
		}
	} else if (!(p1 & 0x10)) {
		/* Handle ext RAM Read */
		 return extRAM[adr & 0xFF];
	//} else if (!(p1 & 0x20)) {
	//	/* Read a Videopac+ register */
	//	return vpp_read(adr);
	} else if (app_data.exrom && (p1 & 0x02)) {
		/* Handle read from exrom */
		return extROM[(p2 << 8) | (adr & 0xFF)];
	}

	return 0;
}


Byte in_bus(void){
	Byte si=0,d=0,mode=0,jn=0;
    int key;


	if ((p1 & 0x08) && (p1 & 0x10)) {
		/* Handle joystick read */
		if (!(p1 & 0x04)) {
			si = (p2 & 7);
		}
		d=0xFF;
		/* Get current input */
		key = emu_ReadKeys();
   
    app_data.stick[0]=0;
    app_data.stick[1]=1;
    /*  
		if ( emu_GetPad() & 0x80 )
		{
			app_data.stick[0]=1;
			app_data.stick[1]=0;
		}
		else
		{
			app_data.stick[0]=0;
			app_data.stick[1]=1;
		}

   
		if (si == 1) {
			mode = app_data.stick[0];
			jn = 0;
		} else {
			mode = app_data.stick[1];
			jn = 1;
		}
    */
		mode=1;
		if (key & 0x8000) jn=1;
		else jn=0;

		switch(mode) {
			case 1:			    
                if (key & MASK_JOY2_RIGHT) d &= 0xF7;
                if (key & MASK_JOY2_LEFT) d &= 0xFD;
                if (key & MASK_JOY2_UP) d &= 0xFE;
                if (key & MASK_JOY2_DOWN) d &= 0xFB;
                if (key & MASK_JOY2_BTN) d &= 0xEF;
				break;
        
			case 2: 
                /* Get current input */
//                key = emu_GetPad();
//                if (key & JKEY_PLEFT)  d &= 0xF7;
//                if (key & JKEY_PRIGHT) d &= 0xFD;
//                if (key & JKEY_PUP)    d &= 0xFE;
//                if (key & JKEY_PDOWN)  d &= 0xFB;
//                if (key & JKEY_PSPACE) d &= 0xEF;
				break;
		}
		if (si == 1) {
			if (dbstick1) d = dbstick1;
		} else {
			if (dbstick2) d = dbstick2;
		}
   }
   return d;
}


void ext_write(Byte dat, ADDRESS adr){
	int i;

	if (!(p1 & 0x08)) {
		/* Handle VDC Write */		
		if (adr == 0xA0){
			if ((VDCwrite[0xA0] & 0x02) && !(dat & 0x02)) {
				y_latch = master_clk/22;
				x_latch = h_clk * 12;
				if (y_latch > 241) y_latch=0xFF;
			}
			if ((master_clk <= VBLCLK) && (VDCwrite[0xA0] != dat)) {
				if (!ccolflag) {
					clear_collision();
					ccolflag=1;
				}
				draw_region();
			}
		} else if (adr == 0xA2) {
			clear_collision();
			ccolflag=1;
		} else if (adr == 0xA3){
			int l;
			//l = snapline((int)((float)master_clk/22.0+0.5), dat, 1);
			for (i=l; i<MAXLINES; i++) ColorVector[i] = (dat & 0x7f) | (p1 & 0x80);
		} else if (adr == 0xAA) {
			for (i=master_clk/22; i<MAXLINES; i++) AudioVector[i]=dat;
		}
		VDCwrite[adr]=dat;
	} else if (!(p1 & 0x10) && !(p1 & 0x40)) {
		adr = adr & 0xFF;
		if (adr < 0x80) {
			/* Handle ext RAM Write */
			extRAM[adr] = dat;
		} else {
			/* Handle The Voice */
			if (!(dat & 0x20))
			{
//JMH				reset_voice();
			}
			else {
//JMH
//				if (adr == 0xE4)
//					set_voice_bank(0);
//				else if ((adr >= 0xE8) && (adr <= 0xEF))
//					set_voice_bank(adr-0xE7);
//				else if (((adr >= 0x80) && (adr <= 0xDF)) || ((adr >= 0xF0) && (adr <= 0xFF)))
//					trigger_voice(adr);
			}
		}
	//} else if (!(p1 & 0x20)) {
	//	/* Write to a Videopac+ register */
	//	vpp_write(dat,adr);
	}
}


static void do_kluges(void){
	if (app_data.crc == 0xA7344D1F) pendirq=1;				/* Atlantis */ 
	if (app_data.crc == 0xFB83171E) pendirq=1;				/* Blockout*/
	if (app_data.crc == 0x881CEAE4) pendirq=1;				/* Wall Street */

	if (app_data.crc == 0x9E42E766) useforen=1;				/* Turtles */
	if (app_data.crc == 0x1C750349) useforen=1;				/* Turtles (European version) */
	if (app_data.crc == 0x202F2749) useforen=1;				/* Q*bert */

	if (app_data.crc == 0xFB83171E) enahirq=0;				/* Blockout*/
	
	if (app_data.crc == 0xFB83171E) regionoff=1;			/* Blockout*/
	if (app_data.crc == 0x202F2749) regionoff=1;			/* Q*bert */
	if (app_data.crc == 0x5216771A) regionoff=1;			/* Popeye */
	if (app_data.crc == 0x0C2E4811) regionoff=12;			/* Out of this World! / Helicopter Rescue! */
	if (app_data.crc == 0x67069924) regionoff=11;			/* Smithereens! */
	if (app_data.crc == 0x44D1A8A5) regionoff=11;			/* Smithereens! (European version) */
	if (app_data.crc == 0xB936BD78) regionoff=12;			/* Type & Tell */
	if (app_data.crc == 0xDC30AD3D) regionoff=10;			/* Dynasty! */
	if (app_data.crc == 0x7810BAD5) regionoff=10;			/* Dynasty! (European) */
	if (app_data.crc == 0xA7344D1F) regionoff=0;			/* Atlantis */ 
	if (app_data.crc == 0xD0BC4EE6) regionoff=12;			/* Frogger */
	if (app_data.crc == 0x825976A9) regionoff=0;			/* Mousing Cat 8kb */
	if (app_data.crc == 0xF390BFEC) regionoff=0;			/* Mousing Cat 4kb */
	if (app_data.crc == 0x3BFEF56B) regionoff=1;			/* Four in 1 Row! */
	if (app_data.crc == 0x9BFC3E01) regionoff=10;			/* Demon Attack */
	if (app_data.crc == 0x6CEBAB74) regionoff=12;			/* P.T. Barnum's Acrobats! (European version) */
	if (app_data.crc == 0xE7B26A56) regionoff=12;			/* P.T. Barnum's Acrobats! (European version - Extra keys) */

	if (app_data.crc == 0xFB83171E) mxsnap=3;				/* Blockout*/
	if (app_data.crc == 0xA57E1724) mxsnap=12;				/* Catch the Ball / Noughts and Crosses */
	if (app_data.crc == 0xFD179F6D) mxsnap=3;				/* Clay Pigeon! */
	if (app_data.crc == 0x9BFC3E01) mxsnap=0;				/* Demon Attack */
	if (app_data.crc == 0x9C9DDDF9) mxsnap=3;				/* Verkehr */
	if (app_data.crc == 0x95936B07) mxsnap=3;				/* Super Cobra */
	if (app_data.crc == 0x881CEAE4) mxsnap=3;				/* Wall Street */
	if (app_data.crc == 0x9E42E766) mxsnap=0;				/* Turtles */
	if (app_data.crc == 0x1C750349) mxsnap=0;				/* Turtles (European version) */
	if (app_data.crc == 0xD0BC4EE6) mxsnap=3;				/* Frogger */
	if (app_data.crc == 0x3BFEF56B) mxsnap=6;				/* Four in 1 Row! */

	if (app_data.crc == 0xA7344D1F) setvideomode(1);		/* Atlantis */ 
	if (app_data.crc == 0x39E31BF0) setvideomode(1);		/* Jake */
	if (app_data.crc == 0x3351FEDA) setvideomode(1);		/* Power Lords */
	if (app_data.crc == 0x40AE062D) setvideomode(1);		/* Power Lords (alternate) */
	if (app_data.crc == 0xD158EEBA) setvideomode(1);		/* Labirinth */
	if (app_data.crc == 0x26B0FF5B) setvideomode(1);		/* Nightmare */	
	if (app_data.crc == 0xDF36683F) setvideomode(1);		/* Shark Hunter */	
	if (app_data.crc == 0xAF307559) setvideomode(1);		/* Super Bee 8Kb */	
	if (app_data.crc == 0x9585D511) setvideomode(1);		/* Super Bee 4Kb */	
	if (app_data.crc == 0x58FA6766) setvideomode(1);		/* War of the Nerves */	
	if (app_data.crc == 0x58FA6766) setvideomode(1);		/* War of the Nerves */	
	if (app_data.crc == 0x39989464) setvideomode(1);		/* Hockey! / Soccer! */	
	if (app_data.crc == 0xB47F3E0B) setvideomode(1);		/* Kill the Attacking Aliens Demo */	
	if (app_data.crc == 0x3BFEF56B) setvideomode(1);		/* Four in 1 Row! */
	if (app_data.crc == 0x68560DC7) setvideomode(1);		/* Jopac Moto Crash */
	if (app_data.crc == 0x202F2749) setvideomode(0);		/* Q*bert */
	if (app_data.crc == 0xFB83171E) setvideomode(0);		/* Blockout*/
	if (app_data.crc == 0x9BFC3E01) setvideomode(0);		/* Demon Attack */	
	if (app_data.crc == 0x239DF97D) setvideomode(0);		/* Pachinko! */	
	if ((app_data.crc == 0xF390BFEC) || (app_data.crc == 0x825976A9)){	/* Mousing Cat */
		setvideomode(1);
		evblclk=7642;
	}
	if (app_data.crc == 0xD0BC4EE6) {						/* Frogger */
		setvideomode(1);
		evblclk=7642;
	}
	if ((app_data.crc == 0x2DCB77F0) || (app_data.crc == 0xF6882734)) {	/* Depth Charge / Marksman */
		setvideomode(1);
		evblclk=9000;
	}
	if (app_data.crc == 0x881CEAE4) {						/* Wall Street */
		setvideomode(1);
		evblclk=6100;
	}
	
	if (app_data.crc == 0xD0BC4EE6) tweakedaudio=1;			/* Frogger */
	if (app_data.crc == 0x5216771A) tweakedaudio=1;			/* Popeye */
	if (app_data.crc == 0xAFB23F89) tweakedaudio=1;			/* Musician */

	if (app_data.crc == 0xD3B09FEC) sproff=1;				/* Volleyball! */		
}

/*
int snapline(int pos, Byte reg, int t){
	int i;
	for (i=0; i<mxsnap; i++){
		if (snapedlines[pos+MAXSNAP-i][reg][t]) return pos-i;
		if (snapedlines[pos+MAXSNAP+i][reg][t]) return pos+i;
	}
	snapedlines[pos+MAXSNAP][reg][t]=1;
	return pos;
}
*/

static void setvideomode(int t){
	if (t) {
		evblclk = EVBLCLK_PAL;
		fps = FPS_PAL;
	} else {
		evblclk = EVBLCLK_NTSC;
		fps = FPS_NTSC;
	}
}
	

