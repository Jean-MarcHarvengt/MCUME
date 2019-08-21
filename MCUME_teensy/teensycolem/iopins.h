#ifndef IOPINS_H
#define IOPINS_H

//#define OLD_LAYOUT 1

#include "tft_t_dma_config.h"

#ifndef OLD_LAYOUT

#ifdef ST7789
// ST7789
#define TFT_SCLK        13
#define TFT_MOSI        12
#define TFT_MISO        11
#define TFT_TOUCH_CS    255
#define TFT_TOUCH_INT   255
#define TFT_DC          9
#define TFT_CS          255
#define TFT_RST         23
#else
// ILI9341
#define TFT_SCLK        13
#define TFT_MOSI        12
#define TFT_MISO        11
#define TFT_TOUCH_CS    255 //38
#define TFT_TOUCH_INT   255 //37
#define TFT_DC          9
#define TFT_CS          23
#define TFT_RST         255
#endif


// SD
#define SD_SCLK        13
#define SD_MOSI        12
#define SD_MISO        11  
#define SD_CS          BUILTIN_SDCARD

// I2C keyboard
#define I2C_SCL_IO     19
#define I2C_SDA_IO     18

// Analog joystick (primary) for JOY2 and 5 extra buttons
#define PIN_JOY2_A1X    A1
#define PIN_JOY2_A2Y    A2
#define PIN_JOY2_BTN    17
#define PIN_KEY_USER1   3 //34
#define PIN_KEY_USER2   4 //35
//#define PIN_KEY_USER3   255
//#define PIN_KEY_USER4   255


#else // OLD LAYOUT!!!!

#define HAS_VGA 1

// ILI9341
#define TFT_SCLK        13
#define TFT_MOSI        12
#define TFT_MISO        11
#define TFT_TOUCH_CS    38
#define TFT_TOUCH_INT   37
#define TFT_DC          9
#define TFT_CS          10
#define TFT_RST         255  //connected to 3.3V

// I2C keyboard
#define I2C_SCL_IO      3
#define I2C_SDA_IO      4

// Analog joystick (primary) for JOY2 and 5 extra buttons
#define PIN_JOY2_A1X    A12
#define PIN_JOY2_A2Y    A13
#define PIN_JOY2_BTN    36
#define PIN_KEY_USER1   35
#define PIN_KEY_USER2   34
#define PIN_KEY_USER3   33
#define PIN_KEY_USER4   39

// Second joystick
#define PIN_JOY1_BTN     30
#define PIN_JOY1_1       16
#define PIN_JOY1_2       17
#define PIN_JOY1_3       18
#define PIN_JOY1_4       19
#endif

#endif




