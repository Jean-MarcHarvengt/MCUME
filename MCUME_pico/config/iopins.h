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

// Speaker
#define AUDIO_PIN       0 //28

// VGA
/*
2-9 RRRGGGBB
10-11 VSYNC and HSYNC
*/
#define VGA_COLORBASE   2
#define VGA_SYNCBASE    14

// TFT
#define TFT_SPIREG      spi0
#define TFT_SPIDREQ     DREQ_SPI0_TX
#define TFT_SCLK        18
#define TFT_MOSI        19
#define TFT_MISO        255 // Not required, used for DC... 
#define TFT_DC          16  //28
#define TFT_CS          255 //17  // 255 for LORES ST7789 (NO CS)
#define TFT_RST         21 //255  // 255 for ILI/ST if connected to 3.3V, required by 256x256 ST

// SD (see SPI0 in code!!!)
#define SD_SPIREG       spi1
#define SD_SCLK         10 //14
#define SD_MOSI         11 //15
#define SD_MISO         12 
#define SD_CS           13
#define SD_DETECT       255 // 22

#ifdef PICOMPUTER
// Keyboard matrix 

//Cols (out)
//1,2,3,4,5,14
//Rows (in)
//6,9,15,8,7,22

#else
// Analog joystick (primary) for JOY2 and 3 extra buttons
#define PIN_JOY2_A1X    26
#define PIN_JOY2_A2Y    27
#define PIN_JOY2_BTN    22
#define PIN_KEY_USER1   20
#define PIN_KEY_USER2   21 

#endif

#endif

// Second joystick (Not available on PICO)
//#define PIN_JOY1_BTN     2
//#define PIN_JOY1_1       14 // UP
//#define PIN_JOY1_2       7  // DOWN
//#define PIN_JOY1_3       6  // RIGHT
//#define PIN_JOY1_4       5  // LEFT

#endif
