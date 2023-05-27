#include <string.h>

#include "emuapi.h"
#include "iopins.h" 

extern "C" {
#include "shared.h"
#include "system.h"
}
#include "arduinoproto.h"

unsigned char * MemPool = NULL;

extern "C" uint8 read_rom(int address) {
  return (MemPool[address]); 
}

extern "C" uint8 readb_swap_rom(int address) {
  return (MemPool[address^1]); 
}

extern "C" uint16 readw_swap_rom(int address) {
  return ((MemPool[address+1]<<8) + MemPool[address]);  
}
extern "C" void  write_rom(int address, uint8 val)  {
  MemPool[address]=val;
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


void gen_Init(void)
{
  emu_printf("Allocating MEM"); 

  bg_pattern_cache = emu_Malloc(BGPATTERN_CACH_SIZE);

  MemPool = emu_SMalloc(5*1024*1024);   
  work_ram = emu_SMalloc(WORK_RAM_SIZE);  //&MemPool[7*1024*1024]; // emu_Malloc(WORK_RAM_SIZE); 
  vram = emu_SMalloc(VRAM_SIZE);  //&MemPool[7*1024*1024+WORK_RAM_SIZE];//emu_Malloc(VRAM_SIZE); 
  zram = emu_Malloc(Z_RAM_SIZE); 
  
  //mem_init();    
  emu_printf("Allocating MEM done");
}

void gen_Input(int click) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard(); 
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

#ifdef HAS_SND
  audio_init(22050);
#endif
  
  system_reset();  

  emu_printf("init done");
}

void gen_Step(void) {

  input.pad[0] = 0;

  int k  = ik;
  int hk = ihk;
  if (iusbhk) hk = iusbhk;

  if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
    input.pad[0] |= INPUT_LEFT;
  }
  if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
    input.pad[0] |= INPUT_RIGHT;
  }
  if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
    input.pad[0] |= INPUT_UP;
  }
  if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
    input.pad[0] |= INPUT_DOWN;
  }  
  if (( k & MASK_JOY2_BTN) || ( k & MASK_JOY1_BTN)) {
    input.pad[0] |= INPUT_A; 
  }

  if (k & MASK_KEY_USER1)   input.pad[0] |= INPUT_C;
#ifdef TEECOMPUTER
  if (k & MASK_KEY_USER2)  input.pad[0] |= INPUT_B;
  if (hk == 'q')  input.pad[0] |= INPUT_START;
#else
  if (k & MASK_KEY_USER2)  input.pad[0] |= INPUT_START;
  if (k & MASK_KEY_USER3)  input.pad[0] |= INPUT_B;
#endif

  prevhk = hk;
  
  system_frame(emu_FrameSkip()); 
  //emu_printi(emu_FrameSkip());

  emu_DrawVsync();    
}

void SND_Process(void *stream, int len) {
#ifdef HAS_SND
audio_play_sample(stream, 0, len);
#endif  
} 
