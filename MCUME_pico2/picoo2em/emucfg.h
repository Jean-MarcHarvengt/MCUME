#ifndef EMUCFG_H
#define EMUCFG_H

#define PALETTE_SIZE         256
#define VID_FRAME_SKIP       0x0
#define TFT_VBUFFER_YCROP    20
//#define SINGLELINE_RENDERING 1
//#define CUSTOM_SND           1
//#define TIMER_REND           1
#define EXTRA_HEAP           0x10
#define FILEBROWSER

// Title:     <                        >
#define TITLE "    Oddysey Emulator"
#define ROMSDIR "o2em"


#define emu_Init(ROM) {odd_Init();odd_Start(ROM);}
#define emu_Step() {odd_Step();}
#define emu_Input(x) {odd_Input(x);}

#ifdef KEYMAP_PRESENT
   
#ifdef PICO2ZX

const unsigned short key_map1[] = {
  '1','2','3','4','5','6','7','8','9','0',0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0
  };  

const unsigned short key_map2[] = {
  0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0
  };  

const unsigned short key_map3[] = {
  0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0
  };

const unsigned short matkeys[] = {
  0x401,0x301,0x201,0x101,0x001,0x008,0x108,0x208,0x308,0x408,0x580, // row 1
  0x402,0x302,0x202,0x102,0x002,0x010,0x110,0x210,0x310,0x410, // row 2
  0x404,0x304,0x204,0x104,0x004,0x040,0x140,0x240,0x340,0x440, // row 3
  0x420,0x320,0x220,0x120,0x020,0x080,0x180,0x280,0x380,0x480, // row 4
  0x520,0x540,0x510,0x508,0x502,0x504 }; // cursor keys up, left, right, down, <<<, >>>
#endif 
#endif

#endif
