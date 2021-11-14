#ifndef EMUAPI_H
#define EMUAPI_H

#include "platform_config.h"

#define CUSTOM_SND  1
//#define TIMER_REND  1

#define EXTRA_HEAP  0x10


#ifdef KEYMAP_PRESENT
   
#define keylables_map1_0 (char *)"qwertyuiop\x1a"
#define keylables_map1_1 (char *)" asdfghjkl\x19"
#define keylables_map1_2 (char *)" zxcvbnm.\x10 "
const unsigned short key_map1[] = {
  'Q','W','E','R','T','Y','U','I','O','P',157,
  0,'A','S','D','F','G','H','J','K','L',0x0D,
  0,'Z','X','C','V','B','N','M','.',' ',
  145,157,29,17
  };  

#define keylables_map2_0 (char *)"1234567890 "
#define keylables_map2_1 (char *)" !@#$%+&*- "
#define keylables_map2_2 (char *)" ()?/\"<>,: "
const unsigned short key_map2[] = {
  '1','2','3','4','5','6','7','8','9','0',0,
  0, '!','@','#','$','%','+','&','*','-','\/',
  0, '(',')','?','\/','"','<','>',',',':',
  0,0,0,0
  };  

#define keylables_map3_0 (char *)"\x11\x12\x13\x14\x15\x16\x17\x18   "
#define keylables_map3_1 (char *)"           "
#define keylables_map3_2 (char *)"        ;= "
const unsigned short key_map3[] = {
  133,134,135,136,137,138,139,140,0,0,0, // function keys
  0, 0,0,0,0,0,0,0,0,0,0,
  0, 0,0,0,0,0,0,0,';','=',
  0,0,0,0
  };

const unsigned short matkeys[] = {
  0x020,0x120,0x220,0x320,0x420,0x408,0x308,0x208,0x108,0x008,0x002, // row 1
  0x510,0x010,0x110,0x210,0x310,0x410,0x401,0x301,0x201,0x101,0x001, // row 2
  0x520,0x102,0x202,0x302,0x402,0x404,0x304,0x204,0x104,0x004, // row 3
  0x508,0x501,0x502,0x504 }; // cursor keys
 
   
#endif


#define MASK_JOY2_RIGHT 0x0001
#define MASK_JOY2_LEFT  0x0002
#define MASK_JOY2_UP    0x0004
#define MASK_JOY2_DOWN  0x0008
#define MASK_JOY2_BTN   0x0010
#define MASK_KEY_USER1  0x0020
#define MASK_KEY_USER2  0x0040
#define MASK_KEY_USER3  0x0080
#define MASK_JOY1_RIGHT 0x0100
#define MASK_JOY1_LEFT  0x0200
#define MASK_JOY1_UP    0x0400
#define MASK_JOY1_DOWN  0x0800
#define MASK_JOY1_BTN   0x1000
#define MASK_KEY_USER4  0x2000



extern void emu_init(void);
extern void emu_start(void);

extern void emu_InitJoysticks(void);
extern int emu_SwapJoysticks(int statusOnly);
extern unsigned short emu_DebounceLocalKeys(void);
extern int emu_ReadKeys(void);
extern int emu_GetPad(void);
extern int emu_ReadAnalogJoyX(int min, int max);
extern int emu_ReadAnalogJoyY(int min, int max);
extern int emu_ReadI2CKeyboard(void);
extern unsigned char emu_ReadI2CKeyboard2(int row);
extern void emu_KeyboardOnUp(int keymodifer, int key);
extern void emu_KeyboardOnDown(int keymodifer, int key);


extern int emu_setKeymap(int index);


#endif
