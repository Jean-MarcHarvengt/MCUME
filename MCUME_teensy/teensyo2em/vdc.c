
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
 *   O2 Video Display Controller emulation
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "vmachine.h"
#include "config.h"
#include "cset.h"
#include "cpu.h"
#include "vpp.h"
#include "vdc.h"

#include "emuapi.h"


typedef struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} PALETTE_ENTRY;


#define COL_SP0   0x01
#define COL_SP1   0x02
#define COL_SP2   0x04
#define COL_SP3   0x08
#define COL_VGRID 0x10
#define COL_HGRID 0x20
#define COL_VPP   0x40
#define COL_CHAR  0x80

#define X_START     8
#define Y_START		24

static const long colortable[16]={
	0x000000,
	0x0e3dd4,
	0x00981b,
	0x00bbd9,
	0xc70008,
	0xcc16b3,
	0x9d8710,
	0xe1dee1,
	0x5f6e6b,
	0x6aa1ff,
	0x3df07a,
	0x31ffff,
	0xff4255,
	0xff98ff,
	0xd9ad5d,
	0xffffff
};




/* The pointer to the graphics buffer And palette */

static PALETTE_ENTRY colors[256];
static Byte * vscreen=0; //[BMPW*BMPH];

/* Collision buffer */
static Byte * colbuf=0; //[BMPW*BMPH];

Byte coltab[256];

long clip_low;
long clip_high;


static void create_cmap(void);
static void draw_char(Byte ypos,Byte xpos,Byte chr,Byte col);
static void draw_grid(void);
INLINE extern void mputvid(unsigned int ad, unsigned int len, Byte d, Byte c);


unsigned char * getGBuf(void)
{
	return(vscreen);
}

void draw_region(void){
	int i;

  //emu_printi(last_line);

	if (regionoff == 0xffff)
		i = master_clk/(LINECNT-1)-5;
	else
		i = master_clk/22+regionoff;
	//i = snapline(i, VDCwrite[0xA0], 0);

	if (i<0) i=0;

#ifdef HALFHEIGHT
  i = i>>1;
#else
#endif
 
 	clip_low = last_line * (long)BMPW;
	clip_high = i * (long)BMPW;
	if (clip_high > BMPW*BMPH) clip_high = BMPW*BMPH;
	if (clip_low < 0) clip_low=0; 
	if (clip_low < clip_high) draw_display();
	last_line=i;
}


#define fastmputvid1(ad,d,c) if ((ad > (unsigned long)clip_low) && (ad < (unsigned long)clip_high)) { vscreen[ad]=d; colbuf[ad] |= c; coltab[c] |= colbuf[ad];}
#define fastmputvid2(ad,d,c) if ((ad > (unsigned long)clip_low) && (ad < (unsigned long)clip_high)) { vscreen[ad]=d; colbuf[ad] |= c; coltab[c] |= colbuf[ad];vscreen[ad+1]=d; colbuf[ad+1] |= c; coltab[c] |= colbuf[ad+1];}
//#define fastmputvid1(ad,d,c) mputvid(ad,1,d,c);
//#define fastmputvid2(ad,d,c) mputvid(ad,2,d,c);
 

INLINE void mputvid(unsigned int ad, unsigned int len, Byte d, Byte c){
	if ((ad > (unsigned long)clip_low) && (ad < (unsigned long)clip_high)) {
		unsigned int i;
// JMH
        Byte d1; 
		d1 = d;
		if ( ((len & 3)==0) && (sizeof(unsigned long) == 4) && ((ad & 3) == 0) ) 
		{
			unsigned long dddd = (((unsigned long)d1) & 0xff) | ((((unsigned long)d1) & 0xff) << 8) | ((((unsigned long)d1) & 0xff) << 16) | ((((unsigned long)d1) & 0xff) << 24);
			unsigned long cccc = (((unsigned long)c) & 0xff) | ((((unsigned long)c) & 0xff) << 8) | ((((unsigned long)c) & 0xff) << 16) | ((((unsigned long)c) & 0xff) << 24);
			for (i=0; i<len>>2; i++) {
				*((unsigned long*)(vscreen+ad)) = dddd;
				cccc |= *((unsigned long*)(colbuf+ad));
				*((unsigned long*)(colbuf+ad)) = cccc;
				coltab[c] |= ((cccc | (cccc >> 8) | (cccc >> 16) | (cccc >> 24)) & 0xff);
				ad += 4;
			}
		} else 
		{
			for (i=0; i<len; i++) {
				vscreen[ad]=d1;
				colbuf[ad] |= c;
				coltab[c] |= colbuf[ad++];
			}
		}
	}
}

static void create_cmap(void){
  int i;

  /* Initialise parts of the colors array */
  for (i = 0; i < 16; i++) {
      /* Use the color values from the color table */
      colors[i+32].r = colors[i].r = (colortable[i] & 0xff0000) >> 16; //18;
      colors[i+32].g = colors[i].g = (colortable[i] & 0x00ff00) >> 8; //10;
      colors[i+32].b = colors[i].b = (colortable[i] & 0x0000ff) >> 0; //2;
  }

  for (i = 16; i < 32; i++) {
      /* Half-bright colors for the 50% scanlines */
      colors[i+32].r = colors[i].r = colors[i-16].r/2;
      colors[i+32].g = colors[i].g = colors[i-16].g/2;
      colors[i+32].b = colors[i].b = colors[i-16].b/2;
  }
	
  for (i = 64; i < 256; i++) colors[i].r = colors[i].g = colors[i].b = 0;
  
  for (i=0;i<256;i++)
  {  
	emu_SetPaletteEntry(colors[i].r, colors[i].g, colors[i].b, i);
  }

}

static void draw_char(Byte ypos,Byte xpos,Byte chr,Byte col){
	int j,c;
	Byte cl,d1;
	int y,b,n;
	unsigned int pnt;

#ifdef HALFHEIGHT
  y = ypos>>1;
#else
  y=(ypos & 0xFE);
#endif
	pnt = y * BMPW + (xpos-8)+BORDERW;

	ypos = ypos >> 1;
	n = 8 - (ypos % 8) - (chr % 8);
	if (n < 3) n = n + 7;
	
#ifdef HALFHEIGHT
  if ((pnt+BMPW*n >= (unsigned long)clip_low) && (pnt <= (unsigned long)clip_high)) {
#else
  if ((pnt+BMPW*2*n >= (unsigned long)clip_low) && (pnt <= (unsigned long)clip_high)) {
#endif

		c=(int)chr + ypos;
		if (col & 0x01) c+=256;
		if (c > 511) c=c-512;

		cl = ((col & 0x0E) >> 1);
		cl = ((cl&2) | ((cl&1)<<2) | ((cl&4)>>2)) + 8;

#ifdef HALFHEIGHT
    if ((y>0) && (y<112) && (xpos<157)) {
#else
    if ((y>0) && (y<232) && (xpos<157)) {
#endif
			for (j=0; j<n; j++) {
				d1 = cset[c+j];
				for (b=0; b<8; b++) {
					if (d1 & 0x80) {
#ifdef HALFHEIGHT
            if ((xpos-8+b < 160) && (y+j < 120)) {
#else
            if ((xpos-8+b < 160) && (y+j < 240)) {
#endif
							fastmputvid1(pnt,cl,COL_CHAR);
#ifdef HALFHEIGHT
#else
              fastmputvid1(pnt+BMPW,cl,COL_CHAR);
#endif
						}
					}
					pnt+=1;
					d1 = d1 << 1;
				}
#ifdef HALFHEIGHT
        pnt += BMPW-8;
#else
        pnt += BMPW*2-8;
#endif
			}
		}
	}
}

static void draw_grid(void){
	unsigned int pnt, pn1;
	Byte mask,d;
	int j,i,x,w;
	Byte color;

	if (VDCwrite[0xA0] & 0x40) {
		for(j=0; j<9; j++) {
#ifdef HALFHEIGHT
      pnt = (((j*12)+12) * BMPW);
#else
      pnt = (((j*24)+24) * BMPW);
#endif
			for (i=0; i<9; i++) {
				pn1 = pnt + (i * 16) + BORDERW;
				color = ColorVector[j*24+24];
				fastmputvid2(pn1, (color & 0x07) | ((color & 0x40) >> 3) | (color & 0x80 ? 0 : 8), COL_HGRID);
				color = ColorVector[j*24+25];
				fastmputvid2(pn1+BMPW, (color & 0x07) | ((color & 0x40) >> 3) | (color & 0x80 ? 0 : 8), COL_HGRID);
#ifdef HALFHEIGHT
#else
        color = ColorVector[j*24+26];
        fastmputvid2(pn1+BMPW*2, (color & 0x07) | ((color & 0x40) >> 3) | (color & 0x80 ? 0 : 8), COL_HGRID);
#endif
			}
		}
	}

	// horizontal 
	mask=0x01;
	for(j=0; j<9; j++) {
#ifdef HALFHEIGHT
    pnt = (((j*12)+12) * BMPW);
#else
    pnt = (((j*24)+24) * BMPW);
#endif
		for (i=0; i<9; i++) {
			pn1 = pnt + (i * 16) + BORDERW;
#ifdef HALFHEIGHT
      if ((pn1+BMPW*2 >= (unsigned long)clip_low) && (pn1 <= (unsigned long)clip_high)) {
#else
      if ((pn1+BMPW*3 >= (unsigned long)clip_low) && (pn1 <= (unsigned long)clip_high)) {
#endif
				d=VDCwrite[0xC0 + i];
				if (j == 8) {
					d=VDCwrite[0xD0+i];
					mask=1;
				}
				if (d & mask)	{
					color = ColorVector[j*24+24];
					mputvid(pn1, 18, (color & 0x07) | ((color & 0x40) >> 3) | (color & 0x80 ? 0 : 8), COL_HGRID);
					color = ColorVector[j*24+25];
					mputvid(pn1+BMPW, 18, (color & 0x07) | ((color & 0x40) >> 3) | (color & 0x80 ? 0 : 8), COL_HGRID);
#ifdef HALFHEIGHT
#else
          color = ColorVector[j*24+26];
          mputvid(pn1+BMPW*2, 18, (color & 0x07) | ((color & 0x40) >> 3) | (color & 0x80 ? 0 : 8), COL_HGRID);
#endif
				}
			}
		}
		mask = mask << 1;
	}

	// vertical
	mask=0x01;
	w=2;
	if (VDCwrite[0xA0] & 0x80) w=16;
	for(j=0; j<10; j++) {
		pnt=(j*16);
		mask=0x01;
		d=VDCwrite[0xE0+j];
		for (x=0; x<8; x++) {
#ifdef HALFHEIGHT
      pn1 = pnt + (((x*12)+12) * BMPW) + BORDERW;
#else
      pn1 = pnt + (((x*24)+24) * BMPW) + BORDERW;
#endif
			if (d & mask) {
#ifdef HALFHEIGHT
        for(i=0; i<12; i++) {
#else
        for(i=0; i<24; i++) {
#endif
					if ((pn1 >= (unsigned long)clip_low) && (pn1 <= (unsigned long)clip_high)) {
						color = ColorVector[x*24+24+i];
						mputvid(pn1, w, (color & 0x07) | ((color & 0x40) >> 3) | (color & 0x80 ? 0 : 8), COL_VGRID);
					}
					pn1+=BMPW;
				}
			}
			mask = mask << 1;
		}
	}

}


void finish_display(void){
	vpp_finish_bmp(vscreen, 9, 5, BMPW-9, BMPH-5, BMPW, BMPH);
}


void clear_collision(void){
	load_colplus(colbuf);
	coltab[0x01]=coltab[0x02]=0;
	coltab[0x04]=coltab[0x08]=0;
	coltab[0x10]=coltab[0x20]=0;
	coltab[0x40]=coltab[0x80]=0;
}


void draw_display(void){
	int i,j,x,sm,t;
	Byte y,xt,yt,b,d1,cl,c;
	unsigned int pnt,pnt2;

#ifdef HALFHEIGHT
  for (i=clip_low/BMPW; i<clip_high/BMPW; i++) {
    memset(vscreen+i*BMPW, ((ColorVector[i<<1] & 0x38) >> 3) | (ColorVector[i<<1] & 0x80 ? 0 : 8), BMPW);
  }
#else
  for (i=clip_low/BMPW; i<clip_high/BMPW; i++) {
    memset(vscreen+i*BMPW, ((ColorVector[i] & 0x38) >> 3) | (ColorVector[i] & 0x80 ? 0 : 8), BMPW);
  }
#endif
	
	if (VDCwrite[0xA0] & 0x08) draw_grid();

	if (useforen && (!(VDCwrite[0xA0] & 0x20))) return;
	
	for(i=0x10; i<0x40; i+=4) draw_char(VDCwrite[i],VDCwrite[i+1],VDCwrite[i+2],VDCwrite[i+3]);

	pnt=0x40;
	for(i=0; i<4; i++) {
		x=y=248;
		for (j=0; j<4; j++){
			xt = VDCwrite[pnt+j*4+1];
			yt = VDCwrite[pnt+j*4];
      if ((xt<240) && (yt<240)){
				x=xt;
				y=yt;
				break;
			}
		}
		for(j=0; j<4; j++) {
			draw_char(y,x,VDCwrite[pnt+2],VDCwrite[pnt+3]);
			x+=16;
			pnt+=4;
		}
	}
	
	c=8;
	for (i=12; i>=0; i -=4) {
		pnt2 = 0x80 + (i * 2);
#ifdef HALFHEIGHT
    y = VDCwrite[i]>>1;
#else
    y = VDCwrite[i];
#endif
		x = VDCwrite[i+1]-8;
		t = VDCwrite[i+2];
		cl = ((t & 0x38) >> 3);
		cl = ((cl&2) | ((cl&1)<<2) | ((cl&4)>>2)) + 8;
//#ifdef HALFHEIGHT
//    if ((x<164) && (y>0) && (y<112)) {
//#else
    if ((x<164) && (y>0) && (y<232)) {
//#endif
			pnt = y * BMPW + x + BORDERW + sproff;
			if ((pnt+BMPW*16 >= (unsigned long)clip_low) && (pnt <= (unsigned long)clip_high)) {
				for (j=0; j<8; j++) {
					sm = (((j%2==0) && (((t>>1) & 1) != (t & 1))) || ((j%2==1) && (t & 1))) ? 1 : 0;
					d1 = VDCwrite[pnt2++];
					for (b=0; b<8; b++) {
						if (d1 & 0x01) {
#ifdef HALFHEIGHT
              if ((x+b+sm<160) && (y+j<129)) {
#else
              if ((x+b+sm<160) && (y+j<249)) {
#endif
								fastmputvid1(sm+pnt,cl,c);
#ifdef HALFHEIGHT
#else
                fastmputvid1(sm+pnt+BMPW,cl,c);
#endif
							}
						}
						pnt += 1;
						d1 = d1 >> 1;
					}
#ifdef HALFHEIGHT
          pnt += BMPW-8;
#else
          pnt += BMPW*2-8;
#endif
				}
			}
		}
		c = c >> 1;
	}
}




void close_display(void) 
{
}

void init_display(void) 
{
	if (vscreen==0) vscreen = (Byte *)emu_Malloc(BMPW*BMPH);
	if (colbuf == 0) colbuf = (Byte *)emu_Malloc(BMPW*BMPH);

	create_cmap();
	memset(colbuf,0,BMPW*BMPH);
}

