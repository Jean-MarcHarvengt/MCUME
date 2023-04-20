/*
  This file is part of DISPLAY library. 
  Supports VGA and TFT display

  DISPLAY library is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

	Teensy VGA inspired from the original Teensy3 uVGA library of Eric PREVOTEAU.
	QTIMER/FlexIO code based on Teensy4 examples of KurtE, Manitou and easone 
	from the Teensy4 forum (https://forum.pjrc.com)
*/

#ifndef _PICO_DSP_H
#define _PICO_DSP_H

#ifdef __cplusplus
#include <stdio.h>
#include <math.h>
#include "pico.h"
#endif

#include "platform_config.h"
#include "iopins.h"


#ifdef LOHRES
#define TFT_WIDTH      240 
#define TFT_REALWIDTH  240
#else
#ifdef OVERRULE_WIDTH
#define TFT_WIDTH      OVERRULE_WIDTH
#else 
#define TFT_WIDTH      320 
#endif
#define TFT_REALWIDTH  320
#endif
#ifdef OVERRULE_HEIGHT
#define TFT_HEIGHT     OVERRULE_HEIGHT
#else 
#define TFT_HEIGHT     240
#endif
#define TFT_REALHEIGHT 240

#define AUDIO_SAMPLE_BUFFER_SIZE 256
#define DEFAULT_VSYNC_PIN 8

typedef uint8_t  vga_pixel;
typedef uint16_t dsp_pixel;

typedef enum gfx_mode_t
{
  MODE_UNDEFINED   = 0,
  MODE_TFT_320x240 = 1,
  MODE_VGA_320x240 = 2
} gfx_mode_t;

typedef enum vga_error_t
{
	GFX_OK = 0,
	GFX_ERROR = -1
} gfx_error_t;

class PICO_DSP
{
public:

  PICO_DSP();

  // Initialization
  gfx_error_t begin(gfx_mode_t mode);
  gfx_mode_t getMode(void);
  void end();
  void startRefresh(void);
  void stopRefresh();
  void begin_audio(int samplesize, void (*callback)(short * stream, int len));
  void end_audio();


  // framebuffer/screen operation
  int get_frame_buffer_size(int *width, int *height);
  void setArea(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
  void flipscreen(bool flip);
  bool isflipped(void);

  // wait next Vsync
  void waitSync();
  void waitLine(int line);

  // =========================================================
  // graphic primitives
  // =========================================================

  //dsp_pixel * getLineBuffer(int j);
  void writeLine(int width, int height, int y, dsp_pixel *buf);
  void writeLinePal(int width, int height, int y, uint8_t *buffer, dsp_pixel *palette16);
  void writeScreenPal(int width, int height, int stride, uint8_t *buf, dsp_pixel *palette16);
  
  void fillScreen(dsp_pixel color);
  void drawText(int16_t x, int16_t y, const char * text, dsp_pixel fgcolor, dsp_pixel bgcolor, bool doublesize);
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, dsp_pixel color);
  void drawSprite(int16_t x, int16_t y, const dsp_pixel *bitmap);
  void drawSprite(int16_t x, int16_t y, const dsp_pixel *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh);

  // NoDMA functions
  void fillScreenNoDma(dsp_pixel color);
  void drawTextNoDma(int16_t x, int16_t y, const char * text, dsp_pixel fgcolor, dsp_pixel bgcolor, bool doublesize);
  void drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, dsp_pixel color);
  void drawSpriteNoDma(int16_t x, int16_t y, const dsp_pixel *bitmap);
  void drawSpriteNoDma(int16_t x, int16_t y, const dsp_pixel *bitmap, uint16_t croparx, uint16_t cropary, uint16_t croparw, uint16_t croparh);

private:
  static uint8_t _vsync_pin;

protected:   
  uint8_t _rst, _cs, _dc;
  uint8_t _miso, _mosi, _sclk, _bkl;
  bool flipped=false;  
};


#endif


