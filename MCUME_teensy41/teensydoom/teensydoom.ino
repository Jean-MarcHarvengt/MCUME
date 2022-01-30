#include "iopins.h"  
#include "emuapi.h"  
extern "C" {
#include "doom.h"
}

#ifdef HAS_T4_VGA
#include "vga_t_dma.h"
TFT_T_DMA tft;
#else
#include "tft_t_dma.h"
TFT_T_DMA tft = TFT_T_DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO, TFT_TOUCH_CS, TFT_TOUCH_INT);
#endif

static IntervalTimer myTimer;
static unsigned char  palette8[PALETTE_SIZE];
#ifndef HAS_T4_VGA
static unsigned short  palette16[PALETTE_SIZE];
#endif
volatile boolean vbl=true;
static int skip=0;

static unsigned long long mscount=0;
volatile unsigned int systime;
int joystick=0;
static int usbjoystick=0;

static void vblCount() {
  if (vbl) {
    vbl = false;
  } else {
    vbl = true;
  }   
  mscount += 20;
  systime += 20;
}

void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index)
{
  if (index<PALETTE_SIZE) {
    palette8[index]  = RGBVAL8(r,g,b);  
  }
}

void * emu_LineBuffer(int line)
{
#ifdef HAS_T4_VGA
    return(NULL);
#else
    return (void*)tft.getLineBuffer(line);    
#endif  
}

void emu_DrawLine(unsigned char * VBuf, int width, int height, int line) 
{
#ifdef HAS_T4_VGA
    tft.writeLine(width,1,line, VBuf, palette8);
#else
    tft.writeLine(width,1,line, VBuf, palette16);
#endif
}  

void emu_DrawLine8(unsigned char * VBuf, int width, int height, int line) 
{
    if (skip==0) {
#ifdef HAS_T4_VGA
      tft.writeLine(width,height,line, VBuf);
#endif      
    }   
} 

void emu_DrawLine16(unsigned short * VBuf, int width, int height, int line) 
{
    if (skip==0) {
#ifdef HAS_T4_VGA
      tft.writeLine16(width,height,line, VBuf);
#else
      tft.writeLine(width,height,line, VBuf);
#endif      
    }   
} 

void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride) 
{
    if (skip==0) {
#ifdef HAS_T4_VGA
      tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette8);
#else
      tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette16);
#endif
    }
}

int emu_FrameSkip(void)
{
  return skip;
}

void emu_DrawVsync(void)
{
  skip += 1;
  skip &= VID_FRAME_SKIP;
#ifdef HAS_T4_VGA
  tft.waitSync();
#else
  volatile boolean vb=vbl;
  while (vbl==vb) {};
#endif
}


// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {

#ifdef HAS_T4_VGA
  tft.begin(VGA_MODE_320x240);
  //NVIC_SET_PRIORITY(IRQ_QTIMER3, 255);
#else
  tft.begin();
#endif  
  
  emu_init(); 

  myTimer.begin(vblCount, 20000);  //to run every 20ms  
}


// ****************************************************
// the loop() method runs continuously
// ****************************************************
void loop(void) 
{
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * wad = menuSelection();
    if (action == ACTION_RUN1) {
      toggleMenu(false);
      tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
      tft.startDMA();
      D_DoomMain(wad);  
      Serial.println("init end");      
      usbjoystick=0;
      emu_start();
    }           
    delay(20);
  } 
  else { 
    joystick = 0;
    // joystick variable is used by doom for directions and keys
    // it uses the following bits:
    // 0x01: up
    // 0x02: down
    // 0x04: left
    // 0x08: right
    // 0x10: fire
    // 0x20: return
    // 0x40: use
    // 0x80: map
    int k = usbjoystick;
    if (k==0) { 
      k = emu_ReadKeys();
      int hk = emu_ReadI2CKeyboard();
      if (hk != 0) {
        switch (hk) {
          case 'q':
            joystick|=0x20; // RETURN
            break;
          case 'w':
            joystick|=0x40; // USE
            break;
          case 'e':
            joystick|=0x80; // MAP
            break;
        }
      }
    }
    if ( (k & MASK_JOY1_DOWN)  || (k & MASK_JOY2_DOWN) )  joystick|=0x02;
    if ( (k & MASK_JOY1_UP)    || (k & MASK_JOY2_UP) )    joystick|=0x01;
    if ( (k & MASK_JOY1_LEFT)  || (k & MASK_JOY2_LEFT) )  joystick|=0x04;
    if ( (k & MASK_JOY1_RIGHT) || (k & MASK_JOY2_RIGHT) ) joystick|=0x08;
    if ( (k & MASK_JOY1_BTN)   || (k & MASK_JOY2_BTN) )   joystick|=0x10; 
    if ( (k & MASK_KEY_USER1) ) joystick|=0x20;
    if ( (k & MASK_KEY_USER2) ) joystick|=0x40;
    if ( (k & MASK_KEY_USER3) ) joystick|=0x80;
    D_DoomLoop();
    emu_DrawVsync(); 
  } 
}


void emu_KeyboardOnDown(int keymodifer, int key) {
  if (key == 218) usbjoystick|=MASK_JOY1_UP;         // UP
  else if (key == 217) usbjoystick|=MASK_JOY1_DOWN;  // DOWN
  else if (key == 216) usbjoystick|=MASK_JOY1_LEFT;  // LEFT
  else if (key == 215) usbjoystick|=MASK_JOY1_RIGHT; // RIGHT
  else if (key == 102) usbjoystick|=MASK_JOY1_BTN;   // FIRE
  else if (key == 10) usbjoystick|=MASK_KEY_USER1;   // RETURN  
  else if (key == 32) usbjoystick|=MASK_KEY_USER2;   // USE  
  else if (key == 9) usbjoystick|=MASK_KEY_USER3;    // MAP (tab) 
}

void emu_KeyboardOnUp(int keymodifer, int key) {
  if (key == 218) usbjoystick&=~MASK_JOY1_UP;         // UP
  else if (key == 217) usbjoystick&=~MASK_JOY1_DOWN;  // DOWN
  else if (key == 216) usbjoystick&=~MASK_JOY1_LEFT;  // LEFT
  else if (key == 215) usbjoystick&=~MASK_JOY1_RIGHT; // RIGHT
  else if (key == 102) usbjoystick&=~MASK_JOY1_BTN;   // FIRE
  else if (key == 10) usbjoystick&=~MASK_KEY_USER1;   // RETURN
  else if (key == 32) usbjoystick&=~MASK_KEY_USER2;   // USE    
  else if (key == 9) usbjoystick&=~MASK_KEY_USER3;    // MAP (tab)  
}



EXTMEM  unsigned char MemPool[8*1024*1024];


extern "C" void emu_GetTimeOfDay(int * usec, int * sec) {
  unsigned long long count=mscount;
  int lsec = count / 1000;
  int lusec = (count % 1000)*1000;
  * usec = lusec;
  * sec = lsec;
}
