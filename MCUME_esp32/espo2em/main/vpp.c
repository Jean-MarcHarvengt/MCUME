
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
 *   Videopac+ G7400 emulation
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "vmachine.h"
#include "vdc.h"
#include "vpp_cset.h"
#include "vpp.h"

#include "emuapi.h"

typedef struct
{
  int w;
  int h;
  unsigned char * line;
 // unsigned char line[320][250];
} BITMAP;

static void clear(BITMAP * bmp)
{
 int i,j;
 	bmp->line = (unsigned char *)emu_Malloc(BMPW*(BMPH+10));


 for(i=0;i<bmp->h;i++)
 for(j=0;j<bmp->w;j++)
	 //bmp->line[i][j] = 0;
	 *bmp->line++ = 0;
} 


static void vpp_draw_char(int x, int y, Byte ch, Byte c0, Byte c1, Byte ext, Byte dw, Byte dh, Byte ul);
static void vpp_update_screen(void);


static Byte LumReg = 0xff, TraReg = 0xff;
static BITMAP svppbmp;
static BITMAP * vppbmp = &svppbmp;
//static Byte *colplus = NULL;
//static Byte colplus[BMPW*BMPH];
static int vppon = 1;
static int vpp_cx = 0;
static int vpp_cy = 0;
static Byte vpp_data = 0;
static int inc_curs=1;
static int slice=0;
static int vpp_y0=0;
static Byte vpp_r=0;
static Byte dchars[2][960];
static Byte vpp_mem[40][32][4];
static int frame_cnt=0;
static int blink_st=0;
static int slicemode=0;
static int need_update=0;




Byte read_PB(Byte p){
	p &= 0x3;
	switch (p) {
		case 0:
			return LumReg >> 4;
			break;
		case 1:
			return LumReg & 0xf;
			break;
		case 2:
			return TraReg >> 4;
			break;
		case 3:
			return TraReg & 0xf;
			break;
	}
	return 0;
}


void write_PB(Byte p, Byte val){
	p &= 0x3;
	val &= 0xf;

	switch (p) {
		case 0:
			LumReg = (val<<4) | (LumReg & 0xf);
			break;
		case 1:
			LumReg = (LumReg & 0xf0) | val;
			break;
		case 2:
			TraReg = (val<<4) | (TraReg & 0xf);
			break;
		case 3:
			TraReg = (TraReg & 0xf0) | val;
			break;
	}	
	need_update = 1;
}


Byte vpp_read(ADDRESS adr){
	Byte t;
	switch (adr){
		case 4:
			return vpp_mem[vpp_cx][vpp_cy][1];
		case 5:
			if (slicemode) {
				Byte ext, chr;
				chr = vpp_mem[vpp_cx][vpp_cy][0];
				ext = (vpp_mem[vpp_cx][vpp_cy][1] & 0x80) ? 1 : 0;
				if (chr < 0xA0)
					t = 0;
				else {
					t = dchars[ext][(chr-0xA0)*10+slice];
					t = ((t&0x80)>>7) | ((t&0x40)>>5) | ((t&0x20)>>3) | ((t&0x10)>>1) | ((t&0x08)<<1) | ((t&0x04)<<3) | ((t&0x02)<<5) | ((t&0x01)<<7);
				}
				slice = (slice+1) % 10;
			} else {
				t = vpp_mem[vpp_cx][vpp_cy][0]; 
				if (inc_curs) {
					vpp_cx++;
					if (vpp_cx >= 40) {
						vpp_cx = 0;
						vpp_cy++;
						if (vpp_cy >= 24) vpp_cy = 0;
					}
				}
			}
			return t;
		case 6:
			return 0;
		default:
			return 0;
	}
}


void vpp_write(Byte dat, ADDRESS adr){
	switch (adr) {
		case 0:
			if (!slicemode) vpp_mem[vpp_cx][vpp_cy][1] = dat;
			break;
		case 1:
			if (slicemode) {
				Byte ext, chr;
				chr = vpp_mem[vpp_cx][vpp_cy][0];
				ext = (vpp_mem[vpp_cx][vpp_cy][1] & 0x80) ? 1 : 0;
				if (chr >= 0xA0) dchars[ext][(chr-0xA0)*10+slice] = ((dat&0x80)>>7) | ((dat&0x40)>>5) | ((dat&0x20)>>3) | ((dat&0x10)>>1) | ((dat&0x08)<<1) | ((dat&0x04)<<3) | ((dat&0x02)<<5) | ((dat&0x01)<<7);
				slice = (slice+1) % 10;
			} else {
				vpp_mem[vpp_cx][vpp_cy][0] = dat;
				if ((dat>0x7f) && (dat<0xa0) && (!(vpp_mem[vpp_cx][vpp_cy][1] & 0x80))) {
					vpp_mem[vpp_cx][vpp_cy][2] = dat;
					vpp_mem[vpp_cx][vpp_cy][3] = vpp_mem[vpp_cx][vpp_cy][1];
				} else {
					vpp_mem[vpp_cx][vpp_cy][2] = vpp_mem[vpp_cx][vpp_cy][3] = 0;
				}
				if (inc_curs) {
					vpp_cx++;
					if (vpp_cx >= 40) {
						vpp_cx = 0;
						vpp_cy++;
						if (vpp_cy >= 24) vpp_cy = 0;
					}
				}
			}
			break;
		case 2:
			vpp_data = dat;
			break;
		case 3:
			switch (dat & 0xe0) {
				case 0x00:	/* plus_cmd_brow */
					vpp_cy = vpp_data & 0x1f;
					vpp_cx = 0;
					break;
				case 0x20:	/* plus_cmd_loady */
					vpp_cy = vpp_data & 0x1f;
					break;
				case 0x40:	/* plus_cmd_loadx */
					vpp_cx = vpp_data % 40;
					break;
				case 0x60:	/* plus_cmd_incc */
					vpp_cx++;
					if (vpp_cx >= 40) {
						vpp_cx = 0;
						vpp_cy++;
						if (vpp_cy >= 24) vpp_cy = 0;
					}
					break;
				case 0x80:	/* plus_cmd_loadm */
					slicemode = 0;
					slice = (vpp_data & 0x1f) % 10;
					switch (vpp_data & 0xe0) {
						case 0x00:	/* plus_loadm_wr */
							inc_curs = 1;
							break;
						case 0x20:	/* plus_loadm_rd */
							inc_curs = 1;
							break;
						case 0x40:	/* plus_loadm_wrni */
							inc_curs = 0;
							break;
						case 0x60:	/* plus_loadm_rdni */
							inc_curs = 0;
							break;
						case 0x80:	/* plus_loadm_wrsl */
							slicemode = 1;
							break;
						case 0xA0:	/* plus_loadm_rdsl */
							slicemode = 1;
							break;
						default:
							break;
					}
					break;
				case 0xA0:	/* plus_cmd_loadr */
					vpp_r = vpp_data;
					break;
				case 0xC0:	/* plus_cmd_loady0 */
					vpp_y0 = (vpp_data & 0x1f) % 24;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	need_update = 1;
}


void vpp_finish_bmp(Byte *vmem, int offx, int offy, int w, int h, int totw, int toth){
	int i, x, y, t, c, nc, clrx, clry;
	int tcol[16], m[8] = {0x01, 0x10, 0x04, 0x40, 0x02, 0x20, 0x08, 0x80};
	Byte *pnt, *pnt2, *pnt3;

	if (vppon) {
		//memset(colplus,0,BMPW*BMPH);
		vppon=0;
	}

	if (TraReg == 0xff) return;

	vppon=1;
	
	frame_cnt--;
	if (frame_cnt<=0) {
		frame_cnt = 100;
		blink_st = 1-blink_st;
		need_update = 1;
	}

	if (need_update) vpp_update_screen();

	for (i=0; i<8; i++) tcol[i] = tcol[i+8] = !(TraReg & m[i]);
		
	if (w > totw-offx) w = totw-offx;
	if (h > toth-offy) h = toth-offy;

	if (w > vppbmp->w) w = vppbmp->w;
	if (h > vppbmp->h) h = vppbmp->h;
	
	clrx = clry = 0;
	for (i=0; (!clrx) && (i<totw); i++) if (tcol[vmem[offy*totw+i]&7]) clrx=1;
	for (i=0; (!clry) && (i<toth); i++) if (tcol[vmem[i*totw+offx]&7]) clry=1;
	if (clrx) for (y=0; y<offy; y++) for (x=0; x<totw; x++) vmem[y*totw+x]=0;
	if (clry) for (y=0; y<toth; y++) for (x=0; x<offx; x++) vmem[y*totw+x]=0;

	for (y=0; y<h; y++){
		pnt = vmem+(offy+y)*totw + offx;
//		pnt2 = (Byte *)vppbmp->line[y];
		pnt2 = (Byte *)&vppbmp->line[y*320];

		x=0;
		while (x < w) {
			pnt3 = pnt;
			c = *pnt++;
			t = x++;

			if ((((x+offx) & 3) == 0) && (sizeof(unsigned long)==4)) {
				unsigned long cccc, dddd, *p = (unsigned long*) pnt;
				int t2=x, w2=w-4;
				cccc = (((unsigned long)c) & 0xff) | ((((unsigned long)c) & 0xff) << 8) | ((((unsigned long)c) & 0xff) << 16) | ((((unsigned long)c) & 0xff) << 24);
				dddd = *p++;
				while ((x<w2) && (dddd == cccc)) {
					x += 4;
					dddd = *p++;
				}
				pnt += x-t2;
			}

			if (c<16) {
				if (tcol[c]){
					if (app_data.openb)
						for (i=0; i<x-t; i++) *pnt3++ = *pnt2++ & 0xf;
					else {
						memcpy(pnt3, pnt2, x-t);
						pnt2 += x-t;
					}
				} else {
					for (i=0; i<x-t; i++) {
						nc = *pnt2++;
						if ((nc & 0x10) && app_data.openb) {
							*pnt3++ = nc & 0xf;
						} else if (nc & 8) {
							//colplus[pnt3++ - vmem] = 0x40;
						} else {
							pnt3++;
						}
					}
				}
			}
			
		}

	}
	
}


static void vpp_draw_char(int x, int y, Byte ch, Byte c0, Byte c1, Byte ext, Byte dw, Byte dh, Byte ul){
	int xx, yy, d, m, k;

	if ((x>39) || (y>24) || (ext>1)) return;

	d = (dh==2) ? 5 : 0;

	for (yy=0; yy<10; yy++) {
		if (ul && (d==9))
			k = 255;
		else if (ch >= 0xA0)
			k = dchars[ext][(ch-0xA0)*10 + d];
		else if (ch >= 0x80)
			k = 255;
		else
			k = vpp_cset[ext][ch * 10 + d];

		m = (dw==2) ? 0x08 : 0x80;

		for (xx=0; xx<8; xx++) {
			//vppbmp->line[y*10+yy][x*8+xx] = (k & m) ? c1 : c0;
			vppbmp->line[(y*10+yy)*320+ x*8+xx] = (k & m) ? c1 : c0;
			if ((xx%2) || (dw==0)) m >>= 1;
		}
		if ((yy%2) || (dh==0)) d++;
	}
}
			

static void vpp_update_screen(void){
	int i,x,y,l,chr,attr,ext,c0,c1,dw,dh,hpar,vpar,lvd,lhd,ser_chr,ser_atr,ul,conc,box;
	int tlum[8], m[8] = {0x01, 0x10, 0x04, 0x40, 0x02, 0x20, 0x08, 0x80};

	clear(vppbmp);

	for (i=0; i<8; i++) tlum[i] = (LumReg & m[i]) ? 0 : 8;

	vpar = lvd = 0;
	for (y=0; y<25; y++) {

		vpar = (lvd==0) ? 0 : 1-vpar;

		l = (y==0) ? 31 : (y-1+vpp_y0)%24;
		c0 = ul = conc = box = 0;

		hpar = lhd = 0;
		for (x=0; x<40; x++) {
			hpar = (lhd==0) ? 0 : 1-hpar;

			chr = vpp_mem[x][l][0];
			attr = vpp_mem[x][l][1];
			c1 = attr & 0x7;
			c1 = ((c1&2) | ((c1&1)<<2) | ((c1&4)>>2));
			ext = (attr & 0x80) ? 1 : 0;

			ser_chr = vpp_mem[x][l][2];
			ser_atr = vpp_mem[x][l][3];
			if (ser_chr) {
				c0 = (ser_atr>>4) & 0x7;
				c0 = ((c0&2) | ((c0&1)<<2) | ((c0&4)>>2));
				ul = ser_chr & 4;
				conc = ser_chr & 1;
				box = ser_chr & 2;
			}

			if (ext) {
				c0 = (attr>>4) & 0x7;
				c0 = ((c0&2) | ((c0&1)<<2) | ((c0&4)>>2));
				dw = dh = 0;
			} else {
				dw = (attr & 0x20) ? (hpar ? 2 : 1) : 0;
				dh = (attr & 0x10) ? (vpar ? 2 : 1) : 0;
				if (dw) lhd=1;
				if (dh) lvd=1;
			}

			if ((vpp_r & 0x80) && (!(attr & 8)) && (!blink_st)) c1=c0;

			if (((y == 0) && (vpp_r & 8)) || ((y != 0) && (vpp_r & 1))) {
				if ((!conc) || (!(vpp_r & 4))) {
					if (box || (!(vpp_r & 2))) {
						if ((!ext) && (attr & 0x40))
							vpp_draw_char(x, y, chr, c1|tlum[c1], c0|tlum[c0], ext, dw, dh, ul);
						else
							vpp_draw_char(x, y, chr, c0|tlum[c0], c1|tlum[c1], ext, dw, dh, ul);
					} else {
						vpp_draw_char(x, y, 255, (app_data.openb) ? 16 : 0, 0, 0, 0, 0, 0);
					}
				}
			}
		}
		
	}

	if (vpp_r & 0x20) {
		for (y = vppbmp->h-1; y >= 10; y--)
//			for (x = 0; x < vppbmp->w; x++) vppbmp->line[y][x] = vppbmp->line[(y-10)/2+10][x];
			for (x = 0; x < vppbmp->w; x++) vppbmp->line[y*320+x] = vppbmp->line[((y-10)/2+10)*320 + x];
	}

	need_update=0;
}


void load_colplus(Byte *col){
	if (vppon) {
		//memcpy(col,colplus,BMPW*BMPH);
	}
	else
		memset(col,0,BMPW*BMPH);
}

void init_vpp(void){
	int i,j,k;
	
    vppbmp->w = 320;
    vppbmp->h = 250;

//	if ((!vppbmp) || (!colplus)) {
//		fprintf(stderr,"Could not allocate memory for Videopac+ screen buffer.\n");
//		exit(EXIT_FAILURE);
//	}

	clear(vppbmp);
	//memset(colplus,0,BMPW*BMPH);

	LumReg = TraReg = 0xff;
	vpp_cx = 0;
	vpp_cy = 0;
	vpp_y0 = 0;
	vpp_r = 0;
	inc_curs = 1;
	vpp_data = 0;
	frame_cnt=0;
	blink_st=0;
	slice = 0;
	slicemode=0;
	need_update = 1;
	vppon = 1;

	for (i=0; i<2; i++)
		for (j=0; j<960; j++) dchars[i][j] = 0;

	for (i=0; i<40; i++)
		for (j=0; j<32; j++)
			for (k=0; k<4; k++) vpp_mem[i][j][k] = 0;
}

