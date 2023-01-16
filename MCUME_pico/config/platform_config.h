#ifndef _PLATFORM_CONFIG_H_
#define _PLATFORM_CONFIG_H_

// MCUME_REV2 is new layout with VGA+TFT+AnalogJoy+3buttons+Sound  
// MCUME_REV1 had only support for VGA+AnalogJoy+3buttons+Sound 
// PICOMPUTER has support for TFT+keymatrix+Sound 


//#define PICOMPUTER     1
//#define PICOMPUTERMAX  1
//#define PICORETROVGA   1
#define MCUME_REV1      1
//#define MCUME_REV2      1

#ifdef PICOMPUTER
//#define SWAP_ALT_DEL   1
#define ST7789         1
#define LOHRES         1
#define FLIP_SCREEN    1
#define INVX           1
#define HAS_SND        1
#endif

#ifdef PICOMPUTERMAX
#undef LOHRES
#undef FLIP_SCREEN
#define ST7789         1
#define ST7789_POL     1
#define INVX           1
#define HAS_SND        1
#define PICOMPUTER     1
#endif

#ifdef PICORETROVGA
#undef LOHRES
#undef FLIP_SCREEN
#undef ST7789
#define USE_VGA        1
#define INVX           1
#define HAS_SND        1
#define PICOMPUTER     1
#endif

#ifdef MCUME_REV1
#define USE_VGA        1
#define INVX           1
#define HAS_SND        1
#endif

#ifdef MCUME_REV2
//#define HAS_PSRAM      1
#define USE_VGA        1
//#define ILI9341        1
//#define ST7789         1
//#define ST7789_POL     1
#define INVX           1
#define HAS_SND        1
#endif

//#define HAS_SND        1
//#define INVY           1
//#define HAS_USBKEY     1
//#define USE_VGA        1
//#define ILI9341        1
//#define ST7789         1
//#define SWAP_JOYSTICK  1
//#define LOHRES         1
//#define ROTATE_SCREEN  1
//#define FLIP_SCREEN  1
//#define HAS_PSRAM      1


#endif
