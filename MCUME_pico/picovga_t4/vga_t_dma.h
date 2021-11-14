/*
  Wrapping class to extend VGA_T4 to TFT_T_DMA
*/

#ifndef _VGA_T_DMAH_
#define _VGA_T_DMAH_

#ifdef __cplusplus
#include "VGA_t4.h"
#endif


#define RGBVAL16(r,g,b)  VGA_RGB(r,g,b)
#define RGBVAL8(r,g,b)   VGA_RGB(r,g,b)



#ifdef HIRES
#define TFT_WIDTH      640
#define TFT_REALWIDTH  640
#else
#define TFT_WIDTH      320 
#define TFT_REALWIDTH  320
#endif

#define TFT_HEIGHT     240
#define TFT_REALHEIGHT 240



#ifdef __cplusplus

class TFT_T_DMA: public VGA_T4
{
  public:
    // Fake touch screen functions     
    bool isTouching(void) { return false; }
    void readRaw(uint16_t * oX, uint16_t * oY, uint16_t * oZ) { }
    void readCal(uint16_t * oX, uint16_t * oY, uint16_t * oZ) { };
    void callibrateTouch(uint16_t xMin,uint16_t yMin,uint16_t xMax,uint16_t yMax) { }

    // fake DMA functions
    void startDMA(void) { };
    void stopDMA(void) { };
    void flipscreen(bool flip) { };
        
    // fake no DMA functions
    void writeScreenNoDma(const vga_pixel *pcolors) { writeScreen(pcolors); }
    void fillScreenNoDma(vga_pixel color) { clear(color); }
    void drawTextNoDma(int16_t x, int16_t y, const char * text, vga_pixel fgcolor, vga_pixel bgcolor, bool doublesize) { drawText(x,y,text,fgcolor,bgcolor,doublesize); }
    void drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, vga_pixel color) { drawRect(x, y, w, h, color); }
    void drawSpriteNoDma(int16_t x, int16_t y, const int16_t *bitmap) { drawSprite(x, y, bitmap); }
    void drawSpriteNoDma(int16_t x, int16_t y, const int16_t *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh) { drawSprite(x, y, bitmap, croparx, cropary, croparw, croparh); }
};


#endif
#endif
