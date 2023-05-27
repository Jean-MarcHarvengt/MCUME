#include <string.h>

#include "emuapi.h"
#include "iopins.h" 

extern "C" {
#include "shared.h"
}

static int rom_offset = 0;


static unsigned char * MemPool;

extern "C" uint8 read_rom(int address) {
  return (MemPool[address+rom_offset]); 
}

extern "C" void  write_rom(int address, uint8 val)  {
  MemPool[address]=val;
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


void sms_Init(void)
{
  emu_printf("Allocating MEM");
  MemPool = emu_SMalloc(8*1024*1024);
  mem_init();    
  emu_printf("Allocating MEM done");
}


void sms_Input(int click) {
  ik  = emu_GetPad();
  ihk = emu_ReadI2CKeyboard();   
}


void sms_Start(char * filename)
{
  emu_printf("load and init");  

  int size = emu_FileSize(filename);
  int pos = 0;
  int n;
  int i;
  char * buf = (char*)cache;
  
  int f = emu_FileOpen(filename, "r+b");
  if (f) {
    while ( (n = emu_FileRead(buf,CACHE_SIZE,f) ) ) {
      for (int i=0; i<n; i++) {
        write_rom(pos++,buf[i]);
      }
      emu_printi(pos);            
      //emu_printi(n);          
    }
    emu_FileClose(f);
  }

  if((size / 512) & 1)
  {
    size -= 512;
    rom_offset += 512;
  }

  cart.pages = (size / 0x4000);

  int namelen = strlen(filename);    
  emu_printf(&filename[namelen-4]);          
  if (namelen > 4) {
    if ( (!strcmp(&filename[namelen-4],".sms")) || (!strcmp(&filename[namelen-4],".SMS")) ) {
      emu_printf("Master system");     
      cart.type = TYPE_SMS;
    }
    else {
      emu_printf("Game Gear");     
      cart.type = TYPE_GG; 
    }
  }

#ifdef HAS_SND
  system_init(22050);  
#else
  system_init(0);
#endif  
  emu_printf("init done");
}

void sms_Step(void) 
{
  input.pad[0]=0;

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
  if (( k & MASK_JOY1_BTN) || ( k & MASK_JOY2_BTN))  {
    input.pad[0] |= INPUT_BUTTON1; 
  }

  if ( (k & MASK_KEY_USER2) || (hk == 'q') )  {
    input.pad[0] |= INPUT_BUTTON2;
  }

  if ( (k & MASK_KEY_USER1) || (hk == 'w') )  input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
  if (hk == 'r')  input.system |= INPUT_HARD_RESET;
  //if (hk == 'e')  input.system |= (IS_GG) ? INPUT_HARD_RESET : INPUT_SOFT_RESET;

  prevhk = hk;

  sms_frame(0); 
  //emu_printi(emu_FrameSkip());

  emu_DrawVsync();    
}

void SND_Process(void *stream, int len) {
#ifdef HAS_SND
  audio_play_sample(stream, 0, len);
#endif
} 
