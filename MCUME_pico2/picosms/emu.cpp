#include <string.h>

#include "emuapi.h"
#include "iopins.h"

extern "C" {
#include "shared.h"
}

static int rom_offset = 0;

#ifdef HAS_PSRAM

#include "psram_t.h"

PSRAM_T psram = PSRAM_T(PSRAM_CS, PSRAM_MOSI, PSRAM_SCLK, PSRAM_MISO);

extern "C" uint8 read_rom(int address) {
  return (psram.psread(address+rom_offset));
}

extern "C" void  write_rom(int address, uint8 val)  {
  psram.pswrite(address,val); 
}
#else
#include "flash_t.h"

extern "C" uint8 read_rom(int address) {
  return flash_start[address+rom_offset];
}

#endif



void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}


void sms_Init(void)
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


void sms_Input(int click) {

  hk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();   
}



void sms_Start(char * filename)
{
  emu_printf("load and init");  

#ifdef HAS_PSRAM
  int size = 0;
  int pos = 0;
  int n;
  int i;
  char * buf = (char*)cache;
  int f = emu_FileOpen(filename,"r+b");
  if (f) {
    while ( (n = emu_FileRead(buf,CACHE_SIZE,f) ) ) {
      size += n;
      for (int i=0; i<n; i++) {
        write_rom(pos++,buf[i]);
      }
      emu_printi(pos);            
      //emu_printi(n);          
    }
    emu_FileClose(f);
  }
#else
  int size = flash_load(filename);
#endif



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
  emu_sndInit();
  sms.use_fm = true;
#else
  system_init(0);
#endif
  emu_printf("init done");
}

void sms_Step(void) 
{
  input.pad[0]=0;

  if (( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT)) {
    input.pad[0] |= INPUT_LEFT;
  }
  if (( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT)) {
    input.pad[0] |= INPUT_RIGHT;
  }
  if (( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP)) {
    input.pad[0] |= INPUT_UP;
  }
  if (( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN)) {
    input.pad[0] |= INPUT_DOWN;
  }  
  if ( k & MASK_JOY2_BTN) {
    input.pad[0] |= INPUT_BUTTON1; 
  }

  if (k & MASK_KEY_USER3)  {
    input.pad[0] |= INPUT_BUTTON2;
  }

  //if (k & MASK_KEY_USER4)  input.system |= INPUT_HARD_RESET;
  //if (k & MASK_KEY_USER1)  input.system |= (IS_GG) ? INPUT_HARD_RESET : INPUT_SOFT_RESET;
  if (k & MASK_KEY_USER1)  input.pad[0] |= INPUT_BUTTON2;
  //if (k & MASK_KEY_USER2)  input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
  
  sms_frame(0); 

  emu_DrawVsync();    
}

#ifdef HAS_SND
static int16 sndl[512];
static int16 sndr[512];
#endif

void SND_Process(void *stream, int len) {
#ifdef HAS_SND
  audio_sample * snd_buf =  (audio_sample *)stream;
  audio_play_sample(&sndl[0], &sndr[0], len);
  for (int i = 0; i< len; i++ )
    *snd_buf++ = ((sndl[i]>>8)+(sndr[i]>>8))/2+128;
#endif  
} 
