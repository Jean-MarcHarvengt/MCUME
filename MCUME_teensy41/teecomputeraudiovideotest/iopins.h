#ifndef IOPINS_H
#define IOPINS_H

#include "platform_config.h"


// Display
#define TFT_SCLK        27
#define TFT_MOSI        26
#define TFT_MISO        255
#define TFT_TOUCH_CS    255
#define TFT_TOUCH_INT   255
#define TFT_DC          23
#define TFT_CS          22  // 255 for LORES ST7789 (NO CS)
#define TFT_RST         255 // 255 for ILI/ST if connected to 3.3V or 24 if really needed


/*
#define TFT_SCLK        13
#define TFT_MOSI        11
#define TFT_MISO        12
#define TFT_TOUCH_CS    255
#define TFT_TOUCH_INT   255
#define TFT_DC          9
#define TFT_CS          22 // 255 for LORES ST7789 (NO CS)
#define TFT_RST         23 // 255 for ILI/ST if connected to 3.3V
*/


// Audio
#define AUDIO_I2S_DIN   7
#define AUDIO_I2S_BCK   21
#define AUDIO_I2S_LCK   20


#endif
