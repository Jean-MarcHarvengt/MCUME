#ifndef EMUCFG_H
#define EMUCFG_H

#define PALETTE_SIZE         256
#define VID_FRAME_SKIP       0x0
#define TFT_VBUFFER_YCROP    0
#define SINGLELINE_RENDERING 1
#define CUSTOM_SND           1
#define CUSTOM_SND_C         1
//#define TIMER_REND           1
#define EXTRA_HEAP           0x10
#define FILEBROWSER

// Title:     <                        >
#define TITLE "    Atari5200 Emulator"
#define ROMSDIR "5200"

#define emu_Init(ROM) {at5_Init(); at5_Start(ROM);}
#define emu_Step(x) {at5_Step();}
#define emu_Input(x) {}

#ifdef KEYMAP_PRESENT
   
#ifdef PICOZX
const unsigned short key_map1[] = {
  15+1,14+1,13+1,11+1,10+1,9+1,7+1,6+1,5+1,2+1,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0 //U L R D L R
  };  

const unsigned short key_map2[] = {
  0,0,1+1,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,3+3,0,0,0,0,
  0,0,0,0,0,0
  };  

const unsigned short key_map3[] = {
  0+1,4+1,8+1,12+1,0,0,0,0,0,0, // function keys break,reset,pause,start
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0
  };

const unsigned short matkeys[] = {
  0x104,0x101,0x108,0x102,0x110,0x120,0x140,0x204,0x201,0x208, // row 1
  0x202,0x210,0x220,0x240,0x304,0x301,0x308,0x302,0x310,0x320, // row 2
  0x340,0x404,0x401,0x408,0x402,0x410,0x420,0x440,0x504,0x501, // row 3
  0x020,0x508,0x502,0x510,0x520,0x540,0x604,0x601,0x040,0x608, // row 4
  0x008,0x001,0x002,0x010,0x620,0x640 }; // cursor keys up, left, right, down, <<<, >>>

#else

#define keylables_map1_0 (char *)"           "
#define keylables_map1_1 (char *)"           "
#define keylables_map1_2 (char *)"           "
const unsigned short key_map1[] = {
  15+1,14+1,13+1,11+1,10+1,9+1,7+1,6+1,5+1,2+1,0,
  0,12+1,8+1,4+1,0+1,1+1,3+1,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,
  0,0,0,0
  };  

// These may be set to 1. The core handles clearing them.
// [BREAK] 0  [ # ] 1  [ 0 ] 2  [ * ] 3
// [RESET] 4  [ 9 ] 5  [ 8 ] 6  [ 7 ] 7
// [PAUSE] 8  [ 6 ] 9  [ 5 ] 10 [ 4 ] 11
// [START] 12 [ 3 ] 13 [ 2 ] 14 [ 1 ] 15

#define keylables_map2_0 (char *)"           "
#define keylables_map2_1 (char *)"           "
#define keylables_map2_2 (char *)"           "
const unsigned short key_map2[] = {
  0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,
  0,0,0,0
  };  

#define keylables_map3_0 (char *)"           "
#define keylables_map3_1 (char *)"           "
#define keylables_map3_2 (char *)"           "
const unsigned short key_map3[] = {
  0,0,0,0,0,0,0,0,0,0,0, // function keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,
  0,0,0,0
  };

const unsigned short matkeys[] = {
  0x020,0x120,0x220,0x320,0x420,0x408,0x308,0x208,0x108,0x008,0x002, // row 1
  0x510,0x010,0x110,0x210,0x310,0x410,0x401,0x301,0x201,0x101,0x001, // row 2
  0x520,0x102,0x202,0x302,0x402,0x404,0x304,0x204,0x104,0x004, // row 3
  0x508,0x501,0x502,0x504 }; // cursor keys
 
#endif 
#endif

#endif
