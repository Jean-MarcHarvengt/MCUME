/*
  Based on C64 ILI9341 dma driver from Frank Bösing, 2017
*/

#ifndef _TFT_T_DMAH_
#define _TFT_T_DMAH_

#ifdef __cplusplus
#include <Arduino.h>
#include <SPI.h>
#include <DMAChannel.h>
#endif

#include "tft_t_dma_config.h"

#define RGBVAL32(r,g,b)  ( (r<<16) | (g<<8) | b )
#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )
#define RGBVAL8(r,g,b)   ( (((r>>5)&0x07)<<5) | (((g>>5)&0x07)<<2) | (((b>>6)&0x3)<<0) )
#define R16(rgb) ((rgb>>8)&0xf8) 
#define G16(rgb) ((rgb>>3)&0xfc) 
#define B16(rgb) ((rgb<<3)&0xf8) 


#define TFT_HEIGHT     240

#ifdef ILI9341
#define TFT_WIDTH      256
#define TFT_REALWIDTH  320
#define TFT_REALHEIGHT 240
#endif
#ifdef ST7789
#define TFT_WIDTH      240
#define TFT_REALWIDTH  240
#define TFT_REALHEIGHT 240
#endif

#define LINES_PER_BLOCK         64
#define NR_OF_BLOCK             4
#define SCREEN_DMA_NUM_SETTINGS NR_OF_BLOCK


#ifdef ILI9341

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR    0x30
#define ILI9341_MADCTL   0x36
#define ILI9341_VSCRSADD 0x37
#define ILI9341_PIXFMT   0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

#define TFT_CASET ILI9341_CASET
#define TFT_PASET ILI9341_PASET
#define TFT_RAMWR ILI9341_RAMWR
#define TFT_MADCTL ILI9341_MADCTL

#endif


#ifdef ST7789

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x00
#define ST77XX_MADCTL_BGR 0x08
#define ST77XX_MADCTL_MH  0x04

#define TFT_CASET ST7735_CASET
#define TFT_PASET ST7735_RASET
#define TFT_RAMWR ST7735_RAMWR
#define TFT_MADCTL ST7735_MADCTL

#endif



#ifdef __cplusplus

class TFT_T_DMA
{
  public:
  	TFT_T_DMA(uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12,  uint8_t touch_cs=38,  uint8_t touch_irq=37);

    void setArea(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);  
	void begin(void);
	void flipscreen(bool flip);
    boolean isflipped(void);
	void startDMA(void);
	void stopDMA();

    // Touch screen functions
    #define TOUCH_ENABLED() ((_touch_cs != 255) && (_touch_irq != 255))      
    bool isTouching(void) { return ((!TOUCH_ENABLED())?false:(digitalRead(_touch_irq) == LOW)); }
    void readRaw(uint16_t * oX, uint16_t * oY, uint16_t * oZ);
    void readCal(uint16_t * oX, uint16_t * oY, uint16_t * oZ);
    void callibrateTouch(uint16_t xMin,uint16_t yMin,uint16_t xMax,uint16_t yMax);

    // NoDMA functions
    void writeScreenNoDma(const uint16_t *pcolors);
    void fillScreenNoDma(uint16_t color);
    void drawTextNoDma(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize);
    void drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap);
    void drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh);

    // DMA functions
    uint16_t * getLineBuffer(int j);
    void writeScreen(int width, int height, int stride, uint8_t *buffer, uint16_t *palette16);
    void writeLine(int width, int height, int stride, uint8_t *buffer, uint16_t *palette16);
    void  writeLine(int width, int height, int y, uint16_t *buf);
	void fillScreen(uint16_t color);
    void drawText(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawSprite(int16_t x, int16_t y, const uint16_t *bitmap);
    void drawSprite(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh);

	
  protected:   
    uint8_t _rst, _cs, _dc;
	uint8_t _miso, _mosi, _sclk;
    uint8_t _touch_irq=255, _touch_cs=255;
	bool flipped=false;

    void wait(void);  
	void enableTouchIrq();
};

#endif
#endif

