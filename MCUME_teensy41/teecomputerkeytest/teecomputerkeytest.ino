#include "emuapi.h"  
#include "iopins.h"  

#ifdef HAS_T4_VGA
#include "vga_t_dma.h"
TFT_T_DMA tft;
#else
#include "tft_t_dma.h"
TFT_T_DMA tft = TFT_T_DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO, TFT_TOUCH_CS, TFT_TOUCH_INT);
#endif

bool vgaMode = false;

static unsigned char  palette8[PALETTE_SIZE];
static unsigned short palette16[PALETTE_SIZE];
static IntervalTimer myTimer;
volatile boolean vbl=true;
static int skip=0;
static elapsedMicros tius;


#define BLUE       RGBVAL16(0, 0, 170)
#define LIGHT_BLUE RGBVAL16(0, 136, 255)
static int fb_width, fb_height;
static const char * digits = "0123456789ABCDEF";
static int hk = 0;
static int prevhk = 0;
static int col=0;
static int row=0;


static void vblCount() { 
  if (vbl) {
    vbl = false;
  } else {
    vbl = true;
  }
  uint16_t bClick = emu_DebounceLocalKeys();
  hk = emu_ReadI2CKeyboard();
  //emu_Input(bClick);   
}

void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index)
{
  if (index<PALETTE_SIZE) {
    palette8[index]  = RGBVAL8(r,g,b);
    palette16[index] = RGBVAL16(r,g,b);    
  }
}

void emu_DrawVsync(void)
{
  volatile boolean vb=vbl;
  skip += 1;
  skip &= VID_FRAME_SKIP;
  if (!vgaMode) {
#ifdef HAS_T4_VGA
    tft.waitSync();
#else
    while (vbl==vb) {};
#endif
  }
}

void emu_DrawLine(unsigned char * VBuf, int width, int height, int line) 
{
  if (!vgaMode) {
#ifdef HAS_T4_VGA
    tft.writeLine(width,1,line, VBuf, palette8);
#else
    tft.writeLine(width,1,line, VBuf, palette16);
#endif
  }  
}  

void emu_DrawLine8(unsigned char * VBuf, int width, int height, int line) 
{
  if (!vgaMode) {
    if (skip==0) {
#ifdef HAS_T4_VGA
      tft.writeLine(width,height,line, VBuf);
#endif      
    }
  }     
} 

void emu_DrawLine16(unsigned short * VBuf, int width, int height, int line) 
{
  if (!vgaMode) {
    if (skip==0) {
#ifndef HAS_T4_VGA
      tft.writeLine(width,height,line, VBuf);
#endif      
    }
  }    
} 

void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride) 
{
  if (!vgaMode) {
    if (skip==0) {
#ifdef HAS_T4_VGA
      tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette8);
#else
      tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette16);
#endif
    }
  } 
}

void emu_CopyLine(int width, int height, int ysrc, int ydst)
{
#ifdef HAS_T4_VGA
  tft.copyLine(width,height,ysrc,ydst);
#endif  
}

int emu_FrameSkip(void)
{
  return skip;
}

void * emu_LineBuffer(int line)
{
  if (!vgaMode) {
    return (void*)tft.getLineBuffer(line);    
  }
}

void emu_tweakVideo(int shiftdelta, int numdelta, int denomdelta) {
#ifdef HAS_T4_VGA
  tft.tweak_video(shiftdelta, numdelta, denomdelta);
#endif  
}



// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {

#ifdef HAS_T4_VGA
#ifdef HIRES
  tft.begin(VGA_MODE_640x480);
#else
  tft.begin(VGA_MODE_320x240);
#endif
  //NVIC_SET_PRIORITY(IRQ_QTIMER3, 0);
#else
  tft.begin();
#endif  
  
  emu_init(); 
  emu_start(); 
  tft.fillScreenNoDma(LIGHT_BLUE);

  tft.get_frame_buffer_size(&fb_width, &fb_height);
  tft.drawRectNoDma((fb_width-320)/2,(fb_height-200)/2, 320,200, BLUE);

  myTimer.begin(vblCount, 20000);  //to run every 20ms
  toggleOSKB(true);  
}

void loop(void) 
{
  char buf[5] = {0,0,0,0,0};
/*  
  uint16_t bClick = emu_GetPad();

  buf[0] = digits[(bClick>>12)&0xf];
  buf[1] = digits[(bClick>>8)&0xf];
  buf[2] = digits[(bClick>>4)&0xf];
  buf[3] = digits[bClick&0xf];
  tft.drawTextNoDma(4*8,0,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false);

  buf[3] = 0;
  int key = emu_ReadI2CKeyboard();
  buf[0] = digits[(key>>8)&0xf];
  buf[1] = digits[(key>>4)&0xf];
  buf[2] = digits[key&0xf];        
  tft.drawTextNoDma(4*8,8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false);

  buf[2] = 0;
  key = emu_ReadI2CKeyboard2(0);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawTextNoDma(9*8+0*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(1);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawTextNoDma(9*8+1*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(2);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawTextNoDma(9*8+2*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(3);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawTextNoDma(9*8+3*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(4);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawTextNoDma(9*8+4*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(5);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawTextNoDma(9*8+5*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
*/
  handleOSKB();
  if ( (hk != 0) && (hk < 128) ) {
      buf[0] = (char)(hk&0xff);  
      buf[1] = 0;
      tft.drawTextNoDma(col*8,(row+3)*8,buf,LIGHT_BLUE,BLUE,false);
      col += 1;
      if (col >= 40) {
          col=0;
          row += 1;
          if (row >= 25) {
              row=0;  
          }
      }
      if (hk != prevhk) {
          delay(200);
      }
      else {
          delay(100);
      }
  }
  prevhk = hk;

  delay(20);  
}

// ****************************************************
// the loop() method runs continuously
// ****************************************************
void loop2(void) 
{
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * floppy1 = menuSelection();
    if (action == ACTION_RUN1) {
      toggleMenu(false);
      vgaMode = false;   
      tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
      tft.startDMA(); 
    }    
       
    delay(20);
  }
  else {
    uint16_t bClick = 0; //emu_DebounceLocalKeys();    
  }  
}

#ifdef HAS_SND

#include "AudioPlaySystem.h"

AudioPlaySystem mymixer;


void emu_sndInit() {
  Serial.println("sound init");  
#ifdef HAS_T4_VGA
  tft.begin_audio(256, mymixer.snd_Mixer);  
#else
  mymixer.begin_audio(256, mymixer.snd_Mixer);  
#endif
 // sgtl5000_1.enable();
 // sgtl5000_1.volume(0.6);
  mymixer.start();
}

void emu_sndPlaySound(int chan, int volume, int freq)
{
  if (chan < 6) {
    mymixer.sound(chan, freq, volume); 
  }
  /*
  Serial.print(chan);
  Serial.print(":" );  
  Serial.print(volume);  
  Serial.print(":" );  
  Serial.println(freq); 
  */ 
}

void emu_sndPlayBuzz(int size, int val) {
  mymixer.buzz(size,val); 
  //Serial.print((val==1)?1:0); 
  //Serial.print(":"); 
  //Serial.println(size); 
}
#endif
