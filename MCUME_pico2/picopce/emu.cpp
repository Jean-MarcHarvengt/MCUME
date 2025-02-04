#include <string.h>

#include "emuapi.h"
#include "iopins.h" 

extern "C" {
#include "pce-go/pce-go.h"
#include "pce-go/pce.h"
#include <pce-go/psg.h>
}

#include "flash_t.h"

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}

#define AUDIO_SAMPLE_RATE 22050
#define AUDIO_BUFFER_LENGTH (AUDIO_SAMPLE_RATE / 60 + 1)

alignas(4) int audio_buffer[AUDIO_BUFFER_LENGTH];


void pce_Init(void)
{
  emu_printf("pce_Init");   
  emu_printf("pce_Init done");
}


static int hk = 0;
static int k = 0;


void pce_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();    
}


void pce_Start(char * filename)
{
  emu_printf("pce_Start");

  int size = flash_load(filename);
  PalettePCE(0);
  InitPCE(AUDIO_SAMPLE_RATE, true, (const void *)flash_start, (size_t)size);

#ifdef HAS_SND  
  emu_sndInit(); 
#endif  

  emu_printf("pce_Start done");
}

void pce_Step(void) {
  RunPCE();
  uint32_t buttons = 0;
  if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
    buttons |= JOY_LEFT;
  }
  if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
    buttons |= JOY_RIGHT;
  }
  if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
    buttons |= JOY_UP;
  }
  if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
    buttons |= JOY_DOWN;
  }  
  if ( k & MASK_JOY2_BTN) {
    buttons |= JOY_A;
  }

  if (k & MASK_KEY_USER3)  buttons |= JOY_B;
  if (k & MASK_KEY_USER1)  buttons |= JOY_SELECT;
  if (k & MASK_KEY_USER2)  buttons |= JOY_RUN;
          
  PCE.Joypad.regs[0] = buttons; 

  //emu_DrawVsync();   
}

void SND_Process(void *stream, int len) {
  int16_t * buf = (int16_t *) audio_buffer;
  psg_update(buf, AUDIO_BUFFER_LENGTH, 0xff);
  audio_sample * snd_buf =  (audio_sample *)stream;
  for (int h = 0; h < len*2; h += 2) {            
      int16_t s1 = buf[h]>>8; 
      int16_t s2 =  buf[h+1]>>8;
      *snd_buf++ = ((s1+s2)/2)+128;
  } 
} 
