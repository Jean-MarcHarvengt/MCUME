#include "emuapi.h"  
#include "iopins.h"  

#include "t4_dsp.h"
extern T4_DSP tft;

#define BLUE       RGBVAL16(0, 0, 170)
#define LIGHT_BLUE RGBVAL16(0, 136, 255)
static int fb_width, fb_height;
static const char * digits = "0123456789ABCDEF";
static int prevhk = 0;
static int col=0;
static int row=0;

// ****************************************************
// the setup() method runs once, when the sketch starts
// ****************************************************
void setup() {
  emu_init();
  char * filename;  
#ifdef FILEBROWSER
  while (true) {      
    if (menuActive()) {
      uint16_t bClick = emu_DebounceLocalKeys();
      int action = handleMenu(bClick);
      filename = menuSelection();   
      if (action == ACTION_RUN1) {
        break;    
      }
      delay(20);
    }
  }
#endif 
 
  emu_start(20000,nullptr,1);
  tft.fillScreen(LIGHT_BLUE);
  tft.get_frame_buffer_size(&fb_width, &fb_height);
  tft.drawRect((fb_width-320)/2,(fb_height-200)/2, 320,200, BLUE);
  toggleOSKB(true);  
}

void loop(void) 
{
  char buf[5] = {0,0,0,0,0};
  uint16_t bClick = emu_GetPad();

  buf[0] = digits[(bClick>>12)&0xf];
  buf[1] = digits[(bClick>>8)&0xf];
  buf[2] = digits[(bClick>>4)&0xf];
  buf[3] = digits[bClick&0xf];
  tft.drawText(4*8,0,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false);

  buf[3] = 0;
  int key = emu_ReadI2CKeyboard();
  buf[0] = digits[(key>>8)&0xf];
  buf[1] = digits[(key>>4)&0xf];
  buf[2] = digits[key&0xf];        
  tft.drawText(4*8,8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),false); 
  buf[2] = 0;
  key = emu_ReadI2CKeyboard2(0);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawText(9*8+0*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(1);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawText(9*8+1*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(2);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawText(9*8+2*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(3);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawText(9*8+3*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(4);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawText(9*8+4*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);
  key = emu_ReadI2CKeyboard2(5);
  buf[0] = digits[(key>>4)&0xf];
  buf[1] = digits[key&0xf];
  tft.drawText(9*8+5*24,0*8,buf,RGBVAL16(0x00,0x00,0x00),RGBVAL16(0xFF,0xFF,0xFF),true);

  bClick = emu_DebounceLocalKeys();
  int hk = hk = emu_ReadI2CKeyboard();  
  //handleOSKB();
  if ( (hk != 0) && (hk < 128) ) {
      buf[0] = (char)(hk&0xff);  
      buf[1] = 0;
      tft.drawText(col*8,(row+3)*8,buf,LIGHT_BLUE,BLUE,false);
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
