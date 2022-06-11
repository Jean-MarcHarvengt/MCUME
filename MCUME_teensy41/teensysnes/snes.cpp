#include <stdio.h>
#include <string.h>

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}
#include <Arduino.h>



// Emulation includes
#include "snes9x.h"
#include "apu.h"

static int ik;    // joypad key
static int pik=0; 

static int ihk;   // I2C keyboard key
static int iusbhk;// USB keyboard key

void snes_Input(int bClick) {
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

void snes_Init(void)
{
  s9x_init();
#ifdef HAS_SND  
  emu_sndInit();
#endif 
}


void snes_Start(char * filename)
{
  emu_printf("emu starting");

  s9x_start(filename);
  S9xUnmapAllControls();
  S9xMapButtonT(0, "Joypad1 Left");
  S9xMapButtonT(1, "Joypad1 Right");
  S9xMapButtonT(2, "Joypad1 Up");
  S9xMapButtonT(3, "Joypad1 Down");
  S9xMapButtonT(4, "Joypad1 A");
  S9xMapButtonT(5, "Joypad1 B");
  S9xMapButtonT(6, "Joypad1 Start");
  S9xMapButtonT(7, "Joypad1 Select");
  S9xMapButtonT(8, "Joypad1 X");
  S9xMapButtonT(9, "Joypad1 Y");
  
  emu_printf("emu started");
}

static int prev_hk = 0;
void snes_Step(void)
{
  int k=ik; 
#ifdef TEECOMPUTER
  int hk = ihk; 
  if (hk == 'q') {  // SELECT
    S9xReportButton(7, true);
    //emu_printf("selectd");
  }  
  else if (hk == 'w') {  // START
    S9xReportButton(6, true); 
    //emu_printf("startd");
  }
  else {
    if (prev_hk == 'q') {
      S9xReportButton(7, false);
      //emu_printf("selectu");
    }  
    else if (prev_hk == 'w') {
      S9xReportButton(6, false);
      //emu_printf("startu");
    } 
  }
  prev_hk = hk;
  // Ignore joypad if shift/fn is pressed!!!
  //if ( !(k & MASK_KEY_USER1) && !(k & MASK_KEY_USER2) )
#endif
  {
    if ( ( !(pik & MASK_JOY2_BTN) && (k & MASK_JOY2_BTN) ) || ( !(pik & MASK_JOY1_BTN) && (k & MASK_JOY1_BTN) ) ) {
      S9xReportButton(9, true);
    }
    else if ( ( (pik & MASK_JOY2_BTN) && !(k & MASK_JOY2_BTN) ) || ( (pik & MASK_JOY1_BTN) && !(k & MASK_JOY1_BTN) ) ) {
      S9xReportButton(9, false);
    } 
    if ( ( !(pik & MASK_JOY2_UP) && (k & MASK_JOY2_UP) ) || ( !(pik & MASK_JOY1_UP) && (k & MASK_JOY1_UP) ) ) {
      S9xReportButton(2, true);
    }
    else if ( ( (pik & MASK_JOY2_UP) && !(k & MASK_JOY2_UP) ) || ( (pik & MASK_JOY1_UP) && !(k & MASK_JOY1_UP) ) ) {
      S9xReportButton(2, false);
    } 
    if ( ( !(pik & MASK_JOY2_DOWN) && (k & MASK_JOY2_DOWN) ) || ( !(pik & MASK_JOY1_DOWN) && (k & MASK_JOY1_DOWN) ) ) {
      S9xReportButton(3, true);
    }
    else if ( ( (pik & MASK_JOY2_DOWN) && !(k & MASK_JOY2_DOWN) ) || ( (pik & MASK_JOY1_DOWN) && !(k & MASK_JOY1_DOWN) ) ) {
      S9xReportButton(3, false);
    } 
    if ( ( !(pik & MASK_JOY2_RIGHT) && (k & MASK_JOY2_RIGHT) ) || ( !(pik & MASK_JOY1_RIGHT) && (k & MASK_JOY1_RIGHT) ) ) {
      S9xReportButton(1, true);
    }
    else if ( ( (pik & MASK_JOY2_RIGHT) && !(k & MASK_JOY2_RIGHT) ) || ( (pik & MASK_JOY1_RIGHT) && !(k & MASK_JOY1_RIGHT) ) ) {
      S9xReportButton(1, false);
    } 
    if ( ( !(pik & MASK_JOY2_LEFT) && (k & MASK_JOY2_LEFT) ) || ( !(pik & MASK_JOY1_LEFT) && (k & MASK_JOY1_LEFT) ) ) {
      S9xReportButton(0, true);
    }
    else if ( ( (pik & MASK_JOY2_LEFT) && !(k & MASK_JOY2_LEFT) ) || ( (pik & MASK_JOY1_LEFT) && !(k & MASK_JOY1_LEFT) ) ) {
      S9xReportButton(0, false);
    } 
  }  

#ifdef TEECOMPUTER
  // But 2
  if ( !(pik & MASK_KEY_USER1) && (k & MASK_KEY_USER1) ) {
    S9xReportButton(5, true); 
  }
  else if ( (pik & MASK_KEY_USER1) && !(k & MASK_KEY_USER1) ) {
    S9xReportButton(5, false); 
  } 
  // But 3
  if ( !(pik & MASK_KEY_USER2) && (k & MASK_KEY_USER2) ) {
    S9xReportButton(7, true);
  }
  else if ( (pik & MASK_KEY_USER2) && !(k & MASK_KEY_USER2) ) {
    S9xReportButton(7, false);
  }
  // But 4
  if ( !(pik & MASK_KEY_USER3) && (k & MASK_KEY_USER3) ) {
    S9xReportButton(4, true);
  }
  else if ( (pik & MASK_KEY_USER3) && !(k & MASK_KEY_USER3) ) {
    S9xReportButton(4, false);
  }
#else
  // But 2
  if ( !(pik & MASK_KEY_USER1) && (k & MASK_KEY_USER1) ) {
    S9xReportButton(5, true); 
  }
  else if ( (pik & MASK_KEY_USER1) && !(k & MASK_KEY_USER1) ) {
    S9xReportButton(5, false); 
  } 
  // Start
  if ( !(pik & MASK_KEY_USER2) && (k & MASK_KEY_USER2) ) {
    S9xReportButton(6, true);
  }
  else if ( (pik & MASK_KEY_USER2) && !(k & MASK_KEY_USER2) ) {
    S9xReportButton(6, false);
  }
#endif

  pik = k;

  s9x_step();
}

#ifdef HAS_SND      
void  SND_Process( void * stream, int len )
{
    S9xMixSamples(stream, len);
}
#endif 
 
