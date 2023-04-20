#define PROGMEM

#include "atari800.h"
#include <string.h>
#include "memory.h"
#include "cpu.h"
#include "atari.h"
#include "akey.h"
#include "pokey.h"
//#include "romatariosa.h"
#include "romatariosb.h"
//#include "romatarixl.h"
#include "antic.h"
#include "gtia.h"
#include "pia.h"
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
  int trig;

} CONTROLLER;


// global variables
unsigned char mem[MEMORY_SIZE];
unsigned char * memory=mem;
int tv_mode = TV_PAL;
UBYTE INPUT_key_consol;


// local variables
static CONTROLLER cont1, cont2;
static int INPUT_key_code = AKEY_NONE;
static int INPUT_key_shift = 0;

/* MEMORY MAP                   INDEX  (0 is invalid)
* 0-3FFF    RAM       (read/write)
* 4000-7FFF RAM       (read/write)
* 8000-9FFF BASIC     right CART 8K
* A000-BFFF BASIC     left  CART 8K
* D000-D0FF GTIA regs
* D200-D2FF POKEY regs
* D300-D3FF PIA  regs
* D400-D4FF ANTIC regs
* D800-FFFF OS ROM (ro)
* E000    I/O expansion
*/


// memory CPU read (load) handler
uint8 Atari_GetByte(uint16 addr)
{
  if (addr < MEMORY_SIZE) { // MAPPER_RAM
      return(memory[addr]);
  }
  else if ( (addr >= 0xD000) && (addr < 0xD100) ) {  // MAPPER_GTIA
      return GTIA_GetByte(addr,1);
  } 
  else if ( (addr >= 0xD200) && (addr < 0xD300) ) {  // MAPPER_POKEY
      return POKEY_GetByte(addr,1);       
  } 
  else if ( (addr >= 0xD300) && (addr < 0xD400) ) {  // MAPPER_PIA
      return PIA_GetByte(addr);       
  } 
  else if ( (addr >= 0xD400) && (addr < 0xD500) ) {  // MAPPER_ANTIC
      return ANTIC_GetByte(addr,1);
  }
  else if ((addr >= 0xD800) && (addr < 0x10000)) {  // MAPPER_ROM (bios)
      //return(memory[addr]);
      return(romos[addr-0xD800]);
  }

  //case MAPPER_IOEXP:  // I/O exp read
  //  return IOEXPread(addr);

  return 0xff;
}

// memory CPU write (store) handler
void Atari_PutByte(uint16 addr, uint8 byte)
{
  if (addr < 0x8000) { // MAPPER_RAM
      memory[addr] = byte;
  }
  else if (addr < 0xC000) {  // MAPPER_ROM
  }
  else if ( (addr >= 0xD000) && (addr < 0xD100) ) {  // MAPPER_GTIA
      GTIA_PutByte(addr, byte);    
  } 
  else if ( (addr >= 0xD200) && (addr < 0xD300)  ) {  // MAPPER_POKEY
      POKEY_PutByte(addr, byte);                     
  } 
  else if ( (addr >= 0xD300) && (addr < 0xD400)  ) {  // MAPPER_PIA
      PIA_PutByte(addr, byte);                     
  } 
  else if ( (addr >= 0xD400) && (addr < 0xD500) ) {  // MAPPER_ANTIC
      ANTIC_PutByte(addr, byte);
  }
  
  //case MAPPER_IOEXP:  // I/O exp write
  //  IOEXPwrite(addr, byte);
}


static void INPUT_Frame(void)
{
  int i;
  static int last_key_code = AKEY_NONE;
  static int last_key_break = 0;

  if (cont1.trig) GTIA_TRIG[0]=0; else GTIA_TRIG[0]=1;


  i = (INPUT_key_code == AKEY_BREAK);
  if (i && !last_key_break) {
    if (POKEY_IRQEN & 0x80) {
      POKEY_IRQST &= ~0x80;
      CPU_GenerateIRQ();
    }
  }
  last_key_break = i;
  POKEY_SKSTAT |= 0xc;
  if (INPUT_key_shift)
    POKEY_SKSTAT &= ~8;

  if (INPUT_key_code < 0) {
    //if (CASSETTE_press_space) {
    //  INPUT_key_code = AKEY_SPACE;
    //  CASSETTE_press_space = 0;
    //}
    //else {
      last_key_code = AKEY_NONE;
    //}
  }  
  if ((INPUT_key_code&~0x17) == AKEY_SHFTCTRL) {
    INPUT_key_code = AKEY_NONE;
  }
  if (INPUT_key_code >= 0) {    
//emu_printi(INPUT_key_code);     
    POKEY_SKSTAT &= ~4;
    if ((INPUT_key_code ^ last_key_code) & ~AKEY_SHFTCTRL) {
    /* ignore if only shift or control has changed its state */
      last_key_code = INPUT_key_code;
      POKEY_KBCODE = (UBYTE) INPUT_key_code;
      if (POKEY_IRQEN & 0x40) {
        if (POKEY_IRQST & 0x40) {       
          POKEY_IRQST &= ~0x40;
          CPU_GenerateIRQ();
        }
        else {
          /* keyboard over-run */
          POKEY_SKSTAT &= ~0x40;
          /* assert(CPU_IRQ != 0); */
        }
      }
    }
  }  
}

// check keyboard and set kbcode on VBI (not for A800)
void INPUT_Scanline(void)
{
//  if (cont1.trig) GTIA_TRIG[0]=0; else GTIA_TRIG[0]=1;
}


static void load_CART(char * cartname) 
{
  int flen = emu_FileSize(cartname);
  int f=emu_FileOpen(cartname, "r+b");
  if (flen < 16384) {
    emu_printf("8k");
    for(int i=0; i<flen; i++) memory[0xA000 + i] = emu_FileGetc(f);    
  }
  else {
    emu_printf("16k");    
    for(int i=0; i<flen; i++) memory[0x8000 + i] = emu_FileGetc(f);
  }

  emu_FileClose(f); 
}

  


void add_esc(UWORD address, UBYTE esc_code)
{
  memory[address++] = 0xf2; /* ESC */
  memory[address++] = esc_code; /* ESC CODE */
  memory[address] = 0x60;   /* RTS */
}

int Atari_PORT(int num)
{
  int nibble_0 = 0xff;
  int nibble_1 = 0xff;

  if (num == 0)
  {  
    if (cont1.left) nibble_0 &= ~0x04;   
    if (cont1.right) nibble_0 &= ~0x08;   
    if (cont1.up) nibble_0 &= ~0x01;   
    if (cont1.down) nibble_0 &= ~0x02;   
    if (cont2.left) nibble_1 &= ~0x04;   
    if (cont2.right) nibble_1 &= ~0x08;   
    if (cont2.up) nibble_1 &= ~0x01;   
    if (cont2.down) nibble_1 &= ~0x02;   
  }
  return (nibble_1 << 4) | nibble_0;
}

static void Initialise(void)
{
  int i;

  emu_printf("Initialising ...");

  // Set up memory area
  memset(memory, 0, MEMORY_SIZE);
  memset(memory+0x8000, 0xff, 0x4000);

  // init controllers
  memset(&cont1, 0, sizeof(CONTROLLER));
  memset(&cont2, 0, sizeof(CONTROLLER));
  

  // Load bios to address 0xF800
  //for (i = 0; i < 0x2800; i++)
  //{
  //  memory[0xD800 + i] = romos[i];
  //}  
  //add_esc(0xe459, ESC_SIOV);  
}

#define R32(rgb) ((rgb>>16)&0xff) 
#define G32(rgb) ((rgb>>8)&0xff) 
#define B32(rgb) (rgb & 0xff) 

void at8_Init(void)
{
   int i;

  // Palette
  for (i = 0; i < PALETTE_SIZE; i++)
  {
    emu_SetPaletteEntry(R32(colourtable[i]), G32(colourtable[i]), B32(colourtable[i]), i);
  }

#if HAS_SND
  emu_sndInit();
  POKEYSND_Init(POKEYSND_FREQ_17_APPROX, 44100, 1, POKEYSND_BIT16);
#endif

  emu_printf("Allocating RAM");
  if (memory == NULL) memory = emu_Malloc(MEMORY_SIZE);

  Initialise();
}

void emu_KeyboardOnDown(int keymodifer, int key) {
}

void emu_KeyboardOnUp(int keymodifer, int key) {
}

void at8_Step(void)
{
  //emu_printf("step");

  int j=emu_ReadKeys();
  int k=emu_GetPad();
  int hk = emu_ReadI2CKeyboard();  

  CONTROLLER * which;
  
  if (j & 0x8000) which=&cont2;
  else which=&cont1;

  // Start
  if (j & MASK_KEY_USER2)
    INPUT_key_consol &= ~0x01;
  else
    INPUT_key_consol |= 0x01;   

  // Select
  if (j & MASK_KEY_USER1)
    INPUT_key_consol &= ~0x02;
  else
    INPUT_key_consol |= 0x02;   

  // OPTION
  if (j & MASK_KEY_USER3)
    INPUT_key_consol &= ~0x04;
  else
    INPUT_key_consol |= 0x04;   

  if (k != 0) {
    INPUT_key_code = k-1;  
  }
  else {
    INPUT_key_code = AKEY_NONE;    
  }
  if (hk != 0) {
    INPUT_key_code = hk-1;  
  }
  else {
    INPUT_key_code = AKEY_NONE;    
  }
  
  // Keyboard
  INPUT_Frame();

  // Joystick side button, trigger and directions
  if (j & MASK_JOY2_BTN) 
      which->trig = 1;    
  else
      which->trig = 0;
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

  GTIA_Frame();
  ANTIC_Frame(1); 
  emu_DrawVsync();
  POKEY_Frame();
  
  //int i;
  //for (i=0xC000; i< 0x10000; i++)
  //  if (memory[i] !=0) emu_printf("bug");  
}



void at8_Start(char * cartname)
{
  load_CART(cartname);

  emu_printf("antic");
  ANTIC_Initialise();
  emu_printf("gtia");  
  GTIA_Initialise();
  emu_printf("pia");
  PIA_Initialise();  
  emu_printf("pokey");
  POKEY_Initialise();
  PORTA = 0x00;
  ANTIC_NMIEN = 0x00;
  ANTIC_NMIST = 0x00;
  memory[0x244] = 1;  
  GTIA_TRIG[0]=1;
  GTIA_TRIG[1]=1;
  GTIA_TRIG[2]=1;
  GTIA_TRIG[3]=1;
  emu_printf("6502 reset");
  CPU_Reset();
    
  emu_printf("init done");   
}
