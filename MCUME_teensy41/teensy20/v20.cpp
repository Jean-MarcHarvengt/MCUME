#include <stdio.h>
#include <string.h>

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}
#include <Arduino.h>


// Vic 20 emulation includes
#include "mos6502.h"
#include "MOS6561.h"
#include "MOS6522.h"
#include "MOS6502Memory.h"
#include "kernalromp.h"
#include "charrom.h"
#include "basicrom.h"

static mos6502 mos;
static MOS6561 mos6561;
static MOS6522 mos6522;
uint8_t vicmemory[0x10000];;


#define VIC20FREQBASE    65535

/* noise magic */
#define NSHIFT(v, n) (((v)<<(n))|((((v)>>(23-(n)))^(v>>(18-(n))))&((1<<(n))-1)))
#define NVALUE(v) (noiseLSB[v&0xff]|noiseMID[(v>>8)&0xff]|noiseMSB[(v>>16)&0xff])
#define NSEED 0x7ffff8

/* Noise tables */
#define NOISETABLESIZE 256
static uint8_t noiseMSB[NOISETABLESIZE];
static uint8_t noiseMID[NOISETABLESIZE];
static uint8_t noiseLSB[NOISETABLESIZE];

/* needed data for one voice */
typedef struct voice_s
{
  /* counter value */
  int    f;
  /* counter step / sample */
  int    fs;
  /* noise shift register. Note! rv may be 0 to 15 shifts 'behind' the
     real noise shift register value. Remaining shifts are done when
     it is referenced */
  int    rv;
} voice_t;

/* needed data for SID */
struct sound_s
{
  /* number of voices */
  voice_t    v[4];
  /* 4-bit volume value */
  uint8_t     vol;
  /* internal constant used for sample rate dependent calculations */
  int    speed1;
};

static sound_s psid;

static void VIC_VOICE_HANDLE(int voice, int value, int shift)
{
  int div = 255 - value;
  if (!div)
    div = 127;
  if (!(value & 0x80))
    psid.v[voice].fs = 0;
  else
    psid.v[voice].fs = psid.speed1*VIC20FREQBASE*(1 << shift)/div;
};


void SND_Process(void * stream, int len) {
  int   o0, o1, o2, o3;

  short * sndbuf =  (short *)stream;
  for (int i = 0; i < len/2; i++)
  {
    /* addfptrs */
    psid.v[0].f += psid.v[0].fs;
    psid.v[1].f += psid.v[1].fs;
    psid.v[2].f += psid.v[2].fs;
    psid.v[3].f += psid.v[3].fs;
    /* noise */
    if (psid.v[3].f < psid.v[3].fs)
        psid.v[3].rv = NSHIFT(psid.v[3].rv, 16);
    /* voices */
    o0 = (psid.v[0].f & 0x80000000) >> 2;
    o1 = (psid.v[1].f & 0x80000000) >> 2;
    o2 = (psid.v[2].f & 0x80000000) >> 2;
    o3 = (int)NVALUE(NSHIFT(psid.v[3].rv, psid.v[3].f >> 28)) << 22;
    /* sample */
    uint16_t s = ((int)((o0+o1+o2+o3)>>20)-0x800)*psid.vol;
    *sndbuf++ =s;
    *sndbuf++ =s;
  } 
}

static char * strext(char * filepath) {
  int str_len = strlen(filepath);
  for (int i=0; i<str_len; i++) { 
    if ( (filepath[i] == '.') && (i != (str_len-1) ) )  {
      return &filepath[i+1];
    }  
  }
  return filepath;
}

static void loadROM(char * filepath, uint16_t atMemoryLocation) {
  uint16_t loadAddress;
  uint16_t endAddress;
  int lSize = emu_FileSize(filepath);
  int f = emu_FileOpen(filepath,"r+b");
  char * ext = strext(filepath);
  if ( (ext[0] == 'p') ||  (ext[0] == 'P') ) {
    unsigned char buffer[2];
    emu_FileRead((char *)&buffer[0], 2,f);
    loadAddress = buffer[0] | buffer[1] << 8;
    endAddress = loadAddress + lSize-2;
    emu_FileRead((char *)&vicmemory[loadAddress], lSize-2, f);
  }
  else {
    switch (ext[0]) {
      case '2':
        loadAddress = 0x2000;
        break;
      case '4':
        loadAddress = 0x4000;
        break;
      case '6':
        loadAddress = 0x6000;
        break;
      case 'a':
      case 'A':
        loadAddress = 0xA000;
        break;
    }
    endAddress = loadAddress + lSize;  
    emu_FileRead((char *)&vicmemory[loadAddress], lSize,f);
  }
  emu_FileClose(f);
  
  // Set basic memory pointers
  silentWriteDWord(0x2B, loadAddress);
  silentWriteDWord(0x2D, loadAddress);
  silentWriteDWord(0x2F, endAddress);
  silentWriteDWord(0x31, endAddress);
  silentWriteDWord(0x33, endAddress);
  silentWriteDWord(0x37, 0xFFFF);
  silentWriteDWord(0xAE, endAddress);
  
}


/*VIC-20 KEYBOARD MATRIX

In my implementation, n = 0xF7EF

         (80      40      20      10      08      04      02      01)
          7f      bf      df      ef      f7      fb      fd      fe
__
01  fe    2       q       CBM     Space   RunStop Control Lft_arr 1
02  fd    4       e       s       z       Shift_L a       w       3
04  fb    6       t       f       c       x       d       r       5
08  f7    8       u       h       b       v       g       y       7
10  ef    0       o       k       m       n       j       i       9
20  df    -       @       :       .       ,       l       p       +
40  bf    Home    Up_arr  =       Shift_R /       ;       *       GBP
80  7f    F7      F5      F3      F1      Down    Right   Return  Del
--
*/

const uint32_t ascii2scan[] = {
 //0 1 2 3 4 5 6 7 8 9 A B C D E F
   0,0,0,0,0,0,0,0,0,0,0xfd7f,0,0,0,0,0, // return
   //                            31:RUNSTOP
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf7fe,
   //sp    !       "       #       $       %       &       '       (       )       *       +       ,       -       .       / 
   0xeffe ,0x1fefe,0x17ffe,0x1fe02,0x17ffd,0x1fefb,0x17ffb,0x1fef7,0x17ff7,0x1feef,0xfdbf ,0xfedf ,0xf7df ,0x7fDf ,0xefdf ,0xf7bf,
   //0     1       2       3       4       5       6       7       8       9       :       ;       <       =       >       ?
   0x7fef ,0xfefe ,0x7ffe ,0xfefd ,0x7ffd ,0xfefb ,0x7ffb ,0xfef7 ,0x7ff7 ,0xfeef ,0xdfdf ,0xfbbf ,0x1f7df,0xdfbf ,0x1efdf,0x1f7bf,
   //@     A       B       C       D       E       F       G       H       I       J       K       L       M       N       O
   0xbfdf ,0xfbfd ,0xeff7 ,0xeffb ,0xfbfb ,0xbffd ,0xdffb ,0xfbf7 ,0xdff7 ,0xfdef ,0xfbef ,0xdfef ,0xfbdf ,0xefef ,0xf7ef ,0xbfef,
   //P     Q       R       S       T       U       V       W       X       Y       Z       [       \       ]       ^       _  
   0xfddf ,0xbffe ,0xfdfb ,0xdffd ,0xbffb ,0xbff7 ,0xf7f7 ,0xfdfd ,0xf7fb ,0xfdf7 ,0xeffd ,0x1dfdf,0xffff ,0x1fbbf,0 ,0,
   //'     a       c       c       d       e       f       g       h       i       j       k       l       m       n       o
   0      ,0xfbfd ,0xeff7 ,0xeffb ,0xfbfb ,0xbffd ,0xdffb ,0xfbf7 ,0xdff7 ,0xfdef ,0xfbef ,0xdfef ,0xfbdf ,0xefef ,0xf7ef ,0xbfef,
   //p     q       r       s       t       u       v       w       x       y       z       {       |       }       ~  DEL  
   0xfddf ,0xbffe ,0xfdfb ,0xdffd ,0xbffb ,0xbff7 ,0xf7f7 ,0xfdfd ,0xf7fb ,0xfdf7 ,0xeffd ,0,      0 ,     0,      0xfe7f ,0,
   // 129:f1 f2      f3     f4      f5     f6      f7     f8 
   0,0xef7f,0x1ef7f,0xdf7f,0x1df7f,0xbf7f,0x1bf7f,0x7f7f,0x17f7f,0,0,0,0,0,0,0,  // 128-143
   //           150:right  left    down   up                 157:left
   0,0,0,0,0,0, 0xfb7f,0x1fb7f,0xf77f,0x1f77f,0,0,0,0,0,0   // 144-159 
};


static int ik;    // joypad key
static int pik=0; 

static int ihk;   // I2C keyboard key
static int iusbhk;// USB keyboard key

void v20_Input(int bClick) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard();
}

void emu_KeyboardOnDown(int keymodifer, int key) {
  int keyCode = -1; //INV_KEY;
  if ((key >=0xc0) && (key <=0xdf)) {
    keyCode = ((key-0xc0) & 0x1f) + 0x7f;
  }
  else {
    keyCode = key & 0x7f;
  }
  
  //Serial.println(keyCode);
  
  if (keyCode != -1) {
    iusbhk = keyCode;
  }
}

void emu_KeyboardOnUp(int keymodifer, int key) {
  iusbhk = 0;
}


/*
uint8_t readWord( uint16_t location) {
  switch (location) {
    case 0x9111:
    case 0x911F:
      mos6522.joy1Input();    
      break;    
    case 0x9120:
      mos6522.joy2Input();    
      break;    
    case 0x9121:
    //case 0x912F:
      mos6522.keyboardInput();    
      break;    
  }  
  return vicmemory[location];
}

void writeWord( uint16_t location, uint8_t value){
  vicmemory[location] = value; 
}
*/


void v20_Init(void)
{

  // Initialize emulation objects
  mos.Reset();
  mos6561.initialize();
  mos6522.setCpu(&mos);
  mos6522.initialize();

  // clear memory
  for(int i=0; i<sizeof(vicmemory); i++) {
    vicmemory[i] = 0;
  }
  // load charrom
  for(int i=0; i<4096; i++) {
    vicmemory[0x8000+i] = charrom[i];
  }
  // load basic rom
  for(int i=0; i<8192; i++) {
    vicmemory[0xC000+i] = basicrom[i];
  }
  // load kernal rom
  for(int i=0; i<8192; i++) {
    vicmemory[0xE000+i] = kernalrom[i];
  }

#ifdef HAS_SND  
  psid.speed1 = (1000000 << 8) / 22000;
  psid.v[3].rv = NSEED;
  for (int i = 0; i < NOISETABLESIZE; i++)
  {
    noiseLSB[i] = (((i>>(7-2))&0x04)|((i>>(4-1))&0x02)|((i>>(2-0))&0x01));
    noiseMID[i] = (((i>>(13-8-4))&0x10)|((i<<(3-(11-8)))&0x08));
    noiseMSB[i] = (((i<<(7-(22-16)))&0x80)|((i<<(6-(20-16)))&0x40)
            |((i<<(5-(16-16)))&0x20));
  }  
  emu_sndInit();
#endif  
}


void v20_Start(char * filename)
{
  emu_printf("emu starting");

  loadROM(filename,0);

  // Reset cpu
  mos.Reset();
  // Execute boot sequence before doing much else
  mos.Run(20000);
  
  emu_printf("emu started");
}

void v20_Step(void)
{
  do  {    
    // bind emulation to clock speed

    // Cpu clock is four times less than 6561 clock
    //int cpuCycles = mos.executeInstruction();
    
    uint64_t cpuCycles = mos.Run(1);
    mos6561.tick(1);
    mos6522.tick();
    mos6522.tick();

    /*
    uint64_t cpuCycles = mos.Run(mos6561.cyclesPerScanline);
    mos6561.tick(mos6561.cyclesPerScanline);
    mos6522.tick();
    mos6522.tick();
    mos6522.tick();
    mos6522.tick();
    mos6522.tick();
    mos6522.tick();
    */

  /*
    // printf("cycles %d\n", cpuCycles);
    while (cpuCycles) {
      cpuCycles--;
      mos6522.tick(); 
  mos6522.tick();
    } 
  */

  } while ((!mos6561.isFrameReady()));

  // Swap buffers if frame ready
  if (mos6561.isFrameReady()) {
      mos6561.renderFrame();
  }    

  // Top basic
  // Unexpanded
  //vicmemory[55] = 0;
  //vicmemory[56] = 0x1E;
  // +3k
  //vicmemory[55] = 0;
  //vicmemory[56] = 0x1E;
  // +8k
  //vicmemory[55] = 0;
  //vicmemory[56] = 0x40;
  // +16k
  //vicmemory[55] = 0;
  //vicmemory[56] = 0x60;
  // +24k
  //vicmemory[55] = 0;
  //vicmemory[56] = 0x80;
  // Bottom basic ???
  //vicmemory[51] = 0;
  //vicmemory[52] = 0x1E;

  emu_DrawVsync();
 
  int hk=ihk;
  if (iusbhk) hk = iusbhk;

  if (hk) {
    int scan = ascii2scan[hk];
    if (scan & 0x10000) mos6522.setShiftPressed(true);
    else mos6522.setShiftPressed(false);  
    mos6522.setKeyPressed(scan & 0xffff);
  }
  else {
    mos6522.setShiftPressed(false);  
    mos6522.setKeyPressed(0);   
  }


  int k=ik; 
#ifdef TEECOMPUTER
  // Ignore joypad if shift/fn is pressed!!!
  if ( !(k & MASK_KEY_USER1) && !(k & MASK_KEY_USER2) )
#endif
  {
    if ( !(pik & MASK_JOY2_BTN) && (k & MASK_JOY2_BTN) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Fire, true);
    }
    else if ( (pik & MASK_JOY2_BTN) && !(k & MASK_JOY2_BTN) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Fire, false);
    } 
    if ( !(pik & MASK_JOY2_UP) && (k & MASK_JOY2_UP) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Up, true);
    }
    else if ( (pik & MASK_JOY2_UP) && !(k & MASK_JOY2_UP) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Up, false);
    } 
    if ( !(pik & MASK_JOY2_DOWN) && (k & MASK_JOY2_DOWN) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Down, true);
    }
    else if ( (pik & MASK_JOY2_DOWN) && !(k & MASK_JOY2_DOWN) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Down, false);
    } 
    if ( !(pik & MASK_JOY2_RIGHT) && (k & MASK_JOY2_RIGHT) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Left, true);
    }
    else if ( (pik & MASK_JOY2_RIGHT) && !(k & MASK_JOY2_RIGHT) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Left, false);
    } 
    if ( !(pik & MASK_JOY2_LEFT) && (k & MASK_JOY2_LEFT) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Right, true);
    }
    else if ( (pik & MASK_JOY2_LEFT) && !(k & MASK_JOY2_LEFT) ) {
      mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Right, false);
    } 
  }  

#ifndef TEECOMPUTER
  if ( !(pik & MASK_KEY_USER1) && (k & MASK_KEY_USER1) ) {
    mos6522.setKeyPressed(0xEF7F);
    mos6522.setShiftPressed(true);  
  }
  else if ( (pik & MASK_KEY_USER1) && !(k & MASK_KEY_USER1) ) {
    mos6522.setShiftPressed(false);  
    mos6522.setKeyPressed(0);
  } 

  if ( !(pik & MASK_KEY_USER2) && (k & MASK_KEY_USER2) ) {
    mos6522.setShiftPressed(false);  
    mos6522.setKeyPressed(ascii2scan['1']);
  }
  else if ( (pik & MASK_KEY_USER2) && !(k & MASK_KEY_USER2) ) {
    mos6522.setShiftPressed(false);  
    mos6522.setKeyPressed(0);
  }
#endif
  pik = k;

#ifdef HAS_SND  
  VIC_VOICE_HANDLE(0, vicmemory[mos6561.Audio1Register], 0);
  VIC_VOICE_HANDLE(1, vicmemory[mos6561.Audio2Register], 1);
  VIC_VOICE_HANDLE(2, vicmemory[mos6561.Audio3Register], 2);
  VIC_VOICE_HANDLE(3, vicmemory[mos6561.Audio4Register], 0);
  int vol = (vicmemory[mos6561.AudioVolRegister] & 0x0f);
  psid.vol = vol;
#endif  
}


 
