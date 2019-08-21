#ifndef IOPINS_H
#define IOPINS_H

// ILI9341
//#define PIN_NUM_CS      (gpio_num_t)15
//#define PIN_NUM_CLK     (gpio_num_t)14
//#define PIN_NUM_MISO    (gpio_num_t)12
//#define PIN_NUM_MOSI    (gpio_num_t)13
//#define PIN_NUM_DC      (gpio_num_t)16

#define TPIN_NUM_CS        (gpio_num_t)32
#define TPIN_NUM_IRQ       (gpio_num_t)33


#define PIN_NUM_CS        (gpio_num_t)17
#define PIN_NUM_CLK       (gpio_num_t)18
#define PIN_NUM_MISO      (gpio_num_t)19
#define PIN_NUM_MOSI      (gpio_num_t)23
#define PIN_NUM_DC        (gpio_num_t)21

// SD card SPI
#define SPIN_NUM_CS      (gpio_num_t)15
#define SPIN_NUM_CLK     (gpio_num_t)14
#define SPIN_NUM_MISO    (gpio_num_t)12
#define SPIN_NUM_MOSI    (gpio_num_t)13


// I2C keyboard
#define I2C_SCL_IO        (gpio_num_t)5 
#define I2C_SDA_IO        (gpio_num_t)4 


// Analog joystick (primary) for JOY2 and 5 extra buttons
#define PIN_JOY2_A1X     ADC2_CHANNEL_7 // 27 //ADC1_CHANNEL_0
#define PIN_JOY2_A2Y     ADC2_CHANNEL_2 // 2  //ADC1_CHANNEL_3
#define PIN_JOY2_BTN    32

#define PIN_KEY_USER1   35
#define PIN_KEY_USER2   34
#define PIN_KEY_USER3   39
#define PIN_KEY_USER4   36
/*
#define PIN_KEY_ESCAPE  23
*/

// Second joystick
/*
#define PIN_JOY1_BTN     30
#define PIN_JOY1_1       16
#define PIN_JOY1_2       17
#define PIN_JOY1_3       18
#define PIN_JOY1_4       19
*/

#endif




