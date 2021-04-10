//--------------------------------------------------------------
// File derived from: 
// Datum    : 27.01.2014
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
//--------------------------------------------------------------
#include "zx_filetyp_z80.h"
#include "emuapi.h"

//-------------------------------------------------------------
extern uint8_t out_ram;

//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
const uint8_t* p_decompFlashBlock(const uint8_t *block_adr);


void ZX_ReadFromFlash_SNA(Z80 *regs, const char * filename)
{
  uint8_t snafile[27];
  if (emu_FileOpen(filename)) {
    if (emu_FileRead(&snafile[0], sizeof(snafile)) == sizeof(snafile)) {
      // Load Z80 registers from SNA
      regs->I        = snafile[ 0];
      regs->HL1.B.l  = snafile[ 1];
      regs->HL1.B.h  = snafile[ 2];
      regs->DE1.B.l  = snafile[ 3];
      regs->DE1.B.h  = snafile[ 4];
      regs->BC1.B.l  = snafile[ 5];
      regs->BC1.B.h  = snafile[ 6];
      regs->AF1.B.l  = snafile[ 7];
      regs->AF1.B.h  = snafile[ 8];
      regs->HL.B.l   = snafile[ 9];
      regs->HL.B.h   = snafile[10];
      regs->DE.B.l   = snafile[11];
      regs->DE.B.h   = snafile[12];
      regs->BC.B.l   = snafile[13];
      regs->BC.B.h   = snafile[14];
      regs->IY.B.l = snafile[15];
      regs->IY.B.h = snafile[16];
      regs->IX.B.l = snafile[17];
      regs->IX.B.h = snafile[18];
 //#define IFF_1       0x01       /* IFF1 flip-flop             */
//#define IFF_IM1     0x02       /* 1: IM1 mode                */
//#define IFF_IM2     0x04       /* 1: IM2 mode                */
//#define IFF_2       0x08       /* IFF2 flip-flop             */
//#define IFF_EI      0x20       /* 1: EI pending              */
//#define IFF_HALT    0x80       /* 1: CPU HALTed              */
      regs->R = snafile[20]; //R.W
      regs->AF.B.l = snafile[21];
      regs->AF.B.h = snafile[22];
      regs->SP.B.l =snafile[23];
      regs->SP.B.h =snafile[24];
      regs->IFF = 0;
      regs->IFF |= (((snafile[19]&0x04) >>2)?IFF_1:0); //regs->IFF1 = regs->IFF2 = ...
      regs->IFF |= (((snafile[19]&0x04) >>2)?IFF_2:0);
      regs->IFF |= (snafile[25]<< 1); // regs->IM = snafile[25];
      //regs->BorderColor = snafile[26];

      

      // load RAM from SNA
      int direc;
      uint8_t b;
      for (direc=0;direc!=0xbfff;direc++)
      {
        emu_FileRead(&b, 1);
        WrZ80(direc+0x4000, b);
      }
      emu_FileClose();
      // SP to PC for SNA run
      regs->PC.B.l = RdZ80(regs->SP.W);
      regs->SP.W++;
      regs->PC.B.h = RdZ80(regs->SP.W);
      regs->SP.W++;                    
    }  
  }
}

//--------------------------------------------------------------
// Unpack data from a file ( type = * .Z80 ) from flash
// And copy them to the memory of the ZX - Spectrum
//
// Data = pointer to the start of data
// Length = number of bytes
//--------------------------------------------------------------
void ZX_ReadFromFlash_Z80(Z80 *R, const uint8_t *data, uint16_t length)
{
  const uint8_t *ptr;
  const uint8_t *akt_block,*next_block;
  uint8_t value1,value2;
  uint8_t flag_version=0;
  uint8_t flag_compressed=0;
  uint16_t header_len;
  uint16_t cur_addr;

  if(length==0) return;
  if(length>0xC020) return;

  //----------------------------------
  // parsing header
  // Byte : [0...29]
  //----------------------------------

  // Set pointer to data beginning
  ptr=data;

  R->AF.B.h=*(ptr++); // A [0]
  R->AF.B.l=*(ptr++); // F [1]
  R->BC.B.l=*(ptr++); // C [2]
  R->BC.B.h=*(ptr++); // B [3]
  R->HL.B.l=*(ptr++); // L [4]
  R->HL.B.h=*(ptr++); // H [5]

  // PC [6+7]
  value1=*(ptr++); 
  value2=*(ptr++);
  R->PC.W=(value2<<8)|value1;
  if(R->PC.W==0x0000) {
    flag_version=1;
  }
  else {
    flag_version=0;
  } 

  // SP [8+9]
  value1=*(ptr++);
  value2=*(ptr++);
  R->SP.W=(value2<<8)|value1;

  R->I=*(ptr++); // I [10]
  R->R=*(ptr++); // R [11]
  
  // Comressed-Flag & Border [12]
  value1=*(ptr++); 
  value2=((value1&0x0E)>>1);
  OutZ80(0xFE, value2); // BorderColor
  if((value1&0x20)!=0) {
    flag_compressed=1;
  } else {
    flag_compressed=0;
  }

  R->DE.B.l=*(ptr++); // E [13]
  R->DE.B.h=*(ptr++); // D [14]
  R->BC1.B.l=*(ptr++); // C1 [15]
  R->BC1.B.h=*(ptr++); // B1 [16]
  R->DE1.B.l=*(ptr++); // E1 [17]
  R->DE1.B.h=*(ptr++); // D1 [18]
  R->HL1.B.l=*(ptr++); // L1 [19]
  R->HL1.B.h=*(ptr++); // H1 [20]
  R->AF1.B.h=*(ptr++); // A1 [21]
  R->AF1.B.l=*(ptr++); // F1 [22]
  R->IY.B.l=*(ptr++); // Y [23]
  R->IY.B.h=*(ptr++); // I [24]
  R->IX.B.l=*(ptr++); // X [25]
  R->IX.B.h=*(ptr++); // I [26]

  // Interrupt-Flag [27]
  value1=*(ptr++); 
  if(value1!=0) {
    // EI
    R->IFF|=IFF_2|IFF_EI;
  }
  else {
    // DI
    R->IFF&=~(IFF_1|IFF_2|IFF_EI);
  }
  value1=*(ptr++); // nc [28]
  // Interrupt-Mode [29]
  value1=*(ptr++);  
  if((value1&0x01)!=0) {
    R->IFF|=IFF_IM1;
  }
  else {
    R->IFF&=~IFF_IM1;
  }
  if((value1&0x02)!=0) {
    R->IFF|=IFF_IM2;
  }
  else {
    R->IFF&=~IFF_IM2;
  }
  
  // restliche Register
  R->ICount   = R->IPeriod;
  R->IRequest = INT_NONE; 
  R->IBackup  = 0;

  //----------------------------------
  // save the data in RAM
  // Byte : [30...n]
  //----------------------------------

  cur_addr=0x4000; // RAM start
  

  if(flag_version==0) {
    //-----------------------
    // old Version
    //-----------------------
    if(flag_compressed==1) {
      //-----------------------
      // compressed
      //-----------------------      
      while(ptr<(data+length)) {
        value1=*(ptr++);
        if(value1!=0xED) {
          WrZ80(cur_addr++, value1);
        }
        else {
          value2=*(ptr++);
          if(value2!=0xED) { 
            WrZ80(cur_addr++, value1);
            WrZ80(cur_addr++, value2);
          }
          else {
            value1=*(ptr++);
            value2=*(ptr++);
            while(value1--) {
              WrZ80(cur_addr++, value2);
            }
          }
        }
      }
    }
    else {
      //-----------------------
      // raw (uncompressed)
      //-----------------------
      while(ptr<(data+length)) {
        value1=*(ptr++);
        WrZ80(cur_addr++, value1);
      }
    }
  }
  else {
    //-----------------------
    // new Version
    //-----------------------
    // Header Laenge [30+31]
    value1=*(ptr++); 
    value2=*(ptr++);
    header_len=(value2<<8)|value1;
    akt_block=(uint8_t*)(ptr+header_len); 
    // PC [32+33]
    value1=*(ptr++); 
    value2=*(ptr++);
    R->PC.W=(value2<<8)|value1;
    
    //------------------------
    // 1st block parsing
    //------------------------
    next_block=p_decompFlashBlock(akt_block);
    //------------------------
    // all other parsing
    //------------------------
    while(next_block<data+length) {
      akt_block=next_block;
      next_block=p_decompFlashBlock(akt_block); 
    } 
  }
}


//--------------------------------------------------------------
// Internal function
// Unpack and store a block of data
// ( From flash ) the new version
//--------------------------------------------------------------
const uint8_t* p_decompFlashBlock(const uint8_t *block_adr)
{
  const uint8_t *ptr;
  const uint8_t *next_block;
  uint8_t value1,value2;
  uint16_t block_len;
  uint8_t flag_compressed=0;
  uint8_t flag_page=0;
  uint16_t cur_addr=0;

  // pointer auf Blockanfang setzen
  ptr=block_adr;

  // Laenge vom Block
  value1=*(ptr++);
  value2=*(ptr++);
  block_len=(value2<<8)|value1;
  if(block_len==0xFFFF) {
    block_len=0x4000;
    flag_compressed=0;
  }
  else {
    flag_compressed=1;
  }
 
  // Page vom Block
  flag_page=*(ptr++);
  
  // next Block ausrechnen
  next_block=(uint8_t*)(ptr+block_len); 

  // Startadresse setzen
  if(flag_page==4) cur_addr=0x8000;
  if(flag_page==5) cur_addr=0xC000;
  if(flag_page==8) cur_addr=0x4000;

  if(flag_compressed==1) {
    //-----------------------
    // komprimiert
    //-----------------------
    while(ptr<(block_adr+3+block_len)) {
      value1=*(ptr++);
      if(value1!=0xED) {
        WrZ80(cur_addr++, value1);
      }
      else {
        value2=*(ptr++);
        if(value2!=0xED) { 
          WrZ80(cur_addr++, value1);
          WrZ80(cur_addr++, value2);
        }
        else {
          value1=*(ptr++);
          value2=*(ptr++);
          while(value1--) {
            WrZ80(cur_addr++, value2);
          }
        }
      }
    }
  }
  else {
    //-----------------------
    // nicht komprimiert
    //-----------------------
    while(ptr<(block_adr+3+block_len)) {
      value1=*(ptr++); 
      WrZ80(cur_addr++, value1);  
    }
  }

  return(next_block); 
}

