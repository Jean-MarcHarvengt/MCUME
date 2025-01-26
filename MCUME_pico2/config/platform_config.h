#ifndef _PLATFORM_CONFIG_H_
#define _PLATFORM_CONFIG_H_

#define PICOHYPERPET   1
#define KEYLAYOUT      KLAYOUT_UK

#define INVX           1
#define HAS_SND        1
#define USE_VGA        1
#define HAS_USBHOST    1
#define HAS_USBPIO     1

//#define ILI9341        1
//#define ST7789         1
//#define SWAP_JOYSTICK  1
//#define LOHRES         1
//#define ROTATE_SCREEN  1
//#define FLIP_SCREEN  1
//#define HAS_PSRAM      1

#ifdef HAS_SND

#define SOUNDRATE 22050                           // sound rate [Hz]

//#define AUDIO_8BIT     1
#define AUDIO_1DMA      1


#ifdef AUDIO_8BIT
typedef uint8_t  audio_sample;
#else
typedef short  audio_sample;
#endif

#endif

#include "program_config.h"

#endif
