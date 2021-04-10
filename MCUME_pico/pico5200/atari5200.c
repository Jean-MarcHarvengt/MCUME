#include "atari5200.h"
#include <string.h>
#include "memory.h"
#include "cpu.h"
#include "atari.h"
#include "pokey.h"
#include "rom.h"
#include "antic.h"
#include "gtia.h"
#include "colours.h"
#include "emuapi.h"
#if HAS_SND
#include "pokeysnd.h"
#endif


// Controllers
typedef struct
{ 
  // All values are 1 or 0, or perhaps not...
  int left;
  int right;
  int up;
  int down;

  unsigned char analog_h;
  unsigned char analog_v;

  int trig;
  int side_button;

  // These may be set to 1. The core handles clearing them.
  // [BREAK]  [ # ]  [ 0 ]  [ * ]
  // [RESET]  [ 9 ]  [ 8 ]  [ 7 ]
  // [PAUSE]  [ 6 ]  [ 5 ]  [ 4 ]
  // [START]  [ 3 ]  [ 2 ]  [ 1 ]
  int key[16];
  int last_key_still_pressed;

  int lastRead;
} CONTROLLER;


#define POT_MAX    223
#define POT_CENTRE 115
#define POT_LEFT   (POT_CENTRE-100) //15
#define POT_RIGHT  (POT_CENTRE+100) // 210

#define CONSOL 0xC01F

#define NUM_16K_ROM_MAPS 200

typedef struct
{ // 80 bytes
  char crc[8];
  int mapping;
  char description[68];
} Map16k_t;


// global variables
int tv_mode = TV_PAL;
//unsigned char mem[MEMORY_SIZE];
unsigned char * memory=NULL; //mem;

// local variables
static char logmsg[64];

static CONTROLLER cont1, cont2;
static int pot_max_left = POT_LEFT;
static int pot_max_right = POT_RIGHT;

static int framesdrawn=0;

/* MEMORY MAP                   INDEX  (0 is invalid)
* 0-3FFF    RAM       (read/write)  1
* 4000-BFFF ROM       (ro)          2
* C000-C0FF GTIA regs               3
*   D300-D3FF   Serial???           7
* D400-D4FF ANTIC regs              4
* E000    I/O expansion             5
* E800-E8FF POKEY regs              6
* EB00-EBFF also POKEY regs???      6
* F800-FFFF ROM (BIOS)(ro)          2
*/


// memory CPU read (load) handler
uint8 Atari_GetByte(uint16 addr)
{
  if (addr < 0xC000) { // MAPPER_RAM or MAPPER_ROM
      return(memory[addr]);
  }
  else if (addr < 0xC100) {  // MAPPER_GTIA
      return GTIA_GetByte(addr,1);
  } 
  else if ( (addr >= 0xD400) && (addr < 0xD500) ) {  // MAPPER_ANTIC
      return ANTIC_GetByte(addr,1);
  }
  else if ( (addr >= 0xE800) && (addr < 0xE900) ) {  // MAPPER_POKEY
      return POKEY_GetByte(addr,1);    
  }  
  else if ( (addr >= 0xEB00) && (addr < 0xEC00) ) {  // MAPPER_POKEY mirror
      return POKEY_GetByte(addr,1);    
  } 
  else if (addr >= 0xF800) {  // MAPPER_ROM (bios)
      return(BIOSData[addr-0xF800]);
  }

  //case MAPPER_IOEXP:  // I/O exp read
  //  return IOEXPread(addr);

  return 0xff;
}

// memory CPU write (store) handler
void Atari_PutByte(uint16 addr, uint8 byte)
{
  if (addr < 0x4000) { // MAPPER_RAM
      memory[addr] = byte;
  }
  else if (addr < 0xC000) {  // MAPPER_ROM
  }
  else if (addr < 0xC100) {  // MAPPER_GTIA
      GTIA_PutByte(addr, byte);    
  } 
  else if ( (addr >= 0xD400) && (addr < 0xD500) ) {  // MAPPER_ANTIC
      ANTIC_PutByte(addr, byte);
  }
  else if ( (addr >= 0xE800) && (addr < 0xE900) ) {  // MAPPER_POKEY
      POKEY_PutByte(addr, byte);              
  }  
  else if ( (addr >= 0xEB00) && (addr < 0xEC00) ) {  // MAPPER_POKEY mirror
      POKEY_PutByte(addr, byte);         
  } 
  //else if (addr >= 0xF800) {  // MAPPER_ROM (bios)
  //    POKEY_PutByte(addr, byte);
  //}
  
  //case MAPPER_IOEXP:  // I/O exp write
  //  IOEXPwrite(addr, byte);
}


// check keyboard and set kbcode on VBI
void INPUT_Scanline(void)
{
  // NB: 5200 will do a keyboard IRQ every 32 scanlines if a key is held down
  CONTROLLER *which = NULL;
  UBYTE consol=GTIA_GetByte(CONSOL,1);
  switch (consol & 0x03) {
    case 0: which = &cont1; break;
    case 1: which = &cont2; break;
      // 3 and 4 in the future
    default: return;
  }
  
  // "loose bit" (bit 5 of KBCODE) - fluctuates 0 or 1 randomly
  uint8 loose_bit = (framesdrawn & 0x1) << 5;

  // Default to "key not pressed"
  POKEY_KBCODE = loose_bit | 0x1F;
  which->last_key_still_pressed = 0;

  for (int8 i = 0; i < 16; i++)
  {
    if (which->key[i])
    {
      //emu_printi(i);
/* 2016-06-18 - commented out (reset in HostDoEvents())
      which->key[i] = 0;
            which->last_key_still_pressed = 0;

      // Don't respond to the same thing twice in a row...
      if (i == which->lastRead)
      {
                which->last_key_still_pressed = 1;      // flag key still held
        return;
      }
*/
      if (i == which->lastRead)
        which->last_key_still_pressed = 1;      // flag key still held

      which->lastRead = i;

      // Write in the change
      POKEY_KBCODE = (i << 1) | loose_bit | 0x1;

      // set KEY interrupt bit (bit 6) to 0 (key int req "on")
      POKEY_IRQST &= 0xbf;

      // check irqen and do interrupt if bit 6 set
      if(POKEY_IRQEN & 0x40)
      {
        CPU_GenerateIRQ();
      }

      return;
    }
  }

  // 2016-06-18 - Reset kbd irq if no key pressed
  // NO - "POKEY_IRQST is latched, only reset by write to IRQEN"
  //POKEY_IRQST |= 0x40;

  // If no keys are down at all, we can write anything again
  which->lastRead = 0xFF;

  // This should in theory work but in practise breaks some games?
  //POKEY_KBCODE = which->lastRead = 0xFF;
}

uint8 INPUT_handle_trigger(uint16 POKEYreg)
{
  CONTROLLER *which;
  switch (POKEYreg)
  {
    case 0: case 1: which = &cont1; if (which->trig) GTIA_TRIG[0]=0; else GTIA_TRIG[0]=1; break;
    case 2: case 3: which = &cont2; if (which->trig) GTIA_TRIG[1]=0; else GTIA_TRIG[1]=1; break;
      // 3 and 4 in the future
    default: return 0x80;
  }
  

  // Top to bottom
  if (POKEYreg & 1)
  {
#if ANALOGJOY
    return which->analog_v;;
#else
    if (which->up) return pot_max_left;
    else if (which->down) return pot_max_right;
    else return POT_CENTRE;
#endif    
  }
  else
  {
#if ANALOGJOY
    return which->analog_h;;
#else
    if (which->left) return pot_max_left;
    else if (which->right) return pot_max_right;
    else return POT_CENTRE;
#endif    
  }
}

uint8 INPUT_handle_skstat(uint16 POKEYreg)
{
  uint8 skstatreg = 0x0C;
  UBYTE consol=GTIA_GetByte(CONSOL,1);
  //emu_printi(consol&3);
  switch(consol & 0x03) {
      case 0 : // controller 1
          if(cont1.side_button) skstatreg &= 0x07;
          if(cont1.last_key_still_pressed) skstatreg &= 0x0B;
          break;
      case 1 : // controller 2
          if(cont2.side_button) skstatreg &= 0x07;
          if(cont2.last_key_still_pressed) skstatreg &= 0x0B;
          break;
  }
  return skstatreg;
}





static void load_CART(char * cartname) 
{
  int i, mapnum, flen;
  char sig[40];
  unsigned long crc32;

  flen = emu_FileSize(cartname);
  
  emu_FileOpen(cartname);
  // set POT left and right values to default
  pot_max_left = POT_LEFT;
  pot_max_right = POT_RIGHT;

  // load cart into memory image
  // Note: 5200 cartridge mapping has only a few
  //       variations, so this mess of code below
  //       works, and it avoids having a cartridge
  //       config file.
  switch (flen)
  {
    case 32768: // 32k cart
      for (i = 0; i < 32768; i++) memory[0x4000 + i] = emu_FileGetc();
      // get crc32 from 32k data
      crc32 = calc_crc32(memory + 0x4000, 32768);
      sprintf(logmsg, "32 Trying to load '%s', crc32=0x%08X\n", cartname, (unsigned int)crc32);
      emu_printf(logmsg);
      break;
    case 16384: // 16k cart
      // here we hack and load it twice (mapped like that?)
      for (i = 0; i < 16384; i++) memory[0x4000 + i] = memory[0x8000 + i] = emu_FileGetc();

      // get crc32 from 16k data
      crc32 = calc_crc32(memory + 0x4000, 16384);
      sprintf(logmsg, "16 Trying to load '%s', crc32=0x%08X\n", cartname, (unsigned int)crc32);
      emu_printf(logmsg);

      // get cart "signature"
      strncpy(sig, &memory[0xBFE8], 20);
      sig[20] = 0;
      //printf("Cart signature is %s\n", sig);

      // check for Moon Patrol
      if (strcmp("@@@@@moon@patrol@@@@", sig) == 0) {
        //printf("Mapping for Moon Patrol  (16+16)\n");
        // already loaded correctly
        break;
      }

      // check for SW-Arcade
      if (strncmp("asfilmLt", sig, 8) == 0) {
        //printf("Mapping for SW-Arcade  (16+16)\n");
        // already loaded correctly
        break;
      }

      // check for Super Pacman using start vector
      if ((memory[0xBFFF] == 0x92) && (memory[0xBFFE] == 0x55)) {
        //printf("Mapping for Super Pacman  (16+16)\n");
        // already loaded correctly
        break;
      }

      // check for other carts with reset vec 8000h
      // (eg: Space Shuttle)
      if (memory[0xBFFF] == 0x80) {
        //printf("Mapping for reset vec = 8000h  (16+16)\n");
        // already loaded corectly
        break;
      }

      // Tempest
      if (memory[0xBFFF] == 0x81) {
        //printf("Mapping for reset vec = 81xxh eg: Tempest (16+16)\n");
        // already loaded corectly
        break;
      }


      // PAM Diagnostics v2.0
      // NB: this seems to prevent the emu from crashing when running
      // pamdiag2.bin
      if ((memory[0xBFFF] == 0x9F) && (memory[0xBFFE] == 0xD0)) {
        //printf("Mapping for reset vector = $9FD0 (PAM DIAG 2.0)\n");
        // move cart up by 0x1000
        break;
      }



#ifdef SKIP
      // Notes: check for megamania cart
      // 8K mirrored at 0x8000 and 0xA000, nothing from 0x4000-0x7FFF
      // see if we have a 16k mapping for this cart in jum52.cfg
      sprintf(sig, "%08X", crc32);
      mapnum = 0; // invalid

      // initialise 16k rom maps
      emu_printf("Allocating p16kMaps");
      int num16kMappings = 0;      // pointer to 16k rom mappings and number of 16k rom mappings
      Map16k_t * p16kMaps = (Map16k_t *)emu_TmpMemory(); //emu_Malloc(sizeof(Map16k_t) * NUM_16K_ROM_MAPS);
      if (p16kMaps) memset(p16kMaps, 0, sizeof(Map16k_t) * NUM_16K_ROM_MAPS);
     
      for (i = 0; i < num16kMappings; i++) {
        if (0 == strncmp(sig, p16kMaps[i].crc, 8)) {
          mapnum = p16kMaps[i].mapping;
          sprintf(logmsg, "Mapping %d found for crc=0x%s !\n", mapnum, sig);
          emu_printf(logmsg);
          i = num16kMappings; // exit search
        }
      }
      //emu_printf("freeing p16kMaps");
      //emu_Free(p16kMaps); 
      // if the mapping was 16+16, then break, since we have loaded it 16+16 already
      if (1 == mapnum)
        break;
#endif
      // default to 16k+8k mapping
      emu_FileSeek(0);
      for(i=0; i<16384; i++) memory[0x6000 + i] = emu_FileGetc();
      for(i=0; i<8192; i++) memory[0xA000 + i] = memory[0x8000 + i];
      break;
    case 8192 : // 8k cart
      // Load mirrored 4 times
      for(i = 0; i < 8192; i++)
        {
        uint8 c = emu_FileGetc();
        memory[0x4000 + i] = c;
        memory[0x6000 + i] = c;
        memory[0x8000 + i] = c;
        memory[0xA000 + i] = c;
        }
      // get crc32 from 8k data
      crc32 = calc_crc32(memory + 0x4000, 8192);
      sprintf(logmsg, "8k cart load '%s', crc32=0x%08X\n", cartname, (unsigned int)crc32);
      emu_printf(logmsg);
      break;
    default:    // oops!
      // these rom dumps are strange, because some carts are 8K, yet
      // all the dumps are either 16K or 32K!
      sprintf(logmsg, "Cartridge ROM size not 16K or 32K. Unable to load.");
      emu_printf(logmsg);      
      return -1;
      break;
  }

  // check for Pengo
  if (strncmp("pengo", memory + 0xBFEF, 8) == 0) {
    emu_printf("Pengo detected! Switching controller to Pengo mode.");
    pot_max_left = 70;
    pot_max_right = 170;
  }

  // check for Centipede
  if (strncmp("centipede", memory + 0xBFEF, 8) == 0) {
    emu_printf("centipede detected! Switching controller to centipede mode.");
    pot_max_left = (POT_CENTRE-10);
    pot_max_right = (POT_CENTRE+10);
  }

  // is cartridge PAL-compatible?
  // (doesn't seem to work!)
  //if(memory[0xBFE7] == 0x02) printf("Cart is PAL-compatible!\n");
  //else printf("Cart is *not* PAL-compatible.\n");

  emu_FileClose(); 
}

static void Initialise(void)
{
  int i;

  emu_printf("Initialising ...");

  // Set up memory area
  memset(memory, 0, MEMORY_SIZE);

  // init controllers
  memset(&cont1, 0, sizeof(CONTROLLER));
  memset(&cont2, 0, sizeof(CONTROLLER));
  
  pot_max_left = POT_LEFT;
  pot_max_right = POT_RIGHT;
  cont1.analog_h = POT_CENTRE;
  cont1.analog_v = POT_CENTRE;
  cont2.analog_h = POT_CENTRE;
  cont2.analog_v = POT_CENTRE;
}

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}


void at5_Init(void)
{
   int i;

  // Palette
  for (i = 0; i < PALETTE_SIZE; i++)
  {
    emu_SetPaletteEntry(R32(colourtable[i]), G32(colourtable[i]), B32(colourtable[i]), i);
  }

#if HAS_SND
  emu_sndInit();
  POKEYSND_Init(POKEYSND_FREQ_17_EXACT, 44100, 1, POKEYSND_BIT16);
#endif

  emu_printf("Allocating RAM");
  if (memory == NULL) memory = emu_Malloc(MEMORY_SIZE);

  Initialise();
}

static int prevKey=-1;
static int countKey = 0;

void at5_Step(void)
{
  //emu_printf("step");

  int k,j;
  j=emu_ReadKeys();
  k=emu_GetPad();

  CONTROLLER * which;
  
  if (j & 0x8000) which=&cont2;
  else which=&cont1;

  // Start
  if (j & MASK_KEY_USER1)
      which->key[12] = 1;    
  else
      which->key[12] = 0;    

  // 1
  if (j & MASK_KEY_USER3)
      which->key[15] = 1;
  else
      which->key[15] = 0;

  // 2
  //if (j & MASK_KEY_USER4)
  //    which->key[14] = 1;
  //else
  //    which->key[14] = 0;

  if (countKey) {
    which->key[prevKey] = 1;
    //emu_printi(cont1.key[prevKey]);
    countKey--;
  }
  else {
    if (prevKey>0) {
      //emu_printf("resetting");
      which->key[prevKey] = 0;
      prevKey=-1;    
      //emu_printi(cont1.key[prevKey]);
    }
    if (k) {
        prevKey = k-1;
        which->key[prevKey] = 1;
        //emu_printi(cont1.key[prevKey]);
        countKey = 4;
    }
  }

   // Joystick side button, trigger and directions
  if (j & MASK_JOY2_BTN) 
      which->trig = 1;    
  else
      which->trig = 0;
  if (j & MASK_KEY_USER2)
      which->side_button = 1;
  else
      which->side_button = 0;
  if (j & MASK_JOY2_DOWN)
      which->down = 1;
  else
      which->down = 0;
  if (j & MASK_JOY2_UP)
      which->up = 1;
  else
      which->up = 0;
  if (j & MASK_JOY2_RIGHT)
      which->left = 1;
  else
      which->left = 0;
  if (j & MASK_JOY2_LEFT)
      which->right = 1;
  else
      which->right = 0; 

  which->analog_h = emu_ReadAnalogJoyX(0,230);
  which->analog_v = emu_ReadAnalogJoyY(0,230);
  
  GTIA_Frame();
  ANTIC_Frame(1); 
  emu_DrawVsync();
  POKEY_Frame();
  //int i;
  //for (i=0xC000; i< 0x10000; i++)
  //  if (memory[i] !=0) emu_printf("bug");  

  framesdrawn = framesdrawn +1;
}



void at5_Start(char * cartname)
{
  int i;
  
  load_CART(cartname);

  emu_printf("antic");
  ANTIC_Initialise();
  emu_printf("gtia");  
  GTIA_Initialise();
  emu_printf("pokey");
  POKEY_Initialise();
  GTIA_TRIG[0]=1;
  GTIA_TRIG[1]=1;
  GTIA_TRIG[2]=1;
  GTIA_TRIG[3]=1;
  emu_printf("6502 reset");
  CPU_Reset();
  
  emu_printf("init done");   
}
