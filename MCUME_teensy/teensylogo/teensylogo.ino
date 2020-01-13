extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}
#include "tft_t_dma.h"
#include "keyboard_osd.h"
#include <math.h>
#include "mcume.h"



TFT_T_DMA tft = TFT_T_DMA(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO, TFT_TOUCH_CS, TFT_TOUCH_INT);
static int xOffLogo=0;
static int swipeAngle=0;

void setup() {
  //emu_sndInit();
 
  tft.begin();
  //emu_sndPlaySound(0, 255, 4000);

  emu_init();
}

static uint8_t col=0x00;

void loop(void) 
{
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)    
#else
//  if ( ((emu_ReadKeys() & (MASK_KEY_USER1+MASK_KEY_USER2)) == (MASK_KEY_USER1+MASK_KEY_USER2))
//     || (emu_ReadKeys() & MASK_KEY_USER4 ) )
//  {  
//    emu_printf((char*)"esc");
//    *(volatile uint32_t *)0xE000ED0C = 0x5FA0004;
//    while (true) {
//      ;
//    }    
//  }
#endif



  uint16_t bClick = emu_DebounceLocalKeys();
  if (menuActive()) {
    int action = handleMenu(bClick);
    char * filename = menuSelection();    
    if (action == ACTION_RUNTFT) {  
      Serial.print("go");        
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
    else if (action == ACTION_RUNVGA)  {
      toggleMenu(false);
    }         
   
    delay(20);
  }
  else if (callibrationActive()) {
    handleCallibration(bClick);
  } 
  else {
#ifdef ILI9341_TOUCH    
    handleVirtualkeyboard();
#endif    
    if ( (!virtualkeyboardIsActive()) ) {
      delay(20);
      //emu_printf("step");
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
}

#ifdef HAS_SND

#include <Audio.h>
#include "AudioPlaySystem.h"

AudioPlaySystem mymixer;
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)    
AudioOutputMQS  mqs;
AudioConnection patchCord9(mymixer, 0, mqs, 1);
#else
AudioOutputAnalog dac1;
AudioConnection   patchCord1(mymixer, dac1);
#endif

void emu_sndInit() {
  Serial.println("sound init");  
  AudioMemory(16);
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
