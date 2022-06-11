//
// Copyright (c) 2004 K. Wilkins
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//

//////////////////////////////////////////////////////////////////////////////
//                       Handy - An Atari Lynx Emulator                     //
//                          Copyright (c) 1996,1997                         //
//                                 K. Wilkins                               //
//////////////////////////////////////////////////////////////////////////////
// System object class                                                      //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This class provides the glue to bind of of the emulation objects         //
// together via peek/poke handlers and pass thru interfaces to lower        //
// objects, all control of the emulator is done via this class. Update()    //
// does most of the work and each call emulates one CPU instruction and     //
// updates all of the relevant hardware if required. It must be remembered  //
// that if that instruction involves setting SPRGO then, it will cause a    //
// sprite painting operation and then a corresponding update of all of the  //
// hardware which will usually involve recursive calls to Update, see       //
// Mikey SPRGO code for more details.                                       //
//                                                                          //
//    K. Wilkins                                                            //
// August 1997                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// Revision History:                                                        //
// -----------------                                                        //
//                                                                          //
// 01Aug1997 KW Document header added & class documented.                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"

//
// Define the global variable list
//
ULONG   gSystemCycleCount=0;
ULONG   gNextTimerEvent=0;
ULONG   gCPUWakeupTime=0;
ULONG   gIRQEntryCycle=0;
ULONG   gCPUBootAddress=0;
ULONG   gBreakpointHit=FALSE;
ULONG   gSingleStepMode=FALSE;
ULONG   gSingleStepModeSprites=FALSE;
ULONG   gSystemIRQ=FALSE;
ULONG   gSystemNMI=FALSE;
ULONG   gSystemCPUSleep=FALSE;
ULONG   gSystemCPUSleep_Saved=FALSE;
ULONG   gSystemHalt=FALSE;
ULONG   gThrottleMaxPercentage=100;
ULONG   gThrottleLastTimerCount=0;
ULONG   gThrottleNextCycleCheckpoint=0;
ULONG   gEndOfFrame=0;
ULONG   gTimerCount=0;
ULONG   gRenderFrame=1;

ULONG   gAudioEnabled=FALSE;
ULONG   *gAudioBuffer;
ULONG   gAudioBufferPointer=0;
ULONG   gAudioLastUpdateCycle=0;
UBYTE   *gPrimaryFrameBuffer=NULL;


extern void lynx_decrypt(unsigned char * result, const unsigned char * encrypted, const int length);

PROGMEM static const ULONG   crc32Table[256] =
{
   0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
   0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
   0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
   0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
   0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
   0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
   0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
   0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
   0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
   0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
   0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
   0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
   0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
   0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
   0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
   0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
   0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
   0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
   0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
   0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
   0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
   0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
   0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
   0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
   0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
   0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
   0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
   0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
   0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
   0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
   0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
   0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


ULONG crc32_le(ULONG crc32, UBYTE const * array, ULONG size)
{
   for (ULONG i = 0; i < size; i++)
      crc32 = ((crc32 >> 8) & 0x00FFFFFF) ^ crc32Table[(crc32 ^ array[i]) & 0xFF];

   return (~crc32);
}

#if 0
int lss_read(void* dest, int varsize, int varcount, LSS_FILE *fp)
{
   ULONG copysize;
   copysize=varsize*varcount;
   if((fp->index + copysize) > fp->index_limit) copysize=fp->index_limit - fp->index;
   memcpy(dest,fp->memptr+fp->index,copysize);
   fp->index+=copysize;
   return copysize;
}

int lss_write(void* src, int varsize, int varcount, LSS_FILE *fp)
{
   ULONG copysize;
   copysize=varsize*varcount;
   //if((fp->index + copysize) > fp->index_limit) copysize=fp->index_limit - fp->index;
   memcpy(fp->memptr+fp->index,src,copysize);
   fp->index+=copysize;
   return copysize;
}

int lss_printf(LSS_FILE *fp, const char *str)
{
   ULONG copysize;
   copysize=strlen(str);
   memcpy(fp->memptr+fp->index,str,copysize);
   fp->index+=copysize;
   return copysize;
}
#endif


CSystem::CSystem(const char* filename, long displayformat, long samplerate)
 : mCart(NULL),
   mRam(NULL),
   mCpu(NULL),
   mMikie(NULL),
   mSusie(NULL),
   mEEPROM(NULL)
{
   UBYTE *filedata = NULL;
   ULONG filesize = 0;
   FILE *fp;

   log_printf("Loading '%s'...\n", filename);

   filesize = emu_FileSize((char*)filename);
   if (filesize > 0) {
      filedata = (UBYTE*)emu_SMalloc(filesize);
      emu_LoadFile((char*)filename, (char*)filedata, filesize);
   }
   /*
   if ((fp = fopen(filename, "rb"))) {
      fseek(fp, 0, SEEK_END);
      filesize = ftell(fp);
      filedata = (UBYTE*)emu_SMalloc(filesize);
      fseek(fp, 0, SEEK_SET);
      if (!filedata) {
         log_printf("-> memory allocation failed (%d bytes)!\n", filesize);
      } else if (fread(filedata, filesize, 1, fp) != 1) {
         log_printf("-> fread failed (%d bytes)!\n", filesize);
      } else {
         log_printf("-> read ok. size=%d, crc32=%08X\n", filesize, crc32_le(0, filedata, filesize));
      }
      fclose(fp);
   } 
   */
   else {
      log_printf("-> fopen failed!\n");
   }

   // Now try and determine the filetype we have opened
   if (!filedata || filesize < 16) {
      mFileType = HANDY_FILETYPE_ILLEGAL;
      mCart = new CCart(0, 0);
      mRam = new CRam(0, 0);
   } else if (!memcmp(filedata + 6, "BS93", 4)) {
      mFileType = HANDY_FILETYPE_HOMEBREW;
      mCart = new CCart(0, 0);
      mRam = new CRam(filedata, filesize);
   } else {
      mFileType = memcmp(filedata, "LYNX", 4) ? HANDY_FILETYPE_RAW : HANDY_FILETYPE_LNX;
      mCart = new CCart(filedata, filesize);
      mRam = new CRam(0, 0);

      // Setup BIOS
      memset(mBiosRom, 0x88, sizeof(mBiosRom));
      mBiosRom[0x00] = 0x8d;
      mBiosRom[0x01] = 0x97;
      mBiosRom[0x02] = 0xfd;
      mBiosRom[0x03] = 0x60; // RTS
      mBiosRom[0x19] = 0x8d;
      mBiosRom[0x20] = 0x97;
      mBiosRom[0x21] = 0xfd;
      mBiosRom[0x4A] = 0x8d;
      mBiosRom[0x4B] = 0x97;
      mBiosRom[0x4C] = 0xfd;
      mBiosRom[0x180] = 0x8d;
      mBiosRom[0x181] = 0x97;
      mBiosRom[0x182] = 0xfd;
   }

   // Vectors
   mBiosVectors[0] = 0x00;
   mBiosVectors[1] = 0x30;
   mBiosVectors[2] = 0x80;
   mBiosVectors[3] = 0xFF;
   mBiosVectors[4] = 0x80;
   mBiosVectors[5] = 0xFF;

   // Regain some memory before initializing the rest
   //emu_SFree(filedata);

   mRamPointer = mRam->GetRamPointer();
   mMemMapReg = 0x00;

   mCycleCountBreakpoint = 0xffffffff;
   mpDebugCallback = NULL;
   mDebugCallbackObject = 0;

   mMikie = new CMikie(*this, displayformat, samplerate);
   mSusie = new CSusie(*this);
   mCpu = new C65C02(*this);
   mEEPROM = new CEEPROM(mCart->CartGetEEPROMType());

   // Now init is complete do a reset, this will cause many things to be reset twice
   // but what the hell, who cares, I don't.....

   Reset();
}

void CSystem::SaveEEPROM(void)
{
   if(mEEPROM!=NULL) mEEPROM->Save();
}

CSystem::~CSystem()
{
   // Cleanup all our objects

   if(mEEPROM!=NULL) delete mEEPROM;
   if(mCart!=NULL) delete mCart;
   if(mRam!=NULL) delete mRam;
   if(mCpu!=NULL) delete mCpu;
   if(mMikie!=NULL) delete mMikie;
   if(mSusie!=NULL) delete mSusie;
}

void CSystem::HLE_BIOS_FE00(void)
{
    // Select Block in A
    C6502_REGS regs;
    mCpu->GetRegs(regs);
    mCart->SetShifterValue(regs.A);
    // we just put an RTS behind in fake ROM!
}

void CSystem::HLE_BIOS_FE19(void)
{
   // (not) initial jump from reset vector
   // Clear full 64k memory!
   memset(mRamPointer, 0x00, RAM_SIZE);

   // Set Load adresse to $200 ($05,$06)
   Poke_CPU(0x0005,0x00);
   Poke_CPU(0x0006,0x02);
   // Call to $FE00
   mCart->SetShifterValue(0);
   // Fallthrough $FE4A
   HLE_BIOS_FE4A();
}

void CSystem::HLE_BIOS_FE4A(void)
{
   UWORD addr=PeekW_CPU(0x0005);

   // Load from Cart (loader blocks)
   unsigned char buff[256];// maximum 5 blocks
   unsigned char res[256];

   buff[0]=mCart->Peek0();
   int blockcount = 0x100 -  buff[0];

   for (int i = 1; i < 1+51*blockcount; ++i) { // first encrypted loader
      buff[i] = mCart->Peek0();
   }

   lynx_decrypt(res, buff, 51);

   for (int i = 0; i < 50*blockcount; ++i) {
      Poke_CPU(addr++, res[i]);
   }

   // Load Block(s), decode to ($05,$06)
   // jmp $200

   C6502_REGS regs;
   mCpu->GetRegs(regs);
   regs.PC=0x0200;
   mCpu->SetRegs(regs);
}

void CSystem::HLE_BIOS_FF80(void)
{
    // initial jump from reset vector ... calls FE19
    HLE_BIOS_FE19();
}

void CSystem::Reset(void)
{
   gSystemCycleCount=0;
   gNextTimerEvent=0;
   gCPUBootAddress=0;
   gBreakpointHit=FALSE;
   gSingleStepMode=FALSE;
   gSingleStepModeSprites=FALSE;
   gSystemIRQ=FALSE;
   gSystemNMI=FALSE;
   gSystemCPUSleep=FALSE;
   gSystemHalt=FALSE;

   gThrottleLastTimerCount=0;
   gThrottleNextCycleCheckpoint=0;

   gTimerCount=0;

   gAudioBufferPointer=0;
   gAudioLastUpdateCycle=0;

#ifdef _LYNXDBG
   gSystemHalt=TRUE;
#endif

   mCart->Reset();
   mEEPROM->Reset();
   mRam->Reset();
   mMikie->Reset();
   mSusie->Reset();
   mCpu->Reset();

   // Homebrew hashup
   if(mFileType==HANDY_FILETYPE_HOMEBREW) {
      mMikie->PresetForHomebrew();
      C6502_REGS regs;
      mCpu->GetRegs(regs);
      regs.PC=(UWORD)gCPUBootAddress;
      mCpu->SetRegs(regs);
   }
}

bool CSystem::ContextSave(LSS_FILE *fp)
{
   bool status=1;

   // fp->index = 0;
   if(!lss_printf(fp, LSS_VERSION)) status=0;

   // Save ROM CRC
   ULONG checksum=mCart->CRC32();
   if(!lss_write(&checksum,sizeof(ULONG),1,fp)) status=0;

   if(!lss_printf(fp, "CSystem::ContextSave")) status=0;

   if(!lss_write(&mCycleCountBreakpoint,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gSystemCycleCount,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gNextTimerEvent,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gCPUWakeupTime,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gCPUBootAddress,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gIRQEntryCycle,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gBreakpointHit,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gSingleStepMode,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gSystemIRQ,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gSystemNMI,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gSystemCPUSleep,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gSystemCPUSleep_Saved,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gSystemHalt,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gThrottleMaxPercentage,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gThrottleLastTimerCount,sizeof(ULONG),1,fp)) status=0;
   if(!lss_write(&gThrottleNextCycleCheckpoint,sizeof(ULONG),1,fp)) status=0;

   if(!lss_write(&gTimerCount,sizeof(ULONG),1,fp)) status=0;

   if(!lss_write(&gAudioLastUpdateCycle,sizeof(ULONG),1,fp)) status=0;

   if(!lss_write(&mMemMapReg,sizeof(UBYTE),1,fp)) status=0;

   // Save other device contexts
   if(!mCart->ContextSave(fp)) status=0;
   if(!mRam->ContextSave(fp)) status=0;
   if(!mMikie->ContextSave(fp)) status=0;
   if(!mSusie->ContextSave(fp)) status=0;
   if(!mCpu->ContextSave(fp)) status=0;
   if(!mEEPROM->ContextSave(fp)) status=0;

   return status;
}

bool CSystem::ContextLoad(LSS_FILE *fp)
{
   bool status=1;

   // fp->index=0;

   char teststr[32];
   // Check identifier
   if(!lss_read(teststr,sizeof(char),4,fp)) status=0;
   teststr[4]=0;

   if(strcmp(teststr,LSS_VERSION)==0) {
      ULONG checksum;
      // Read CRC32 and check against the CART for a match
      lss_read(&checksum,sizeof(ULONG),1,fp);
      if(mCart->CRC32()!=checksum) {
         log_printf("CSystem::ContextLoad() LSS Snapshot CRC does not match the loaded cartridge image...\n");
         // return 0;
      }

      // Check our block header
      if(!lss_read(teststr,sizeof(char),20,fp)) status=0;
      teststr[20]=0;
      if(strcmp(teststr,"CSystem::ContextSave")!=0) status=0;

      if(!lss_read(&mCycleCountBreakpoint,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gSystemCycleCount,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gNextTimerEvent,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gCPUWakeupTime,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gCPUBootAddress,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gIRQEntryCycle,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gBreakpointHit,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gSingleStepMode,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gSystemIRQ,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gSystemNMI,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gSystemCPUSleep,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gSystemCPUSleep_Saved,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gSystemHalt,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gThrottleMaxPercentage,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gThrottleLastTimerCount,sizeof(ULONG),1,fp)) status=0;
      if(!lss_read(&gThrottleNextCycleCheckpoint,sizeof(ULONG),1,fp)) status=0;

      if(!lss_read(&gTimerCount,sizeof(ULONG),1,fp)) status=0;

      if(!lss_read(&gAudioLastUpdateCycle,sizeof(ULONG),1,fp)) status=0;

      if(!lss_read(&mMemMapReg,sizeof(UBYTE),1,fp)) status=0;

      if(!mCart->ContextLoad(fp)) status=0;
      if(!mRam->ContextLoad(fp)) status=0;
      if(!mMikie->ContextLoad(fp)) status=0;
      if(!mSusie->ContextLoad(fp)) status=0;
      if(!mCpu->ContextLoad(fp)) status=0;
      if(!mEEPROM->ContextLoad(fp)) status=0;

      gAudioBufferPointer = 0;
   } else {
      log_printf("CSystem::ContextLoad() Not a recognised LSS file!\n");
   }

   return status;
}

#ifdef _LYNXDBG

void CSystem::DebugTrace(int address)
{
   char message[1024+1];
   int count=0;

   log_printf(message,"%08x - DebugTrace(): ",gSystemCycleCount);
   count=strlen(message);

   if(address) {
      if(address==0xffff) {
         C6502_REGS regs;
         char linetext[1024];
         // Register dump
         mCpu->GetRegs(regs);
         log_printf(linetext,"PC=$%04x SP=$%02x PS=0x%02x A=0x%02x X=0x%02x Y=0x%02x",regs.PC,regs.SP, regs.PS,regs.A,regs.X,regs.Y);
         strcat(message,linetext);
         count=strlen(message);
      } else {
         // The RAM address contents should be dumped to an open debug file in this function
         do {
            message[count++]=Peek_RAM(address);
         } while(count<1024 && Peek_RAM(address++)!=0);
      }
   } else {
      strcat(message,"CPU Breakpoint");
      count=strlen(message);
   }
   message[count]=0;

   // Callback to dump the message
   if(mpDebugCallback) {
      (*mpDebugCallback)(mDebugCallbackObject,message);
   }
}

void CSystem::DebugSetCallback(void (*function)(ULONG objref,char *message),ULONG objref)
{
   mDebugCallbackObject=objref;
   mpDebugCallback=function;
}

#endif
