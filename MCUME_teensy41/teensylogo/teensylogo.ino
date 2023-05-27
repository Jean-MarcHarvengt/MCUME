extern "C" {
  #include "iopins.h"  
  #include "emuapi.h"  
}

#include <math.h>
#include "mcume.h"

#include "t4_dsp.h"
extern T4_DSP tft;


static int xOffLogo=0;
static int swipeAngle=0;

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  emu_init();
}

static void vbl(void) {
    uint16_t bClick = emu_DebounceLocalKeys();
    emu_Input(bClick);
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
      emu_start(16666,nullptr);
      emu_Init(filename);
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
      uint16_t bClick = emu_DebounceLocalKeys();    
      xOffLogo = 16*sin((2*3.14*swipeAngle)/256)+30;
      swipeAngle = (swipeAngle + 4)&0xff;   
      tft.drawSprite(xOffLogo,10,(uint16_t*)logo);   
      //if (bClick & MASK_JOY2_BTN) {  
      //  tft.stopRefresh();
      //  emu_init();    
      //  toggleMenu(true);      
      //}
      delay(20);            
  }  
}
