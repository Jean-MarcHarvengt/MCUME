#ifndef IOPINS_H
#define IOPINS_H

#include "platform_config.h"


#ifdef MCUME_REV1

// Speaker
#define AUDIO_PIN       28

// VGA
/*
2-9 RRRGGGBB
10-11 VSYNC and HSYNC
*/
#define VGA_COLORBASE   2
#define VGA_SYNCBASE    10

// TFT
#define TFT_SPIREG      spi1
#define TFT_SPIDREQ     DREQ_SPI1_TX
#define TFT_SCLK        14
#define TFT_MOSI        15
#define TFT_MISO        12
#define TFT_DC          28
#define TFT_CS          13  // 255 for LORES ST7789 (NO CS)
#define TFT_RST         255 // 255 for ILI/ST if connected to 3.3V
#define TFT_BACKLIGHT   255 // hardwired to 3.3v

// SD (see SPI0 in code!!!)
#define SD_SPIREG       spi0
#define SD_SCLK         18
#define SD_MOSI         19
#define SD_MISO         16 
#define SD_CS           17
#define SD_DETECT       255 // 22

// Analog joystick (primary) for JOY2 and 3 extra buttons
#define PIN_JOY2_A1X    26
#define PIN_JOY2_A2Y    27
#define PIN_JOY2_BTN    22
#define PIN_KEY_USER1   20
#define PIN_KEY_USER2   21 

// I2C keyboard (Not available on PICO)
/*
#define I2C_SCL_IO      15? 
#define I2C_SDA_IO      14?
*/

#else

#if (defined(PICOMPUTER) && defined(USE_VGA) )   
// Speaker
#define AUDIO_PIN       9
// VGA
/* RRRGGGBB
   CSYNC */
#define VGA_COLORBASE   0
#define VGA_SYNCBASE    8
#else
// Speaker
#define AUDIO_PIN       0
// VGA
/* RRRGGGBB
   VSYNC and HSYNC */
#define VGA_COLORBASE   2
#define VGA_SYNCBASE    14
#endif


// TFT
#define TFT_SPIREG      spi0
#define TFT_SPIDREQ     DREQ_SPI0_TX
#define TFT_SCLK        18
#define TFT_MOSI        19
#define TFT_MISO        255 // Not required, used for DC... 
#define TFT_DC          16
#ifdef PICOMPUTER
#ifdef PICOMPUTERMAX
#define TFT_RST         255
#define TFT_CS          21
#define TFT_BACKLIGHT   20
#else
#define TFT_RST         21
#define TFT_CS          255
#define TFT_BACKLIGHT   20
#endif
#else
// MCUME_REV2 (ILI)
#define TFT_RST         21
#define TFT_CS          17
#define TFT_BACKLIGHT   255 // hardwired to 3.3v
#endif


// SD (see SPI0 in code!!!)
#define SD_SPIREG       spi1
#define SD_SCLK         10 //14
#define SD_MOSI         11 //15
#define SD_MISO         12 
#define SD_CS           13
#define SD_DETECT       255 // 22

// PSRAM (exclusive with TFT)
#define PSRAM_SPIREG    spi0
#define PSRAM_SCLK      18
#define PSRAM_MOSI      19
#define PSRAM_MISO      16 // DC
#define PSRAM_CS        17


#ifdef PICOMPUTER
#if defined(USE_VGA)  
// Keyboard matrix 
//Cols (out)
#define KCOLOUT1        20
#define KCOLOUT2        21
#define KCOLOUT3        22
#define KCOLOUT4        26
#define KCOLOUT5        27
#define KCOLOUT6        28
//Rows (in)
#define KROWIN1         14
#define KROWIN2         15
#define KROWIN3         16
#define KROWIN4         17
#define KROWIN5         18
#define KROWIN6         19
#else
// Keyboard matrix 
//Cols (out)
#define KCOLOUT1        1
#define KCOLOUT2        2
#define KCOLOUT3        3
#define KCOLOUT4        4
#define KCOLOUT5        5
#define KCOLOUT6        14
//Rows (in)
#define KROWIN1         6
#define KROWIN2         9
#define KROWIN3         15
#define KROWIN4         8
#define KROWIN5         7
#define KROWIN6         22
#endif

#define KLED            25

#else
#ifdef MCUME_REV1

// Analog joystick (primary) for JOY2 and 2 extra buttons
#define PIN_JOY2_A1X    26
#define PIN_JOY2_A2Y    27
#define PIN_JOY2_BTN    22
#define PIN_KEY_USER1   20
#define PIN_KEY_USER2   21 
#else

// Digital joystick (primary) for JOY2 and 2 extra buttons
//#define PIN_JOY2_A1X    26
//#define PIN_JOY2_A2Y    27

#define PIN_JOY2_1      27  // UP
#define PIN_JOY2_2      26  // DOWN
#define PIN_JOY2_3      28  // RIGHT
#define PIN_JOY2_4      22  // LEFT
#define PIN_JOY2_BTN    1
#define PIN_KEY_USER1   20
#define PIN_KEY_USER2   21 

#endif
#endif

#endif

// Second joystick (Not available on PICO)
//#define PIN_JOY1_BTN     2
//#define PIN_JOY1_1       14 // UP
//#define PIN_JOY1_2       7  // DOWN
//#define PIN_JOY1_3       6  // RIGHT
//#define PIN_JOY1_4       5  // LEFT

#endif
