/*
* Castaway
*  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
*
* fdc.c - wd1772/dma emulation
*
* This file is distributed under the GPL, version 2 or at your
* option any later version.  See doc/license.txt for details.
*
* revision history
*  23.05.2002  0.02.00 JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  09.06.2002  0.02.00 JH  Renamed io.c to st.c again (io.h conflicts with system headers)
*/

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dcastaway.h"
#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"

#include "emuapi.h"


#define DISKNULL \
	"\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0"



/*
* FDC Registers
*/
unsigned char fdc_data, fdc_track, fdc_sector, fdc_status, fdc_command, fdc_motor;
unsigned char fdc_int = 0;

static char fdcdir=1;
static int fdc_commands_executed=0;
static unsigned char disk_ejected[2]={0,1};
static unsigned char disk_changed[2];
static int discpos[2];

struct Disk disk[2] = {
    { NULL, DISKNULL, 0, SIDES, TRACKS, SECTORS, SECSIZE },
    { NULL, DISKNULL, 0, SIDES, TRACKS, SECTORS, SECSIZE },
};


PROGMEM static void SetMemBBB (unsigned long address, unsigned char value) {
  address &= MEMADDRMASK;
  WriteB(address + membase, value);
}

PROGMEM static unsigned char GetMemBBB (unsigned long address) {
  address &= MEMADDRMASK;
  return (ReadB(address + membase));
}

PROGMEM int discread(unsigned long address,int len,int discn)
{
  int i;
	int totlen = len; 
  unsigned char buf[256];
	while (totlen>=256) {
    emu_FileRead(buf,256, disk[discn].file);
		totlen -= 256;		
		for (i=0; i<256; i++) {
			SetMemBBB(address, buf[i]);
			address++;
		}
	}
	if (totlen) {
		emu_FileRead(buf,totlen, disk[discn].file);
		for (i=0; i<totlen; i++) {
			SetMemBBB(address, buf[i]);
			address++;
		}
	}
	discpos[discn]=discpos[discn]+len;

  emu_FileClose(disk[discn].file);
  disk[discn].file = emu_FileOpen(disk[discn].name,"a+rw");
  emu_FileSeek(disk[discn].file,discpos[discn],SEEK_SET);  
	return len;
}

PROGMEM int discwrite(unsigned long address, int len,int discn)
{
  int i;
  int totlen = len;
  unsigned char buf[256];
   
  while (totlen>=256) {
    for (i=0; i<256; i++) {
      buf[i] = GetMemBBB(address);
      address++;
    } 
    emu_FileWrite(buf,256, disk[discn].file);
    totlen -= 256;
  }
  if (totlen) {   
    for (i=0; i<totlen; i++) {
      buf[i] = GetMemBBB(address);
      address++;
    }       
    emu_FileWrite(buf,totlen, disk[discn].file);
  }
  discpos[discn]=discpos[discn]+len;

  emu_FileClose(disk[discn].file);
  disk[discn].file = emu_FileOpen(disk[discn].name,"a+rw");
  emu_FileSeek(disk[discn].file,discpos[discn],SEEK_SET); 
	return len;
}

int discseek(int discn,int pos)
{
	if (pos>(1050*1024)){
		return -1;
	}
	discpos[discn]=pos;
	if (disk[discn].file) emu_FileSeek(disk[discn].file,pos,SEEK_SET);   
	return 0;
}


PROGMEM int FDCInit(int i)
{
  int  len2,calcsides,calcsectors,calctracks,badbootsector;

  discpos[i]=0;

 	int len = emu_FileSize(disk[i].name);

  disk[i].file = emu_FileOpen(disk[i].name,"a+rw");
  disk[i].disksize = len;    

  unsigned char buf[256]; 
 	if (disk[i].file) {
    emu_FileRead(buf, 256, disk[i].file);
    emu_FileSeek(disk[i].file,0,SEEK_SET);
  }	
     
  disk[i].head = 0;
  disk[i].sides = (int) *(buf + 26);
  disk[i].sectors = (int) *(buf + 24);
  disk[i].secsize = 512; //(int) ((*(buf + 12) << 8) | *(buf + 11));
  
	if (disk[i].sectors * disk[i].sides)  {
    disk[i].tracks = (int) ((*(buf + 20) << 8) | *(buf + 19)) / (disk[i].sectors * disk[i].sides);  
	}
      
	// Second Check more precise 
	if (len> (500*1024)) calcsides = 2;
	else calcsides = 1;
 
	if (!(((len/calcsides)/512)%9)&&(((len/calcsides)/512)/9)<86) calcsectors=9;
	else if (!(((len/calcsides)/512)%10)&&(((len/calcsides)/512)/10)<86) calcsectors=10;
	else if (!(((len/calcsides)/512)%11)&&(((len/calcsides)/512)/11)<86) calcsectors=11;
	else if (!(((len/calcsides)/512)%12)) calcsectors=12;
	calctracks =((len/calcsides)/512)/calcsectors;
	
	if (disk[i].sides!=calcsides||disk[i].sectors!=calcsectors||disk[i].tracks!=calctracks){
		if (disk[i].sides==calcsides&&disk[i].sectors==calcsectors){
			disk[i].tracks=calctracks;
			badbootsector=0;
		}else{
			disk[i].sides=calcsides;
			disk[i].tracks=calctracks;
			disk[i].sectors=calcsectors;
			badbootsector=(i<<24)|(calcsides<<16)|(calctracks<<8)|(calcsectors);
		}
		
	}else{
		badbootsector=0;
	}
	disk_ejected[i]=0;
	disk_changed[i]=1;
	fdc_status |= 0x40;
	disk[i].head = 0;
	fdc_track = 0;

  return badbootsector;
}

PROGMEM void FDCchange(int i){
	disk[(i>>24)&0xff].sides=(i>>16)&0xff;
	disk[(i>>24)&0xff].tracks=(i>>8)&0xff;
	disk[(i>>24)&0xff].sectors=i&0xff;
}

PROGMEM void FDCeject(int num){
	int i;
       
	disk[num].file = NULL;
	sprintf(disk[num].name,"disk%01d",num);
	disk[num].sides = SIDES;
	disk[num].tracks = TRACKS;
	disk[num].sectors = SECTORS;
	disk[num].secsize = 512;
	disk_ejected[num]=1;
	fdc_status |= 0x40;
}

PROGMEM void            FDCCommand(void)
{
  static char     motor = 1;
  int             sides, drives;
  long            address;    /* dma target/source address */
  long            offset;     /* offset in disk file */
  unsigned long   count;      /* number of byte to transfer */
 	int n;

  if (fdc_commands_executed<64)
  	fdc_commands_executed++;
  /* DMA target/source address */
  address = (dma_adrh << 16) + (dma_adrm << 8) + dma_adrl;
	/*	
  if (address>MEMSIZE) {
	  fdc_status |= 0x10;
	  return;	
  }
	*/

  /* status of side select and drive select lines */
  sides = (~psg[14]) & 0x1;
  drives = (~psg[14]) & 0x6;
  if (disk_ejected[drives>>2]==1) drives=2;

	switch (drives) 
	{
    case 2:                     /* Drive A */
      drives = 0;
      break;
    case 4:                     /* Drive B */
      drives = 1;
      break;
    case 6:                     /* both, error */
    case 0:                     /* no drive selected */
      drives = -1;
      break;
  }
    
  fdc_status = 0;             /* clear fdc status */    
  if (fdc_command < 0x80) {   /* TYPE-I fdc commands */
    if (drives >= 0) {      /* drive selected */
      switch (fdc_command & 0xf0) {
        case 0x00:          /* RESTORE */
          disk[drives].head = 0;
          fdc_track = 0;
          break;
        case 0x10:          /* SEEK */
          disk[drives].head += (fdc_data - fdc_track);
          fdc_track = fdc_data;
          if (disk[drives].head < 0 || disk[drives].head >= disk[drives].tracks)
              disk[drives].head = 0;
          break;
        case 0x30:          /* STEP */
          fdc_track += fdcdir;
        case 0x20:
          disk[drives].head += fdcdir;
          break;
        case 0x50:          /* STEP-IN */
          fdc_track++;
        case 0x40:
          if (disk[drives].head < disk[drives].tracks)
              disk[drives].head++;
          fdcdir = 1;
          break;
        case 0x70:          /* STEP-OUT */
          fdc_track--;
        case 0x60:
          if (disk[drives].head > 0)
              disk[drives].head--;
          fdcdir = -1;
          break;
      }
      
      if (disk[drives].head == 0) {
          fdc_status |= 0x4;
      }
      if (disk[drives].head != fdc_track && fdc_command & 0x4) {  /* Verify? */
          fdc_status |= 0x10;
      }
      if (motor) {
          fdc_status |= 0x20;     /* spin-up flag */
      }
    } 
    else {                /* no drive selected */
      fdc_status |= 0x10;
    }
  } 
    
  else if ((fdc_command & 0xf0) == 0xd0) {  /* FORCE INTERRUPT */
    if (fdc_command == 0xd8) 
    {
        fdc_int = 1;
    } else if (fdc_command == 0xd0) 
    {
        fdc_int = 0;
    }
  } 
  
  else {                    /* OTHERS */
    if (drives >= 0) {      /* drive selected */
      /* offset within floppy-file */
      offset = disk[drives].secsize * (((disk[drives].sectors * disk[drives].sides * disk[drives].head)) + (disk[drives].sectors * sides) + (fdc_sector - 1));
      switch (fdc_command & 0xf0) 
      {
        case 0x80:          /* READ SECTOR */
          count = 512;
          if (!discseek (drives, offset)) 
          {
    				if (address<MEMSIZE)
    				{
    					if (count == discread (address, count, drives)) 
    					{
    						address += count;
    						dma_adrl = address & 0xff;
    						dma_adrm = (address >> 8) & 0xff;
    						dma_adrh = (address >> 16) & 0xff;
    						dma_scr = 0;
    						dma_sr = 1;
    						break;
    					}
    				}
    				else 
    				{
              address += count;
              dma_adrl = address & 0xff;
              dma_adrm = (address >> 8) & 0xff;
              dma_adrh = (address >> 16) & 0xff;
              dma_scr = 0;
              dma_sr = 1;
    					mfp_gpip |= 0x20;
    					fdc_status |= 0x1;
              break;
    				}
          }
          fdc_status |= 0x10;
          dma_sr = 1;
          break;
          
    		case 0x90:			/* READ SECTOR multiple */
    			count = dma_scr * 512;
    			if (count+(fdc_sector-1)*512>disk[drives].sectors*512) count=disk[drives].sectors*512-(fdc_sector-1)*512;
          if (!discseek (drives, offset)) 
          {
    				if (address<MEMSIZE)
    				{
    					if (count == discread (address, count, drives)) 
    					{
    						address += count;
    						dma_adrl = address & 0xff;
    						dma_adrm = (address >> 8) & 0xff;
    						dma_adrh = (address >> 16) & 0xff;
    						dma_scr = 0;
    						dma_sr = 1;
    						fdc_sector += count/disk[drives].secsize;
    						break;
    					}
    				}
    				else
            {
              address += count;
              dma_adrl = address & 0xff;
              dma_adrm = (address >> 8) & 0xff;
              dma_adrh = (address >> 16) & 0xff;
              dma_scr = 0;
              dma_sr = 1;
    					mfp_gpip |= 0x20;
    					fdc_status |= 0x1;
              break;
    				}
    			}
    			fdc_status |= 0x10;
          dma_sr = 1;
          break;

        case 0xa0:          /* WRITE SECTOR */
          count = dma_scr * 512;
          if (!discseek (drives, offset)) {
            if (count == discwrite (address, count, drives)) 
            {
              address += count;
              dma_adrl = address & 0xff;
              dma_adrm = (address >> 8) & 0xff;
              dma_adrh = (address >> 16) & 0xff;
              dma_scr = 0;
              dma_sr = 1;
              break;
            }
          }
          fdc_status |= 0x10;
          dma_sr = 1;
          break;
            
        case 0xb0:          /* WRITE SECTOR multiple */
          count = dma_scr * 512;
          if (!discseek (drives, offset)) 
          {
            if (count == discwrite (address, count, drives)) {
              address += count;
              dma_adrl = address & 0xff;
              dma_adrm = (address >> 8) & 0xff;
              dma_adrh = (address >> 16) & 0xff;
              dma_scr = 0;
              dma_sr = 1;
              fdc_sector += dma_scr * (512 / disk[drives].secsize);
              break;
            }
          }
          fdc_status |= 0x10;
          dma_sr = 1;
          break;
            
        case 0xc0:          /* READ ADDRESS */
          fdc_status |= 0x10;
          break;
            
        case 0xe0:          /* READ TRACK */
	        count = disk[drives].sectors * 512;
	        offset = disk[drives].secsize * (((disk[drives].sectors * disk[drives].sides * disk[drives].head)) + (disk[drives].sectors * sides));
          if (!discseek (drives, offset)) 
          {
		        if (address<MEMSIZE)
		        {
    					if (dma_scr==0x1f)
    					{
    						count=0;
    						address += 302;
    					}
    					if (count == discread (address, count, drives)) 
    					{
    						dma_adrl = address & 0xff;
    						dma_adrm = (address >> 8) & 0xff;
    						dma_adrh = (address >> 16) & 0xff;
    						dma_scr = 0;
    						dma_sr = 1;
    						break;
    					}
		        }
		        else
		        {
              address += 302;
              dma_adrl = address & 0xff;
              dma_adrm = (address >> 8) & 0xff;
              dma_adrh = (address >> 16) & 0xff;
              dma_scr = 0;
              dma_sr = 1;
	            mfp_gpip |= 0x20;
	            fdc_status |= 0x1;
              break;
		        }
	        }
	        fdc_status |= 0x10;
          dma_sr = 1;
          break;
            
        case 0xf0:          /* WRITE TRACK */
          fdc_status |= 0x10;
          break;
      }      
         
      if (disk[drives].head != fdc_track) 
      {
        fdc_status |= 0x10;
      }
    } 
    else 
    {
      fdc_status |= 0x10; /* no drive selected */
    }
  }
   
  if (motor) 
  {
    fdc_status |= 0x80;     /* motor on flag */
    fdc_motor=1;
  }
  if (!(fdc_status & 0x01)) 
  { /* not busy */
    mfp_iprb |= (0x80 & mfp_ierb);  /* Request Interrupt */
    mfp_gpip &= ~0x20;
  }
}
