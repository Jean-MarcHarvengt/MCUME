#include <string.h>

#include "emuapi.h"
#include "psram_t.h"
#include "iopins.h" 

extern "C" {
#include "shared.h"

#include "defs.h"
#include "regs.h"
#include "lcd.h"
#include "rc.h"
#include "cpu.h"
#include "mem.h"
#include "hw.h"
#include "rtc.h"
#include "sound.h"

#include "loader.h"
}

static int rom_offset = 0;

#ifdef HAS_T41 
EXTMEM static unsigned char MemPool[8*1024*1024];

extern "C" uint8 read_rom(int address) {
  return (MemPool[address+rom_offset]); 
}

extern "C" void  write_rom(int address, uint8 val)  {
  MemPool[address]=val;
}

#else

#include "psram_t.h"

PSRAM_T psram = PSRAM_T(PSRAM_CS, PSRAM_MOSI, PSRAM_SCLK, PSRAM_MISO);

extern "C" uint8 read_rom(int address) {
  return (psram.psread(address+rom_offset));
}

extern "C" void  write_rom(int address, uint8 val)  {
  psram.pswrite(address,val); 
}
#endif

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}




void gbe_Init(void)
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


void gbe_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();   
}

static char buf[0x1000];
extern "C" int loadfile(char * filename);
int loadfile(char * filename)
{
  int romsize = emu_FileSize(filename);
  int pos = 0;
  int n;
  int i;
  if (emu_FileOpen(filename)) {
    while ( (n = emu_FileRead(buf,sizeof(buf))) ) 
    {
      for (int i=0; i<n; i++) {
        write_rom(pos++,buf[i]);
      }
      emu_printi(pos);            
    }
    emu_FileClose();
  }

  return(romsize);
}


void gbe_Start(char * Cartridge)
{
  emu_printf("load and init");  

  loader_init(Cartridge);
 
  hw_reset();
  lcd_reset();
  cpu_reset();
  mbc_reset();
#ifdef SOUND_PRESENT
#ifdef HAS_SND
  sound_reset(22050);
  emu_sndInit();
#endif    
#endif  
  lcd_begin();
  emu_printf("init done");
}

void gbe_Step(void) 
{
  hw.pad = 0;

  if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
    hw.pad|=PAD_RIGHT;
  }
  if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
    hw.pad|=PAD_LEFT;
  }
  if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
    hw.pad|=PAD_UP;
  }
  if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
    hw.pad|=PAD_DOWN;
  }  
  if ( k & MASK_JOY2_BTN) {
    hw.pad|=PAD_A; 
  }

  if (k & MASK_KEY_USER1)  {
    hw.pad|=PAD_B;
  }

  if (k & MASK_KEY_USER2)  hw.pad|=PAD_START; //PAD_SELECT;


//  cpu_emulate(2280);
  cpu_emulate(3120);

  while (R_LY > 0 && R_LY < YRES)
    cpu_emulate(cpu.lcdc);

  rtc_tick();
  //emu_printi(emu_FrameSkip());

  emu_DrawVsync();    
}

void SND_Process(void *stream, int len) {
#ifdef SOUND_PRESENT
#ifdef HAS_SND
  audio_play_sample(stream, 0, len);
#endif  
#endif  
} 
