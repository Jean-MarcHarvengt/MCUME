/*
  TFT/VGA driver
  DMA TFT driver based on C64 ILI9341 dma driver from Frank Bösing, 2017
*/

#ifndef _T4_DSPH_
#define _T4_DSPH_

#ifdef __cplusplus
#include <Arduino.h>
#include <DMAChannel.h>
#endif

#include "platform_config.h"
#include "iopins.h"



#ifndef TFT_WIDTH
#define TFT_WIDTH      320 
#endif
#define TFT_REALWIDTH  320

#ifndef TFT_HEIGHT
#define TFT_HEIGHT     240
#endif
#define TFT_REALHEIGHT 240


typedef enum gfx_mode_t
{
  MODE_UNDEFINED   = 0,
  MODE_TFTILI_320x240 = 1,
  MODE_TFTST_320x240 = 2,
  MODE_VGA_320x240 = 3,
  MODE_VGA_320x480 = 4,
  MODE_VGA_352x240 = 5,
  MODE_VGA_352x480 = 6,
  MODE_VGA_512x240 = 7,
  MODE_VGA_512x480 = 8,
  MODE_VGA_640x240 = 9,
  MODE_VGA_640x480 = 10
} gfx_mode_t;

typedef enum gfx_error_t
{
  GFX_OK = 0,
  GFX_ERROR = -1
} gfx_error_t;


#ifdef __cplusplus

class T4_DSP
{
  public:
    T4_DSP();

    gfx_error_t begin(gfx_mode_t mode);
    gfx_mode_t getMode(void);
    void startRefresh(void);
    void stopRefresh();
    
    int get_frame_buffer_size(int *width, int *height);
    void setArea(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);

    // wait next Vsync
    void waitSync();
    void waitLine(int line);
    
    // NoDMA functions
    void fillScreenNoDma(uint16_t color);
    void drawTextNoDma(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize);
    void drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap);
    void drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh);

    // DMA functions
    void fillScreen(uint16_t color);
    void drawText(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawSprite(int16_t x, int16_t y, const uint16_t *bitmap);
    void drawSprite(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh);

    void writeLine(int width, int height, int y, uint16_t *buf);
    void writeLinePal(int width, int height, int y, uint8_t *buf, uint16_t *palette16);
    void writeScreenPal(int width, int height, int stride, uint8_t *buf, uint16_t *palette16);
    void writeLine8(int width, int height, int y, uint8_t *buf, uint16_t *palette16);
  
  protected:   
    static uint8_t _rst, _cs, _dc;
    static uint8_t _mosi, _sclk;
    static uint8_t _vsync_pin;
    static DMAChannel flexio1DMA;
    static DMAChannel flexio2DMA; 
    void tft_setup(bool isST);
    static void TFT_isr(void);
    static void QT3_isr(void);
};

#endif
#endif
