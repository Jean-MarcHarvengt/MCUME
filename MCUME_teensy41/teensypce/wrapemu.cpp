#include <string.h>

#include "emuapi.h"
#include "iopins.h" 

extern "C" {
#include "shared.h"
#include "system.h"
}


static unsigned char * MemPool;

extern "C" uint8 read_rom(int address) {
  return (MemPool[address]); 
}

extern "C" void  write_rom(int address, uint8 val)  {
  MemPool[address]=val;
}


void pce_Init(void)
{
  MemPool = emu_SMalloc(8*1024*1024);
  
  emu_printf("Allocating MEM"); 
  mem_init();    
  emu_printf("Allocating MEM done");
}


static int ik;     // joypad key
static int ihk;    // I2C keyboard key
static int iusbhk; // USB keyboard key
static int prevhk; // previous keyboard key

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

void pce_Input(int click) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard();  
}


void pce_Start(char * filename)
{
  emu_printf("init started");
  load_rom(filename, 0,0);
 
#ifdef HAS_SND
  system_init(22050); 
#else
  system_init(0);
#endif
  system_reset();

  emu_printf("init done");
}

void pce_Step(void) {

  input.pad[0] = 0;

  int k  = ik;
  int hk = ihk;
  if (iusbhk) hk = iusbhk;

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
  if ( ( k & MASK_JOY1_BTN) || ( k & MASK_JOY2_BTN) ) {
    input.pad[0] |= INPUT_B2; 
  }

  if (k & MASK_KEY_USER1)  input.pad[0] |= INPUT_B1;
  if ( (k & MASK_KEY_USER2) || (hk == 'q') ) input.pad[0] |= INPUT_RUN;
  //if (k & MASK_KEY_USER3)  input.pad[0] |= INPUT_SELECT;

  prevhk = hk;

  //emu_printi(emu_FrameSkip());

  system_frame((emu_FrameSkip()==0)?0:1);
  emu_DrawVsync();    
}

void SND_Process(void *stream, int len) {
  psg_update(stream, 0, len);  
} 
