extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}

#include "emu.h"

#include "emu.h"
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

static void vblCount() { 
  if (vbl) {
    vbl = false;
  } else {
    vbl = true;
  }
}

void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index)
{
  if (index<PALETTE_SIZE) {
    //Serial.println("%d: %d %d %d\n", index, r,g,b);
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
#ifdef HAS_T4_VGA
      tft.writeLine16(width,height,line, VBuf);
#else
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


// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {

#ifdef HAS_T4_VGA
  tft.begin(VGA_MODE_320x240);
//  NVIC_SET_PRIORITY(IRQ_QTIMER3, 0);
#else
  tft.begin();
#endif  
  
  emu_init();
}


// ****************************************************
// the loop() method runs continuously
// ****************************************************
void loop(void) 
{
  if (menuActive()) {
    uint16_t bClick = emu_DebounceLocalKeys();
    int action = handleMenu(bClick);
    char * filename = menuSelection();    
    if (action == ACTION_RUN1) {
      toggleMenu(false);
      vgaMode = false;       
      emu_start();
      emu_Init(filename);       
      tft.fillScreenNoDma( RGBVAL16(0x00,0x00,0x00) );
      tft.startDMA(); 
      myTimer.begin(vblCount, 20000);  //to run every 20ms  
    }    
    delay(20);
  }
  else {
      uint16_t bClick = emu_DebounceLocalKeys();
      emu_Input(bClick);  
      emu_Step();
      delay(10);
      //uint16_t bClick = emu_DebounceLocalKeys();
      //if (bClick & MASK_KEY_USER1) {
      //  emu_Input(bClick); 
      //}           
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
