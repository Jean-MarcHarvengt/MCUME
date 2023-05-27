#include <stdio.h>
#include <string.h>

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}
#include <Arduino.h>


// Emulation includes


static int ik;     // joypad key
static int pik=0; 

static int ihk;    // I2C keyboard key
static int iusbhk; // USB keyboard key

void xxxx_Input(int bClick) {
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

void xxxx_Init(void)
{
#ifdef HAS_SND  
  emu_sndInit();
#endif 
}


void xxxx_Start(char * filename)
{
  emu_printf("emu starting");
  
  emu_printf("emu started");
}

static int prev_hk = 0;
void xxxx_Step(void)
{
  int k=ik; 
#ifdef TEECOMPUTER
  int hk = ihk; 
  if (hk == 'q') {  // SELECT
    //emu_printf("selectd");
  }  
  else if (hk == 'w') {  // START
    //emu_printf("startd");
  }
  else {
    if (prev_hk == 'q') {
      //emu_printf("selectu");
    }  
    else if (prev_hk == 'w') {
      //emu_printf("startu");
    } 
  }
  prev_hk = hk;
  // Ignore joypad if shift/fn is pressed!!!
  //if ( !(k & MASK_KEY_USER1) && !(k & MASK_KEY_USER2) )
#endif
  {
    if ( ( !(pik & MASK_JOY2_BTN) && (k & MASK_JOY2_BTN) ) || ( !(pik & MASK_JOY1_BTN) && (k & MASK_JOY1_BTN) ) ) {

    }
    else if ( ( (pik & MASK_JOY2_BTN) && !(k & MASK_JOY2_BTN) ) || ( (pik & MASK_JOY1_BTN) && !(k & MASK_JOY1_BTN) ) ) {

    } 
    if ( ( !(pik & MASK_JOY2_UP) && (k & MASK_JOY2_UP) ) || ( !(pik & MASK_JOY1_UP) && (k & MASK_JOY1_UP) ) ) {

    }
    else if ( ( (pik & MASK_JOY2_UP) && !(k & MASK_JOY2_UP) ) || ( (pik & MASK_JOY1_UP) && !(k & MASK_JOY1_UP) ) ) {

    } 
    if ( ( !(pik & MASK_JOY2_DOWN) && (k & MASK_JOY2_DOWN) ) || ( !(pik & MASK_JOY1_DOWN) && (k & MASK_JOY1_DOWN) ) ) {

    }
    else if ( ( (pik & MASK_JOY2_DOWN) && !(k & MASK_JOY2_DOWN) ) || ( (pik & MASK_JOY1_DOWN) && !(k & MASK_JOY1_DOWN) ) ) {

    } 
    if ( ( !(pik & MASK_JOY2_RIGHT) && (k & MASK_JOY2_RIGHT) ) || ( !(pik & MASK_JOY1_RIGHT) && (k & MASK_JOY1_RIGHT) ) ) {

    }
    else if ( ( (pik & MASK_JOY2_RIGHT) && !(k & MASK_JOY2_RIGHT) ) || ( (pik & MASK_JOY1_RIGHT) && !(k & MASK_JOY1_RIGHT) ) ) {

    } 
    if ( ( !(pik & MASK_JOY2_LEFT) && (k & MASK_JOY2_LEFT) ) || ( !(pik & MASK_JOY1_LEFT) && (k & MASK_JOY1_LEFT) ) ) {

    }
    else if ( ( (pik & MASK_JOY2_LEFT) && !(k & MASK_JOY2_LEFT) ) || ( (pik & MASK_JOY1_LEFT) && !(k & MASK_JOY1_LEFT) ) ) {

    } 
  }  

  pik = k;

}

#ifdef HAS_SND      
void  SND_Process( void * stream, int len )
{
}
#endif 
 
