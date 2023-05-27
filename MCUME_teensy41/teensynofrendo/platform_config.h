#ifndef _PLATFORM_CONFIG_H_
#define _PLATFORM_CONFIG_H_

#define TEECOMPUTER    1

#ifdef TEECOMPUTER
#define TFTSPI1        1
//#define HAS_T4_VGA     1
#define HAS_SND        1
#define HAS_USB        1
#define HAS_USBKEY     1
//#define HAS_USBMOUSE   1
//#define HAS_USBMIDI    1
#define PT8211         1

#else

#define HAS_T4_VGA     1
//#define INVX           1
#define INVY           1
#define HAS_SND        1
#define HAS_USB        1
#define HAS_USBKEY     1
//#define HAS_USBMOUSE   1
//#define HAS_USBMIDI    1
#define PT8211         1

#endif


//#define ILI9341        1
//#define ST7789         1
//#define SWAP_JOYSTICK  1
//#define LOHRES         1
//#define ROTATE_SCREEN  1
//#define EXTERNAL_SD    1


//#define USE_SDFAT      1
//#define SD_FAT_TYPE    1
//#define USE_SDFS       1
//#define SDFSDEV        "1:"


#endif
