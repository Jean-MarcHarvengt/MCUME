

#ifndef __LCD_H__
#define __LCD_H__

#include "defs.h"

struct vissprite
{
	byte *buf;
	int x;
	byte pal, pri, pad[6];
};

struct scan
{
	int bg[64];
	int wnd[64];
	byte buf[256];
	un16 pal2[64];
	byte pri[256];
	struct vissprite vs[16];
	int ns, l, x, y, s, t, u, v, wx, wy, wt, wv;
};

struct obj
{
	byte y;
	byte x;
	byte pat;
	byte flags;
};

struct lcd
{
	byte vbank[2][8192];
	union
	{
		byte mem[256];
		struct obj obj[40];
	} oam;
	byte pal[128];
};

extern struct lcd lcd;
extern struct scan scan;


extern void lcd_reset();
extern void lcd_begin();



#endif



