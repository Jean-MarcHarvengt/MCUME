#ifndef EMUCFG_H
#define EMUCFG_H

#include "wrapemu.h"

// Title:     <                        >
#define TITLE "   AtariST Emulator     "
#define ROMSDIR "/st"

#define emu_Init(FLOPPY1, FLOPPY2, MODE) {ast_Init(); ast_Start(FLOPPY1, FLOPPY2, MODE);}
#define emu_Step(x) {ast_Step();}
#define emu_Input(x) {ast_Input(x);}

#define MAX_FILENAME_PATH    64
#define NB_FILE_HANDLER      4

#define PALETTE_SIZE         256
#define VID_FRAME_SKIP       0x0
#define TFT_VBUFFER_YCROP    0
#define SINGLELINE_RENDERING 1
#define CUSTOM_SND           1
//#define TIMER_REND           1
//#define EXTPAD               1
#define EXTRA_HEAP           0x10
#define FILEBROWSER          1


#ifdef KEYMAP_PRESENT

/*
const unsigned short key_map1[] = {
  11,12,13,14,15,16,17,18,19,20, 39,
  0, 21,22,23,24,25,26,27,28,29,30,
  0, 31,32,33,34,35,36,37,60,40,
  66,68,69,67 //U L R D
  };
  
const unsigned short key_map2[] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10,0,
  0, 0,0,0,0,0,53,0,51,52,54,
  0, 61,62,0,59,0,0,0,38,58,
  66,68,69,67
  };

const unsigned short key_map3[] = {
  40,41,42,43,44,45,46,47,48,49, 0, // function keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 63,64,0,0,0,0,0,58,65,
  66,68,69,67
  };
*/

#define keylables_map0_0 (char *)"qwertyuiop\x1a"
#define keylables_map0_1 (char *)" asdfghjkl\x19"
#define keylables_map0_2 (char *)" zxcvbnm,.;/"
#define keylables_map0_3 (char *)" +\x10-"
const unsigned short key_map0[] = {
  'q','w','e','r','t','y','u','i','o','p',127,  //lowecase
  0,'a','s','d','f','g','h','j','k','l',10,
  0,'z','x','c','v','b','n','m',',','.',';','/',
  218,216,215,217, //U L R D
  0,'+',' ','-'
  };  

#define keylables_map1_0 (char *)"1234567890 "
#define keylables_map1_1 (char *)"           "
#define keylables_map1_2 (char *)"            "
#define keylables_map1_3 (char *)"    "
const unsigned short key_map1[] = {
  '1','2','3','4','5','6','7','8','9','0',0, // digit keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,0,0,
  218,216,215,217, //U L R D
  0,0,' ',0
  };

#define keylables_map2_0 (char *)"!\"#$%^&*()@"
#define keylables_map2_1 (char *)" |\\[]{}    "
#define keylables_map2_2 (char *)"        <>:?"
#define keylables_map2_3 (char *)" =\x10_"
const unsigned short key_map2[] = {
  '!','"','#','$','%','^','&','*','(',')','@',  // shiftothers
  0, '|','\\','[',']','{','}','\'',0,0,0,
  0, 0,0,0,0,0,0,0,'<','>',':','?',
  218,216,215,217, //U L R D
  0,'=',' ','_'
  };  

#define keylables_map3_0 (char *)"QWERTYUIOP@"
#define keylables_map3_1 (char *)" ASDFGHJKL\x19"
#define keylables_map3_2 (char *)" ZXCVBNM<>:?"
#define keylables_map3_3 (char *)" =\x10_"
const unsigned short key_map3[] = {
  'Q','W','E','R','T','Y','U','I','O','P','@',  //shift uppercase
  0,'A','S','D','F','G','H','J','K','L',10,
  0,'Z','X','C','V','B','N','M','<','>',':','?',
  218,216,215,217, //U L R D
  0,'=',' ','_'
  };  

#define keylables_map4_0 (char *)"\x11\x12\x13\x14\x15\x16\x17\x18   "
#define keylables_map4_1 (char *)"           "
#define keylables_map4_2 (char *)"            "
#define keylables_map4_3 (char *)"    "

const unsigned short key_map4[] = {
  194,195,196,197,198,199,200,201,202,203, 0, // function keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,0,0,
  218,216,215,217, //U L R D
  0,0,' ',0
  };

#define keylables_map5_0 (char *)"           "
#define keylables_map5_1 (char *)"           "
#define keylables_map5_2 (char *)"            "
#define keylables_map5_3 (char *)"    "

const unsigned short key_map5[] = {
  0,0,0,0,0,0,0,0,0,0,0, // extra keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,0,0,0,0,
  218,216,215,217, //U L R D
  0,0,' ',0
  };

const unsigned short matkeys[] = {
  0x004,0x008,0x108,0x104,0x208,0x204,0x308,0x304,0x408,0x404,0x410, // row 1
  0x502,0x002,0x020,0x102,0x120,0x202,0x220,0x302,0x320,0x402,0x420, // row 2
  0x508,0x001,0x040,0x101,0x140,0x201,0x240,0x210,0x340,0x301,0x401,0x440, // row 3
  0x504,0x520,0x540,0x501, // UP LEFT RIGHT DOWN
  0x510,0x010,0x110,0x310, // row 4
  };
  
#endif

#endif
