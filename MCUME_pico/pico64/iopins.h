#ifndef IOPINS_H
#define IOPINS_H

#include "platform_config.h"


// VGA (see in code!!!)
/*
2-9 RRRGGGBB
10-11 VSYNC and HSYNC
*/

// SD (see SPI0 in code!!!)
/*
#define SD_SCLK        18
#define SD_MOSI        19
#define SD_MISO        16 
#define SD_CS          17
*/

// I2C keyboard (not used)
/*
#define I2C_SCL_IO     15? 
#define I2C_SDA_IO     14?
*/

// Analog joystick (primary) for JOY2 and 3 extra buttons
#define PIN_JOY2_A1X    26
#define PIN_JOY2_A2Y    27
#define PIN_JOY2_BTN    22
#define PIN_KEY_USER1   20
#define PIN_KEY_USER2   21 

// Second joystick
//#define PIN_JOY1_BTN     2
//#define PIN_JOY1_1       14 // UP
//#define PIN_JOY1_2       7  // DOWN
//#define PIN_JOY1_3       6  // RIGHT
//#define PIN_JOY1_4       5  // LEFT


#endif
