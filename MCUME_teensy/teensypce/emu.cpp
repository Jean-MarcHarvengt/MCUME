#include <string.h>

#include "emuapi.h"
#include "tft_t_dma.h"
#include "iopins.h" 

extern "C" {
#include "shared.h"
#include "system.h"
}

#ifdef HAS_T41 

EXTMEM static unsigned char MemPool[8*1024*1024];

extern "C" uint8 read_rom(int address) {
  return (MemPool[address]); 
}

extern "C" void  write_rom(int address, uint8 val)  {
  MemPool[address]=val;
}

#else

#include "psram_t.h"

PSRAM_T psram = PSRAM_T(PSRAM_CS, PSRAM_MOSI, PSRAM_SCLK, PSRAM_MISO);

extern "C" uint8 read_rom(int address) {
  return (psram.psread(address)); 
}

extern "C" void  write_rom(int address, uint8 val)  {
  psram.pswrite(address,val); 

}
#endif


void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}


void pce_Init(void)
{
  emu_printf("Allocating MEM");
#ifndef HAS_T41   
  psram.begin();
#endif  
  mem_init();    
  emu_printf("Allocating MEM done");
}


static int hk = 0;
static int k = 0;


void pce_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();   
}


void pce_Start(char * filename)
{
  emu_printf("init started");
  load_rom(filename, 0,0);
 
#ifdef SOUND_PRESENT
  system_init(22050);
#ifdef HAS_SND  
  emu_sndInit();
#endif  
#else
  system_init(0);
#endif
  system_reset();

  emu_printf("init done");
}

void pce_Step(void) {

  input.pad[0] = 0;

  if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
    input.pad[0] |= INPUT_RIGHT;
  }
  if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
    input.pad[0] |= INPUT_LEFT;
  }
  if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
    input.pad[0] |= INPUT_UP;
  }
  if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
    input.pad[0] |= INPUT_DOWN;
  }  
  if ( k & MASK_JOY2_BTN) {
    input.pad[0] |= INPUT_B2; 
  }

  if (k & MASK_KEY_USER1)  input.pad[0] |= INPUT_B1;
  //if (k & MASK_KEY_USER3)  input.pad[0] |= INPUT_SELECT;
  if (k & MASK_KEY_USER2)  input.pad[0] |= INPUT_RUN;

  //emu_printi(emu_FrameSkip());

  system_frame((emu_FrameSkip()==0)?0:1);
  emu_DrawVsync();    
}

void SND_Process(void *stream, int len) {
  psg_update(stream, 0, len);  
} 
