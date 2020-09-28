#include <string.h>

#include "emuapi.h"
#include "tft_t_dma.h"
#include "iopins.h" 


#include "shared.h"
#include "Z80.h"            /* Z80 CPU emulation             */
#include "MSX.h"
#include "Sound.h"
#include <stdlib.h>
#include <ctype.h>
//#include <time.h>



static void RefreshLineTx80(byte Y);
static void RefreshLine0(byte Y);
static void RefreshLine1(byte Y);
static void RefreshLine2(byte Y);
static void RefreshLine3(byte Y);
static void RefreshLine4(byte Y);
static void RefreshLine5(byte Y);
static void RefreshLine6(byte Y);
static void RefreshLine7(byte Y);
static void RefreshLine8(byte Y);
static void RefreshLine10(byte Y);
static void RefreshLine12(byte Y);


/** Zero-terminated arrays of disk names for each drive ******/
extern char *Disks[2][MAXDISKS+1];


/**************************************
* Local macros/typedef
**************************************/
#define WIDTH  272
#define HEIGHT 228

#define RGB2INT(R,G,B) ((B)|((int)(G)<<8)|((int)(R)<<16))

typedef byte pixel;


/**************************************
* Local procedures
**************************************/
static void  Sprites(byte Y,pixel *Line);
static void  ColorSprites(byte Y,byte *ZBuf);
static pixel *RefreshBorder(byte Y,pixel C);
static void  ClearLine(pixel *P,pixel C);
static pixel YJKColor(int Y,int J,int K);

/** Internal Functions ***************************************/
/** These functions are defined and internally used by the  **/
/** code in MSX.c.                                          **/
/*************************************************************/
byte *LoadROM(const char *Name,int Size,byte *Buf);
int  LoadCart(const char *Name,int Slot);
int  GuessROM(const byte *Buf,int Size);
void SetMegaROM(int Slot,byte P0,byte P1,byte P2,byte P3);
void MapROM(word A,byte V);       /* Switch MegaROM banks            */
void SSlot(byte V);               /* Switch secondary slots          */
void VDPOut(byte R,byte V);       /* Write value into a VDP register */
void Printer(byte V);             /* Send a character to a printer   */
void PPIOut(byte New,byte Old);   /* Set PPI bits (key click, etc.)  */
void CheckSprites(void);          /* Check collisions and 5th sprite */
byte RTCIn(byte R);               /* Read RTC registers              */
byte SetScreen(void);             /* Change screen mode              */
word SetIRQ(byte IRQ);            /* Set/Reset IRQ                   */
word StateID(void);               /* Compute emulation state ID      */

static void RefreshLineF(register byte Y);
static void RefreshLine0(register byte Y);
static void RefreshLine1(register byte Y);
static void RefreshLine2(register byte Y);
static void RefreshLine3(register byte Y);
static void RefreshLine4(register byte Y);
static void RefreshLine5(register byte Y);
static void RefreshLine8(register byte Y);
static void RefreshLine10(register byte Y);
static void RefreshLine12(register byte Y);
//#ifdef NARROW
static void RefreshLine7(register byte Y);
static void RefreshLineTx80(register byte Y);
//#endif

/**************************************
* Local variables
**************************************/
static unsigned int BPal[256],XPal[80],XPal0; 

static int FirstLine = 18;     /* First scanline in the framebuffer */

/** User-defined parameters for fMSX *************************/
byte Verbose     = 1;              /* Debug msgs ON/OFF      */
byte UPeriod     = 2;              /* Interrupts/scr. update */
int  VPeriod     = CPU_VPERIOD;    /* CPU cycles per VBlank  */
int  HPeriod     = CPU_HPERIOD;    /* CPU cycles per HBlank  */
byte SaveCMOS    = 0;              /* Save CMOS.ROM on exit  */
byte SaveSRAM    = 0;              /* ~ GMASTER2.RAM on exit */
byte MSXVersion  = 1;              /* 0=MSX1,1=MSX2,2=MSX2+  */
byte JoyTypeA    = 1;              /* 0=None,1=Joystick,     */
byte JoyTypeB    = 0;              /* 2=MouseAsJstk,3=Mouse  */
byte ROMTypeA    = MAXMAPPERS;     /* MegaROM types          */
byte ROMTypeB    = MAXMAPPERS;
int  RAMPages    = 4;              /* Number of RAM pages    */
int  VRAMPages   = 2;              /* Number of VRAM pages   */
byte AutoFire    = 0;              /* Autofire on [SPACE]    */
byte UseDrums    = 0;              /* Use drms for PSG noise */
byte ExitNow     = 0;              /* 1 = Exit the emulator  */

/** Main hardware: CPU, RAM, VRAM, mappers *******************/
Z80 CPU;                           /* Z80 CPU state and regs */

byte *VRAM,*VPAGE;                 /* Video RAM              */

byte *RAM[8];                      /* Main RAM (8x8kB pages) */
byte *EmptyRAM;                    /* Empty RAM page (8kB)   */
byte *SRAM;                        /* SRAM (battery backed)  */
byte *MemMap[4][4][8];   /* Memory maps [PPage][SPage][Addr] */

byte *RAMData;                     /* RAM Mapper contents    */
byte RAMMapper[4];                 /* RAM Mapper state       */
byte RAMMask;                      /* RAM Mapper mask        */

byte *ROMData[2];                  /* ROM Mapper contents    */
byte ROMMapper[2][4];              /* ROM Mappers state      */
byte ROMMask[2];                   /* ROM Mapper masks       */

byte EnWrite[4];                   /* 1 if write enabled     */
byte PSL[4],SSL[4];                /* Lists of current slots */
byte PSLReg,SSLReg;      /* Storage for A8h port and (FFFFh) */

/** Memory blocks to free in TrashMSX() **********************/
byte *Chunks[256];                 /* Memory blocks to free  */
byte CCount;                       /* Number of memory blcks */


/** Cartridge files used by fMSX *****************************/
char CartA[128]; //"CARTA.ROM";    /* Cartridge A ROM file   */
char *CartB      = "CARTB.ROM";    /* Cartridge B ROM file   */

/** Disk images used by fMSX *********************************/
char *DiskA      = "DRIVEA.DSK";   /* Drive A disk image  */
char *DiskB      = "DRIVEB.DSK";   /* Drive B disk image  */

/** Fixed font used by fMSX **********************************/
char *FontName   = "DEFAULT.FNT";  /* Font file for text     */
byte *FontBuf;                     /* Font for text modes    */
byte UseFont     = 0;              /* Use ext. font when 1   */

/** Printer **************************************************/
#ifdef unused
char *PrnName    = NULL;           /* Printer redirect. file */
int *PrnStream;
#endif

/** Cassette tape ********************************************/
#ifdef unused
char *CasName    = "DEFAULT.CAS";  /* Tape image file        */
int *CasStream;
#endif

/** Serial port **********************************************/
#ifdef unused
char *ComName    = NULL;           /* Serial redirect. file  */
int *ComIStream;
int *ComOStream;
#endif

/** Kanji font ROM *******************************************/
byte *Kanji;                       /* Kanji ROM 4096x32      */
int  KanLetter;                    /* Current letter index   */
byte KanCount;                     /* Byte count 0..31       */

/** Keyboard and mouse ***************************************/
byte KeyMap[16];                   /* Keyboard map           */
byte Buttons[2];                   /* Mouse button states    */
byte MouseDX[2],MouseDY[2];        /* Mouse offsets          */
byte OldMouseX[2],OldMouseY[2];    /* Old mouse coordinates  */
byte MCount[2];                    /* Mouse nibble counter   */

/** General I/O registers: i8255 *****************************/
I8255 PPI;                         /* i8255 PPI at A8h-ABh   */
byte IOReg;                        /* Storage for AAh port   */

/** Sound hardware: PSG, SCC, OPLL ***************************/
AY8910 PSG;                        /* PSG registers & state  */
YM2413 OPLL;                       /* OPLL registers & state */
SCC  SCChip;                       /* SCC registers & state  */
byte SCCOn[2];                     /* 1 = SCC page active    */

/** Serial I/O hardware: i8251+i8253 *************************/
I8251 SIO;                         /* SIO registers & state  */

/** Real-time clock ******************************************/
byte RTCReg,RTCMode;               /* RTC register numbers   */
byte RTC[4][13];                   /* RTC registers          */

/** Video processor ******************************************/
byte *ChrGen,*ChrTab,*ColTab;      /* VDP tables (screen)    */
byte *SprGen,*SprTab;              /* VDP tables (sprites)   */
int  ChrGenM,ChrTabM,ColTabM;      /* VDP masks (screen)     */
int  SprTabM;                      /* VDP masks (sprites)    */
word VAddr;                        /* VRAM address in VDP    */
byte VKey,PKey,WKey;               /* Status keys for VDP    */
byte FGColor,BGColor;              /* Colors                 */
byte XFGColor,XBGColor;            /* Second set of colors   */
byte ScrMode;                      /* Current screen mode    */
byte VDP[64],VDPStatus[16];        /* VDP registers          */
byte IRQPending;                   /* Pending interrupts     */
int  ScanLine;                     /* Current scanline       */
byte VDPData;                      /* VDP data buffer        */
byte PLatch;                       /* Palette buffer         */
byte ALatch;                       /* Address buffer         */
int  Palette[16];                  /* Current palette        */

/** Places in DiskROM to be patched with ED FE C9 ************/
const word DiskPatches[] = { 0x4010,0x4013,0x4016,0x401C,0x401F,0 };

/** Places in BIOS to be patched with ED FE C9 ***************/
const word BIOSPatches[] = { 0x00E1,0x00E4,0x00E7,0x00EA,0x00ED,0x00F0,0x00F3,0 };

/** Screen Mode Handlers [number of screens + 1] *************/
const void (*RefreshLine[MAXSCREEN+2])(byte Y) =
{
  RefreshLine0,   /* SCR 0:  TEXT 40x24  */
  RefreshLine1,   /* SCR 1:  TEXT 32x24  */
  RefreshLine2,   /* SCR 2:  BLK 256x192 */
  RefreshLine3,   /* SCR 3:  64x48x16    */
  RefreshLine4,   /* SCR 4:  BLK 256x192 */
  RefreshLine5,   /* SCR 5:  256x192x16  */
  RefreshLine6,   /* SCR 6:  512x192x4   */
  RefreshLine7,   /* SCR 7:  512x192x16  */
  RefreshLine8,   /* SCR 8:  256x192x256 */
  0,              /* SCR 9:  NONE        */
  RefreshLine10,  /* SCR 10: YAE 256x192 */
  RefreshLine10,  /* SCR 11: YAE 256x192 */
  RefreshLine12,  /* SCR 12: YJK 256x192 */
  RefreshLineTx80 /* SCR 0:  TEXT 80x24  */
};

/** VDP Address Register Masks *******************************/
const struct { byte R2,R3,R4,R5,M2,M3,M4,M5; } MSK[MAXSCREEN+2] =
{
  { 0x7F,0x00,0x3F,0x00,0x00,0x00,0x00,0x00 }, /* SCR 0:  TEXT 40x24  */
  { 0x7F,0xFF,0x3F,0xFF,0x00,0x00,0x00,0x00 }, /* SCR 1:  TEXT 32x24  */
  { 0x7F,0x80,0x3C,0xFF,0x00,0x7F,0x03,0x00 }, /* SCR 2:  BLK 256x192 */
  { 0x7F,0x00,0x3F,0xFF,0x00,0x00,0x00,0x00 }, /* SCR 3:  64x48x16    */
  { 0x7F,0x80,0x3C,0xFC,0x00,0x7F,0x03,0x03 }, /* SCR 4:  BLK 256x192 */
  { 0x60,0x00,0x00,0xFC,0x1F,0x00,0x00,0x03 }, /* SCR 5:  256x192x16  */
  { 0x60,0x00,0x00,0xFC,0x1F,0x00,0x00,0x03 }, /* SCR 6:  512x192x4   */
  { 0x20,0x00,0x00,0xFC,0x1F,0x00,0x00,0x03 }, /* SCR 7:  512x192x16  */
  { 0x20,0x00,0x00,0xFC,0x1F,0x00,0x00,0x03 }, /* SCR 8:  256x192x256 */
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* SCR 9:  NONE        */
  { 0x20,0x00,0x00,0xFC,0x1F,0x00,0x00,0x03 }, /* SCR 10: YAE 256x192 */
  { 0x20,0x00,0x00,0xFC,0x1F,0x00,0x00,0x03 }, /* SCR 11: YAE 256x192 */
  { 0x20,0x00,0x00,0xFC,0x1F,0x00,0x00,0x03 }, /* SCR 12: YJK 256x192 */
  { 0x7C,0xF8,0x3F,0x00,0x03,0x07,0x00,0x00 }  /* SCR 0:  TEXT 80x24  */
};

/** MegaROM Mapper Names *************************************/
char *ROMNames[MAXMAPPERS+1] = 
{ 
  "GENERIC/8kB","GENERIC/16kB","KONAMI5/8kB",
  "KONAMI4/8kB","ASCII/8kB","ASCII/16kB",
  "GMASTER2/SRAM","UNKNOWN"
};

static byte JoyState;


static unsigned char linebuffer[WIDTH];

char *Disks[2][MAXDISKS+1];     /* Disk names for each drive */

extern void * loc_Malloc(int size);



void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}

void msx_Init(void)
{
  /* Clear everything */
//  CartCount=TypeCount=JoyCount=0;
//  DiskCount[0]=DiskCount[1]=0;

  /* Default disk images */
  Disks[0][1]=Disks[1][1]=0;
  Disks[0][0]=DiskA;
  Disks[1][0]=DiskB;
  /* Terminate disk lists and set initial disk names */
//  if(DiskCount[0]) { Disks[0][DiskCount[0]]=0;DiskA=Disks[0][0]; }
//  if(DiskCount[1]) { Disks[1][DiskCount[1]]=0;DiskB=Disks[1][0]; }

  /* Start fMSX! */
//  if(!InitMachine()) return(1);
//  StartMSX();
//  TrashMSX();
//  TrashMachine();     
  emu_printf("Allocating MEM done");
}


static int hk = 0;
static int k = 0;


void msx_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();   
}



static int SoundRate    = 0;
static int MasterVolume = 200;
static int MasterSwitch = (1<<SND_CHANNELS)-1;
static int LoopFreq     = 25;
static int NoiseGen     = 1;

static struct   
{
  int Type;                       /* Channel type (SND_*)             */
  int Freq;                       /* Channel frequency (Hz)           */
  int Volume;                     /* Channel volume (0..255)          */

  signed char Data[SND_SAMPLESIZE]; /* Wave data (-128..127 each)     */
  int Length;                     /* Wave length in Data              */
  int Rate;                       /* Wave playback rate (or 0Hz)      */
  int Pos;                        /* Wave current position in Data    */

  int Count;                      /* Phase counter                    */
} CH[SND_CHANNELS];




static void handleSoundCmd(unsigned char * Buf, int len)
{
  unsigned char R;
  int J;

  switch(R=Buf[0])
  {
    case 0xFF: MasterVolume=Buf[1];
               MasterSwitch=Buf[2]+Buf[3]*256;
               break;
    case 0xFE: R=Buf[1];
               J=Buf[2]+Buf[3]*256+2;
               /* If everything is correct... */
               if((R<SND_CHANNELS)&&(J<SND_SAMPLESIZE))
                 //if(read(PipeFD[0],Buf,2)==2)
                 //{
                   J-=2;
                   CH[R].Type   = SND_WAVE;
                   CH[R].Rate   = Buf[0]+Buf[1]*256;
                   CH[R].Length = J;
                   CH[R].Count  = 0;
                   CH[R].Pos    = 0;
                   memcpy(CH[R].Data,&Buf[4],J);
                   J=0;
                   //I=read(PipeFD[0],CH[R].Data,J);
                   //if(I>0) J-=I;
                 //}

               break;
    case 0xFC: R=Buf[1];
               if(R<SND_CHANNELS) CH[R].Type=Buf[2];
               break;
    default:   if(R<SND_CHANNELS)
               {
                 CH[R].Volume = Buf[1];
                 CH[R].Freq   = Buf[2]+Buf[3]*256;
               }
               break;
  }
}

static int Wave[SND_BUFSIZE];

static void mixaudio(char * dest, int size) {
  register int J,I,K,L,M,N,L1,L2,A1,A2,V;

  /* Waveform generator */
  for(J=0,M=MasterSwitch;M&&(J<SND_CHANNELS);J++,M>>=1)
    if(CH[J].Freq&&(V=CH[J].Volume)&&(M&1))
      switch(CH[J].Type)
      {
        case SND_NOISE: /* White Noise */
          /* For high frequencies, recompute volume */
          if(CH[J].Freq<=SoundRate) K=0x10000*CH[J].Freq/SoundRate;
          else { V=V*SoundRate/CH[J].Freq;K=0x10000; }
          L1=CH[J].Count;
          V<<=7;
          for(I=0;I<size;I++)
          {
            L1+=K;
            if(L1&0xFFFF0000)
            {
              L1&=0xFFFF;
              if((NoiseGen<<=1)&0x80000000) NoiseGen^=0x08000001;  
            }
            Wave[I]+=NoiseGen&1? V:-V;
          }
          CH[J].Count=L1;
          break;
            
        case SND_WAVE: /* Custom Waveform */
          /* Waveform data must have correct length! */
          if(CH[J].Length<=0) break;
          /* Start counting */
          K  = CH[J].Rate>0? (SoundRate<<15)/CH[J].Freq/CH[J].Rate
                           : (SoundRate<<15)/CH[J].Freq/CH[J].Length;
          L1 = CH[J].Pos%CH[J].Length;
          L2 = CH[J].Count;
          A1 = CH[J].Data[L1]*V;
          /* If expecting interpolation... */
          if(L2<K)
          {
            /* Compute interpolation parameters */
            A2 = CH[J].Data[(L1+1)%CH[J].Length]*V;
            L  = (L2>>15)+1;
            N  = ((K-(L2&0x7FFF))>>15)+1;
          } 
          /* Add waveform to the buffer */  
          for(I=0;I<size;I++)
            if(L2<K)
            {
              /* Interpolate linearly */
              Wave[I]+=A1+L*(A2-A1)/N;
              /* Next waveform step */
              L2+=0x8000;  
              /* Next interpolation step */
              L++;
            }
            else
            {
              L1 = (L1+L2/K)%CH[J].Length;
              L2 = (L2%K)+0x8000;
              A1 = CH[J].Data[L1]*V;
              Wave[I]+=A1;
              /* If expecting interpolation... */
              if(L2<K)
              {
                /* Compute interpolation parameters */
                A2 = CH[J].Data[(L1+1)%CH[J].Length]*V;
                L  = 1;
                N  = ((K-L2)>>15)+1;  
              }
            }
          /* End counting */
          CH[J].Pos   = L1;
          CH[J].Count = L2;
          break;
              
        case SND_MELODIC:  /* Melodic Sound   */
        case SND_TRIANGLE: /* Triangular Wave */
        default:           /* Default Sound   */
          /* Triangular wave has twice less volume */
          if(CH[J].Type==SND_TRIANGLE) V=(V+1)>>1;
          /* Do not allow frequencies that are too high */
          if(CH[J].Freq>=SoundRate/3) break;
          K=0x10000*CH[J].Freq/SoundRate;
          L1=CH[J].Count;
          V<<=7;
          for(I=0;I<size;I++)
          {
            L2=L1+K;
            Wave[I]+=L1&0x8000? (L2&0x8000? V:0):(L2&0x8000? 0:-V);
            L1=L2;
          }
          CH[J].Count=L1;  
          break;
      }

#define AUDIO_CONV(A) (128+(A))

    /* Mix and convert waveforms */   
    for(J=0;J<size;J++)
    {
      I=(Wave[J]*MasterVolume)>>16;
      I=I<-128? -128:I>127? 127:I;
      //Buf[J]=AUDIO_CONV(I);
      *dest++ = I;
      *dest++ = I;
      Wave[J]=0;
    }



}

/** Generate sound of given frequency (Hz) and volume       **/
/** (0..255) via given channel.                             **/
/*************************************************************/
static void DSound(int Channel,int NewFreq,int NewVolume)
{
  unsigned char Buf[4];

  if((Channel<0)||(Channel>=SND_CHANNELS)) return;
  if(!SoundRate||!(MasterSwitch&(1<<Channel))) return;
  if(!NewVolume||!NewFreq) { NewVolume=0;NewFreq=0; }

  if((CH[Channel].Volume!=NewVolume)||(CH[Channel].Freq!=NewFreq))
  {
    CH[Channel].Volume = NewVolume;
    CH[Channel].Freq   = NewFreq;

    Buf[0]=Channel;
    Buf[1]=NewVolume;
    Buf[2]=NewFreq&0xFF;
    Buf[3]=NewFreq>>8;
    handleSoundCmd(Buf, 4);

//    write(PipeFD[1],Buf,4);
  }
}

/** Set master volume (0..255) and turn channels on/off.    **/
/** Each bit in Toggle corresponds to a channel (1=on).     **/
/*************************************************************/
static void DSetChannels(int MVolume,int MSwitch)
{
  unsigned char Buf[4];
  int J;

  if(!SoundRate) return;

  /* Sending new MasterVolume/MasterSwitch */
  Buf[0]=0xFF;
  Buf[1]=MVolume;
  Buf[2]=MSwitch&0xFF;
  Buf[3]=MSwitch>>8;
  handleSoundCmd(Buf, 4);
//  write(PipeFD[1],Buf,4);

  /* Switching channels on/off */
  for(J=0;J<SND_CHANNELS;J++)
    if((MSwitch^MasterSwitch)&(1<<J))
    {
      /* Modifying channel #J */
      Buf[0]=J;

      /* Set volume/frequency */
      if(!(MSwitch&(1<<J))) Buf[1]=Buf[2]=Buf[3]=0;
      else
      {
        Buf[1]=CH[J].Volume;
        Buf[2]=CH[J].Freq&0xFF;
        Buf[3]=CH[J].Freq>>8;
      }
 
      /* Write out */
      handleSoundCmd(Buf, 4);
//      write(PipeFD[1],Buf,4);
    }

  /* Set new MasterSwitch value */
  MasterSwitch=MSwitch;
  MasterVolume=MVolume;
}

/** Set sound type (SND_NOISE/SND_MELODIC) for a given      **/
/** channel.                                                **/
/*************************************************************/
static void DSetSound(int Channel,int NewType)
{
  unsigned char Buf[4];

  if(!SoundRate) return;
  if((Channel<0)||(Channel>=SND_CHANNELS)) return;
  CH[Channel].Type=NewType;

  Buf[0]=0xFC;
  Buf[1]=Channel;
  Buf[2]=NewType&0xFF;
  Buf[3]=NewType>>8;
  handleSoundCmd(Buf, 4);
//  write(PipeFD[1],Buf,4);
}

/** Set waveform for a given channel. The channel will be   **/
/** marked with sound type SND_WAVE. Set Rate=0 if you want **/
/** waveform to be an instrument or set it to the waveform  **/
/** own playback rate.                                      **/
/*************************************************************/
static void DSetWave(int Channel,const signed char *Data,int Length,int Rate)
{
  unsigned char Buf[6+SND_SAMPLESIZE];

  if(!SoundRate) return;
  if((Channel<0)||(Channel>=SND_CHANNELS)) return;
  if((Length<=0)||(Length>SND_SAMPLESIZE)) return;

  Buf[0]=0xFE;
  Buf[1]=Channel;
  Buf[2]=Length&0xFF;
  Buf[3]=Length>>8;
  Buf[4]=Rate&0xFF;
  Buf[5]=Rate>>8;
  memcpy(Buf+6,Data,Length);
  handleSoundCmd(Buf, 6+Length);
//  write(PipeFD[1],Buf,6+Length);
}

static void DDrum(int Type,int Force)
{
}


void msx_Start(char * Cartridge)
{
  emu_printf("init started");
 


  /*** MSX versions: ***/
  static const char *Versions[] = { "MSX","MSX2","MSX2+" };

  /*** Joystick types: ***/
  static const char *JoyTypes[] =
  {
    "nothing","normal joystick",
    "mouse in joystick mode","mouse in real mode"
  };

  /*** CMOS ROM default values: ***/
  static const byte RTCInit[4][13]  =
  {
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    {  0, 0, 0, 0,40,80,15, 4, 4, 0, 0, 0, 0 },
    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  };

  /*** VDP status register states: ***/
  static const byte VDPSInit[16] = { 0x9F,0,0x6C,0,0,0,0,0,0,0,0,0,0,0,0,0 };

  /*** VDP control register states: ***/
  static const byte VDPInit[64]  =
  {
    0x00,0x10,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };

  /*** Initial palette: ***/
  static const byte PalInit[16][3] =
  {
    {0x00,0x00,0x00},{0x00,0x00,0x00},{0x20,0xC0,0x20},{0x60,0xE0,0x60},
    {0x20,0x20,0xE0},{0x40,0x60,0xE0},{0xA0,0x20,0x20},{0x40,0xC0,0xE0},
    {0xE0,0x20,0x20},{0xE0,0x60,0x60},{0xC0,0xC0,0x20},{0xC0,0xC0,0x80},
    {0x20,0x80,0x20},{0xC0,0x40,0xA0},{0xA0,0xA0,0xA0},{0xE0,0xE0,0xE0}
  };

  int *T,I,J,K;
  byte *P;
  word A;
  //FILE *F;

#ifdef SOUND_PRESENT
#ifdef HAS_SND  
  emu_sndInit();
  SndDriver.SetSound    = DSetSound;
  SndDriver.Drum        = DDrum;
  SndDriver.SetChannels = DSetChannels;
  SndDriver.Sound       = DSound;
  SndDriver.SetWave     = DSetWave;

  //int J;
  /* Reset all channels */
  for(J=0;J<SND_CHANNELS;J++)
  {
    CH[J].Type   = SND_MELODIC;
    CH[J].Count  = 0;
    CH[J].Volume = 0;
    CH[J].Freq   = 0;
  }  
  SoundRate = 22050;  
#endif  
#else
#endif
  /* Zero everyting */
#ifdef unused  
  CasStream=PrnStream=ComIStream=ComOStream=NULL;
#endif  
  ROMData[0]=ROMData[1]=NULL;
  FontBuf  = NULL;
  VRAM     = NULL;
  SRAM     = NULL;
  Kanji    = NULL;
  SaveCMOS = 0;
  SaveSRAM = 0;
  ExitNow  = 0;
  CCount   = 0;

  /* Calculate IPeriod from VPeriod/HPeriod */
  if(UPeriod<1) UPeriod=1;
  if(HPeriod<CPU_HPERIOD) HPeriod=CPU_HPERIOD;
  if(VPeriod<CPU_VPERIOD) VPeriod=CPU_VPERIOD;
  CPU.TrapBadOps = Verbose&0x10;
  CPU.IPeriod    = CPU_H240;
  CPU.IAutoReset = 0;

  /* Check parameters for validity */
  if(MSXVersion>2) MSXVersion=2;
  if((RAMPages<(MSXVersion? 8:4))||(RAMPages>256))
    RAMPages=MSXVersion? 8:4;
  if((VRAMPages<(MSXVersion? 8:2))||(VRAMPages>8))
    VRAMPages=MSXVersion? 8:2;

  /* Number of RAM pages should be power of 2 */
  /* Calculate RAMMask=(2^RAMPages)-1 */
  for(J=1;J<RAMPages;J<<=1);
  RAMPages=J;
  RAMMask=J-1;

  /* Number of VRAM pages should be a power of 2 */
  for(J=1;J<VRAMPages;J<<=1);
  VRAMPages=J;

  /* Initialize ROMMasks to zeroes for now */
  ROMMask[0]=ROMMask[1]=0;

  /* Joystick types are in 0..3 range */
  JoyTypeA&=3;
  JoyTypeB&=3;

  /* Allocate 16kB for the empty space (scratch RAM) */
  if(Verbose) 
    emu_printf("Allocating 16kB for empty space...");
  EmptyRAM=emu_MallocI(0x4000); 
  memset(EmptyRAM,NORAM,0x4000);
  Chunks[CCount++]=EmptyRAM;

  /* Reset memory map to the empty space */
  for(I=0;I<4;I++)
    for(J=0;J<4;J++)
      for(K=0;K<8;K++)
        MemMap[I][J][K]=EmptyRAM;

  /* Allocate VRAMPages*16kB for VRAM */
  /* 128k */
  if(Verbose) {
    emu_printi(VRAMPages*16);
    emu_printf("kB for VRAM...");
  }
  VRAM=emu_MallocI(VRAMPages*0x4000);
  memset(VRAM,0x00,VRAMPages*0x4000);
  Chunks[CCount++]=VRAM;

  /* Allocate RAMPages*16kB for RAM */
  if(Verbose) {
    emu_printi(RAMPages);
    emu_printf("x16kB RAM pages...");
  }
  RAMData=emu_Malloc(RAMPages*0x4000);
  memset(RAMData,NORAM,RAMPages*0x4000);
  Chunks[CCount++]=RAMData;


  if(Verbose) {
    emu_printf("Loading ROMs for");
    emu_printf(Versions[MSXVersion]);
  } 


  /* Open/load system ROM(s) */
  switch(MSXVersion)
  {
    case 0:
      if(Verbose) emu_printf("Opening MSX.ROM...");
      P=LoadROM("roms/MSX.ROM",0x8000,0);
      if(!P) return(0);
      MemMap[0][0][0]=P;
      MemMap[0][0][1]=P+0x2000;
      MemMap[0][0][2]=P+0x4000;
      MemMap[0][0][3]=P+0x6000;
      break;

    case 1:
      if(Verbose) emu_printf("Opening MSX2.ROM...");
      P=LoadROM("roms/MSX2.ROM",0x8000,0);
      if(!P) return(0);
      MemMap[0][0][0]=P;
      MemMap[0][0][1]=P+0x2000;
      MemMap[0][0][2]=P+0x4000;
      MemMap[0][0][3]=P+0x6000;
      if(Verbose) emu_printf("Opening MSX2EXT.ROM...");
      P=LoadROM("roms/MSX2EXT.ROM",0x4000,0);
      if(!P) return(0);
      MemMap[3][1][0]=P;
      MemMap[3][1][1]=P+0x2000;
      break;

    case 2:
      if(Verbose) emu_printf("Opening MSX2P.ROM...");
      P=LoadROM("roms/MSX2P.ROM",0x8000,0);
      if(!P) return(0);
      MemMap[0][0][0]=P;
      MemMap[0][0][1]=P+0x2000;
      MemMap[0][0][2]=P+0x4000;
      MemMap[0][0][3]=P+0x6000;
      if(Verbose) emu_printf("Opening MSX2PEXT.ROM...");
      P=LoadROM("roms/MSX2PEXT.ROM",0x4000,0);
      if(!P) return(0);
      MemMap[3][1][0]=P;
      MemMap[3][1][1]=P+0x2000;
      break;
  }

  /* Try loading DiskROM */
  if(P=LoadROM("roms/DISK.ROM",0x4000,0))
  {
    if(Verbose) emu_printf("Opening DISK.ROM...OK");
    MemMap[3][1][2]=P;
    MemMap[3][1][3]=P+0x2000;
  }

  /* Apply patches to BIOS */
  if(Verbose) emu_printf("Patching BIOS: ");
  for(J=0;BIOSPatches[J];J++)
  {
    P=MemMap[0][0][0]+BIOSPatches[J];
    P[0]=0xED;P[1]=0xFE;P[2]=0xC9;
  }
  if(Verbose) emu_printf("OK\n");

  /* Apply patches to BDOS */
  if(MemMap[3][1][2]!=EmptyRAM)
  {
    if(Verbose) emu_printf("Patching BDOS: ");
    for(J=0;DiskPatches[J];J++)
    {
      P=MemMap[3][1][2]+DiskPatches[J]-0x4000;
      P[0]=0xED;P[1]=0xFE;P[2]=0xC9;
    }
    if(Verbose) emu_printf("OK\n");
  }

  if(Verbose) emu_printf("Loading other ROMs: ");



  /* Try loading CMOS memory contents */
  if(LoadROM("roms/CMOS.ROM",sizeof(RTC),(byte *)RTC))
  { if(Verbose) emu_printf("CMOS.ROM.."); }
  else memcpy(RTC,RTCInit,sizeof(RTC));

  /* Try loading Kanji alphabet ROM */
  if(Kanji=LoadROM("roms/KANJI.ROM",0x20000,0))
  { if(Verbose) emu_printf("KANJI.ROM.."); }

  /* Try loading RS232 support ROM */
  if(P=LoadROM("roms/RS232.ROM..",0x4000,0))
  {
    if(Verbose) emu_printf("RS232.ROM..");
    MemMap[3][0][2]=P;
    MemMap[3][0][3]=P+0x2000;
  }

  /* Try loading FM-PAC support ROM */
  if(P=LoadROM("roms/FMPAC.ROM",0x4000,0))
  {
    if(Verbose) emu_printf("FMPAC.ROM..");
    MemMap[3][3][2]=P;
    MemMap[3][3][3]=P+0x2000;
  }

  if(Verbose) emu_printf("OK\n");




  /* Loading cartridge into slot A... */
  strcpy(CartA, "CARTA.ROM");
  if (Cartridge != NULL)
    strcpy(CartA, Cartridge);

  LoadCart(CartA,0);
  J=(ROMMask[0]+1)*8192;


  /* Guess mapper is not given */
  if((ROMTypeA>=MAXMAPPERS)&&(J>0x8000))
  {
    ROMTypeA=GuessROM(ROMData[0],J);
    if(Verbose) {
      emu_printf("Cartridge A: Guessed");
      emu_printf(ROMNames[ROMTypeA]);
    }
  }

  /* For Generic/16kB carts, set ROM pages as 0:1:N-2:N-1 */
  if((ROMTypeA==1)&&(J>0x8000))
    SetMegaROM(0,0,1,ROMMask[0]-1,ROMMask[0]);

  /* Loading cartridge into slot B... */
  LoadCart(CartB,1);
  J=(ROMMask[1]+1)*8192;

  /* Guess mapper is not given */
  if((ROMTypeB>=MAXMAPPERS)&&(J>0x8000))
  {
    ROMTypeB=GuessROM(ROMData[1],J);
    if(Verbose) {
      emu_printf("Cartridge B: Guessed");
      emu_printf(ROMNames[ROMTypeB]);
    }
  }


  /* For Generic/16kB carts, set ROM pages as 0:1:N-2:N-1 */
  if((ROMTypeB==1)&&(J>0x8000))
    SetMegaROM(1,0,1,ROMMask[1]-1,ROMMask[1]);

  /* For GameMaster2, allocate and load SRAM */
  if((ROMTypeA==6)||(ROMTypeB==6))
  {
    if(Verbose) emu_printf("Allocating 16kB for GameMaster2 SRAM..."); 
    SRAM=emu_Malloc(0x4000);
    /* Erase SRAM and add it to the list of mallocs */
    memset(SRAM,NORAM,0x4000);
    Chunks[CCount++]=SRAM;
#ifdef unused
    /* Try loading SRAM data form disk */
    if(!(F=fopen("GMASTER2.RAM","rb"))) { 
        emu_printf("failed opening"); 
    }
    else
    {
      if(Verbose) emu_printf("loading GMASTER2.RAM...");
      if(fread(SRAM,1,0x2000,F)!=0x2000) 
      { 
        emu_printf("failed"); 
      }
      else
      {
        /* Mirror GameMaster2 SRAM as needed */
        memcpy(SRAM+0x2000,SRAM+0x1000,0x1000);
        memcpy(SRAM+0x3000,SRAM+0x1000,0x1000);
        memcpy(SRAM+0x1000,SRAM,0x1000);
      }
      /* Done with the file */
      fclose(F);
    }
#endif
  }

  /* Load MSX2-dependent cartridge ROMs: MSXDOS2 and PAINTER */
  if(MSXVersion>0)
  {
    /* Find an empty slot */
    if(MemMap[1][0][2]==EmptyRAM) J=1;
    else if(MemMap[2][0][2]==EmptyRAM) J=2;
         else J=0;

    /* Try loading MSXDOS2 cartridge, if slot */
    /* found and DiskROM present              */
    if(J&&(MemMap[3][1][2]!=EmptyRAM))
    {
      if(J==2) ROMTypeB=1; else ROMTypeA=1;
      if(LoadCart("MSXDOS2.ROM",J-1))
        SetMegaROM(J-1,0,1,ROMMask[J-1]-1,ROMMask[J-1]);
    }

    /* Find an empty slot */
    if(MemMap[1][0][2]==EmptyRAM) J=1;
    else if(MemMap[2][0][2]==EmptyRAM) J=2;
         else J=0;

    /* Try loading PAINTER ROM if slot found */
    if(J) LoadCart("PAINTER.ROM",J-1);
  }

  /* Try loading font */
  if(Verbose) {
    emu_printf("Loading font...");
    emu_printf(FontName);
  }
  FontBuf=LoadROM(FontName,0x800,0);

#ifdef unused  
  /* Open stream for a printer */
  if(!PrnName) PrnStream=stdout;
  else
  {
    if(Verbose) printf("Redirecting printer output to %s...",PrnName);
    if(!(PrnStream=fopen(PrnName,"wb"))) PrnStream=stdout;
  }
#endif

#ifdef unused  
  /* Open streams for serial IO */
  if(!ComName) { ComIStream=stdin;ComOStream=stdout; }
  else
  {
    if(Verbose) printf("Redirecting serial I/O to %s...",ComName);
    if(!(ComOStream=ComIStream=fopen(ComName,"r+b")))
    { ComIStream=stdin;ComOStream=stdout; }
  }
#endif

  /* Open disk images */
#ifdef DISK
  if(ChangeDisk(0,DiskA)) 
    if(Verbose) {
      emu_printf("Inserting floppy into drive A");
      emu_printf(DiskA);
    }  
  if(ChangeDisk(1,DiskB))
    if(Verbose) {
      emu_printf("Inserting floppy into drive B");
      emu_printf(DiskB);
    }
#endif /* DISK */

#ifdef unused  
  /* Open casette image */
  if(CasName)
    if(CasStream=fopen(CasName,"r+b"))
      if(Verbose) {
        emu_printf("Inserting tape");
        emu_printf(CasName);
      }
#endif

  if(Verbose)
  {
    emu_printf("Attaching joystick port A");
    emu_printf(JoyTypes[JoyTypeA]);
    emu_printf("Attaching joystick port B");
    emu_printf(JoyTypes[JoyTypeB]);
    emu_printf("Initializing memory mappers...");
  }

  for(J=0;J<4;J++)
  {
    EnWrite[J]=0;                      /* Write protect ON for all slots */
    PSL[J]=SSL[J]=0;                   /* PSL=0:0:0:0, SSL=0:0:0:0       */
    MemMap[3][2][J*2]   = RAMData+(3-J)*0x4000;        /* RAMMap=3:2:1:0 */
    MemMap[3][2][J*2+1] = MemMap[3][2][J*2]+0x2000;
    RAMMapper[J]        = 3-J;
    RAM[J*2]            = MemMap[0][0][J*2];           /* Setting RAM    */
    RAM[J*2+1]          = MemMap[0][0][J*2+1];
  }

  if(Verbose) emu_printf("Initializing VDP, PSG, OPLL, SCC, and CPU...");

  /* Initialize palette */
  for(J=0;J<16;J++)
  {
    Palette[J]=RGB2INT(PalInit[J][0],PalInit[J][1],PalInit[J][2]);
    SetColor(J,PalInit[J][0],PalInit[J][1],PalInit[J][2]);
  }

  /* Reset mouse coordinates/counters */
  for(J=0;J<2;J++)
    Buttons[J]=MouseDX[J]=MouseDY[J]=OldMouseX[J]=OldMouseY[J]=MCount[J]=0;

  /* Initialize sound logging */
  //InitMIDI("SndName");

  /* Reset sound chips */
  Reset8910(&PSG,0);
  ResetSCC(&SCChip,AY8910_CHANNELS);
  Reset2413(&OPLL,AY8910_CHANNELS);
  Sync8910(&PSG,AY8910_SYNC);
  SyncSCC(&SCChip,SCC_SYNC);
  Sync2413(&OPLL,YM2413_SYNC);

  /* Reset serial I/O */
#ifdef unused  
  Reset8251(&SIO,ComIStream,ComOStream);
#else
  Reset8251(&SIO,NULL,NULL);
#endif  
  /* Reset PPI chips and slot selectors */
  Reset8255(&PPI);
  PPI.Rout[0]=PSLReg=0x00;
  PPI.Rout[2]=IOReg=0x00;
  SSLReg=0x00;

  memcpy(VDP,VDPInit,sizeof(VDP));
  memcpy(VDPStatus,VDPSInit,sizeof(VDPStatus));
  memset(KeyMap,0xFF,16);               /* Keyboard         */
  IRQPending=0x00;                      /* No IRQs pending  */
  SCCOn[0]=SCCOn[1]=0;                  /* SCCs off for now */
  RTCReg=RTCMode=0;                     /* Clock registers  */
  KanCount=0;KanLetter=0;               /* Kanji extension  */
  ChrTab=ColTab=ChrGen=VRAM;            /* VDP tables       */
  SprTab=SprGen=VRAM;
  ChrTabM=ColTabM=ChrGenM=SprTabM=~0;   /* VDP addr. masks  */
  VPAGE=VRAM;                           /* VRAM page        */
  FGColor=BGColor=XFGColor=XBGColor=0;  /* VDP colors       */
  ScrMode=0;                            /* Screen mode      */
  VKey=PKey=1;WKey=0;                   /* VDP keys         */
  VAddr=0x0000;                         /* VRAM access addr */
  ScanLine=0;                           /* Current scanline */
  VDPData=NORAM;                        /* VDP data buffer  */
  UseFont=0;                            /* Extern. font use */   

  /* Set "V9958" VDP version for MSX2+ */
  if(MSXVersion>=2) VDPStatus[1]|=0x04;

  /* Reset CPU */
  ResetZ80(&CPU);

  /* Done with initialization */
  if(Verbose)
  {
    emu_printi(HPeriod);
    emu_printf("CPU cycles per HBlank");
    emu_printi(VPeriod);
    emu_printf("CPU cycles per VBlank");
    emu_printi(VPeriod/HPeriod);
    emu_printf("scanlines");
  }


  /* Start execution of the code */
  if(Verbose) emu_printf("RUNNING ROM CODE...\n");
  A=RunZ80(&CPU);


  emu_printf("init done");
}

struct { byte Pos,Mask; } Keys[] =
{
  {0,0x01}, //'0'
  {0,0x02}, //'1'
  {0,0x04}, //'2'
  {0,0x08}, //'3'
  {0,0x10}, //'4'
  {0,0x20}, //'5'
  {0,0x40}, //'6'
  {0,0x80}, //'7'
  {1,0x01}, //'8'
  {1,0x02}, //'9'

  {4,0x40}, //'q'
  {5,0x10}, //'w'
  {3,0x04}, //'e'
  {4,0x80}, //'r'
  {5,0x02}, //'t'
  {5,0x40}, //'y'
  {5,0x04}, //'u'
  {3,0x40}, //'i'
  {4,0x10}, //'o'
  {4,0x20}, //'p'

  {2,0x40}, //'a'
  {5,0x01}, //'s'
  {3,0x02}, //'d'
  {3,0x08}, //'f'
  {3,0x10}, //'g'
  {3,0x20}, //'h'
  {3,0x80}, //'j'
  {4,0x01}, //'k'
  {4,0x02}, //'l'
  {7,0x80}, //'return'   

  {2,0x04}, //','
  {5,0x80}, //'z'
  {5,0x20}, //'x'
  {3,0x01}, //'c'
  {5,0x08}, //'v'
  {2,0x80}, //'b'
  {4,0x08}, //'n'
  {4,0x04}, //'m'
  {7,0x20}, // 'BackSpace'
  {8,0x01}  //' '
};  

void msx_Step(void) {
  JoyState = 0;
  if (k & MASK_JOY2_DOWN)  JoyState|=0x02;
  if (k & MASK_JOY2_UP)    JoyState|=0x01;
  if (k & MASK_JOY2_LEFT)  JoyState|=0x04;
  if (k & MASK_JOY2_RIGHT) JoyState|=0x08;
  if (k & MASK_JOY2_BTN) JoyState|=0x10; 
  if (k & MASK_KEY_USER2) JoyState|=0x20;  

  if (hk != 0) {
    emu_printh(hk);
    KeyMap[Keys[hk-1].Pos] &=~ Keys[hk-1].Mask;   
  }
  else  {
    memset(KeyMap,0xFF,16);
  }

  RunZ80(&CPU);

  emu_DrawVsync();    
}






void SND_Process(void *stream, int len) {
  //psg_update(stream, 0, len); 
  mixaudio(stream, len); 
} 





void SetColor(byte N,byte R,byte G,byte B)
{
  XPal[N] = N; 
  emu_SetPaletteEntry(R,G,B,N);
}

void Keyboard(void)
{
}

byte Joystick(register byte N) { return(JoyState); }

/** Mouse() **************************************************/
/** Query coordinates of a mouse connected to port N.       **/
/** Returns F2.F1.Y.Y.Y.Y.Y.Y.Y.Y.X.X.X.X.X.X.X.X.          **/
/*************************************************************/
int Mouse(register byte N)
{
  return(0);
}






/** ClearLine() **********************************************/
/** Clear 256 pixels from P with color C.                   **/
/*************************************************************/
static void ClearLine(register pixel *P,register pixel C)
{
  register int J;

  for(J=0;J<256;J++) P[J]=C;
}

/** YJKColor() ***********************************************/
/** Given a color in YJK format, return the corresponding   **/
/** palette entry.                                          **/
/*************************************************************/
static pixel YJKColor(register int Y,register int J,register int K)
{
  register int R,G,B;
    
  R=Y+J;
  G=Y+K;
  B=((5*Y-2*J-K)/4);

  R=R<0? 0:R>31? 31:R;
  G=G<0? 0:G>31? 31:G;
  B=B<0? 0:B>31? 31:B;

  return(BPal[(R&0x1C)|((G&0x1C)<<3)|(B>>3)]);
}

/** RefreshBorder() ******************************************/
/** This function is called from RefreshLine#() to refresh  **/
/** the screen border. It returns a pointer to the start of **/
/** scanline Y in FB or 0 if scanline is beyond FB.         **/
/*************************************************************/
pixel *RefreshBorder(register byte Y,register pixel C)
{
  register pixel *P;
  register int H;

  /* First line number in the buffer */
  if(!Y) FirstLine=(ScanLines212? 8:18)+VAdjust;

  /* Return 0 if we've run out of the screen buffer due to overscan */
  if(Y+FirstLine>=HEIGHT) return(0);

  /* Set up the transparent color */
  XPal[0]=(!BGColor||SolidColor0)? XPal0:XPal[BGColor];

  /* Start of the buffer */
  P=(pixel *)&linebuffer[0];

  /* Paint top of the screen */
  //if(!Y) for(H=WIDTH*FirstLine-1;H>=0;H--) P[H]=C;

  /* Start of the line */
  //P+=WIDTH*(FirstLine+Y);

  /* Paint left/right borders */
  for(H=(WIDTH-256)/2+HAdjust;H>0;H--) P[H-1]=C;
  for(H=(WIDTH-256)/2-HAdjust;H>0;H--) P[WIDTH-H]=C;

  /* Paint bottom of the screen */
  //H=ScanLines212? 212:192;
  //if(Y==H-1) for(H=WIDTH*(HEIGHT-H-FirstLine+1)-1;H>=WIDTH;H--) P[H]=C;

  /* Return pointer to the scanline in FB */
  return(P+(WIDTH-256)/2+HAdjust);
}

/** Sprites() ************************************************/
/** This function is called from RefreshLine#() to refresh  **/
/** sprites in SCREENs 1-3.                                 **/
/*************************************************************/
void Sprites(register byte Y,register pixel *Line)
{
  register pixel *P,C;
  register byte H,*PT,*AT;
  register unsigned int M;
  register int L,K;

  /* Assign initial values before counting */
  H=Sprites16x16? 16:8;
  C=0;M=0;L=0;
  AT=SprTab-4;
  Y+=VScroll;

  /* Count displayed sprites */
  do
  {
    M<<=1;AT+=4;L++;    /* Iterating through SprTab      */
    K=AT[0];            /* K = sprite Y coordinate       */
    if(K==208) break;   /* Iteration terminates if Y=208 */
    if(K>256-H) K-=256; /* Y coordinate may be negative  */

    /* Mark all valid sprites with 1s, break at MAXSPRITE1 sprites */
    if((Y>K)&&(Y<=K+H)) { M|=1;C++;if(C==MAXSPRITE1) break; }
  }
  while(L<32);

  /* Draw all marked sprites */
  for(;M;M>>=1,AT-=4)
    if(M&1)
    {
      C=AT[3];                  /* C = sprite attributes */
      L=C&0x80? AT[1]-32:AT[1]; /* Sprite may be shifted left by 32 */
      C&=0x0F;                  /* C = sprite color */

      if((L<256)&&(L>-H)&&C)
      {
        K=AT[0];                /* K = sprite Y coordinate */
        if(K>256-H) K-=256;     /* Y coordinate may be negative */

        P=Line+L;
        PT=SprGen+((int)(H>8? AT[2]&0xFC:AT[2])<<3)+Y-K-1;
        C=XPal[C];

        /* Mask 1: clip left sprite boundary */
        K=L>=0? 0x0FFFF:(0x10000>>-L)-1;
        /* Mask 2: clip right sprite boundary */
        if(L>256-H) K^=((0x00200>>(H-8))<<(L-257+H))-1;
        /* Get and clip the sprite data */
        K&=((int)PT[0]<<8)|(H>8? PT[16]:0x00);

        /* Draw left 8 pixels of the sprite */
        if(K&0xFF00)
        {
          if(K&0x8000) P[0]=C;if(K&0x4000) P[1]=C;
          if(K&0x2000) P[2]=C;if(K&0x1000) P[3]=C;
          if(K&0x0800) P[4]=C;if(K&0x0400) P[5]=C;
          if(K&0x0200) P[6]=C;if(K&0x0100) P[7]=C;
        }

        /* Draw right 8 pixels of the sprite */
        if(K&0x00FF)
        {
          if(K&0x0080) P[8]=C; if(K&0x0040) P[9]=C;
          if(K&0x0020) P[10]=C;if(K&0x0010) P[11]=C;
          if(K&0x0008) P[12]=C;if(K&0x0004) P[13]=C;
          if(K&0x0002) P[14]=C;if(K&0x0001) P[15]=C;
        }
      }
    }
}

/** ColorSprites() *******************************************/
/** This function is called from RefreshLine#() to refresh  **/
/** color sprites in SCREENs 4-8. The result is returned in **/
/** ZBuf, whose size must be 304 bytes (32+256+16).         **/
/*************************************************************/
void ColorSprites(register byte Y,byte *ZBuf)
{
  register byte C,H,J,OrThem;
  register byte *P,*PT,*AT;
  register int L,K;
  register unsigned int M;

  /* Clear ZBuffer and exit if sprites are off */
  memset(ZBuf+32,0,256);
  if(SpritesOFF) return;

  /* Assign initial values before counting */
  H=Sprites16x16? 16:8;
  C=0;M=0;L=0;
  AT=SprTab-4;
  OrThem=0x00;

  /* Count displayed sprites */
  do
  {
    M<<=1;AT+=4;L++;          /* Iterating through SprTab      */
    K=AT[0];                  /* Read Y from SprTab            */
    if(K==216) break;         /* Iteration terminates if Y=216 */
    K=(byte)(K-VScroll);      /* Sprite's actual Y coordinate  */
    if(K>256-H) K-=256;       /* Y coordinate may be negative  */

    /* Mark all valid sprites with 1s, break at MAXSPRITE2 sprites */
    if((Y>K)&&(Y<=K+H)) { M|=1;C++;if(C==MAXSPRITE2) break; }
  }
  while(L<32);

  /* Draw all marked sprites */
  for(;M;M>>=1,AT-=4)
    if(M&1)
    {
      K=(byte)(AT[0]-VScroll); /* K = sprite Y coordinate */
      if(K>256-H) K-=256;      /* Y coordinate may be negative */

      J=Y-K-1;
      C=SprTab[-0x0200+((AT-SprTab)<<2)+J];
      OrThem|=C&0x40;

      if(C&0x0F)
      {
        PT=SprGen+((int)(H>8? AT[2]&0xFC:AT[2])<<3)+J;
        P=ZBuf+AT[1]+(C&0x80? 0:32);
        C&=0x0F;
        J=PT[0];

        if(OrThem&0x20)
        {
          if(J&0x80) P[0]|=C;if(J&0x40) P[1]|=C;
          if(J&0x20) P[2]|=C;if(J&0x10) P[3]|=C;
          if(J&0x08) P[4]|=C;if(J&0x04) P[5]|=C;
          if(J&0x02) P[6]|=C;if(J&0x01) P[7]|=C;
          if(H>8)
          {
            J=PT[16];
            if(J&0x80) P[8]|=C; if(J&0x40) P[9]|=C;
            if(J&0x20) P[10]|=C;if(J&0x10) P[11]|=C;
            if(J&0x08) P[12]|=C;if(J&0x04) P[13]|=C;
            if(J&0x02) P[14]|=C;if(J&0x01) P[15]|=C;
          }
        }
        else
        {
          if(J&0x80) P[0]=C;if(J&0x40) P[1]=C;
          if(J&0x20) P[2]=C;if(J&0x10) P[3]=C;
          if(J&0x08) P[4]=C;if(J&0x04) P[5]=C;
          if(J&0x02) P[6]=C;if(J&0x01) P[7]=C;
          if(H>8)
          {
            J=PT[16];
            if(J&0x80) P[8]=C; if(J&0x40) P[9]=C;
            if(J&0x20) P[10]=C;if(J&0x10) P[11]=C;
            if(J&0x08) P[12]=C;if(J&0x04) P[13]=C;
            if(J&0x02) P[14]=C;if(J&0x01) P[15]=C;
          }
        }
      }

      /* Update overlapping flag */
      OrThem>>=1;
    }
}

/** RefreshLineF() *******************************************/
/** Dummy refresh function called for non-existing screens. **/
/*************************************************************/
static void RefreshLineF(register byte Y)
{
  register pixel *P;

  //if(Verbose>1)
  //  printf
  //  (
  //    "ScrMODE %d: ChrTab=%X ChrGen=%X ColTab=%X SprTab=%X SprGen=%X\n",
  //    ScrMode,ChrTab-VRAM,ChrGen-VRAM,ColTab-VRAM,SprTab-VRAM,SprGen-VRAM
  //  );

  P=RefreshBorder(Y,XPal[BGColor]);
  if(P) ClearLine(P,XPal[BGColor]);
}

/** RefreshLine0() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN0.                 **/
/*************************************************************/
static void RefreshLine0(register byte Y)
{
  register pixel *P,FC,BC;
  register byte X,*T,*G;

  BC=XPal[BGColor];
  P=RefreshBorder(Y,BC);
  if(!P) return;

  if(!ScreenON) ClearLine(P,BC);
  else
  {
    P[0]=P[1]=P[2]=P[3]=P[4]=P[5]=P[6]=P[7]=P[8]=BC;

    G=(UseFont&&FontBuf? FontBuf:ChrGen)+((Y+VScroll)&0x07);
    T=ChrTab+40*(Y>>3);
    FC=XPal[FGColor];
    P+=9;

    for(X=0;X<40;X++,T++,P+=6)
    {
      Y=G[(int)*T<<3];
      P[0]=Y&0x80? FC:BC;P[1]=Y&0x40? FC:BC;
      P[2]=Y&0x20? FC:BC;P[3]=Y&0x10? FC:BC;
      P[4]=Y&0x08? FC:BC;P[5]=Y&0x04? FC:BC;
    }

    P[0]=P[1]=P[2]=P[3]=P[4]=P[5]=P[6]=BC;
  }
}

/** RefreshLine1() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN1, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine1(register byte Y)
{
  register pixel *P,FC,BC;
  register byte K,X,*T,*G;

  P=RefreshBorder(Y,XPal[BGColor]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,XPal[BGColor]);
  else
  {
    Y+=VScroll;
    G=(UseFont&&FontBuf? FontBuf:ChrGen)+(Y&0x07);
    T=ChrTab+((int)(Y&0xF8)<<2);

    for(X=0;X<32;X++,T++,P+=8)
    {
      K=ColTab[*T>>3];
      FC=XPal[K>>4];
      BC=XPal[K&0x0F];
      K=G[(int)*T<<3];
      P[0]=K&0x80? FC:BC;P[1]=K&0x40? FC:BC;
      P[2]=K&0x20? FC:BC;P[3]=K&0x10? FC:BC;
      P[4]=K&0x08? FC:BC;P[5]=K&0x04? FC:BC;
      P[6]=K&0x02? FC:BC;P[7]=K&0x01? FC:BC;
    }

    if(!SpritesOFF) Sprites(Y,P-256);
  }
}

/** RefreshLine2() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN2, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine2(register byte Y)
{
  register pixel *P,FC,BC;
  register byte K,X,*T;
  register int I,J;

  P=RefreshBorder(Y,XPal[BGColor]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,XPal[BGColor]);
  else
  {
    Y+=VScroll;
    T=ChrTab+((int)(Y&0xF8)<<2);
    I=((int)(Y&0xC0)<<5)+(Y&0x07);

    for(X=0;X<32;X++,T++,P+=8)
    {
      J=(int)*T<<3;
      K=ColTab[(I+J)&ColTabM];
      FC=XPal[K>>4];
      BC=XPal[K&0x0F];
      K=ChrGen[(I+J)&ChrGenM];
      P[0]=K&0x80? FC:BC;P[1]=K&0x40? FC:BC;
      P[2]=K&0x20? FC:BC;P[3]=K&0x10? FC:BC;
      P[4]=K&0x08? FC:BC;P[5]=K&0x04? FC:BC;
      P[6]=K&0x02? FC:BC;P[7]=K&0x01? FC:BC;
    }

    if(!SpritesOFF) Sprites(Y,P-256);
  }
}

/** RefreshLine3() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN3, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine3(register byte Y)
{
  register pixel *P;
  register byte X,K,*T,*G;

  P=RefreshBorder(Y,XPal[BGColor]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,XPal[BGColor]);
  else
  {
    Y+=VScroll;
    T=ChrTab+((int)(Y&0xF8)<<2);
    G=ChrGen+((Y&0x1C)>>2);

    for(X=0;X<32;X++,T++,P+=8)
    {
      K=G[(int)*T<<3];
      P[0]=P[1]=P[2]=P[3]=XPal[K>>4];
      P[4]=P[5]=P[6]=P[7]=XPal[K&0x0F];
    }

    if(!SpritesOFF) Sprites(Y,P-256);
  }
}

/** RefreshLine4() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN4, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine4(register byte Y)
{
  register pixel *P,FC,BC;
  register byte K,X,C,*T,*R;
  register int I,J;
  byte ZBuf[304];

  P=RefreshBorder(Y,XPal[BGColor]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,XPal[BGColor]);
  else
  {
    ColorSprites(Y,ZBuf);
    R=ZBuf+32;
    Y+=VScroll;
    T=ChrTab+((int)(Y&0xF8)<<2);
    I=((int)(Y&0xC0)<<5)+(Y&0x07);

    for(X=0;X<32;X++,R+=8,P+=8,T++)
    {
      J=(int)*T<<3;
      K=ColTab[(I+J)&ColTabM];
      FC=XPal[K>>4];
      BC=XPal[K&0x0F];
      K=ChrGen[(I+J)&ChrGenM];

      C=R[0];P[0]=C? XPal[C]:(K&0x80)? FC:BC;
      C=R[1];P[1]=C? XPal[C]:(K&0x40)? FC:BC;
      C=R[2];P[2]=C? XPal[C]:(K&0x20)? FC:BC;
      C=R[3];P[3]=C? XPal[C]:(K&0x10)? FC:BC;
      C=R[4];P[4]=C? XPal[C]:(K&0x08)? FC:BC;
      C=R[5];P[5]=C? XPal[C]:(K&0x04)? FC:BC;
      C=R[6];P[6]=C? XPal[C]:(K&0x02)? FC:BC;
      C=R[7];P[7]=C? XPal[C]:(K&0x01)? FC:BC;
    }
  }
}

/** RefreshLine5() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN5, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine5(register byte Y)
{
  register pixel *P;
  register byte I,X,*T,*R;
  byte ZBuf[304];

  P=RefreshBorder(Y,XPal[BGColor]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,XPal[BGColor]);
  else
  {
    ColorSprites(Y,ZBuf);
    R=ZBuf+32;
    T=ChrTab+(((int)(Y+VScroll)<<7)&ChrTabM&0x7FFF);

    for(X=0;X<16;X++,R+=16,P+=16,T+=8)
    {
      I=R[0];P[0]=XPal[I? I:T[0]>>4];
      I=R[1];P[1]=XPal[I? I:T[0]&0x0F];
      I=R[2];P[2]=XPal[I? I:T[1]>>4];
      I=R[3];P[3]=XPal[I? I:T[1]&0x0F];
      I=R[4];P[4]=XPal[I? I:T[2]>>4];
      I=R[5];P[5]=XPal[I? I:T[2]&0x0F];
      I=R[6];P[6]=XPal[I? I:T[3]>>4];
      I=R[7];P[7]=XPal[I? I:T[3]&0x0F];
      I=R[8];P[8]=XPal[I? I:T[4]>>4];
      I=R[9];P[9]=XPal[I? I:T[4]&0x0F];
      I=R[10];P[10]=XPal[I? I:T[5]>>4];
      I=R[11];P[11]=XPal[I? I:T[5]&0x0F];
      I=R[12];P[12]=XPal[I? I:T[6]>>4];
      I=R[13];P[13]=XPal[I? I:T[6]&0x0F];
      I=R[14];P[14]=XPal[I? I:T[7]>>4];
      I=R[15];P[15]=XPal[I? I:T[7]&0x0F];
    }
  }
}

/** RefreshLine8() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN8, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine8(register byte Y)
{
  static byte SprToScr[16] =
  {
    0x00,0x02,0x10,0x12,0x80,0x82,0x90,0x92,
    0x49,0x4B,0x59,0x5B,0xC9,0xCB,0xD9,0xDB
  };
  register pixel *P;
  register byte C,X,*T,*R;
  byte ZBuf[304];

  P=RefreshBorder(Y,BPal[VDP[7]]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,BPal[VDP[7]]);
  else
  {
    ColorSprites(Y,ZBuf);
    R=ZBuf+32;
    T=ChrTab+(((int)(Y+VScroll)<<8)&ChrTabM&0xFFFF);

    for(X=0;X<32;X++,T+=8,R+=8,P+=8)
    {
      C=R[0];P[0]=BPal[C? SprToScr[C]:T[0]];
      C=R[1];P[1]=BPal[C? SprToScr[C]:T[1]];
      C=R[2];P[2]=BPal[C? SprToScr[C]:T[2]];
      C=R[3];P[3]=BPal[C? SprToScr[C]:T[3]];
      C=R[4];P[4]=BPal[C? SprToScr[C]:T[4]];
      C=R[5];P[5]=BPal[C? SprToScr[C]:T[5]];
      C=R[6];P[6]=BPal[C? SprToScr[C]:T[6]];
      C=R[7];P[7]=BPal[C? SprToScr[C]:T[7]];
    }
  }
}

/** RefreshLine10() ******************************************/
/** Refresh line Y (0..191/211) of SCREEN10/11, including   **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine10(register byte Y)
{
  register pixel *P;
  register byte C,X,*T,*R;
  register int J,K;
  byte ZBuf[304];

  P=RefreshBorder(Y,BPal[VDP[7]]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,BPal[VDP[7]]);
  else
  {
    ColorSprites(Y,ZBuf);
    R=ZBuf+32;
    T=ChrTab+(((int)(Y+VScroll)<<8)&ChrTabM&0xFFFF);

    /* Draw first 4 pixels */
    C=R[0];P[0]=C? XPal[C]:BPal[VDP[7]];
    C=R[1];P[1]=C? XPal[C]:BPal[VDP[7]];
    C=R[2];P[2]=C? XPal[C]:BPal[VDP[7]];
    C=R[3];P[3]=C? XPal[C]:BPal[VDP[7]];
    R+=4;P+=4;

    for(X=0;X<63;X++,T+=4,R+=4,P+=4)
    {
      K=(T[0]&0x07)|((T[1]&0x07)<<3);
      if(K&0x20) K-=64;
      J=(T[2]&0x07)|((T[3]&0x07)<<3);
      if(J&0x20) J-=64;

      C=R[0];Y=T[0]>>3;P[0]=C? XPal[C]:Y&1? XPal[Y>>1]:YJKColor(Y,J,K);
      C=R[1];Y=T[1]>>3;P[1]=C? XPal[C]:Y&1? XPal[Y>>1]:YJKColor(Y,J,K);
      C=R[2];Y=T[2]>>3;P[2]=C? XPal[C]:Y&1? XPal[Y>>1]:YJKColor(Y,J,K);
      C=R[3];Y=T[3]>>3;P[3]=C? XPal[C]:Y&1? XPal[Y>>1]:YJKColor(Y,J,K);
    }
  }
}

/** RefreshLine12() ******************************************/
/** Refresh line Y (0..191/211) of SCREEN12, including      **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine12(register byte Y)
{
  register pixel *P;
  register byte C,X,*T,*R;
  register int J,K;
  byte ZBuf[304];

  P=RefreshBorder(Y,BPal[VDP[7]]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,BPal[VDP[7]]);
  else
  {
    ColorSprites(Y,ZBuf);
    R=ZBuf+32;
    T=ChrTab+(((int)(Y+VScroll)<<8)&ChrTabM&0xFFFF);

    if(HScroll512&&(HScroll>255)) T=(byte *)((int)T^0x10000);
    T+=HScroll&0xFC;

    /* Draw first 4 pixels */
    C=R[0];P[0]=C? XPal[C]:BPal[VDP[7]];
    C=R[1];P[1]=C? XPal[C]:BPal[VDP[7]];
    C=R[2];P[2]=C? XPal[C]:BPal[VDP[7]];
    C=R[3];P[3]=C? XPal[C]:BPal[VDP[7]];
    R+=4;P+=4;

    for(X=1;X<64;X++,T+=4,R+=4,P+=4)
    {
      K=(T[0]&0x07)|((T[1]&0x07)<<3);
      if(K&0x20) K-=64;
      J=(T[2]&0x07)|((T[3]&0x07)<<3);
      if(J&0x20) J-=64;

      C=R[0];P[0]=C? XPal[C]:YJKColor(T[0]>>3,J,K);
      C=R[1];P[1]=C? XPal[C]:YJKColor(T[1]>>3,J,K);
      C=R[2];P[2]=C? XPal[C]:YJKColor(T[2]>>3,J,K);
      C=R[3];P[3]=C? XPal[C]:YJKColor(T[3]>>3,J,K);
    }
  }
}

//#ifdef NARROW

/** RefreshLine6() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN6, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine6(register byte Y)
{
  register pixel *P;
  register byte X,*T,*R,C;
  byte ZBuf[304];

  P=RefreshBorder(Y,XPal[BGColor&0x03]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,XPal[BGColor&0x03]);
  else
  {
    ColorSprites(Y,ZBuf);
    R=ZBuf+32;
    T=ChrTab+(((int)(Y+VScroll)<<7)&ChrTabM&0x7FFF);

    for(X=0;X<32;X++)
    {
      C=R[0];P[0]=XPal[C? C:T[0]>>6];
      C=R[1];P[1]=XPal[C? C:(T[0]>>2)&0x03];
      C=R[2];P[2]=XPal[C? C:T[1]>>6];
      C=R[3];P[3]=XPal[C? C:(T[1]>>2)&0x03];
      C=R[4];P[4]=XPal[C? C:T[2]>>6];
      C=R[5];P[5]=XPal[C? C:(T[2]>>2)&0x03];
      C=R[6];P[6]=XPal[C? C:T[3]>>6];
      C=R[7];P[7]=XPal[C? C:(T[3]>>2)&0x03];
      R+=8;P+=8;T+=4;
    }
  }
}
  
/** RefreshLine7() *******************************************/
/** Refresh line Y (0..191/211) of SCREEN7, including       **/
/** sprites in this line.                                   **/
/*************************************************************/
static void RefreshLine7(register byte Y)
{
  register pixel *P;
  register byte C,X,*T,*R;
  byte ZBuf[304];

  P=RefreshBorder(Y,XPal[BGColor]);
  if(!P) return;

  if(!ScreenON) ClearLine(P,XPal[BGColor]);
  else
  {
    ColorSprites(Y,ZBuf);
    R=ZBuf+32;
    T=ChrTab+(((int)(Y+VScroll)<<8)&ChrTabM&0xFFFF);

    for(X=0;X<32;X++)
    {
      C=R[0];P[0]=XPal[C? C:T[0]>>4];
      C=R[1];P[1]=XPal[C? C:T[1]>>4];
      C=R[2];P[2]=XPal[C? C:T[2]>>4];
      C=R[3];P[3]=XPal[C? C:T[3]>>4];
      C=R[4];P[4]=XPal[C? C:T[4]>>4];
      C=R[5];P[5]=XPal[C? C:T[5]>>4];
      C=R[6];P[6]=XPal[C? C:T[6]>>4];
      C=R[7];P[7]=XPal[C? C:T[7]>>4];
      R+=8;P+=8;T+=8;
    }
  }
}

/** RefreshLineTx80() ****************************************/
/** Refresh line Y (0..191/211) of TEXT80.                  **/
/*************************************************************/
static void RefreshLineTx80(register byte Y)
{
  register pixel *P,FC,BC;
  register byte X,M,*T,*C,*G;

  BC=XPal[BGColor];
  P=RefreshBorder(Y,BC);
  if(!P) return;

  if(!ScreenON) ClearLine(P,BC);
  else
  {
    P[0]=P[1]=P[2]=P[3]=P[4]=P[5]=P[6]=P[7]=P[8]=BC;
    G=(UseFont&&FontBuf? FontBuf:ChrGen)+((Y+VScroll)&0x07);
    T=ChrTab+((80*(Y>>3))&ChrTabM);
    C=ColTab+((10*(Y>>3))&ColTabM);
    P+=9;

    for(X=0,M=0x00;X<80;X++,T++,P+=3)
    {
      if(!(X&0x07)) M=*C++;
      if(M&0x80) { FC=XPal[XFGColor];BC=XPal[XBGColor]; }
      else       { FC=XPal[FGColor];BC=XPal[BGColor]; }
      M<<=1;
      Y=*(G+((int)*T<<3));
      P[0]=Y&0xC0? FC:BC;
      P[1]=Y&0x30? FC:BC;
      P[2]=Y&0x0C? FC:BC;
    }

    P[0]=P[1]=P[2]=P[3]=P[4]=P[5]=P[6]=XPal[BGColor];
  }
}

//#endif /* NARROW */














/** RdZ80() **************************************************/
/** Z80 emulation calls this function to read a byte from   **/
/** address A of Z80 address space. Now moved to Z80.c and  **/
/** made inlined to speed things up.                        **/
/*************************************************************/
#ifndef FMSX
byte RdZ80(word A)
{
  if(A!=0xFFFF) return(RAM[A>>13][A&0x1FFF]);
  else return(PSL[3]==3? ~SSLReg:RAM[7][0x1FFF]);
}
#endif

/** WrZ80() **************************************************/
/** Z80 emulation calls this function to write byte V to    **/
/** address A of Z80 address space.                         **/
/*************************************************************/
void WrZ80(word A,byte V)
{
  if(A!=0xFFFF)
  {
    if(EnWrite[A>>14]) RAM[A>>13][A&0x1FFF]=V;
    else if((A>0x3FFF)&&(A<0xC000)) MapROM(A,V);
  }
  else
  {
    if(PSL[3]==3) SSlot(V);
    else if(EnWrite[3]) RAM[7][A&0x1FFF]=V;
  }
}

/** InZ80() **************************************************/
/** Z80 emulation calls this function to read a byte from   **/
/** a given I/O port.                                       **/
/*************************************************************/
byte InZ80(word Port)
{
  /* MSX only uses 256 IO ports */
  Port&=0xFF;

  /* Return an appropriate port value */
  switch(Port)
  {

case 0x90: return(0xFD);                   /* Printer READY signal */
case 0xB5: return(RTCIn(RTCReg));          /* RTC registers        */

case 0xA8: /* Primary slot state   */
case 0xA9: /* Keyboard port        */
case 0xAA: /* General IO register  */
case 0xAB: /* PPI control register */
  PPI.Rin[1]=KeyMap[PPI.Rout[2]&0x0F];
  return(Read8255(&PPI,Port-0xA8));

case 0xFC: /* Mapper page at 0000h */
case 0xFD: /* Mapper page at 4000h */
case 0xFE: /* Mapper page at 8000h */
case 0xFF: /* Mapper page at C000h */
  return(RAMMapper[Port-0xFC]|~RAMMask);

case 0xD9: /* Kanji support */
  Port=Kanji? Kanji[KanLetter+KanCount]:NORAM;
  KanCount=(KanCount+1)&0x1F;
  return(Port);

case 0x80: /* SIO data */
case 0x81:
case 0x82:
case 0x83:
case 0x84:
case 0x85:
case 0x86:
case 0x87:
  return(NORAM);
  /*return(Rd8251(&SIO,Port&0x07));*/

case 0x98: /* VRAM read port */
  /* Read from VRAM data buffer */
  Port=VDPData;
  /* Reset VAddr latch sequencer */
  VKey=1;
  /* Fill data buffer with a new value */
  VDPData=VPAGE[VAddr];
  /* Increment VRAM address */
  VAddr=(VAddr+1)&0x3FFF;
  /* If rolled over, modify VRAM page# */
  if(!VAddr&&(ScrMode>3))
  {
    VDP[14]=(VDP[14]+1)&(VRAMPages-1);
    VPAGE=VRAM+((int)VDP[14]<<14);
  }
  return(Port);

case 0x99: /* VDP status registers */
  /* Read an appropriate status register */
  Port=VDPStatus[VDP[15]];
  /* Reset VAddr latch sequencer */
  VKey=1;
  /* Update status register's contents */
  switch(VDP[15])
  {
    case 0: VDPStatus[0]&=0x5F;SetIRQ(~INT_IE0);break;
    case 1: VDPStatus[1]&=0xFE;SetIRQ(~INT_IE1);break;
    case 7: VDPStatus[7]=VDP[44]=VDPRead();break;
  }
  /* Return the status register value */
  return(Port);

case 0xA2: /* PSG input port */
  /* PSG[14] returns joystick/mouse data */
  if(PSG.Latch==14)
  {
    int DX,DY,L;

    /* Number of a joystick port */
    Port=PSG.R[15]&0x40? 1:0;
    L=Port? JoyTypeB:JoyTypeA;

    /* If no joystick, return dummy value */
    if(!L) return(0x7F);

    /* @@@ For debugging purposes */
    /*printf("Reading from PSG[14]: MCount[%d]=%d PSG[15]=%02Xh Value=",Port,MCount[Port],PSG.R[15]);*/

    /* Poll mouse position, if needed */
    if((L==2)||(MCount[Port]==1))
    {
      /* Read new mouse coordinates */
      L=Mouse(Port);
      Buttons[Port]=(~L>>12)&0x30;
      DY=(L>>8)&0xFF;
      DX=L&0xFF;

      /* Compute offsets and store coordinates  */
      L=OldMouseX[Port]-DX;OldMouseX[Port]=DX;DX=L;
      L=OldMouseY[Port]-DY;OldMouseY[Port]=DY;DY=L;

      /* Adjust offsets */
      MouseDX[Port]=(DX>127? 127:(DX<-127? -127:DX))&0xFF;
      MouseDY[Port]=(DY>127? 127:(DY<-127? -127:DY))&0xFF;
    }

    /* Determine return value */
    switch(MCount[Port])
    {
      case 0: /* Normal joystick */
        if(PSG.R[15]&(Port? 0x20:0x10)) Port=0x3F;
        else
          if((Port? JoyTypeB:JoyTypeA)<2) Port=~Joystick(Port)&0x3F;
          else Port=Buttons[Port]
                   |(MouseDX[Port]? (MouseDX[Port]<128? 0x08:0x04):0x0C)
                   |(MouseDY[Port]? (MouseDY[Port]<128? 0x02:0x01):0x03);
        break;

      case 1: Port=(MouseDX[Port]>>4)|Buttons[Port];break;
      case 2: Port=(MouseDX[Port]&0x0F)|Buttons[Port];break;
      case 3: Port=(MouseDY[Port]>>4)|Buttons[Port];break;
      case 4: Port=(MouseDY[Port]&0x0F)|Buttons[Port];break;
    }

    /* @@@ For debugging purposes */
    /*printf("%02Xh\n",Port|0x40);*/

    /* 6th bit is always 1 */
    return(Port|0x40);
  }

  /* PSG[15] resets mouse counters */
  if(PSG.Latch==15)
  {
    /* @@@ For debugging purposes */
    /*printf("Reading from PSG[15]\n");*/

    /*MCount[0]=MCount[1]=0;*/
    return(PSG.R[15]&0xF0);
  }

  /* Return PSG[0-13] as they are */
  return(RdData8910(&PSG));
}

  /* Return NORAM for non-existing ports */
  return(NORAM);
}

/** OutZ80() *************************************************/
/** Z80 emulation calls this function to write byte V to a  **/
/** given I/O port.                                         **/
/*************************************************************/
void OutZ80(word Port,byte Value)
{
  register byte I,J,K;  

  Port&=0xFF;
  switch(Port)
  {

case 0x7C: WrCtrl2413(&OPLL,Value);return;        /* OPLL Register# */
case 0x7D: WrData2413(&OPLL,Value);return;        /* OPLL Data      */
case 0x91: Printer(Value);return;                 /* Printer Data   */
case 0xA0: WrCtrl8910(&PSG,Value);return;         /* PSG Register#  */
case 0xB4: RTCReg=Value&0x0F;return;              /* RTC Register#  */ 

case 0xD8: /* Upper bits of Kanji ROM address */
  KanLetter=(KanLetter&0x1F800)|((int)(Value&0x3F)<<5);
  KanCount=0;
  return;

case 0xD9: /* Lower bits of Kanji ROM address */
  KanLetter=(KanLetter&0x007E0)|((int)(Value&0x3F)<<11);
  KanCount=0;
  return;

case 0x80: /* SIO data */
case 0x81:
case 0x82:
case 0x83:
case 0x84:
case 0x85:
case 0x86:
case 0x87:
  return;
  /*Wr8251(&SIO,Port&0x07,Value);
  return;*/

case 0x98: /* VDP Data */
  VKey=1;
  if(WKey)
  {
    /* VDP set for writing */
    VDPData=VPAGE[VAddr]=Value;
    VAddr=(VAddr+1)&0x3FFF;
  }
  else
  {
    /* VDP set for reading */
    VDPData=VPAGE[VAddr];
    VAddr=(VAddr+1)&0x3FFF;
    VPAGE[VAddr]=Value;
  }
  /* If VAddr rolled over, modify VRAM page# */
  if(!VAddr&&(ScrMode>3)) 
  {
    VDP[14]=(VDP[14]+1)&(VRAMPages-1);
    VPAGE=VRAM+((int)VDP[14]<<14);
  }
  return;

case 0x99: /* VDP Address Latch */
  if(VKey) { ALatch=Value;VKey=0; }
  else
  {
    VKey=1;
    switch(Value&0xC0)
    {
      case 0x80:
        /* Writing into VDP registers */
        VDPOut(Value&0x3F,ALatch);
        break;
      case 0x00:
      case 0x40:
        /* Set the VRAM access address */
        VAddr=(((word)Value<<8)+ALatch)&0x3FFF;
        /* WKey=1 when VDP set for writing into VRAM */
        WKey=Value&0x40;
        /* When set for reading, perform first read */
        if(!WKey)
        {
          VDPData=VPAGE[VAddr];
          VAddr=(VAddr+1)&0x3FFF;
          if(!VAddr&&(ScrMode>3))
          {
            VDP[14]=(VDP[14]+1)&(VRAMPages-1);
            VPAGE=VRAM+((int)VDP[14]<<14);
          }
        }
        break;
    }
  }
  return;

case 0x9A: /* VDP Palette Latch */
  if(PKey) { PLatch=Value;PKey=0; }
  else
  {
    byte R,G,B;
    /* New palette entry written */
    PKey=1;
    J=VDP[16];
    /* Compute new color components */
    R=(PLatch&0x70)*255/112;
    G=(Value&0x07)*255/7;
    B=(PLatch&0x07)*255/7;
    /* Set new color for palette entry J */
    Palette[J]=RGB2INT(R,G,B);
    SetColor(J,R,G,B);
    /* Next palette entry */
    VDP[16]=(J+1)&0x0F;
  }
  return;

case 0x9B: /* VDP Register Access */
  J=VDP[17]&0x3F;
  if(J!=17) VDPOut(J,Value);
  if(!(VDP[17]&0x80)) VDP[17]=(J+1)&0x3F;
  return;

case 0xA1: /* PSG Data */
  /* PSG[15] is responsible for joystick/mouse */
  if(PSG.Latch==15)
  {
    /* @@@ For debugging purposes */
    /*printf("Writing PSG[15] <- %02Xh\n",Value);*/

    /* For mouse, update nibble counter      */
    /* For joystick, set nibble counter to 0 */
    if((Value&0x0C)==0x0C) MCount[1]=0;
    else if((JoyTypeB>2)&&((Value^PSG.R[15])&0x20))
           MCount[1]+=MCount[1]==4? -3:1;

    /* For mouse, update nibble counter      */
    /* For joystick, set nibble counter to 0 */
    if((Value&0x03)==0x03) MCount[0]=0;
    else if((JoyTypeA>2)&&((Value^PSG.R[15])&0x10))
           MCount[0]+=MCount[0]==4? -3:1;
  }

  /* Put value into a register */
  WrData8910(&PSG,Value);
  return;

case 0xB5: /* RTC Data */
  if(RTCReg<13)
  {
    /* J = register bank# now */
    J=RTCMode&0x03;
    /* Store the value */
    RTC[J][RTCReg]=Value;
    /* If CMOS modified, we need to save it */
    if(J>1) SaveCMOS=1;
    return;
  }
  /* RTC[13] is a register bank# */
  if(RTCReg==13) RTCMode=Value;
  return;

case 0xA8: /* Primary slot state   */
case 0xA9: /* Keyboard port        */
case 0xAA: /* General IO register  */
case 0xAB: /* PPI control register */
  /* Write to PPI */
  Write8255(&PPI,Port-0xA8,Value);
  /* If general I/O register has changed... */
  if(PPI.Rout[2]!=IOReg) { PPIOut(PPI.Rout[2],IOReg);IOReg=PPI.Rout[2]; }
  /* If primary slot state has changed... */
  if(PPI.Rout[0]!=PSLReg)
    for(J=0,PSLReg=Value=PPI.Rout[0];J<4;J++,Value>>=2)
    {
      PSL[J]=Value&3;I=J<<1;
      K=PSL[J]==3? SSL[J]:0;
      EnWrite[J]=(K==2)&&(MemMap[3][2][I]!=EmptyRAM);
      RAM[I]=MemMap[PSL[J]][K][I];
      RAM[I+1]=MemMap[PSL[J]][K][I+1];
    }
  /* Done */  
  return;

case 0xFC: /* Mapper page at 0000h */
case 0xFD: /* Mapper page at 4000h */
case 0xFE: /* Mapper page at 8000h */
case 0xFF: /* Mapper page at C000h */
  J=Port-0xFC;
  Value&=RAMMask;
  if(RAMMapper[J]!=Value)
  {
    if(Verbose&0x08) {
      //printf("RAM-MAPPER: block %d at %Xh\n",Value,J*0x4000);
      emu_printf("RAM-MAPPER: block");
      emu_printi(Value);
      emu_printi(J*0x4000);
    } 
    I=J<<1;
    RAMMapper[J]      = Value;
    MemMap[3][2][I]   = RAMData+((int)Value<<14);
    MemMap[3][2][I+1] = MemMap[3][2][I]+0x2000;
    if((PSL[J]==3)&&(SSL[J]==2))
    {
      EnWrite[J] = 1;
      RAM[I]     = MemMap[3][2][I];
      RAM[I+1]   = MemMap[3][2][I+1];
    }
  }
  return;

  }
}

/** MapROM() *************************************************/
/** Switch ROM Mapper pages. This function is supposed to   **/
/** be called when ROM page registers are written to.       **/
/*************************************************************/
void MapROM(register word A,register byte V)
{
  byte I,J,K,*P;

/* @@@ For debugging purposes
printf("(%04Xh) = %02Xh at PC=%04Xh\n",A,V,CPU.PC.W);
*/

  /* J contains 16kB page number 0-3  */
  J=A>>14;

  /* I contains slot number 0/1  */
  if(PSL[J]==1) I=0;
  else if(PSL[J]==2) I=1;
       else return;

  /* K contains MegaROM type */
  K=I? ROMTypeB:ROMTypeA;

  /* SCC: enable/disable for no cart */
  if(!ROMData[I]&&(A==0x9000)) SCCOn[I]=(V==0x3F)? 1:0;

  /* SCC: types 0, 2, or no cart */
  if(((A&0xFF00)==0x9800)&&SCCOn[I])
  {
    /* Compute SCC register number */
    J=A&0x00FF;

    /* When no MegaROM present, we allow the program */
    /* to write into SCC wave buffer using EmptyRAM  */
    /* as a scratch pad.                             */
    if(!ROMData[I]&&(J<0x80)) EmptyRAM[0x1800+J]=V;

    /* Output data to SCC chip */
    WriteSCC(&SCChip,J,V);
    return;
  }

  /* SCC+: types 0, 2, or no cart */
  if(((A&0xFF00)==0xB800)&&SCCOn[I])
  {
    /* Compute SCC register number */
    J=A&0x00FF;

    /* When no MegaROM present, we allow the program */
    /* to write into SCC wave buffer using EmptyRAM  */
    /* as a scratch pad.                             */
    if(!ROMData[I]&&(J<0xA0)) EmptyRAM[0x1800+J]=V;

    /* Output data to SCC chip */
    WriteSCCP(&SCChip,J,V);
    return;
  }

  /* If no cartridge or no mapper, exit */
  if(!ROMData[I]||!ROMMask[I]) return;

  switch(K)
  {
    case 0: /*** Generic 8kB cartridges (Konami, etc.) ***/
      if((A<0x4000)||(A>0xBFFF)) return;
      J=(A-0x4000)>>13;
      /* Turn SCC on/off on writes to 8000h-9FFFh */
      if(J==2) SCCOn[I]=(V==0x3F)? 1:0;
      /* Switch ROM pages */
      V&=ROMMask[I];
      if(V!=ROMMapper[I][J])
      {
        RAM[J+2]=MemMap[I+1][0][J+2]=ROMData[I]+((int)V<<13);
        ROMMapper[I][J]=V;
      }
      break;

    case 1: /*** Generic 16kB cartridges (MSXDOS2, HoleInOneSpecial) ***/
      if((A<0x4000)||(A>0xBFFF)) return;
      J=(A&0x8000)>>14;
      V=(V<<1)&ROMMask[I];
      if(V!=ROMMapper[I][J])
      {
        RAM[J+2]=MemMap[I+1][0][J+2]=ROMData[I]+((int)V<<13);
        RAM[J+3]=MemMap[I+1][0][J+3]=RAM[2]+0x2000;
        ROMMapper[I][J]=V;
      }
      break;

    case 2: /*** KONAMI5 8kB cartridges ***/
      if((A<0x5000)||(A>0xB000)||((A&0x1FFF)!=0x1000)) return;
      J=(A-0x5000)>>13;
      /* Turn SCC on/off on writes to 9000h */
      if(J==2) SCCOn[I]=(V==0x3F)? 1:0;
      /* Switch ROM pages */
      V&=ROMMask[I];
      if(V!=ROMMapper[I][J])
      {
        RAM[J+2]=MemMap[I+1][0][J+2]=ROMData[I]+((int)V<<13);
        ROMMapper[I][J]=V;
      }
      break;

    case 3: /*** KONAMI4 8kB cartridges ***/
      /* Page at 4000h is fixed */
      if((A<0x6000)||(A>0xA000)||(A&0x1FFF)) return;
      J=(A-0x4000)>>13;
      V&=ROMMask[I];
      if(V!=ROMMapper[I][J])
      {
        RAM[J+2]=MemMap[I+1][0][J+2]=ROMData[I]+((int)V<<13);
        ROMMapper[I][J]=V;
      }
      break;

    case 4: /*** ASCII 8kB cartridges ***/
      if((A<0x6000)||(A>0x7FFF)) return;
      J=(A&0x1800)>>11;
      V&=ROMMask[I];
      if(V!=ROMMapper[I][J])
      {
        P=ROMData[I]+((int)V<<13);
        MemMap[I+1][0][J+2]=P;
        ROMMapper[I][J]=V;
        /* Only update memory when cartridge's slot selected */
        if(PSL[(J>>1)+1]==I+1) RAM[J+2]=P;
      }
      break;

    case 5: /*** ASCII 16kB cartridges ***/
      if((A<0x6000)||(A>0x7FFF)) return;
      J=(A&0x1000)>>11;
      V=(V<<1)&ROMMask[I];
      if(V!=ROMMapper[I][J])
      {
        P=ROMData[I]+((int)V<<13);
        MemMap[I+1][0][J+2]=P;
        MemMap[I+1][0][J+3]=P+0x2000;
        ROMMapper[I][J]=V;
        /* Only update memory when cartridge's slot selected */
        if(PSL[(J>>1)+1]==I+1)
        {
          RAM[J+2]=P;
          RAM[J+3]=P+0x2000;
        }
      }
      break;

    case 6: /*** GameMaster2+SRAM cartridge ***/
      /* Switch ROM and SRAM pages, page at 4000h is fixed */
      if((A>=0x6000)&&(A<=0xA000)&&!(A&0x1FFF))
      {
        /* If changing SRAM page... */
        if(V&0x10)
        {
          /* Select SRAM page */
          RAM[5]=MemMap[I+1][0][5]=(SRAM? SRAM:EmptyRAM)+(V&0x20? 0x2000:0);
          /* SRAM is now on */
          ROMMapper[I][3]=0xFF;
        }
        else
        {
          /* Figure out which ROM page gets switched */
          J=(A-0x4000)>>13;
          /* Compute new ROM page number */
          V&=ROMMask[I];
          /* If ROM page number has changed... */
          if(V!=ROMMapper[I][J])
          {
            RAM[J+2]=MemMap[I+1][0][J+2]=ROMData[I]+((int)V<<13);
            ROMMapper[I][J]=V;
          }
        }
        /* Done with page switch */
        break;
      }
      /* Write to SRAM */
      if((A>=0xB000)&&(A<0xC000)&&(ROMMapper[I][3]==0xFF))
      {
        RAM[5][(A&0x0FFF)|0x1000]=RAM[5][A&0x0FFF]=V;
        SaveSRAM=1;
        break;
      }
      /* Done */
      return;

    default: /*** No MegaROM mapper by default ***/
      return; 
  }

  if(Verbose&0x08)
  {
    /* K=1 for 16kB pages */
    K=(K==1)||(K==5);

    //printf
    //(
    //  "ROM-MAPPER %c: %dkB block %d at %04Xh\n",
    //  I+'A',8*(K+1),K? (V>>1):V,J*0x2000+0x4000
    //);
  }
}

/** SSlot() **************************************************/
/** Switch secondary memory slots. This function is called  **/
/** when value in (FFFFh) changes.                          **/
/*************************************************************/
void SSlot(register byte V)
{
  register byte I,J;
  
  if(SSLReg!=V)
  {
    SSLReg=V;

    for(J=0;J<4;J++,V>>=2)
    {
      SSL[J]=V&3;
      if(PSL[J]==3)
      {
        I=J<<1;
        EnWrite[J]=(SSL[J]==2)&&(MemMap[3][2][I]!=EmptyRAM);
        RAM[I]=MemMap[3][SSL[J]][I];
        RAM[I+1]=MemMap[3][SSL[J]][I+1];
      }
    }
  }
}


/** LoadROM() ************************************************/
/** Load a file, allocating memory as needed. Returns addr. **/
/** of the alocated space or 0 if failed.                   **/
/*************************************************************/
byte *LoadROM(const char *Name,int Size,byte *Buf)
{
  byte *P;

  /* Can't give address without size! */
  if(Buf&&!Size) return(0);

  Size = emu_FileSize(Name);

  if (Size<=0) return (0);

  P=Buf? Buf:loc_Malloc(Size);
  emu_LoadFile(Name, P, Size);

  /* Add a new chunk to free */
  if(!Buf) Chunks[CCount++]=P;

  return(P);

#ifdef unused
  FILE *F;
  byte *P;

  /* Can't give address without size! */
  if(Buf&&!Size) return(0);

  /* Open file */
  if(!(F=fopen(Name,"rb"))) return(0);

  /* Determine data size, if wasn't given */
  if(!Size)
  {
    /* Determine size via fseek()/ftell() or by reading */
    /* entire [GZIPped] stream                          */
    if(fseek(F,0,SEEK_END)>=0) Size=ftell(F);
    else while(fgetc(F)>=0) Size++;
    /* Rewind file to the beginning */
    fseek(F,0,SEEK_SET);
  }

  /* Allocate memory */
  P=Buf? Buf:loc_Malloc(Size);
  if(!P)
  {
    fclose(F);
    return(0);
  }

  /* Read data */
  if(fread(P,1,Size,F)!=Size)
  {
    if(!Buf) emu_Free(P);
    fclose(F);
    return(0);
  }

  /* Add a new chunk to free */
  if(!Buf) Chunks[CCount++]=P;

  /* Done */
  fclose(F);
  return(P);
#endif  
}


/** LoadCart() ***********************************************/
/** Load a cartridge ROM from .ROM file into given slot.    **/
/*************************************************************/
int LoadCart(const char *Name,int Slot)
{
  char C1,C2;
  int C3,ROM64,LastFirst;

  /* Check slot #, try to open file */
  if((Slot!=0)&&(Slot!=1)) return(0);

  int Size = emu_FileSize(Name);

  if(!(emu_FileOpen(Name))) return(0);
  if(Verbose) emu_printf(Name);
  if(Verbose) emu_printi(Size);

  /* Check "AB" signature in a file */
  ROM64=LastFirst=0;
  emu_FileRead(&C1,1);
  emu_FileRead(&C2,1);

  /* Maybe this is a flat 64kB ROM? */
  if((C1!='A')||(C2!='B')) {
    if(Verbose) emu_printf("check next");
    if(emu_FileSeek(0x4000/*,SEEK_SET*/)>=0)
    {
      emu_FileRead(&C1,1);
      emu_FileRead(&C2,1);
      ROM64=(C1=='A')&&(C2=='B');
    }    
  }

  /* Maybe it is the last page that contains "AB" signature? */
  if((C1!='A')||(C2!='B')) {
    if(Verbose) emu_printf("check next");
    if(emu_FileSeek(Size-0x4000/*,SEEK_END*/)>=0)
    {
      emu_FileRead(&C1,1);
      emu_FileRead(&C2,1);
      LastFirst=(C1=='A')&&(C2=='B');
    }    
  }

  /* If we can't find "AB" signature, drop out */     
  if((C1!='A')||(C2!='B'))
  {
    if(Verbose) emu_printf("  Not a valid cartridge ROM");
    emu_FileClose();
    return(0);
  }

  if(Verbose) {
    emu_printf(" Cartridge");
    char slot[2] = {0,0};
    slot[0] = 'A'+Slot;
    emu_printf(slot);
  } 



  /* Length must be a multiple of 8kB */
  /* Flat 64kB ROM must be 40..64kB */
  //if(Size&0x1FFF)           { emu_printf("failed");return(0); }
  //if(ROM64&&(Size<0xA000))  { emu_printf("failed");return(0); }
  //if(ROM64&&(Size>0x10000)) { emu_printf("failed");return(0); }

  /* Show ROM type and size */
  //if(Verbose)
  //  printf
  //  (
  //    "%dkB %s ROM...",C1/1024,
  //    !ROM64&&(C1>0x8000)? ROMNames[Slot? ROMTypeB:ROMTypeA]:"NORMAL"
  //  );

  /* Compute size in 8kB pages */
  Size >>= 13;

  /* Calculate 2^n closest to number of pages */
  for(C3=1;C3<Size;C3<<=1);

  /* Assign ROMMask for MegaROMs */
  ROMMask[Slot]=!ROM64&&(Size>4)? C3-1:0x00;

  /* Allocate space for the ROM */
  ROMData[Slot]=loc_Malloc(C3*0x2000);
  if(!ROMData[Slot]) { emu_printf("failed");return(0); }
  Chunks[CCount++]=ROMData[Slot];

  /* Try loading ROM */
  if(!LoadROM(Name,Size*0x2000,ROMData[Slot])) { emu_printf("failed");return(0); }

  /* Mirror ROM if it is smaller than 2^n pages */
  if(Size<C3)
    memcpy
    (
      ROMData[Slot]+Size*0x2000,
      ROMData[Slot]+(Size-C3/2)*0x2000,
      (C3-Size)*0x2000
    ); 

  /* Set memory map depending on the ROM size */
  switch(Size)
  {
    case 1:
      /* 8kB ROMs are mirrored 8 times: 0:0:0:0:0:0:0:0 */
      MemMap[Slot+1][0][0]=ROMData[Slot];
      MemMap[Slot+1][0][1]=ROMData[Slot];
      MemMap[Slot+1][0][2]=ROMData[Slot];
      MemMap[Slot+1][0][3]=ROMData[Slot];
      MemMap[Slot+1][0][4]=ROMData[Slot];
      MemMap[Slot+1][0][5]=ROMData[Slot];
      MemMap[Slot+1][0][6]=ROMData[Slot];
      MemMap[Slot+1][0][7]=ROMData[Slot];
      break;

    case 2:
      /* 16kB ROMs are mirrored 4 times: 0:1:0:1:0:1:0:1 */
      MemMap[Slot+1][0][0]=ROMData[Slot];
      MemMap[Slot+1][0][1]=ROMData[Slot]+0x2000;
      MemMap[Slot+1][0][2]=ROMData[Slot];
      MemMap[Slot+1][0][3]=ROMData[Slot]+0x2000;
      MemMap[Slot+1][0][4]=ROMData[Slot];
      MemMap[Slot+1][0][5]=ROMData[Slot]+0x2000;
      MemMap[Slot+1][0][6]=ROMData[Slot];
      MemMap[Slot+1][0][7]=ROMData[Slot]+0x2000;
      break;

    case 3:
    case 4:
      /* 24kB and 32kB ROMs are mirrored twice: 0:1:0:1:2:3:2:3 */
      MemMap[Slot+1][0][0]=ROMData[Slot];
      MemMap[Slot+1][0][1]=ROMData[Slot]+0x2000;
      MemMap[Slot+1][0][2]=ROMData[Slot];
      MemMap[Slot+1][0][3]=ROMData[Slot]+0x2000;
      MemMap[Slot+1][0][4]=ROMData[Slot]+0x4000;
      MemMap[Slot+1][0][5]=ROMData[Slot]+0x6000;
      MemMap[Slot+1][0][6]=ROMData[Slot]+0x4000;
      MemMap[Slot+1][0][7]=ROMData[Slot]+0x6000;
      break;

    default:
      if(ROM64)
      {
        /* 64kB ROMs are loaded to fill slot: 0:1:2:3:4:5:6:7 */
        MemMap[Slot+1][0][0]=ROMData[Slot];
        MemMap[Slot+1][0][1]=ROMData[Slot]+0x2000;
        MemMap[Slot+1][0][2]=ROMData[Slot]+0x4000;
        MemMap[Slot+1][0][3]=ROMData[Slot]+0x6000;
        MemMap[Slot+1][0][4]=ROMData[Slot]+0x8000;
        MemMap[Slot+1][0][5]=ROMData[Slot]+0xA000;
        MemMap[Slot+1][0][6]=ROMData[Slot]+0xC000;
        MemMap[Slot+1][0][7]=ROMData[Slot]+0xE000;
      }
      else
      {
        /* MegaROMs are switched into 4000h-BFFFh */
        if(!LastFirst) SetMegaROM(Slot,0,1,2,3);
        else SetMegaROM(Slot,Size-2,Size-1,Size-2,Size-1);
      }
      break;
  }

  /* Show starting address */
  if(Verbose) {
    emu_printf("starts at");
    emu_printi(MemMap[Slot+1][0][2][2]+256*MemMap[Slot+1][0][2][3]);    
  }


  /* Done loading cartridge */
  return(1);
}



/** SetIRQ() *************************************************/
/** Set or reset IRQ. Returns IRQ vector assigned to        **/
/** CPU.IRequest. When upper bit of IRQ is 1, IRQ is reset. **/
/*************************************************************/
word SetIRQ(register byte IRQ)
{
  if(IRQ&0x80) IRQPending&=IRQ; else IRQPending|=IRQ;
  CPU.IRequest=IRQPending? INT_IRQ:INT_NONE;
  return(CPU.IRequest);
}

/** SetScreen() **********************************************/
/** Change screen mode. Returns new screen mode.            **/
/*************************************************************/
byte SetScreen(void)
{
  register byte I,J;

  switch(((VDP[0]&0x0E)>>1)|(VDP[1]&0x18))
  {
    case 0x10: J=0;break;
    case 0x00: J=1;break;
    case 0x01: J=2;break;
    case 0x08: J=3;break;
    case 0x02: J=4;break;
    case 0x03: J=5;break;
    case 0x04: J=6;break;
    case 0x05: J=7;break;
    case 0x07: J=8;break;
    case 0x12: J=MAXSCREEN+1;break;
    default:   J=ScrMode;break;
  }

  /* Recompute table addresses */
  I=(J>6)&&(J!=MAXSCREEN+1)? 11:10;
  ChrTab  = VRAM+((int)(VDP[2]&MSK[J].R2)<<I);
  ChrGen  = VRAM+((int)(VDP[4]&MSK[J].R4)<<11);
  ColTab  = VRAM+((int)(VDP[3]&MSK[J].R3)<<6)+((int)VDP[10]<<14);
  SprTab  = VRAM+((int)(VDP[5]&MSK[J].R5)<<7)+((int)VDP[11]<<15);
  SprGen  = VRAM+((int)VDP[6]<<11);
  ChrTabM = ((int)(VDP[2]|~MSK[J].M2)<<I)|((1<<I)-1);
  ChrGenM = ((int)(VDP[4]|~MSK[J].M4)<<11)|0x007FF;
  ColTabM = ((int)(VDP[3]|~MSK[J].M3)<<6)|0x1C03F;
  SprTabM = ((int)(VDP[5]|~MSK[J].M5)<<7)|0x1807F;

  /* Return new screen mode */
  ScrMode=J;
  return(J);
}

/** SetMegaROM() *********************************************/
/** Set MegaROM pages for a given slot. SetMegaROM() always **/
/** assumes 8kB pages.                                      **/
/*************************************************************/
void SetMegaROM(int Slot,byte P0,byte P1,byte P2,byte P3)
{
  P0&=ROMMask[Slot];
  P1&=ROMMask[Slot];
  P2&=ROMMask[Slot];
  P3&=ROMMask[Slot];
  MemMap[Slot+1][0][2]=ROMData[Slot]+P0*0x2000;
  MemMap[Slot+1][0][3]=ROMData[Slot]+P1*0x2000;
  MemMap[Slot+1][0][4]=ROMData[Slot]+P2*0x2000;
  MemMap[Slot+1][0][5]=ROMData[Slot]+P3*0x2000;
  ROMMapper[Slot][0]=P0;
  ROMMapper[Slot][1]=P1;
  ROMMapper[Slot][2]=P2;
  ROMMapper[Slot][3]=P3;
}

/** VDPOut() *************************************************/
/** Write value into a given VDP register.                  **/
/*************************************************************/
void VDPOut(register byte R,register byte V)
{ 
  register byte J;

  switch(R)  
  {
    case  0: /* Reset HBlank interrupt if disabled */
             if((VDPStatus[1]&0x01)&&!(V&0x10))
             {
               VDPStatus[1]&=0xFE;
               SetIRQ(~INT_IE1);
             }
             /* Set screen mode */
             if(VDP[0]!=V) { VDP[0]=V;SetScreen(); }
             break;
    case  1: /* Set/Reset VBlank interrupt if enabled or disabled */
             if(VDPStatus[0]&0x80) SetIRQ(V&0x20? INT_IE0:~INT_IE0);
             /* Set screen mode */
             if(VDP[1]!=V) { VDP[1]=V;SetScreen(); }
             break;
    case  2: J=(ScrMode>6)&&(ScrMode!=MAXSCREEN+1)? 11:10;
             ChrTab  = VRAM+((int)(V&MSK[ScrMode].R2)<<J);
             ChrTabM = ((int)(V|~MSK[ScrMode].M2)<<J)|((1<<J)-1);
             break;
    case  3: ColTab  = VRAM+((int)(V&MSK[ScrMode].R3)<<6)+((int)VDP[10]<<14);
             ColTabM = ((int)(V|~MSK[ScrMode].M3)<<6)|0x1C03F;
             break;
    case  4: ChrGen  = VRAM+((int)(V&MSK[ScrMode].R4)<<11);
             ChrGenM = ((int)(V|~MSK[ScrMode].M4)<<11)|0x007FF;
             break;
    case  5: SprTab  = VRAM+((int)(V&MSK[ScrMode].R5)<<7)+((int)VDP[11]<<15);
             SprTabM = ((int)(V|~MSK[ScrMode].M5)<<7)|0x1807F;
             break;
    case  6: V&=0x3F;SprGen=VRAM+((int)V<<11);break;
    case  7: FGColor=V>>4;BGColor=V&0x0F;break;
    case 10: V&=0x07;
             ColTab=VRAM+((int)(VDP[3]&MSK[ScrMode].R3)<<6)+((int)V<<14);
             break;
    case 11: V&=0x03;
             SprTab=VRAM+((int)(VDP[5]&MSK[ScrMode].R5)<<7)+((int)V<<15);
             break;
    case 14: V&=VRAMPages-1;VPAGE=VRAM+((int)V<<14);
             break;
    case 15: V&=0x0F;break;
    case 16: V&=0x0F;PKey=1;break;
    case 17: V&=0xBF;break;
    case 25: VDP[25]=V;
             SetScreen();
             break;
    case 44: VDPWrite(V);break;
    case 46: VDPDraw(V);break;
  }

  /* Write value into a register */
  VDP[R]=V;
} 

/** Printer() ************************************************/
/** Send a character to the printer.                        **/
/*************************************************************/
#ifdef unused
void Printer(byte V) { fputc(V,PrnStream); }
#else
void Printer(byte V) {  }
#endif

/** PPIOut() *************************************************/
/** This function is called on each write to PPI to make    **/
/** key click sound, motor relay clicks, and so on.         **/
/*************************************************************/
void PPIOut(register byte New,register byte Old)
{
  /* Keyboard click bit */
  if((New^Old)&0x80) Drum(DRM_CLICK,64);
  /* Motor relay bit */
  if((New^Old)&0x10) Drum(DRM_CLICK,255);
}

byte RTCIn(register byte R)
{
  register byte J;

  /* Only 16 registers/mode */
  R&=0x0F;

  /* Bank mode 0..3 */
  J=RTCMode&0x03;

  if(R>12) J=R==13? RTCMode:NORAM;
  else
    if(J) J=RTC[J][R];
    else
    {
#ifdef unused  
      static time_t PrevTime;
      static struct tm TM;

      /* Retrieve system time if any time passed */
      time_t CurTime=time(NULL);
      if(CurTime!=PrevTime)
      {
        TM=*localtime(&CurTime);
        PrevTime=CurTime;
      }

      /* Parse contents of last retrieved TM */
      switch(R)
      {
        case 0:  J=TM.tm_sec%10;break;
        case 1:  J=TM.tm_sec/10;break;
        case 2:  J=TM.tm_min%10;break;
        case 3:  J=TM.tm_min/10;break;
        case 4:  J=TM.tm_hour%10;break;
        case 5:  J=TM.tm_hour/10;break;
        case 6:  J=TM.tm_wday;break;
        case 7:  J=TM.tm_mday%10;break;
        case 8:  J=TM.tm_mday/10;break;
        case 9:  J=(TM.tm_mon+1)%10;break;
        case 10: J=(TM.tm_mon+1)/10;break;
        case 11: J=(TM.tm_year-80)%10;break;
        case 12: J=((TM.tm_year-80)/10)%10;break;
        default: J=0x0F;break;
      } 
#endif
    }
  /* Four upper bits are always high */
  return(J|0xF0);
}

/** LoopZ80() ************************************************/
/** Refresh screen, check keyboard and sprites. Call this   **/
/** function on each interrupt.                             **/
/*************************************************************/
word LoopZ80(register Z80 *R, int * ras)
//word LoopZ80(Z80 *R)
{
  static byte BFlag=0;
  static byte BCount=0;
  static int  UCount=1;
  static byte ACount=0;
  static byte Drawing=0;
  register int J;

  /* Flip HRefresh bit */
  VDPStatus[2]^=0x20;

  /* If HRefresh is now in progress... */
  if(!(VDPStatus[2]&0x20))
  {
    /* HRefresh takes most of the scanline */
    R->IPeriod=!ScrMode||(ScrMode==MAXSCREEN+1)? CPU_H240:CPU_H256;

    /* New scanline */
    ScanLine=ScanLine<(PALVideo? 312:261)? ScanLine+1:0;

    /* If first scanline of the screen... */
    if(!ScanLine)
    {
      /* Drawing now... */
      Drawing=1;

      /* Reset VRefresh bit */
      VDPStatus[2]&=0xBF;

      /* Refresh display */
      if(UCount) UCount--;
      else
      {
        UCount=UPeriod-1;
        //RefreshScreen();
      }

      /* Blinking for TEXT80 */
      if(BCount) BCount--;
      else
      {
        BFlag=!BFlag;
        if(!VDP[13]) { XFGColor=FGColor;XBGColor=BGColor; }
        else
        {
          BCount=(BFlag? VDP[13]&0x0F:VDP[13]>>4)*10;
          if(BCount)
          {
            if(BFlag) { XFGColor=FGColor;XBGColor=BGColor; }
            else      { XFGColor=VDP[12]>>4;XBGColor=VDP[12]&0x0F; }
          }
        }
      }
    }

    /* Line coincidence is active at 0..255 */
    /* in PAL and 0..234/244 in NTSC        */
    J=PALVideo? 256:ScanLines212? 245:235;

    /* When reaching end of screen, reset line coincidence */
    if(ScanLine==J)
    {
      VDPStatus[1]&=0xFE;
      SetIRQ(~INT_IE1);
    }

    /* When line coincidence is active... */
    if(ScanLine<J)
    {
      /* Line coincidence processing */
      J=(((ScanLine+VScroll)&0xFF)-VDP[19])&0xFF;
      if(J==2)
      {
        /* Set HBlank flag on line coincidence */
        VDPStatus[1]|=0x01;
        /* Generate IE1 interrupt */
        if(VDP[0]&0x10) SetIRQ(INT_IE1);
      }
      else
      {
        /* Reset flag immediately if IE1 interrupt disabled */
        if(!(VDP[0]&0x10)) VDPStatus[1]&=0xFE;
      }
    }

    /* Return whatever interrupt is pending */
    R->IRequest=IRQPending? INT_IRQ:INT_NONE;
    return(R->IRequest);
  }

  /*********************************/
  /* We come here for HBlanks only */
  /*********************************/

  /* HBlank takes HPeriod-HRefresh */
  R->IPeriod=!ScrMode||(ScrMode==MAXSCREEN+1)? CPU_H240:CPU_H256;
  R->IPeriod=HPeriod-R->IPeriod;

  /* If last scanline of VBlank, see if we need to wait more */
  J=PALVideo? 313:262;
  if(ScanLine>=J-1)
  {
    J*=CPU_HPERIOD;
    if(VPeriod>J) R->IPeriod+=VPeriod-J;
  }

  /* If first scanline of the bottom border... */
  if(ScanLine==(ScanLines212? 212:192)) Drawing=0;

  /* If first scanline of VBlank... */
  J=PALVideo? (ScanLines212? 212+42:192+52):(ScanLines212? 212+18:192+28);
  if(!Drawing&&(ScanLine==J))
  {
    /* Set VBlank bit, set VRefresh bit */
    VDPStatus[0]|=0x80;
    VDPStatus[2]|=0x40;

    /* Generate VBlank interrupt */
    if(VDP[1]&0x20) SetIRQ(INT_IE0);
  }

  /* Run V9938 engine */
  LoopVDP();

  /* Refresh scanline, possibly with the overscan */
  if(!UCount&&Drawing&&(ScanLine<256))
  {
    if(!ModeYJK||(ScrMode<7)||(ScrMode>8))
      (RefreshLine[ScrMode])(ScanLine);
    else
      if(ModeYAE) RefreshLine10(ScanLine);
      else RefreshLine12(ScanLine);

    emu_DrawLine(&linebuffer[0], WIDTH , HEIGHT, ScanLine+FirstLine);  
  }

  /* Keyboard, sound, and other stuff always runs at line 192    */
  /* This way, it can't be shut off by overscan tricks (Maarten) */
  if(ScanLine==192)
  {
    /* Check sprites and set Collision, 5Sprites, 5thSprite bits */
    if(!SpritesOFF&&ScrMode&&(ScrMode<MAXSCREEN+1)) CheckSprites();

    /* Count MIDI ticks */
#ifdef unused    
    MIDITicks(VPeriod/CPU_CLOCK);
#endif
    /* Update AY8910 state every VPeriod/CPU_CLOCK milliseconds */
    Loop8910(&PSG,VPeriod/CPU_CLOCK);

    /* Flush changes to the sound channels */
    Sync8910(&PSG,AY8910_FLUSH|(UseDrums? AY8910_DRUMS:0));
    SyncSCC(&SCChip,SCC_FLUSH);
    Sync2413(&OPLL,YM2413_FLUSH);

    /* Check keyboard */
    Keyboard();

    /* Exit emulation if requested */
    //if(ExitNow) return(INT_QUIT);

    *ras = 1;

    /* Autofire emulation */
    ACount=(ACount+1)&0x07;
    if((ACount>3)&&AutoFire) KeyMap[8]|=0x01;
  }

  /* Return whatever interrupt is pending */
  R->IRequest=IRQPending? INT_IRQ:INT_NONE;
  return(R->IRequest);
}

/** CheckSprites() *******************************************/
/** Check for sprite collisions and 5th/9th sprite in a     **/
/** row.                                                    **/
/*************************************************************/
void CheckSprites(void)
{
  register word LS,LD;
  register byte DH,DV,*PS,*PD,*T;
  byte I,J,N,M,*S,*D;

  /* Clear 5Sprites, Collision, and 5thSprite bits */
  VDPStatus[0]=(VDPStatus[0]&0x9F)|0x1F;

  for(N=0,S=SprTab;(N<32)&&(S[0]!=208);N++,S+=4);
  M=SolidColor0;

  if(Sprites16x16)
  {
    for(J=0,S=SprTab;J<N;J++,S+=4)
      if((S[3]&0x0F)||M)
        for(I=J+1,D=S+4;I<N;I++,D+=4)
          if((D[3]&0x0F)||M) 
          {
            DV=S[0]-D[0];
            if((DV<16)||(DV>240))
      {
              DH=S[1]-D[1];
              if((DH<16)||(DH>240))
        {
                PS=SprGen+((int)(S[2]&0xFC)<<3);
                PD=SprGen+((int)(D[2]&0xFC)<<3);
                if(DV<16) PD+=DV; else { DV=256-DV;PS+=DV; }
                if(DH>240) { DH=256-DH;T=PS;PS=PD;PD=T; }
                while(DV<16)
                {
                  LS=((word)*PS<<8)+*(PS+16);
                  LD=((word)*PD<<8)+*(PD+16);
                  if(LD&(LS>>DH)) break;
                  else { DV++;PS++;PD++; }
                }
                if(DV<16) { VDPStatus[0]|=0x20;return; }
              }
            }
          }
  }
  else
  {
    for(J=0,S=SprTab;J<N;J++,S+=4)
      if((S[3]&0x0F)||M)
        for(I=J+1,D=S+4;I<N;I++,D+=4)
          if((D[3]&0x0F)||M)
          {
            DV=S[0]-D[0];
            if((DV<8)||(DV>248))
            {
              DH=S[1]-D[1];
              if((DH<8)||(DH>248))
              {
                PS=SprGen+((int)S[2]<<3);
                PD=SprGen+((int)D[2]<<3);
                if(DV<8) PD+=DV; else { DV=256-DV;PS+=DV; }
                if(DH>248) { DH=256-DH;T=PS;PS=PD;PD=T; }
                while((DV<8)&&!(*PD&(*PS>>DH))) { DV++;PS++;PD++; }
                if(DV<8) { VDPStatus[0]|=0x20;return; }
              }
            }
          }
  }
}

/** GuessROM() ***********************************************/
/** Guess MegaROM mapper of a ROM.                          **/
/*************************************************************/
int GuessROM(const byte *Buf,int Size)
{
  int J,I,ROMCount[6];

  /* Clear all counters */
  for(J=0;J<6;J++) ROMCount[J]=1;
  ROMCount[0]+=1; /* Mapper #0 is default */
  ROMCount[4]-=1; /* #5 preferred over #4 */

  /* Count occurences of characteristic addresses */
  for(J=0;J<Size-2;J++)
  {
    I=Buf[J]+((int)Buf[J+1]<<8)+((int)Buf[J+2]<<16);
    switch(I)
    {
      case 0x500032: ROMCount[2]++;break;
      case 0x900032: ROMCount[2]++;break;
      case 0xB00032: ROMCount[2]++;break;
      case 0x400032: ROMCount[3]++;break;
      case 0x800032: ROMCount[3]++;break;
      case 0xA00032: ROMCount[3]++;break;
      case 0x680032: ROMCount[4]++;break;
      case 0x780032: ROMCount[4]++;break;
      case 0x600032: ROMCount[3]++;ROMCount[4]++;ROMCount[5]++;break;
      case 0x700032: ROMCount[2]++;ROMCount[4]++;ROMCount[5]++;break;
      case 0x77FF32: ROMCount[5]++;break;
    }
  }

  /* Find which mapper type got more hits */
  for(I=0,J=0;J<6;J++)
    if(ROMCount[J]>ROMCount[I]) I=J;

  /* Return the most likely mapper type */
  return(I);
}
