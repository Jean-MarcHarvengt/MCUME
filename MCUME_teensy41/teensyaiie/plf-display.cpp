#include "plf-display.h"

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}
#include <Arduino.h>
#include "bios-font.h"

#ifdef HAS_T4_VGA
#include "vga_t_dma.h"
typedef uint8_t Pixel;
#define RGBVAL(r,g,b)  RGBVAL8(r,g,b)
#else
#include "tft_t_dma.h"
typedef uint16_t Pixel;
#define RGBVAL(r,g,b)  RGBVAL16(r,g,b)
#endif

#include "globals.h"
#include "applevm.h"


#define BLACK       RGBVAL(0,0,0)       //0x0000 // 0 black
#define MAGENTA     RGBVAL(192,0,48)    //0xC006 // 1 magenta
#define DARK_BLUE   RGBVAL(0,0,128)     //0x0010 // 2 dark blue
#define PURPLE      RGBVAL(160,52,168)  //0xA1B5 // 3 purple
#define DARK_GREEN  RGBVAL(0,144,0)     //0x0480 // 4 dark green
#define DARK_GREY   RGBVAL(104,104,104) //0x6B4D // 5 dark grey
#define BLUE        RGBVAL(24,112,248)  //0x1B9F // 6 med blue
#define LIGHT_BLUE  RGBVAL(8,188,232)   //0x0DFD // 7 light blue
#define BROWN       RGBVAL(144,84,40)   //0x92A5 // 8 brown
#define ORANGE      RGBVAL(248,24,40)   //0xF8C5 // 9 orange
#define LIGHT_GREY  RGBVAL(144,168,168) //0x9555 // 10 light gray
#define PINK        RGBVAL(248,156,144) //0xFCF2 // 11 pink
#define GREEN       RGBVAL(0,252,0)     //0x07E0 // 12 green
#define YELLOW      RGBVAL(248,252,0)   //0xFFE0 // 13 yellow
#define AQUA        RGBVAL(128,252,128) //0x87F0 // 14 aqua
#define WHITE       RGBVAL(248,252,248) //0xFFFF  // 15 white

// RGB map of each of the lowres colors
const Pixel loresPixelColors[16] = { 
					BLACK, // 0 black
					MAGENTA, // 1 magenta
					DARK_BLUE, // 2 dark blue
					PURPLE, // 3 purple
					DARK_GREEN, // 4 dark green
					DARK_GREY, // 5 dark grey
					BLUE, // 6 med blue
					LIGHT_BLUE, // 7 light blue
					BROWN, // 8 brown
					ORANGE, // 9 orange
					LIGHT_GREY, // 10 light gray
					PINK, // 11 pink
					GREEN, // 12 green
					YELLOW, // 13 yellow
					AQUA, // 14 aqua
					WHITE  // 15 white
};

Pixel backgroundColor;

PlfDisplay::PlfDisplay()
{
  /*
  for (int i = 0; i<16; i++) {
    uint16_t val = loresPixelColors[i];
    uint16_t r = ((val >> (6+5))&0x1f)<<3;
    uint16_t g = ((val >> (5))&0x3f)<<2;
    uint16_t b = ((val)&0x1f)<<3;
    printf("c  %i RGBVAL(%u,%u,%u)\n", i,r,g,b);
  }
*/
  backgroundColor = DARK_BLUE;
}

PlfDisplay::~PlfDisplay()
{
}


void PlfDisplay::redraw()
{
}


void PlfDisplay::blit(AiieRect r)
{
  uint8_t *videoBuffer = g_vm->videoBuffer; // FIXME: poking deep
  uint16_t pixel;
  for (uint8_t y=r.top; y<=r.bottom; y++) {
    Pixel * scrlinept = (Pixel *)emu_LineBuffer(y);
    scrlinept += (TFT_WIDTH - (r.right - r.left))/2;
    for (uint16_t x=r.left; x<=r.right; x++) {
      pixel = y * (DISPLAYRUN >> 1) + (x >> 1);
      uint8_t colorIdx;
      if (!(x & 0x01)) {
        colorIdx = videoBuffer[pixel] >> 4;
      } else {
        colorIdx = videoBuffer[pixel] & 0x0F;
      }
      scrlinept[x] = loresPixelColors[colorIdx];
    }
  }
}

void PlfDisplay::drawDriveDoor(uint8_t which, bool isOpen)
{
}

void PlfDisplay::setDriveIndicator(uint8_t which, bool isRunning)
{
}

void PlfDisplay::drawBatteryStatus(uint8_t percent)
{
}
