#ifndef IOPINS_H
#define IOPINS_H

#include "platform_config.h"

#define VGA_DMA_CHANNEL 2 // requires 2 channels
#define TFT_DMA_CHANNEL 2 // requires 1 channel
#define AUD_DMA_CHANNEL 4 // requires 1 or 3 channels
#define PSR_DMA_CHANNEL 7 // requires 2 channels (PSRAM)

//#####################################################

#ifdef PICO2ZX

// Speaker
#define AUDIO_PIN       0

// TFT
#define TFT_SPIREG      spi1
#define TFT_SPIDREQ     DREQ_SPI1_TX
#define TFT_SCLK        10
#define TFT_MOSI        11
#define TFT_MISO        255
#define TFT_DC          9
#define TFT_CS          8  // 255 for LORES ST7789 (NO CS)
#define TFT_RST         255 // 255 for ILI/ST if connected to 3.3V
#define TFT_BACKLIGHT   255 // hardwired to 3.3v

// SD
#define SD_SCLK         6
#define SD_MOSI         7
#define SD_MISO         4 
#define SD_CS           5
#define SD_DETECT       255
// Second SPI bus and DMA not conflicting with USB
#define SD_SPIREG       spi0

// HDMI
#define HDMI_CLK_MINUS  0 // GPIO12
#define HDMI_CLK_PLUS   1 // GPIO13
#define HDMI_D0_MINUS   2 // GPIO14 
#define HDMI_D0_PLUS    3 // GPIO15
#define HDMI_D1_MINUS   4 // GPIO16
#define HDMI_D1_PLUS    5 // GPIO17
#define HDMI_D2_MINUS   6 // GPIO18
#define HDMI_D2_PLUS    7 // GPIO19

// KEYBOARD MATRIX
#define RP_DAT          2
#define RP_CLK          3
#define KROWIN0         20
#define KROWIN1         21
#define KROWIN2         22
#define KROWIN3         26
#define KROWIN4         27
#define KROWIN5         28
#endif

//#####################################################

#ifdef PICOHYPERPET

// Speaker
#define AUDIO_PIN       21

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

// 2 buttons
#define PIN_KEY_USER1   28
#define PIN_KEY_USER2   27 

// SD
#define SD_SCLK         6
#define SD_MOSI         7
#define SD_MISO         4 
#define SD_CS           5
#define SD_DETECT       255
// Second SPI bus and DMA not conflicting with USB
#define SD_SPIREG       spi0


#define PSRAM_PIN_SCK       10
#define PSRAM_PIN_MOSI      11
#define PSRAM_PIN_MISO      8 
#define PSRAM_PIN_CS        9

#define PSRAM_ASYNC         1
#define PSRAM_SCLK          10
#define PSRAM_MOSI          11
#define PSRAM_MISO          8 
#define PSRAM_CS            9
#define PSRAM_SPIREG    	spi1

// HDMI
#define HDMI_D0_PLUS    0 // GPIO12
#define HDMI_D0_MINUS   1 // GPIO13
#define HDMI_CLK_PLUS   2 // GPIO14 
#define HDMI_CLK_MINUS  3 // GPIO15
#define HDMI_D2_PLUS    4 // GPIO16
#define HDMI_D2_MINUS   5 // GPIO17
#define HDMI_D1_PLUS    6 // GPIO18
#define HDMI_D1_MINUS   7 // GPIO19

#endif


#endif
