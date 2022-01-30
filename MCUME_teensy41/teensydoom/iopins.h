#ifndef IOPINS_H
#define IOPINS_H

#include "platform_config.h"

#ifdef TEECOMPUTER

// Teecomputer layout

// VGA
// R                    3  2K
// R                    4  1K
// R                    33 500
// G                    11 2K
// G                    13 1K
// G                    2  500
// B                    10 820
// B                    12 390
// HSYNC                15 82
// VSYNC                8  82

// Display
#define TFT_SCLK        27
#define TFT_MOSI        26
#define TFT_MISO        255
#define TFT_TOUCH_CS    255
#define TFT_TOUCH_INT   255
#define TFT_DC          23
#define TFT_CS          22 // 255 for LORES ST7789 (NO CS)
#define TFT_RST         255 // 255 for ILI/ST if connected to 3.3V or 24 if really needed


// SD
#define SD_CS           BUILTIN_SDCARD

// Audio
#define AUDIO_I2S_DIN   7
#define AUDIO_I2S_BCK   21
#define AUDIO_I2S_LCK   20

// Keyboard matrix
#define KLED            14
//Cols (out)
//pico 1,2,3,4,5,14
//teen 16,6,24,25,28,31
#define KCOLOUT1        16
#define KCOLOUT2        6
#define KCOLOUT3        24
#define KCOLOUT4        25
#define KCOLOUT5        28
#define KCOLOUT6        31
//Rows (in)
//pico 9,8,6,15,7,22
//teen 19,18,17,5,29,30,32 //5,6,16,17,18,19
#define KROWIN1         19
#define KROWIN2         18
#define KROWIN3         17
#define KROWIN4         5
#define KROWIN5         29
#define KROWIN6         30
#define KROWIN7         32

#define PIN_KEY_USER1   41
#define PIN_KEY_USER2   40

// Second joystick (external)
#define PIN_JOY1_BTN     34
#define PIN_JOY1_1       35 // UP
#define PIN_JOY1_2       36 // DOWN
#define PIN_JOY1_3       38 // RIGHT
#define PIN_JOY1_4       37 // LEFT

#else

// Original Layout
#define TFT_SCLK        13
#define TFT_MOSI        11
#define TFT_MISO        12
#define TFT_TOUCH_CS    255
#define TFT_TOUCH_INT   255
#define TFT_DC          9
#define TFT_CS          22 // 255 for LORES ST7789 (NO CS)
#define TFT_RST         23 // 255 for ILI/ST if connected to 3.3V

// SD
#define SD_CS          BUILTIN_SDCARD

// I2C keyboard
#define I2C_SCL_IO     19
#define I2C_SDA_IO     18

// Analog joystick (primary) for JOY2 and 5 extra buttons
#ifdef HAS_T4_VGA
#define PIN_JOY2_A1X    A3
#define PIN_JOY2_A2Y    A2
#define PIN_JOY2_BTN    14
#define PIN_KEY_USER1   22
#define PIN_KEY_USER2   23

// Second joystick
#define PIN_JOY1_BTN     34
#define PIN_JOY1_1       35 // UP
#define PIN_JOY1_2       36 // DOWN
#define PIN_JOY1_3       38 // RIGHT
#define PIN_JOY1_4       37 // LEFT

#else
#define PIN_JOY2_A1X    A1
#define PIN_JOY2_A2Y    A2
#define PIN_JOY2_BTN    17
#define PIN_KEY_USER1   3 //34
#define PIN_KEY_USER2   4 //35

// Second joystick
#define PIN_JOY1_BTN     2
#define PIN_JOY1_1       14 // UP
#define PIN_JOY1_2       7  // DOWN
#define PIN_JOY1_3       6  // RIGHT
#define PIN_JOY1_4       5  // LEFT
#endif

#endif

#endif
