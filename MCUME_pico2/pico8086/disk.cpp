#include <stdint.h>
#include <stdio.h>
#include "intf.h"
#include "emuapi.h"

extern uint8_t bootdrive, hdcount;
extern uint16_t segregs[6];
extern uint8_t cf; 
extern _bytewordregs_ regs;

int file;

struct struct_drive {
  uint32_t filesize;
  uint16_t cyls;
  uint16_t sects;
  uint16_t heads;
  uint8_t inserted;
} disk[4];
uint8_t sectorbuffer[512];

uint8_t insertdisk(uint8_t drivenum) {
  if (drivenum & 0x80) {
    drivenum -= 126;
    disk[drivenum].sects = 63;
    disk[drivenum].heads = 16;
    disk[drivenum].cyls = 1023; //up to 512 MB
    hdcount = 1;
  } else {
#ifdef FDD_144M
    disk[drivenum].cyls = 80;
    disk[drivenum].sects = 18;
    disk[drivenum].heads = 2;
#endif
#ifdef FDD_122M
    disk[drivenum].cyls = 80;
    disk[drivenum].sects = 15;
    disk[drivenum].heads = 2;
#endif
#ifdef FDD_720K
    disk[drivenum].cyls = 80;
    disk[drivenum].sects = 9;
    disk[drivenum].heads = 2;
#endif
#ifdef FDD_360K
    disk[drivenum].cyls = 40;
    disk[drivenum].sects = 9;
    disk[drivenum].heads = 2;
#endif
#ifdef FDD_320K
    disk[drivenum].cyls = 40;
    disk[drivenum].sects = 8;
    disk[drivenum].heads = 2;
#endif
#ifdef FDD_180K
    disk[drivenum].cyls = 40;
    disk[drivenum].sects = 9;
    disk[drivenum].heads = 1;
#endif
  }
  disk[drivenum].inserted = 1;
  return 0;
}

void ejectdisk(uint8_t drivenum) {
  if (drivenum & 0x80) drivenum -= 126;
  disk[drivenum].inserted = 0;
}

extern uint16_t ramseg;
extern "C" {
extern void emu_SdReadBlock(int block, void * buf);
}
uint8_t sectdone;
void getsect(uint32_t lba, uint8_t *dst) {
#ifdef USB_DISK
  uint8_t chksum;
  uint32_t curmicros;
retrysectget:
  Serial.write(0xFF);
  Serial.write(0x05);
  outByte(lba & 0xFF); chksum = lba & 0xFF;
  outByte((lba >> 8) & 0xFF); chksum += (lba >> 8) & 0xFF;
  outByte((lba >> 16) & 0xFF); chksum += (lba >> 16) & 0xFF;
  outByte((lba >> 24) & 0xFF); chksum += (lba >> 24) & 0xFF;
  outByte(chksum);
  Serial.write(0xFE);
  Serial.write(0x02);
  sectdone = 0;
  curmicros = micros();
  while (!sectdone) {
    if (micros() < curmicros) curmicros = micros();
    if ((micros() - curmicros) >= 200000) goto retrysectget;
    net_loop();
  }
#else
  if (file) 
  {  
    //printf("read block %d\n",lba);  
    emu_FileSeek(file,lba*512,0);
    emu_FileRead(dst,1*512,file); 
  }   
#endif
}

void putsect(uint32_t lba, uint8_t *src) {
#ifdef USB_DISK
  uint8_t chksum;
  uint16_t i;
  uint32_t curmicros;
retrysectput:
  Serial.write(0xFF);
  Serial.write(0x06);
  outByte(lba & 0xFF); chksum = lba & 0xFF;
  outByte((lba >> 8) & 0xFF); chksum += (lba >> 8) & 0xFF;
  outByte((lba >> 16) & 0xFF); chksum += (lba >> 16) & 0xFF;
  outByte((lba >> 24) & 0xFF); chksum += (lba >> 24) & 0xFF;
  for (i=0; i<512; i++) {
    outByte(src[i]);
    chksum += src[i];
  }
  outByte(chksum);
  Serial.write(0xFE);
  Serial.write(0x02);
  sectdone = 0;
  curmicros = micros();
  while (!sectdone) {
    if (micros() < curmicros) curmicros = micros();
    if ((micros() - curmicros) >= 200000) goto retrysectput;
    net_loop();
  }
#else
  //card.writeBlock(lba, src);
#endif
}
void readdisk(uint8_t drivenum, uint16_t dstseg, uint16_t dstoff, uint16_t cyl, uint16_t sect, uint16_t head, uint16_t sectcount) {
  uint32_t memdest, goodsects, dummy, lba;
  if ((sect == 0) || !disk[drivenum].inserted) return;
#ifdef MEGA
  SPI.setClockDivider(SPI_CLOCK_SDCARD);
#endif
  lba = ((long)cyl * (long)disk[drivenum].heads + (long)head) * (long)disk[drivenum].sects + (long)sect - 1;
  memdest = (uint32_t)dstseg * 16 + (uint32_t)dstoff;
  for (goodsects = 0; goodsects < sectcount; goodsects++) {
    getsect(lba, sectorbuffer);
    memdest = (uint32_t)dstseg * 16 + (uint32_t)dstoff;
    for (dummy = 0; dummy < 512; dummy++) {
      write86(memdest++, sectorbuffer[dummy]);
      //Serial.write(sectorbuffer[dummy]);
    }
    dstoff += 512;
    lba++;
  }
  cf = 0; regs.byteregs[regah] = 0; regs.byteregs[regal] = sectcount;
#ifdef MEGA
  SPI.setClockDivider(SPI_CLOCK_SPIRAM);
#endif
}

void writedisk(uint8_t drivenum, uint16_t dstseg, uint16_t dstoff, uint16_t cyl, uint16_t sect, uint16_t head, uint16_t sectcount) {
  uint32_t memdest, goodsects, dummy, lba;
  if ((sect == 0) || !disk[drivenum].inserted) return;
#ifdef MEGA
  SPI.setClockDivider(SPI_CLOCK_SDCARD);
#endif
  lba = ((long)cyl * (long)disk[drivenum].heads + (long)head) * (long)disk[drivenum].sects + (long)sect - 1;
  for (goodsects = 0; goodsects < sectcount; goodsects++) {
    memdest = (uint32_t)dstseg * 16 + (uint32_t)dstoff;
    for (dummy = 0; dummy < 512; dummy++) {
      sectorbuffer[dummy] = read86(memdest++);
    }
    //card.erase(lba, lba);
    putsect(lba, sectorbuffer);
    dstoff += 512;
    lba++;
  }
  cf = 0; regs.byteregs[regah] = 0; regs.byteregs[regal] = sectcount;
#ifdef MEGA
  SPI.setClockDivider(SPI_CLOCK_SPIRAM);
#endif
}

void diskhandler() {
  static uint8_t lastdiskah[4], lastdiskcf[4];
  uint8_t drivenum;
  drivenum = regs.byteregs[regdl];
  if (drivenum & 0x80) drivenum -= 126;
  switch (regs.byteregs[regah]) {
    case 0: //reset disk system
      regs.byteregs[regah] = 0; cf = 0; //useless function in an emulator. say success and return.
      break;
    case 1: //return last status
      regs.byteregs[regah] = lastdiskah[drivenum];
      cf = lastdiskcf[drivenum];
      return;
    case 2: //read sector(s) into memory
      if (disk[drivenum].inserted) {
        readdisk(drivenum, segregs[reges], getreg16(regbx), (uint16_t)regs.byteregs[regch] + ((uint16_t)regs.byteregs[regcl] / 64) * 256, regs.byteregs[regcl] & 63, regs.byteregs[regdh], regs.byteregs[regal]);
        cf = 0; regs.byteregs[regah] = 0;
      } else {
        cf = 1;
        regs.byteregs[regah] = 1;
      }
      break;
    case 3: //write sector(s) from memory
      if (disk[drivenum].inserted) {
        writedisk(drivenum, segregs[reges], getreg16(regbx), regs.byteregs[regch] + (regs.byteregs[regcl] / 64) * 256, regs.byteregs[regcl] & 63, regs.byteregs[regdh], regs.byteregs[regal]);
        cf = 0; regs.byteregs[regah] = 0;
      } else {
        cf = 1;
        regs.byteregs[regah] = 1;
      }
      break;
    case 4:
    case 5: //format track
      cf = 0; regs.byteregs[regah] = 0;
      break;
    case 8: //get drive parameters
      if (disk[drivenum].inserted) {
        cf = 0; regs.byteregs[regah] = 0;
        regs.byteregs[regch] = disk[drivenum].cyls - 1;
        regs.byteregs[regcl] = disk[drivenum].sects & 63;
        regs.byteregs[regcl] = regs.byteregs[regcl] + (disk[drivenum].cyls / 256) * 64;
        regs.byteregs[regdh] = disk[drivenum].heads - 1;
        //segregs[reges] = 0; regs.wordregs[regdi] = 0x7C0B; //floppy parameter table
        if (drivenum < 2) {
          regs.byteregs[regbl] = 4; //else regs.byteregs[regbl] = 0;
          regs.byteregs[regdl] = 2;
        } else regs.byteregs[regdl] = hdcount;
      } else {
        cf = 1; regs.byteregs[regah] = 0xAA;
      }
      break;
    default:
      cf = 1;
  }
  lastdiskah[drivenum] = regs.byteregs[regah];
  lastdiskcf[drivenum] = cf;
  if (regs.byteregs[regdl] & 0x80) write86(0x474, regs.byteregs[regah]);
}

void initDisk(char * filename) {
  int len=emu_FileSize(filename);
  file = emu_FileOpen(filename,"r+b");  
  if (len) {
    bootdrive = 0x80;
    insertdisk(0x80);
    hdcount = 1;
  }
  else {
    bootdrive = 0xFF;
    emu_FileClose(file);
    file = 0;
  }
}

