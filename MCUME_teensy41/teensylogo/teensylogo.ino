extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}

#include <math.h>
#include "mcume.h"


#ifdef HAS_T4_VGA
#include "vga_t_dma.h"
TFT_T_DMA tft;
#else
#include "tft_t_dma.h"
TFT_T_DMA tft = TFT_T_DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO, TFT_TOUCH_CS, TFT_TOUCH_INT);
#endif

static int xOffLogo=0;
static int swipeAngle=0;

void setup() {
  //emu_sndInit();
#ifdef HAS_T4_VGA
  tft.begin(VGA_MODE_320x240);
//  NVIC_SET_PRIORITY(IRQ_QTIMER3, 0);
#else
  tft.begin();
#endif 
  emu_init();  
}

static uint8_t col=0x00;

void loop(void) 
{
  
  uint16_t bClick = emu_DebounceLocalKeys();
  if (menuActive()) {
    int action = handleMenu(bClick);
    char * filename = menuSelection();    
    if (action == ACTION_RUN1) {         
      toggleMenu(false);
      tft.fillScreenNoDma(RGBVAL16(0x00,0x00,0x00));
      tft.startDMA();
      tft.drawRect(0,TFT_HEIGHT/2, TFT_WIDTH, TFT_HEIGHT/2, RGBVAL16(0x00,0x00,0x80));
      tft.drawText(0,TFT_HEIGHT/2, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), true); 
      tft.drawText(0,TFT_HEIGHT/2+16, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false); 
      tft.drawText(0,TFT_HEIGHT/2+24, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false); 
      tft.drawText(0,TFT_HEIGHT/2+32, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false); 
      tft.drawText(0,TFT_HEIGHT/2+40, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false); 
      tft.drawText(0,TFT_HEIGHT/2+48, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false); 
      tft.drawText(0,TFT_HEIGHT/2+56, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false); 
      tft.drawText(0,TFT_HEIGHT/2+64, "hello hello hello hello hello hello hell", RGBVAL16(0x00,0xff,0xff), RGBVAL16(0x00,0x00,0xff), false); 
    }    
    delay(20);
  }
  else {        
      delay(20);
      xOffLogo = 16*sin((2*3.14*swipeAngle)/256)+30;
      swipeAngle = (swipeAngle + 4)&0xff;   
      tft.drawSprite(xOffLogo,10,(uint16_t*)logo);   
      if (bClick & MASK_JOY2_BTN) {  
        tft.stopDMA();
        emu_init();    
        toggleMenu(true);      
      }         
  }  
}

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
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
