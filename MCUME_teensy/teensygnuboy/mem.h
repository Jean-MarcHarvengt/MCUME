
#ifndef __MEM_H__
#define __MEM_H__


#include "defs.h"



#define MBC_NONE 0
#define MBC_MBC1 1
#define MBC_MBC2 2
#define MBC_MBC3 3
#define MBC_MBC5 5
#define MBC_RUMBLE 15
#define MBC_HUC1 0xC1
#define MBC_HUC3 0xC3

struct mbc
{
	int type;
	int model;
	int rombank;
	int rambank;
	int romsize;
	int ramsize;
	int enableram;
	int batt;
	byte *rmap[0x10], *wmap[0x10];
};


struct ram
{
	byte hi[256];
	byte ibank[8][4096];
	byte (*sbank)[8192];
	int loaded;
};


extern struct mbc mbc;
extern struct rom rom;
extern struct ram ram;





void mem_updatemap(void); //gp32
void ioreg_write(byte r, byte b);
void mbc_write(int a, byte b);
void mem_write(int a, byte b);
byte mem_read(int a);


static byte readb(int a)
{
	return mem_read(a);
}

static void writeb(int a, byte b)
{
	byte *p = mbc.wmap[a>>12];
	if (p) p[a] = b;
	else mem_write(a, b);
}

static int readw(int a)
{
	return mem_read(a) | (mem_read(a+1)<<8);
}

static void writew(int a, int w)
{
	if ((a+1) & 0xfff)
	{
		byte *p = mbc.wmap[a>>12];
		if (p)
		{
#ifdef IS_LITTLE_ENDIAN
#ifndef ALLOW_UNALIGNED_IO
			if (a&1)
			{
				p[a] = w;
				p[a+1] = w >> 8;
				return;
			}
#endif
			*(uint16 *)(p+a) = w;
			return;
#else
			p[a] = w;
			p[a+1] = w >> 8;
			return;
#endif
		}
	}
	mem_write(a, w);
	mem_write(a+1, w>>8);
}

static byte readhi(int a)
{
	return readb(a | 0xff00);
}

static void writehi(int a, byte b)
{
	writeb(a | 0xff00, b);
}



extern void mbc_reset();


#endif



