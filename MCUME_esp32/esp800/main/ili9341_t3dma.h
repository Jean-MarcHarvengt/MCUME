/*
  ILI9341 SPI driver inspired from the Teensy version of Frank Bösing, 2017
*/

#ifndef _ILI9341_T3DMAH_
#define _ILI9341_T3DMAH_


#define FLIP_SCREEN 1


#define RGBVAL32(r,g,b)  ( (r<<16) | (g<<8) | b )
#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )
#define RGBVAL8(r,g,b)   ( (((r>>5)&0x07)<<5) | (((g>>5)&0x07)<<2) | (((b>>6)&0x3)<<0) )


#define ILI9341_TFTWIDTH      320
#define ILI9341_TFTHEIGHT     240
#define ILI9341_TFTREALWIDTH  320
#define ILI9341_TFTREALHEIGHT 240


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

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

#ifdef __cplusplus


#define MAX_SPI_TRANS   7  
#define NR_OF_BLOCK     4
#define LINES_PER_BLOCK 64 //(ILI9341_TFTREALHEIGHT/4)

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;


class ILI9341_t3DMA
{
  public:
    ILI9341_t3DMA(uint8_t _CS = 17, uint8_t _DC = 18, uint8_t _RST = 255, uint8_t _MOSI=23, uint8_t _CLK=18, uint8_t _MISO=19,  uint8_t touch_cs=32,  uint8_t touch_irq=33);

    //void setFrameBuffer(uint16_t * fb);
    //static uint16_t * getFrameBuffer(void);

    void begin(void);
    void flipscreen(bool flip);
    bool isflipped(void);
    void start(void);
    void refresh(void);
    void refreshPrepare(void);
    void refreshFinish(void);
    //void stop();
    //void wait(void);	
	uint16_t * getLineBuffer(int j);

    void fillScreen(uint16_t color);
    void writeScreen(int width, int height, int stride, uint8_t *buffer, uint16_t *palette16);
    void writeLine(int width, int height, int stride, uint8_t *buffer, uint16_t *palette16);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawSprite(int16_t x, int16_t y, const uint16_t *bitmap);
    void drawSprite(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh);
    void drawText(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize);

    void fillScreenNoDma(uint16_t color) { fillScreen(color); };
    void drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap) { drawSprite(x,y,bitmap); };
    void drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh) { drawSprite(x,y,bitmap,croparx,cropary,croparw,croparh); };
    void drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) { drawRect(x,y,w,h,color); };
    void drawTextNoDma(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) { drawText(x,y,text,fgcolor,bgcolor,doublesize); };

    /*
    void writeScreenNoDma(const uint16_t *pcolors);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    uint16_t getPixel(int16_t x, int16_t y);
    */
        
     
    void touchBegin();
    void readRo(uint16_t * oX, uint16_t * oY, uint16_t * oZ);
    bool isTouching();
    void readRaw(uint16_t * oX, uint16_t * oY, uint16_t * oZ);
    void readCal(uint16_t * oX, uint16_t * oY, uint16_t * oZ);
    void callibrateTouch(uint16_t xMin,uint16_t yMin,uint16_t xMax,uint16_t yMax);
  	
  protected:
	  bool flipped=false;
    void enableTouchIrq();   
};

#endif
#endif

