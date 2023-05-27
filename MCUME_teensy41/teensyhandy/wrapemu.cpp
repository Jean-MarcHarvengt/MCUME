#include <stdio.h>
#include <string.h>

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}
#include <Arduino.h>
#include "handy.h"


// Emulation includes

static CSystem *lynx = NULL;



#define AUDIORPTRMASK (((AUDIO_BUFFER_LENGTH-1)<<8)+0xff)

static ULONG sndbuffer[AUDIO_BUFFER_LENGTH]; // (short)(L<<16)+(short)R

static long audioowptr = 0;
static long audioorptr = 0;
static long audiorptr  = 0;
static long deltar     = 0;

  

#define AVG_COUNT 16
static int avgcounter = AVG_COUNT;
static long avgr     = 0;
static long avgw     = 0;
static long inc      = 0;
static long incdelta = 0;
static long sndinc   = 0x100; // default read increment

// distance between read and write buffer
static long pdelta   = 0x1000;
static bool pdown    = true; 


static int dpad_mapped_up;
static int dpad_mapped_down;
static int dpad_mapped_left;
static int dpad_mapped_right;

#define DISPLAY_ROTATION_OFF   0
#define DISPLAY_ROTATION_LEFT  1
#define DISPLAY_ROTATION_RIGHT 2

static int ik;    // joypad key
static int pik=0; 

static int ihk;   // I2C keyboard key
static int iusbhk;// USB keyboard key

void lnx_Input(int bClick) {
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

void lnx_Init(void)
{
}


void lnx_Start(char * filename)
{
  emu_printf("emu starting");

  lynx = new CSystem(filename, MIKIE_PIXEL_FORMAT_16BPP_565, HANDY_AUDIO_SAMPLE_FREQ);
  if (lynx->mFileType == HANDY_FILETYPE_ILLEGAL)
  {
      emu_printf("illegal");
      return;
  }

//  gPrimaryFrameBuffer = (UBYTE*)(emu_SMalloc((HANDY_SCREEN_HEIGHT+1)*HANDY_SCREEN_STRIDE*sizeof(HandyPixel)));
  gPrimaryFrameBuffer = (UBYTE*)(emu_SMalloc((OUTPUT_SCREEN_HEIGHT)*OUTPUT_SCREEN_WIDTH*sizeof(HandyPixel)));
  memset((void *)gPrimaryFrameBuffer, 0,(OUTPUT_SCREEN_HEIGHT)*OUTPUT_SCREEN_WIDTH*sizeof(HandyPixel));
  gAudioBuffer = &sndbuffer[0];
  memset(&sndbuffer[0], AUDIO_BUFFER_LENGTH*4,0);
  gAudioEnabled = 1;
  gAudioBufferPointer = AUDIO_BUFFER_LENGTH/2;

  int  rotation = DISPLAY_ROTATION_OFF;

  uint32_t crc32 = lynx->mCart->CRC32();
  emu_printh(crc32);
  switch (crc32)
  {
      case 0x97501709: // Centipede
      case 0x0271B6E9: // Lexis
      case 0x006FD398: // NFL Football
      case 0x1D9EC645:
      case 0xBCD10C3A: // Raiden
          rotation = DISPLAY_ROTATION_LEFT;
          break;
      case 0x7F0EC7AD: // Gauntlet
      case 0xAC564BAA: // Gauntlet - The Third Encounter
      case 0xFE19F59F:
      case 0xA53649F1: // Klax
      case 0xB8C75C2C: // Klax      
          rotation = DISPLAY_ROTATION_RIGHT;
          break;
      default:
          if (lynx->mCart->CartGetRotate() == CART_ROTATE_LEFT)
              rotation = DISPLAY_ROTATION_LEFT;
          if (lynx->mCart->CartGetRotate() == CART_ROTATE_RIGHT)
              rotation = DISPLAY_ROTATION_RIGHT;
  }
  switch(rotation)
  {
      case DISPLAY_ROTATION_LEFT:
          lynx->mMikie->SetRotation(MIKIE_ROTATE_L);
          dpad_mapped_up    = BUTTON_RIGHT;
          dpad_mapped_down  = BUTTON_LEFT;
          dpad_mapped_left  = BUTTON_UP;
          dpad_mapped_right = BUTTON_DOWN;
          break;
      case DISPLAY_ROTATION_RIGHT:
          lynx->mMikie->SetRotation(MIKIE_ROTATE_R);
          dpad_mapped_up    = BUTTON_LEFT;
          dpad_mapped_down  = BUTTON_RIGHT;
          dpad_mapped_left  = BUTTON_DOWN;
          dpad_mapped_right = BUTTON_UP;
          break;
      default:
          lynx->mMikie->SetRotation(MIKIE_NO_ROTATE);
          dpad_mapped_up    = BUTTON_UP;
          dpad_mapped_down  = BUTTON_DOWN;
          dpad_mapped_left  = BUTTON_LEFT;
          dpad_mapped_right = BUTTON_RIGHT;
          break;
  }


  emu_printf("emu started");
}

void lnx_Step(void)
{
  ULONG buttons = 0;

  int k=ik;
#ifdef TEECOMPUTER
  int hk = ihk; 
  if (hk == 'q') {
    buttons |= BUTTON_OPT1;
  }
  if (hk == 'w') {
    buttons |= BUTTON_OPT2;
  }
  if (hk == 'e') {
    buttons |= BUTTON_PAUSE;
  }
#endif      
  if ( (k & MASK_JOY2_UP) || (k & MASK_JOY1_UP) )     buttons |= dpad_mapped_up;
  if ( (k & MASK_JOY2_DOWN) || (k & MASK_JOY1_DOWN) )    buttons |= dpad_mapped_down;
  if ( (k & MASK_JOY2_LEFT) || (k & MASK_JOY1_LEFT) )    buttons |= dpad_mapped_left;
  if ( (k & MASK_JOY2_RIGHT) || (k & MASK_JOY1_RIGHT) )   buttons |= dpad_mapped_right;
  if ( (k & MASK_JOY2_BTN) || (k & MASK_JOY1_BTN) )       buttons |= BUTTON_A;
  if ( (k & MASK_KEY_USER1) ) buttons |= BUTTON_A;
  if ( (k & MASK_KEY_USER2) ) buttons |= BUTTON_B;
  if ( (k & MASK_KEY_USER3) ) buttons |= BUTTON_OPT1;

  lynx->SetButtonData(buttons);

  lynx->UpdateFrame(true);

  HandyPixel * buf = (HandyPixel *)gPrimaryFrameBuffer;
  for (int j=0; j<OUTPUT_SCREEN_HEIGHT; j++) {
    emu_DrawLine16(&buf[OUTPUT_SCREEN_STRIDE*j], OUTPUT_SCREEN_WIDTH, OUTPUT_SCREEN_HEIGHT, j);
  }

  emu_DrawVsync();

  pik = k;


  // #sample written per frame
  long ptr = gAudioBufferPointer;
  long wdelta = ptr - audioowptr;
  if (wdelta < 0) wdelta = AUDIO_BUFFER_LENGTH+wdelta;
  audioowptr = ptr; 

  // #sample read per frame
  int rdelta = deltar;
  deltar = 0;

 
  // Compute average R/W over AVG_COUNT frame
  avgcounter--;
  avgw += wdelta;
  avgr += rdelta;
  inc = (wdelta<<8)/(rdelta);
  if (avgcounter == 0) {
    
    wdelta = avgw/AVG_COUNT;
    rdelta = avgr/AVG_COUNT;
    avgw = 0;
    avgr = 0;
    avgcounter = AVG_COUNT;

    //emu_printi(wdelta);
    //emu_printi(rdelta);

    long delta = ptr - audiorptr>>8;
    if (delta < 0) delta = AUDIO_BUFFER_LENGTH+delta;


    // we try to be keep read and write buffer at half distance of each other
    bool down;
    if (delta < pdelta)  {
      down = true; // keep going down
      if (delta < (AUDIO_BUFFER_LENGTH/2)) {
        if ( (down) && (pdown) )
          incdelta += 2; 
        else
          incdelta = 1;  
      }
    }
    else if (delta > pdelta) {
      down = false; // goes up again
      if (delta > (AUDIO_BUFFER_LENGTH/2)) { 
        if ( (!down) && (!pdown) )
          incdelta -= 2; 
        else
          incdelta = -1;  
      }
    }
    else {
      incdelta = 0; 
    }
    // Hard reset sound buffer?
    
    if ( (delta < AUDIO_BUFFER_LENGTH/4) || (delta > (AUDIO_BUFFER_LENGTH-AUDIO_BUFFER_LENGTH/4)) ) {
      /*
      memset(sndbuffer,sizeof(sndbuffer),0);
      gAudioBufferPointer = 0;
      audioowptr = 0; 
      audiorptr=(AUDIO_BUFFER_LENGTH/2)<<8;
      delta = AUDIO_BUFFER_LENGTH/2;
      */
      //emu_printf("reset");        
    }
    
    pdelta = delta;
    pdown = down; 
    //emu_printi(delta);     
  }
  //emu_printi(incdelta);     

  sndinc = inc+incdelta;
}




void SND_Process(void *stream, int len) {
   
    short * dst = (short*)stream;
    
    len = len >> 1;
    for (int i=0;i<len;i++)
    { 
      ULONG val = sndbuffer[audiorptr>>8];
      *dst++ = (val >> 16);
      *dst++ = (val & 0xffff);
      audiorptr += sndinc;
      audiorptr &= AUDIORPTRMASK;       
      deltar +=1;   
    }
/*    
    long pt = (gAudioBufferPointer) - len;
    if (pt < 0) pt+=AUDIO_BUFFER_LENGTH;
    for (int i=0;i<len;i++)
    { 
      ULONG val = sndbuffer[pt++];
      *dst++ = (val >> 16);
      *dst++ = (val & 0xffff);        
      pt &= (AUDIO_BUFFER_LENGTH-1);
    } 
*/     
} 
 
