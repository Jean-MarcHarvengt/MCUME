#include <string.h>

#include "emuapi.h"
#include "iopins.h" 

extern "C" {
#include "shared.h"
#include "system.h"
}

static int frameskip = 2;
#define HAS_PSRAM 1

#ifdef HAS_PSRAM

#include "psram_t.h"

PSRAM_T psram = PSRAM_T(PSRAM_CS, PSRAM_MOSI, PSRAM_SCLK, PSRAM_MISO);

extern "C" uint8 read_rom(int address) {
  return (psram.psread(address)); 
}

extern "C" uint8 readb_swap_rom(int address) {
  return(psram.psread(address^1));
}

extern "C" uint16 readw_swap_rom(int address) {
  return psram.psread_w(address);  
}
extern "C" void  write_rom(int address, uint8 val)  {
  psram.pswrite(address,val); 
}
#else
#include "flash_t.h"

static uint8 * flash_start;

extern "C" uint8 read_rom(int address) {
  return flash_start[address];
}

extern "C" uint8 readb_swap_rom(int address) {
  return flash_start[address^1];
}

extern "C" uint16 readw_swap_rom(int address) {
  uint16 * flash_start16 = (uint16*)flash_start;
  return flash_start16[address];
  //return psram.psread_w(address);  
}

#endif

static uint8 romversion;
extern "C" uint8 rom_version(void) {
  return romversion;
}

static void deinterleave_block(int offset, int srcoffset)
{
    int i;
    int srcaddr = offset + srcoffset;
    uint8 * block = work_ram; // 0x4000 bytes tmp buffer 
    for(i = 0; i < 0x4000; i += 1) {
      block[i] = read_rom(srcaddr++);
    }
    for(i = 0; i < 0x2000; i += 1)
    {
        write_rom(srcoffset + i*2+1, block[0x2000 + (i)]);
        write_rom(srcoffset + i*2+0, block[0x0000 + (i)]);
    }
}

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}

void gen_Init(void)
{
  emu_printf("Allocating MEM");
#ifdef HAS_PSRAM
  psram.begin();
#endif  
  mem_init();    
  emu_printf("Allocating MEM done");
}


static int hk = 0;
static int k = 0;


void gen_Input(int click) {
  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();   
}


void gen_Start(char * filename)
{
  emu_printf("load and init");  

  int size;
  int offset = 0;
  int i;

  size = emu_FileSize(filename);

#ifdef HAS_PSRAM  
  int pos = 0;
  int n;
  char * buf = (char*)bg_pattern_cache;
  size = 0;
  int f = emu_FileOpen(filename,"r+b");  
  if (f) {
    while ( (n = emu_FileRead(buf,0x40000,f) ) ) {
      size += n;
      for (int i=0; i<n; i++) {
        write_rom(pos++,buf[i]);
      }
      emu_printi(size);            
      //emu_printi(n);          
    }
    emu_FileClose(f);
  }
#else
  flash_start = (uint8 *)flash_load(filename);  
#endif  

  if((size / 512) & 1)
  {
      //emu_printf("deinterleave");
      size -= 512;
      offset += 512;
      for(i = 0; i < (size / 0x4000); i += 1)
      {
          deinterleave_block(offset, (i * 0x4000));
      }
  }
  else {
    for(i = 0; i < size; i += 2)
    {
        uint8 temp = read_rom(i+0);
        write_rom(i+0,read_rom(offset+i+1));
        write_rom(i+1, temp);
    }
  }

  romversion = read_rom(0x0001F0);

 
  system_init();

#ifdef SOUND_PRESENT
#ifdef HAS_SND
//  audio_init(22050);  
//  emu_sndInit();
#endif  
#endif
  
  system_reset();  

  emu_printf("init done");
}

void gen_Step(void) {

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
    input.pad[0] |= INPUT_A; 
  }

  if (k & MASK_KEY_USER1)  input.pad[0] |= INPUT_B;
  if (k & MASK_KEY_USER2)  input.pad[0] |= INPUT_START;
  //if (k & MASK_KEY_USER3)  input.pad[0] |= INPUT_C;
  
  system_frame(frameskip); 
  //emu_printi(emu_FrameSkip());

  emu_DrawVsync();    
}

void SND_Process(void *stream, int len) {
#ifdef SOUND_PRESENT
#ifdef HAS_SND
//audio_play_sample(stream, 0, len);
#endif  
#endif  
} 
