#include "options.h"

#include "Z80.h"            /* Z80 CPU emulation             */
#include "SN76489.h"        /* SN76489 PSG emulation         */
#include <stdlib.h>
#include <ctype.h>

#include "emuapi.h"


/**************************************
* Local macros/typedef
**************************************/
#define WIDTH     256
#define HEIGHT    192

#define MAXSCREEN 3         /* Highest screen mode supported */
#define NORAM     0xFF      /* Byte to be returned from      */
                            /* non-existing pages and ports  */

/***** Following are macros to be used in screen drivers *****/
#define BigSprites    (VDP[1]&0x01)   /* Zoomed sprites      */
#define Sprites16x16  (VDP[1]&0x02)   /* 16x16/8x8 sprites   */
#define ScreenON      (VDP[1]&0x40)   /* Show screen         */

/***************************************
* Local procedures definition
***************************************/
static void snd_Reset(void);
static void snd_Sound(int C, int F, int V);
static void SetColor(byte N,byte R,byte G,byte B);
static void RefreshSprites(byte Y);
static void RefreshBorder(byte Y);
static void RefreshLine0(byte Y);
static void RefreshLine1(byte Y);
static void RefreshLine2(byte Y);
static void RefreshLine3(byte Y);
static void RefreshScreen(void);
static void VDPOut(byte Reg,byte Value); /* Write value into VDP */
static void CheckSprites(void);          /* Collisions/5th spr.  */
static void Play(int C,int F,int V);     /* Log and play sound   */

/***************************************
* Local data
***************************************/
static byte * XBuf=0; // = (byte *)XBuf32;
static byte XPal[16],XPal0;


/*** TMS9918/9928 Palette *******************************************/
struct { byte R,G,B; } Palette[16] =
{
  {0x00,0x00,0x00},{0x00,0x00,0x00},{0x20,0xC0,0x20},{0x60,0xE0,0x60},
  {0x20,0x20,0xE0},{0x40,0x60,0xE0},{0xA0,0x20,0x20},{0x40,0xC0,0xE0},
  {0xE0,0x20,0x20},{0xE0,0x60,0x60},{0xC0,0xC0,0x20},{0xC0,0xC0,0x80},
  {0x20,0x80,0x20},{0xC0,0x40,0xA0},{0xA0,0xA0,0xA0},{0xE0,0xE0,0xE0}
};

byte Verbose     = 1;          /* Debug msgs ON/OFF             */
byte UPeriod     = 2;          /* Interrupts/scr. update        */
int  VPeriod     = 60000;      /* Number of cycles per VBlank   */
int  HPeriod     = 215;        /* Number of cycles per HBlank   */
byte AutoA=0,AutoB=0;          /* 1: Autofire for A,B buttons   */
byte Adam        = 0;          /* 1: Emulate Coleco Adam        */

#define MEMRELOC -0x4000

#define VRAMSIZE  0x4000
#define RAMSIZE   0xC000
	

/* Main and Video RAMs           */
//static byte * VRAM=0; //[VRAMSIZE];
//static byte * RAM=0; //RAM[RAMSIZE];
static byte VRAM[VRAMSIZE];
static byte RAM[RAMSIZE];

Z80 ccpu;                       /* Z80 CPU registers and state   */
SN76489 PSG;                   /* SN76489 PSG state             */

byte *ChrGen,*ChrTab,*ColTab;  /* VDP tables (screens)          */
byte *SprGen,*SprTab;          /* VDP tables (sprites)          */
pair WVAddr,RVAddr;            /* Storage for VRAM addresses    */
byte VKey;                     /* VDP address latch key         */
byte FGColor,BGColor;          /* Colors                        */
byte ScrMode;                  /* Current screen mode           */
byte CurLine;                  /* Current scanline              */
byte VDP[8],VDPStatus;         /* VDP registers                 */

byte JoyMode;                  /* Joystick controller mode      */
word JoyState[2];              /* Joystick states               */

/*** Screen handlers and masks for VDP table address registers ******/
struct
{
  void (*Refresh)(byte Y);
  byte R2,R3,R4,R5;
} SCR[MAXSCREEN+1] =
{
  { RefreshLine0,0x7F,0x00,0x3F,0x00 },   /* SCREEN 0:TEXT 40x24    */
  { RefreshLine1,0x7F,0xFF,0x3F,0xFF },   /* SCREEN 1:TEXT 32x24    */
  { RefreshLine2,0x7F,0x80,0x3C,0xFF },   /* SCREEN 2:BLOCK 256x192 */
  { RefreshLine3,0x7F,0x00,0x3F,0xFF }    /* SCREEN 3:GFX 64x48x16  */
};

/***************************************
* Global data
***************************************/

/***************************************
* Exported procedures
***************************************/
void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}

void coc_Init(void)
{
  int J;

  /* Set up the palette */
  for(J=0;J<16;J++)
    SetColor(J,Palette[J].R,Palette[J].G,Palette[J].B);

  //if (VRAM == 0) VRAM = (byte *)emu_Malloc(VRAMSIZE);
  //if (RAM == 0) RAM = (byte *)emu_Malloc(RAMSIZE);
#if SINGLELINE_RENDERING
  if (XBuf == 0) XBuf = (byte *)emu_Malloc(WIDTH);
#else
  if (XBuf == 0) XBuf = (byte *)emu_Malloc(WIDTH*HEIGHT);
#endif  

}

int coc_Start(char * Cartridge)
{ 
  int *T,I,J;
  char *P;

  /*** VDP control register states: ***/
  static byte VDPInit[8] =
  { 0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00 };

  /*** STARTUP CODE starts here: ***/
  T=(int *)"\01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
#ifdef LSB_FIRST
  if(*T!=1)
  {
    emu_printf("********** This machine is high-endian. **********\n");
    emu_printf("Take #define LSB_FIRST out and compile ColEm again.\n");    
    return(0);
  }
#else
  if(*T==1)
  {
    emu_printf("********** This machine is low-endian. **********\n");
    emu_printf("Insert #define LSB_FIRST and compile ColEm again.\n");    
    return(0);
  }
#endif

  /* Calculate IPeriod from VPeriod */
  if(UPeriod<1) UPeriod=1;
  if(VPeriod/HPeriod<256) VPeriod=256*HPeriod;
  ccpu.IPeriod=HPeriod;
  ccpu.TrapBadOps=Verbose&0x04;
  ccpu.IAutoReset=0;

  memset(RAM,NORAM,RAMSIZE);
  memset(VRAM,NORAM,VRAMSIZE);

  if(Verbose) emu_printf("OK\nLoading ROMs:\nOpening COLECO.ROM...");
  P=NULL;
  if (emu_LoadFile("coleco.rom", (unsigned char *)RAM, 0x2000) != 0x2000)
    P="NOT FOUND OR SHORT FILE";
 
  //if(P) { if(Verbose) puts(P);return(0); }
  if(Verbose) emu_printf("OK\nOpening ROM");
  if(Verbose) emu_printf(Cartridge);

  P=NULL;
  J= emu_LoadFile(Cartridge, (unsigned char *)RAM+0x8000+MEMRELOC, 0x8000);

  if(J<0x1000) P="SHORT FILE";
  I=RAM[0x8000+MEMRELOC];J=RAM[0x8001+MEMRELOC];
  if( !( ((I==0x55)&&(J==0xAA))||((I==0xAA)&&(J==0x55)) ) )
    P="INVALID IMAGE";

  //if(P) { if(Verbose) puts(P);return(0); }
  if(Verbose) emu_printf("bytes loaded\n");

  if(Verbose)
  {
    emu_printf("Initializing CPU and System Hardware:\n");
    //emu_printf("  VBlank = %d cycles\n  HBlank = %d cycles\n",VPeriod,HPeriod);
  }

#ifdef HAS_SND
  snd_Reset();
#endif  
#ifdef SOUND_PRESENT
  snd_Open(44100/*22050*/, 2, 4096/*16384*/,(void*)snd_Mixer);
#endif

  /* Initialize VDP registers */
  memcpy(VDP,VDPInit,sizeof(VDP));

  /* Initialize internal variables */
  VKey=1;                              /* VDP address latch key */
  VDPStatus=0x9F;                      /* VDP status register   */
  FGColor=BGColor=0;                   /* Fore/Background color */
  ScrMode=0;                           /* Current screenmode    */
  CurLine=0;                           /* Current scanline      */
  ChrTab=ColTab=ChrGen=VRAM;           /* VDP tables (screen)   */
  SprTab=SprGen=VRAM;                  /* VDP tables (sprites)  */
  JoyMode=0;                           /* Joystick mode key     */
  JoyState[0]=JoyState[1]=0xFFFF;      /* Joystick states       */
  Reset76489(&PSG,Play);               /* Reset SN76489 PSG     */
  Sync76489(&PSG,PSG_SYNC);            /* Make it synchronous   */
  ResetZ80(&ccpu);                      /* Reset Z80 registers   */

  if(Verbose) emu_printf("RUNNING ROM CODE...\n");
  return(1);
}

void coc_Step(void)
{
  RunZ80(&ccpu);
}

void coc_Stop(void)
{
}


#ifdef HAS_SND

static void snd_Reset(void)
{
  emu_sndInit();
}

static void snd_Sound(int C, int F, int V)
{
  emu_sndPlaySound(C, V, F);
}
#endif

/** Joysticks ************************************************/
/** Check for keyboard events, parse them, and modify       **/
/** joystick controller status                              **/
/*************************************************************/

void SetColor(byte N,byte R,byte G,byte B)
{
  unsigned char val = R;
  XPal[N] = N; //(R&0xe0) | ((G>>3) & 0x1c) | ((B>>6) & 0x3); // RGBVAL(R,G,B); //(byte)lld_SetPaletteEntry(-1, R,G,B,0);
  emu_SetPaletteEntry(R,G,B,N);
}

void Joysticks(void)
{
  int k;
  //int j;
  int hk;
  int N=0;
  word JS[2] = { 0xFFFF,0xFFFF };

  k = emu_GetPad() & 0x7fff;
  hk = emu_ReadI2CKeyboard();

  //if (j & 0x8000) N = 1;
  //else N = 0;

  if(k)
     JS[N]=(JS[N]&0xFFF0)|(k-1);
  if(hk)
     JS[N]=(JS[N]&0xFFF0)|(hk-1);

  if (k & MASK_JOY2_BTN)
  {
          JS[N]&=0xBFFF; //Fire 1
  }
#if (defined(ILI9341) || defined(ST7789)) && defined(USE_VGA)
#else  
  if (k & MASK_KEY_USER1)
  {
          JS[N]&=0xFFBF; //Fire 2
  }
  if (k & MASK_KEY_USER2)
  {
          JS[0]=(JS[0]&0xFFF0)|(2); //1
  }
#endif  
  //   JS[0]=(JS[0]&0xFFF0)|(12);
  //   JS[0]=(JS[0]&0xFFF0)|(13);

  if (k & MASK_JOY2_DOWN)
          JS[N]&=0xFBFF; //Down
  if (k & MASK_JOY2_UP)
          JS[N]&=0xFEFF; //Up
  if (k & MASK_JOY2_RIGHT)
          JS[N]&=0xF7FF; //Right
  if (k & MASK_JOY2_LEFT)
          JS[N]&=0xFDFF; //Left

  JoyState[0]=JS[0];JoyState[1]=JS[1];
}

/** WrZ80() **************************************************/
/** Z80 emulation calls this function to write byte V to    **/
/** address A of Z80 address space.                         **/
/*************************************************************/
void WrZ80(register word A,register byte V)
{
  if((A>0x5FFF)&&(A<0x8000))
  {
    A&=0x03FF;
    RAM[0x6000+A+MEMRELOC]=RAM[0x6400+A+MEMRELOC]=RAM[0x6800+A+MEMRELOC]=RAM[0x6C00+A+MEMRELOC]=
    RAM[0x7000+A+MEMRELOC]=RAM[0x7400+A+MEMRELOC]=RAM[0x7800+A+MEMRELOC]=RAM[0x7C00+A+MEMRELOC]=V;
  }
}

/** RdZ80() **************************************************/
/** Z80 emulation calls this function to read a byte from   **/
/** address A of Z80 address space. Now  moved to z80.c and **/
/** made inlined to speed things up.                        **/
/*************************************************************/

byte RdZ80(register word A) { 
  if ( (A>=0x6000) && (A<0x10000) )
    return(RAM[A+MEMRELOC]); 
  else
    return(RAM[A]); 
}


/** PatchZ80() ***********************************************/
/** Z80 emulation calls this function when it encounters a  **/
/** special patch command (ED FE) provided for user needs.  **/
/*************************************************************/
void PatchZ80(Z80 *R) {}

/** InZ80() **************************************************/
/** Z80 emulation calls this function to read a byte from   **/
/** a given I/O port.                                       **/
/*************************************************************/
byte InZ80(register word Port)
{
  static byte KeyCodes[16] =
  {
    0x0A,0x0D,0x07,0x0C,0x02,0x03,0x0E,0x05,
    0x01,0x0B,0x06,0x09,0x08,0x04,0x0F,0x0F,
  };

  switch(Port&0xE0)
  {

case 0x40: /* Printer Status */
  if(Adam&&(Port==0x40)) return(0xFF);
  break;

case 0xE0: /* Joysticks Data */
  Port=(Port>>1)&0x01;
  Port=JoyMode?
    (JoyState[Port]>>8):
    (JoyState[Port]&0xF0)|KeyCodes[JoyState[Port]&0x0F];
  return((Port|0xB0)&0x7F);

case 0xA0: /* VDP Status/Data */
  if(Port&0x01) { Port=VDPStatus;VDPStatus&=0x5F;VKey=1; }
  else { Port=VRAM[RVAddr.W];RVAddr.W=(RVAddr.W+1)&0x3FFF; }
  return(Port);
  }

  /* No such port */
  return(NORAM);
}

/** OutZ80() *************************************************/
/** Z80 emulation calls this function to write a byte to a  **/
/** given I/O port.                                         **/
/*************************************************************/
void OutZ80(register word Port,register byte Value)
{
  static byte SR,VR; /* Sound and VDP register storage */

  switch(Port&0xE0)
  {

case 0x80: JoyMode=0;return;
case 0xC0: JoyMode=1;return;
case 0xE0: Write76489(&PSG,Value);return; 

case 0x40:
//  if(Adam&&(Port==0x40)) fputc(Value,PrnStream);
  return;

case 0xA0:
  if(Port&0x01)
    if(VKey) { VR=Value;VKey--; }
    else
    {
      VKey++;
      switch(Value&0xC0)
      {
        case 0x80: VDPOut(Value&0x07,VR);break;
        case 0x40: WVAddr.B.l=VR;WVAddr.B.h=Value&0x3F;break;
        case 0x00: RVAddr.B.l=VR;RVAddr.B.h=Value;
      }
    }
  else
    if(VKey)
    { VRAM[WVAddr.W]=Value;WVAddr.W=(WVAddr.W+1)&0x3FFF; }
  return;

  }
}

/** LoopZ80() ************************************************/
/** Z80 emulation calls this function periodically to check **/
/** if the system hardware requires any interrupts.         **/
/*************************************************************/
word LoopZ80(Z80 *R, int * ras)
{
  static byte UCount=0;
  static byte ACount=0;

  /* Next scanline */
  CurLine=(CurLine+1)%193;

  /* Refresh scanline if needed */
  if(CurLine<192)
  {
    if(!UCount) {
      (SCR[ScrMode].Refresh)(CurLine);
#if SINGLELINE_RENDERING
      emu_DrawLine(XBuf, WIDTH, HEIGHT, CurLine);
#else
#endif    
    }
    R->IPeriod=HPeriod;
    return(INT_NONE);
  }

  /* End of screen reached... */

  /* Set IPeriod to the beginning of next screen */
  R->IPeriod=VPeriod-HPeriod*192;

  /* Check joysticks */
  Joysticks();

  /* Autofire emulation */
  ACount=(ACount+1)&0x07;
  if(ACount>3)
  {
    if(AutoA) { JoyState[0]|=0x0040;JoyState[1]|=0x0040; }
    if(AutoB) { JoyState[0]|=0x4000;JoyState[1]|=0x4000; }
  }


  /* Flush any accumulated sound changes */
  Sync76489(&PSG,PSG_FLUSH);
    
  /* Refresh screen if needed */
  if(UCount) 
    UCount--; 
  else 
  { 
    UCount=UPeriod-1;
    RefreshScreen(); 
  }

  /* Setting VDPStatus flags */
  VDPStatus=(VDPStatus&0xDF)|0x80;

  /* Checking sprites: */
  if(ScrMode) CheckSprites();

  /* If exit requested, return INT_QUIT */
//  if(ExitNow) return(INT_QUIT);
  *ras = 1;
  /* Generate VDP interrupt */
  return(VKey&&(VDP[1]&0x20)? INT_NMI:INT_NONE);
}

/** VDPOut() *************************************************/
/** Emulator calls this function to write byte V into a VDP **/
/** register R.                                             **/
/*************************************************************/
void VDPOut(register byte R,register byte V)
{ 
  register byte J;

  switch(R)  
  {
    case  0: switch(((V&0x0E)>>1)|(VDP[1]&0x18))
             {
               case 0x10: J=0;break;
               case 0x00: J=1;break;
               case 0x01: J=2;break;
               case 0x08: J=3;break;
               default:   J=ScrMode;
             }   
             if(J!=ScrMode)
             {
               ChrTab=VRAM+((long)(VDP[2]&SCR[J].R2)<<10);
               ChrGen=VRAM+((long)(VDP[4]&SCR[J].R4)<<11);
               ColTab=VRAM+((long)(VDP[3]&SCR[J].R3)<<6);
               SprTab=VRAM+((long)(VDP[5]&SCR[J].R5)<<7);
               SprGen=VRAM+((long)VDP[6]<<11);
               ScrMode=J;
             }
             break;
    case  1: switch(((VDP[0]&0x0E)>>1)|(V&0x18))
             {
               case 0x10: J=0;break;
               case 0x00: J=1;break;
               case 0x01: J=2;break;
               case 0x08: J=3;break;
               default:   J=ScrMode;
             }   
             if(J!=ScrMode)
             {
               ChrTab=VRAM+((long)(VDP[2]&SCR[J].R2)<<10);
               ChrGen=VRAM+((long)(VDP[4]&SCR[J].R4)<<11);
               ColTab=VRAM+((long)(VDP[3]&SCR[J].R3)<<6);
               SprTab=VRAM+((long)(VDP[5]&SCR[J].R5)<<7);
               SprGen=VRAM+((long)VDP[6]<<11);
               ScrMode=J;
             }
             break;       
    case  2: ChrTab=VRAM+((long)(V&SCR[ScrMode].R2)<<10);break;
    case  3: ColTab=VRAM+((long)(V&SCR[ScrMode].R3)<<6);break;
    case  4: ChrGen=VRAM+((long)(V&SCR[ScrMode].R4)<<11);break;
    case  5: SprTab=VRAM+((long)(V&SCR[ScrMode].R5)<<7);break;
    case  6: V&=0x3F;SprGen=VRAM+((long)V<<11);break;
    case  7: FGColor=V>>4;BGColor=V&0x0F;break;

  }
  VDP[R]=V;return;
} 

/** CheckSprites() *******************************************/
/** This function is periodically called to check for the   **/
/** sprite collisions and 5th sprite, and set appropriate   **/
/** bits in the VDP status register.                        **/
/*************************************************************/
void CheckSprites(void)
{
  register word LS,LD;
  register byte DH,DV,*PS,*PD,*T;
  byte I,J,N,*S,*D;

  VDPStatus=(VDPStatus&0x9F)|0x1F;
  for(N=0,S=SprTab;(N<32)&&(S[0]!=208);N++,S+=4);

  if(Sprites16x16)
  {
    for(J=0,S=SprTab;J<N;J++,S+=4)
      if(S[3]&0x0F)
        for(I=J+1,D=S+4;I<N;I++,D+=4)
          if(D[3]&0x0F)
          {
            DV=S[0]-D[0];
            if((DV<16)||(DV>240))
            {
              DH=S[1]-D[1];
              if((DH<16)||(DH>240))
              {
                PS=SprGen+((long)(S[2]&0xFC)<<3);
                PD=SprGen+((long)(D[2]&0xFC)<<3);
                if(DV<16) PD+=DV; else { DV=256-DV;PS+=DV; }
                if(DH>240) { DH=256-DH;T=PS;PS=PD;PD=T; }
                while(DV<16)
                {
                  LS=((word)*PS<<8)+*(PS+16);
                  LD=((word)*PD<<8)+*(PD+16);
                  if(LD&(LS>>DH)) break;
                  else { DV++;PS++;PD++; }
                }
                if(DV<16) { VDPStatus|=0x20;return; }
              }
            }
          }
  }
  else
  {
    for(J=0,S=SprTab;J<N;J++,S+=4)
      if(S[3]&0x0F)
        for(I=J+1,D=S+4;I<N;I++,D+=4)
          if(D[3]&0x0F)
          {
            DV=S[0]-D[0];
            if((DV<8)||(DV>248))
            {
              DH=S[1]-D[1];
              if((DH<8)||(DH>248))
              {
                PS=SprGen+((long)S[2]<<3);
                PD=SprGen+((long)D[2]<<3);
                if(DV<8) PD+=DV; else { DV=256-DV;PS+=DV; }
                if(DH>248) { DH=256-DH;T=PS;PS=PD;PD=T; }
                while((DV<8)&&!(*PD&(*PS>>DH))) { DV++;PS++;PD++; }
                if(DV<8) { VDPStatus|=0x20;return; }
              }
            }
          }
  }
}


/** Play() ***************************************************/
/** Log and play sound of given frequency (Hz) and volume   **/
/** (0..255) via given channel (0..3).                      **/
/*************************************************************/
void Play(int C,int F,int V)
{
  /* Play actual sound */
#ifdef HAS_SND  
  snd_Sound(C,F,V);
#endif  
}


/** RefreshScreen() ******************************************/
/** Refresh screen. This function is called in the end of   **/
/** refresh cycle to show the entire screen.                **/
/*************************************************************/
void RefreshScreen(void) 
{ 
#if SINGLELINE_RENDERING
#else
  emu_DrawScreen(XBuf, WIDTH, HEIGHT, WIDTH);
#endif
  emu_DrawVsync();  
}

/** RefreshBorder() ******************************************/
/** This function is called from RefreshLine#() to refresh  **/
/** the screen border.                                      **/
/*************************************************************/
void RefreshBorder(register byte Y)
{
//  if(!Y)
//    memset(XBuf,XPal[BGColor],WIDTH*(HEIGHT-192)/2);
//  if(Y==191)
//    memset(XBuf+WIDTH*(HEIGHT+192)/2,XPal[BGColor],WIDTH*(HEIGHT-192)/2);  
}

/** RefreshSprites() *****************************************/
/** This function is called from RefreshLine#() to refresh  **/
/** sprites.                                                **/
/*************************************************************/
void RefreshSprites(register byte Y)
{
  register byte C,H;
  register byte *P,*PT,*AT;
  register int L,K;
  register unsigned int M;

  H=Sprites16x16? 16:8;
  C=0;M=0;L=0;AT=SprTab-4;
  do
  {
    M<<=1;AT+=4;L++;    /* Iterating through SprTab */
    K=AT[0];            /* K = sprite Y coordinate */
    if(K==208) break;   /* Iteration terminates if Y=208 */
    if(K>256-H) K-=256; /* Y coordinate may be negative */

    /* Mark all valid sprites with 1s, break at 4 sprites */
    if((Y>K)&&(Y<=K+H)) { M|=1;if(++C==4) break; }
  }
  while(L<32);

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

#if SINGLELINE_RENDERING
        P=XBuf+L;
#else
        P=XBuf+WIDTH*(HEIGHT-192)/2+(WIDTH-256)/2+WIDTH*Y+L;
#endif
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

/** RefreshLine0() *******************************************/
/** Refresh line Y (0..191) of SCREEN0, including sprites   **/
/** in this line.                                           **/
/*************************************************************/
void RefreshLine0(register byte Y)
{
  register byte X,K,Offset,FC,BC;
  register byte *P,*T;
#if SINGLELINE_RENDERING
  P=XBuf;
#else
  P=XBuf+WIDTH*(HEIGHT-192)/2+WIDTH*Y;
#endif  
  XPal[0]=BGColor? XPal[BGColor]:XPal0;

  if(!ScreenON) memset(P,XPal[BGColor],WIDTH);
  else
  {
    BC=XPal[BGColor];
    FC=XPal[FGColor];
    T=ChrTab+(Y>>3)*40;
    Offset=Y&0x07;

    //memset(P,BC,(WIDTH-240)/2);
    //P+=(WIDTH-240)/2;

    for(X=0;X<40;X++)
    {
      K=ChrGen[((int)*T<<3)+Offset];
      P[0]=K&0x80? FC:BC;P[1]=K&0x40? FC:BC;
      P[2]=K&0x20? FC:BC;P[3]=K&0x10? FC:BC;
      P[4]=K&0x08? FC:BC;P[5]=K&0x04? FC:BC;
      P+=6;T++;
    }

    //memset(P,BC,(WIDTH-240)/2);
  }

  //RefreshBorder(Y);
}

/** RefreshLine1() *******************************************/
/** Refresh line Y (0..191) of SCREEN1, including sprites   **/
/** in this line.                                           **/
/*************************************************************/
void RefreshLine1(register byte Y)
{
  register byte X,K,Offset,FC,BC;
  register byte *P,*T;

#if SINGLELINE_RENDERING
  P=XBuf;
#else
  P=XBuf+WIDTH*(HEIGHT-192)/2+WIDTH*Y;
#endif  
  XPal[0]=BGColor? XPal[BGColor]:XPal0;

  if(!ScreenON) memset(P,XPal[BGColor],WIDTH);
  else
  {
    T=ChrTab+(Y>>3)*32;
    Offset=Y&0x07;

    //memset(P,XPal[BGColor],(WIDTH-256)/2);
    //P+=(WIDTH-256)/2;

    for(X=0;X<32;X++)
    {
      K=*T;
      BC=ColTab[K>>3];
      K=ChrGen[((int)K<<3)+Offset];
      FC=XPal[BC>>4];
      BC=XPal[BC&0x0F];
      P[0]=K&0x80? FC:BC;P[1]=K&0x40? FC:BC;
      P[2]=K&0x20? FC:BC;P[3]=K&0x10? FC:BC;
      P[4]=K&0x08? FC:BC;P[5]=K&0x04? FC:BC; 
      P[6]=K&0x02? FC:BC;P[7]=K&0x01? FC:BC;
      P+=8;T++;
    }

    //memset(P,XPal[BGColor],(WIDTH-256)/2);
    RefreshSprites(Y);
  }

  RefreshBorder(Y);
}

/** RefreshLine2() *******************************************/
/** Refresh line Y (0..191) of SCREEN2, including sprites   **/
/** in this line.                                           **/
/*************************************************************/
void RefreshLine2(register byte Y)
{
  register byte X,K,FC,BC,Offset;
  register byte *P,*T,*PGT,*CLT;
  register int I;

#if SINGLELINE_RENDERING
  P=XBuf;
#else
  P=XBuf+WIDTH*(HEIGHT-192)/2+WIDTH*Y;
#endif  
  XPal[0]=BGColor? XPal[BGColor]:XPal0;

  if(!ScreenON) memset(P,XPal[BGColor],WIDTH);
  else
  {
    I=(int)(Y&0xC0)<<5;
    PGT=ChrGen+I;
    CLT=ColTab+I;
    T=ChrTab+(Y>>3)*32;
    Offset=Y&0x07;

    //memset(P,XPal[BGColor],(WIDTH-256)/2);
    //P+=(WIDTH-256)/2;

    for(X=0;X<32;X++)
    {
      I=((int)*T<<3)+Offset;
      K=PGT[I];
      BC=CLT[I];
      FC=XPal[BC>>4];
      BC=XPal[BC&0x0F];
      P[0]=K&0x80? FC:BC;P[1]=K&0x40? FC:BC;
      P[2]=K&0x20? FC:BC;P[3]=K&0x10? FC:BC;
      P[4]=K&0x08? FC:BC;P[5]=K&0x04? FC:BC;
      P[6]=K&0x02? FC:BC;P[7]=K&0x01? FC:BC;
      P+=8;T++;
    }

    //memset(P,XPal[BGColor],(WIDTH-256)/2);
    RefreshSprites(Y);
  }    

  RefreshBorder(Y);
}

/** RefreshLine3() *******************************************/
/** Refresh line Y (0..191) of SCREEN3, including sprites   **/
/** in this line.                                           **/
/*************************************************************/
void RefreshLine3(register byte Y)
{
  register byte X,K,Offset;
  register byte *P,*T;

#if SINGLELINE_RENDERING
  P=XBuf;
#else
  P=XBuf+WIDTH*(HEIGHT-192)/2+WIDTH*Y;
#endif  
  XPal[0]=BGColor? XPal[BGColor]:XPal0;

  if(!ScreenON) memset(P,XPal[BGColor],WIDTH);
  else
  {
    T=ChrTab+(Y>>3)*32;
    Offset=(Y&0x1C)>>2;

    //memset(P,XPal[BGColor],(WIDTH-256)/2);
    //P+=(WIDTH-256)/2;

    for(X=0;X<32;X++)
    {
      K=ChrGen[((int)*T<<3)+Offset];
      P[0]=P[1]=P[2]=P[3]=XPal[K>>4];
      P[4]=P[5]=P[6]=P[7]=XPal[K&0x0F];
      P+=8;T++;
    }

    //memset(P,XPal[BGColor],(WIDTH-256)/2);
    RefreshSprites(Y);
  }

  RefreshBorder(Y);
}
