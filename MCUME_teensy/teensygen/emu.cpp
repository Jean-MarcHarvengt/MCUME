#include <string.h>

#include "emuapi.h"
#include "tft_t_dma.h"
#include "psram_t.h"
#include "iopins.h" 

extern "C" {
#include "shared.h"
#include "system.h"
}

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

static uint8 romversion;
extern "C" uint8 rom_version(void) {
  return romversion;
}

static deinterleave_block(int offset, int srcoffset)
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


void gen_Init(void)
{
  emu_printf("Allocating MEM");
  psram.begin();
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

  size = emu_FileSize(filename);
  int pos = 0;
  int n;
  int i;
  char * buf = (char*)bg_pattern_cache;
  size = 0;  
  if (emu_FileOpen(filename)) {
    while ( (n = emu_FileRead(buf,0x40000) ) ) {
      size += n;
      for (int i=0; i<n; i++) {
        write_rom(pos++,buf[i]);
      }
      emu_printi(size);            
      //emu_printi(n);          
    }
    emu_FileClose();
  }


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
  audio_init(22050);  
  emu_sndInit();
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
  
  system_frame(emu_FrameSkip()); 
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




