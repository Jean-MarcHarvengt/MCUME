#include <string.h>

#include "emuapi.h"
#include "iopins.h" 

extern "C" {
#include "pce-go/pce-go.h"
#include "pce-go/pce.h"
}

#include "flash_t.h"

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}


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
  InitPCE(22050, false, (const void *)flash_start, (size_t)size);

#ifdef SOUND_PRESENT
#ifdef HAS_SND  
  emu_sndInit();
#endif  
#else
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

  //psg_update((int16_t *) audio_buffer, AUDIO_BUFFER_LENGTH, 0xff);
  //i2s_dma_write(&i2s_config, (const int16_t *) audio_buffer);

  //emu_DrawVsync();   
}

void SND_Process(void *stream, int len) {
//  psg_update((int16*)stream, 0, len);  
} 
