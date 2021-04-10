/*
	This file is part of VGA_t4 library.

	VGA_t4 library is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Copyright (C) 2020 J-M Harvengt

	Inspired from the original Teensy3 uVGA library of Eric PREVOTEAU.
	QTIMER/FlexIO code based on Teensy4 examples of KurtE, Manitou and easone 
	from the Teensy4 forum (https://forum.pjrc.com)
*/

#include "VGA_t4.h"
#include "VGA_font8x8.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
//#include "pico/scanvideo.h"
//#include "pico/scanvideo/composable_scanline.h"
#include "scanvideo.h"
#include "composable_scanline.h"
#include "pico/sync.h"
#include <string.h>

#define R16(rgb) ((rgb>>8)&0xf8) 
#define G16(rgb) ((rgb>>3)&0xfc) 
#define B16(rgb) ((rgb<<3)&0xf8) 


// 8 bits 320x240 frame buffer => 64K
static vga_pixel * visible_framebuffer = NULL;
static vga_pixel * framebuffer = NULL;
static vga_pixel * fb0 = NULL;
static vga_pixel * fb1 = NULL;

static int  fb_width;
static int  fb_height;
static int  fb_stride;
static int  left_border;
static int  top_border;
static int  currentLine;

static semaphore_t video_initted;
static void core1_func();

PolyDef	PolySet;  // will contain a polygon data

#define vga_mode vga_mode_320x240_60

#define RGBVAL16(r,g,b)  ( (((b>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((r>>3)&0x1f)<<0) )
#define PICO_SCANVIDEO_PIXEL_FROM_RGBVAL8(rgb) (((rgb&0x3)<<(PICO_SCANVIDEO_PIXEL_BSHIFT))|(((rgb&0x1C)>>2)<<(PICO_SCANVIDEO_PIXEL_GSHIFT))|(((rgb&0xE0)>>5)<<(PICO_SCANVIDEO_PIXEL_RSHIFT)))



static void draw_fb_line(scanvideo_scanline_buffer_t *buffer) {
    currentLine = scanvideo_scanline_number(buffer->scanline_id);
    uint16_t *p = (uint16_t *) buffer->data;
 
    //if ( (currentLine > top_border)  && (currentLine < (fb_height+top_border)) ) 
    {
      currentLine -= top_border;
      //if (left_border) {
      //  *p++ = COMPOSABLE_COLOR_RUN;
      //  *p++ = 0;
      //  *p++ = left_border/2 - 3;
      //}  
      uint8_t *linebuffer = &visible_framebuffer[fb_stride*currentLine];
      *p++ = COMPOSABLE_RAW_RUN;
      *p++ = *linebuffer++; // first pixel (8bit to 16bits conv) 
      *p++ = fb_width - 3;  // len - 3
      for (uint bar = 0; bar < (fb_width-1); bar++) {
          *p++ =  *linebuffer++; // other pixels (8bit to 16bits conv)        
      }
    }

    // black pixel to end line
    *p++ = COMPOSABLE_RAW_1P;
    *p++ = 0;
    // end of line with alignment padding
    *p++ = COMPOSABLE_EOL_SKIP_ALIGN;
    *p++ = 0;

    buffer->data_used = ((uint32_t *) p) - buffer->data;
    assert(buffer->data_used < buffer->data_max);

    buffer->status = SCANLINE_OK;
}


static void core1_func() {
    // initialize video and interrupts on core 1
    scanvideo_setup(&vga_mode);
    scanvideo_timing_enable(true);
    sem_release(&video_initted);

    while (true) {
        scanvideo_scanline_buffer_t *scanline_buffer = scanvideo_begin_scanline_generation(true);
        draw_fb_line(scanline_buffer);
        scanvideo_end_scanline_generation(scanline_buffer);
    }
}

VGA_T4::VGA_T4()
{
}

void VGA_T4::tweak_video(int shiftdelta, int numdelta, int denomdelta)
{
}

// display VGA image
vga_error_t VGA_T4::begin(vga_mode_t mode)
{
  switch(mode) {
    case VGA_MODE_320x240:
      fb_width = 320; //200;
      fb_height = 240; //200; //240 ;
      left_border = (320-fb_width)/2;
      top_border = (240-fb_height)/2;
      fb_stride = fb_width;
      break;
    case VGA_MODE_320x480:
      break;   
    case VGA_MODE_640x240:
      break;
    case VGA_MODE_640x480:
      break;   
    case VGA_MODE_512x240:
      break;
    case VGA_MODE_512x480:
      break; 
    case VGA_MODE_352x240:
      break;
    case VGA_MODE_352x480:
      break;         
  }	


  /* initialize gfx buffer */
  if (fb0 == NULL) {
    fb0 = (vga_pixel *)malloc(fb_stride*fb_height*sizeof(vga_pixel)); 
  }
  memset((void*)fb0,0, fb_stride*fb_height*sizeof(vga_pixel));
  visible_framebuffer = fb0;
  framebuffer = fb0;

  // create a semaphore to be posted when video init is complete
  sem_init(&video_initted, 0, 1);

  // launch all the video on core 1, so it isn't affected by USB handling on core 0
  multicore_launch_core1(core1_func);

  // wait for initialization of video to be complete
  sem_acquire_blocking(&video_initted);

  // init buffer with random
  uint8_t * buf = &framebuffer[0];
  for( int i=0; i<(fb_width*fb_height); i++)
      *buf++ = VGA_RGB(rand() % 255,rand() % 255,rand() % 255);

  return(VGA_OK);
}

void VGA_T4::end()
{
}

void VGA_T4::debug()
{ 
}

// retrieve size of the frame buffer
void VGA_T4::get_frame_buffer_size(int *width, int *height)
{
  *width = fb_width;
  *height = fb_height;
}

void VGA_T4::waitSync()
{
  scanvideo_wait_for_vblank();
}

void VGA_T4::waitLine(int line)
{
//  while (currentLine != line) {};
}

void VGA_T4::clear(vga_pixel color) {
  int i,j;
  for (j=0; j<fb_height; j++)
  {
    vga_pixel * dst=&framebuffer[j*fb_stride];
    for (i=0; i<fb_width; i++)
    {
      *dst++ = color;
    }
  }
}


void VGA_T4::drawPixel(int x, int y, vga_pixel color){
	if((x>=0) && (x<=fb_width) && (y>=0) && (y<=fb_height))
		framebuffer[y*fb_stride+x] = color;
}

vga_pixel VGA_T4::getPixel(int x, int y){
  return(framebuffer[y*fb_stride+x]);
}

vga_pixel * VGA_T4::getLineBuffer(int j) {
  return (&framebuffer[j*fb_stride]);
}

void VGA_T4::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, vga_pixel color) {
  int i,j,l=y;
  for (j=0; j<h; j++)
  {
    vga_pixel * dst=&framebuffer[l*fb_stride+x];
    for (i=0; i<w; i++)
    {
      *dst++ = color;
    }
    l++;
  }
}

void VGA_T4::drawText(int16_t x, int16_t y, const char * text, vga_pixel fgcolor, vga_pixel bgcolor, bool doublesize) {
  vga_pixel c;
  vga_pixel * dst;
  
  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];

    int l=y;
    for (int i=0;i<8;i++)
    {     
      unsigned char bits;
      if (doublesize) {
        dst=&framebuffer[l*fb_stride+x];
        bits = *charpt;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;     
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        l++;
      }
      dst=&framebuffer[l*fb_stride+x]; 
      bits = *charpt++;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      l++;
    }
    x +=8;
  } 
}

void VGA_T4::drawSprite(int16_t x, int16_t y, const int16_t *bitmap) {
    drawSprite(x,y,bitmap, 0,0,0,0);
}

void VGA_T4::drawSprite(int16_t x, int16_t y, const int16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
{
  int bmp_offx = 0;
  int bmp_offy = 0;
  int16_t *bmp_ptr;
    
  int w =*bitmap++;
  int h = *bitmap++;


  if ( (arw == 0) || (arh == 0) ) {
    // no crop window
    arx = x;
    ary = y;
    arw = w;
    arh = h;
  }
  else {
    if ( (x>(arx+arw)) || ((x+w)<arx) || (y>(ary+arh)) || ((y+h)<ary)   ) {
      return;
    }
    
    // crop area
    if ( (x > arx) && (x<(arx+arw)) ) { 
      arw = arw - (x-arx);
      arx = arx + (x-arx);
    } else {
      bmp_offx = arx;
    }
    if ( ((x+w) > arx) && ((x+w)<(arx+arw)) ) {
      arw -= (arx+arw-x-w);
    }  
    if ( (y > ary) && (y<(ary+arh)) ) {
      arh = arh - (y-ary);
      ary = ary + (y-ary);
    } else {
      bmp_offy = ary;
    }
    if ( ((y+h) > ary) && ((y+h)<(ary+arh)) ) {
      arh -= (ary+arh-y-h);
    }     
  }

   
  int l=ary;
  bitmap = bitmap + bmp_offy*w + bmp_offx;
  for (int row=0;row<arh; row++)
  {
    vga_pixel * dst=&framebuffer[l*fb_stride+arx];  
    bmp_ptr = (int16_t *)bitmap;
    for (int col=0;col<arw; col++)
    {
        uint16_t pix= *bmp_ptr++;
        *dst++ = VGA_RGB(R16(pix),G16(pix),B16(pix));
    } 
    bitmap +=  w;
    l++;
  } 
}

void VGA_T4::writeLine(int width, int height, int y, uint8_t *buf, vga_pixel *palette) {
  if ( (height<fb_height) && (height > 2) ) y += (fb_height-height)/2;
  vga_pixel * dst=&framebuffer[y*fb_stride];
  if (width > fb_width) {
#ifdef TFT_LINEARINT    
    int delta = (width/(width-fb_width))-1;
    int pos = delta;
    for (int i=0; i<fb_width; i++)
    {
      uint16_t val = palette[*buf++];
      pos--;
      if (pos == 0) {
#ifdef LINEARINT_HACK
        val  = ((uint32_t)palette[*buf++] + val)/2;
#else
        uint16_t val2 = *buf++;
        val = RGBVAL16((R16(val)+R16(val2))/2,(G16(val)+G16(val2))/2,(B16(val)+B16(val2))/2);
#endif        
        pos = delta;
      }
      *dst++=val;
    }
#else
    int step = ((width << 8)/fb_width);
    int pos = 0;
    for (int i=0; i<fb_width; i++)
    {
      *dst++=palette[buf[pos >> 8]];
      pos +=step;
    }  
#endif
  }
  else if ((width*2) == fb_width) {
    for (int i=0; i<width; i++)
    {
      *dst++=palette[*buf];
      *dst++=palette[*buf++];
    } 
  }
  else {
    if (width <= fb_width) {
      dst += (fb_width-width)/2;
    }
    for (int i=0; i<width; i++)
    {
      *dst++=palette[*buf++];
    } 
  }
}

void VGA_T4::writeLine(int width, int height, int y, vga_pixel *buf) {
  if ( (height<fb_height) && (height > 2) ) y += (fb_height-height)/2;
  uint8_t * dst=&framebuffer[y*fb_stride];    
  if (width > fb_width) {
    int step = ((width << 8)/fb_width);
    int pos = 0;
    for (int i=0; i<fb_width; i++)
    {
      *dst++ = buf[pos >> 8];
      pos +=step;
    }        
  }
  else if ((width*2) == fb_width) {
    for (int i=0; i<width; i++)
    {
      *dst++=*buf;
      *dst++=*buf++;
    }     
  }
  else {
    if (width <= fb_width) {
      dst += (fb_width-width)/2;
    }
    for (int i=0; i<width; i++)
    {
      *dst++=*buf++;
    }
  }
}

void VGA_T4::writeLine16(int width, int height, int y, uint16_t *buf) {
  if ( (height<fb_height) && (height > 2) ) y += (fb_height-height)/2;
  uint8_t * dst=&framebuffer[y*fb_stride];    
  if (width > fb_width) {
    int step = ((width << 8)/fb_width);
    int pos = 0;
    for (int i=0; i<fb_width; i++)
    {
      uint16_t pix = buf[pos >> 8];
      *dst++ = VGA_RGB(R16(pix),G16(pix),B16(pix)); 
      pos +=step;
    }        
  }
  else if ((width*2) == fb_width) {
    for (int i=0; i<width; i++)
    {
      uint16_t pix = *buf++;
      uint8_t col = VGA_RGB(R16(pix),G16(pix),B16(pix));
      *dst++= col;
      *dst++= col;
    }       
  }
  else {
    if (width <= fb_width) {
      dst += (fb_width-width)/2;
    }
    for (int i=0; i<width; i++)
    {
      uint16_t pix = *buf++;
      *dst++=VGA_RGB(R16(pix),G16(pix),B16(pix));
    }      
  }
}

void VGA_T4::writeScreen(int width, int height, int stride, uint8_t *buf, vga_pixel *palette) {
  uint8_t *buffer=buf;
  uint8_t *src; 

  int i,j,y=0;
  if (width*2 <= fb_width) {
    for (j=0; j<height; j++)
    {
      vga_pixel * dst=&framebuffer[y*fb_stride];
      src=buffer;
      for (i=0; i<width; i++)
      {
        vga_pixel val = palette[*src++];
        *dst++ = val;
        *dst++ = val;
      }
      y++;
      if (height*2 <= fb_height) {
        dst=&framebuffer[y*fb_stride];    
        src=buffer;
        for (i=0; i<width; i++)
        {
          vga_pixel val = palette[*src++];
          *dst++ = val;
          *dst++ = val;
        }
        y++;
      } 
      buffer += stride;
    }
  }
  else if (width <= fb_width) {
    //dst += (fb_width-width)/2;
    for (j=0; j<height; j++)
    {
      vga_pixel * dst=&framebuffer[y*fb_stride+(fb_width-width)/2];  
      src=buffer;
      for (i=0; i<width; i++)
      {
        vga_pixel val = palette[*src++];
        *dst++ = val;
      }
      y++;
      if (height*2 <= fb_height) {
        dst=&framebuffer[y*fb_stride+(fb_width-width)/2];   
        src=buffer;
        for (i=0; i<width; i++)
        {
          vga_pixel val = palette[*src++];
          *dst++ = val;
        }
        y++;
      }
      buffer += stride;  
    }
  }   
}

void VGA_T4::copyLine(int width, int height, int ysrc, int ydst) {
  if ( (height<fb_height) && (height > 2) ) {
    ysrc += (fb_height-height)/2;
    ydst += (fb_height-height)/2;
  }    
  uint8_t * src=&framebuffer[ysrc*fb_stride];    
  uint8_t * dst=&framebuffer[ydst*fb_stride]; 
  memcpy(dst,src,width);   
} 


//--------------------------------------------------------------
// Draw a line between 2 points
// x1,y1   : 1st point
// x2,y2   : 2nd point
// Color   : 16bits color
//--------------------------------------------------------------
void VGA_T4::drawline(int16_t x1, int16_t y1, int16_t x2, int16_t y2, vga_pixel color){
  uint8_t yLonger = 0;
  int incrementVal, endVal;
  int shortLen = y2-y1;
  int longLen = x2-x1;
  int decInc;
  int j = 0, i = 0;

  if(ABS(shortLen) > ABS(longLen)) {
    int swap = shortLen;
    shortLen = longLen;
    longLen = swap;
    yLonger = 1;
  }

  endVal = longLen;

  if(longLen < 0) {
    incrementVal = -1;
    longLen = -longLen;
    endVal--;
  } else {
    incrementVal = 1;
    endVal++;
  }

  if(longLen == 0)
    decInc = 0;
  else
    decInc = (shortLen << 16) / longLen;

  if(yLonger) {
    for(i = 0;i != endVal;i += incrementVal) {
      drawPixel(x1 + (j >> 16),y1 + i,color);
      j += decInc;
    }
  } else {
    for(i = 0;i != endVal;i += incrementVal) {
      drawPixel(x1 + i,y1 + (j >> 16),color);
      j += decInc;
    }
  }
}

//--------------------------------------------------------------
// Draw a horizontal line
// x1,y1   : starting point
// lenght  : lenght in pixels
// color   : 16bits color
//--------------------------------------------------------------
void VGA_T4::draw_h_line(int16_t x, int16_t y, int16_t lenght, vga_pixel color){
	drawline(x , y , x + lenght , y , color);
}

//--------------------------------------------------------------
// Draw a vertical line
// x1,y1   : starting point
// lenght  : lenght in pixels
// color   : 16bits color
//--------------------------------------------------------------
void VGA_T4::draw_v_line(int16_t x, int16_t y, int16_t lenght, vga_pixel color){
	drawline(x , y , x , y + lenght , color);
}

//--------------------------------------------------------------
// Draw a circle.
// x, y - center of circle.
// r - radius.
// color - color of the circle.
//--------------------------------------------------------------
void VGA_T4::drawcircle(int16_t x, int16_t y, int16_t radius, vga_pixel color){
  int16_t a, b, P;

  a = 0;
  b = radius;
  P = 1 - radius;

  do {
      drawPixel(a+x, b+y, color);
      drawPixel(b+x, a+y, color);
      drawPixel(x-a, b+y, color);
      drawPixel(x-b, a+y, color);
      drawPixel(b+x, y-a, color);
      drawPixel(a+x, y-b, color);
      drawPixel(x-a, y-b, color);
      drawPixel(x-b, y-a, color);

    if(P < 0)
      P+= 3 + 2*a++;
    else
      P+= 5 + 2*(a++ - b--);
  } while(a <= b);    
}
  
//--------------------------------------------------------------
// Displays a full circle.
// x          : specifies the X position
// y          : specifies the Y position
// radius     : specifies the Circle Radius
// fillcolor  : specifies the Circle Fill Color
// bordercolor: specifies the Circle Border Color
//--------------------------------------------------------------
void VGA_T4::drawfilledcircle(int16_t x, int16_t y, int16_t radius, vga_pixel fillcolor, vga_pixel bordercolor){
  int32_t  D;    /* Decision Variable */
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */

  D = 3 - (radius << 1);

  CurX = 0;
  CurY = radius;

  while (CurX <= CurY)
  {
    if(CurY > 0)
    {
      draw_v_line(x - CurX, y - CurY, 2*CurY, fillcolor);
      draw_v_line(x + CurX, y - CurY, 2*CurY, fillcolor);
    }

    if(CurX > 0)
    {
      draw_v_line(x - CurY, y - CurX, 2*CurX, fillcolor);
      draw_v_line(x + CurY, y - CurX, 2*CurX, fillcolor);
    }
    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }

  drawcircle(x, y, radius,bordercolor);
}
  
//--------------------------------------------------------------
// Displays an Ellipse.
// cx: specifies the X position
// cy: specifies the Y position
// radius1: minor radius of ellipse.
// radius2: major radius of ellipse.
// color: specifies the Color to use for draw the Border from the Ellipse.
//--------------------------------------------------------------
void VGA_T4::drawellipse(int16_t cx, int16_t cy, int16_t radius1, int16_t radius2, vga_pixel color){
  int x = -radius1, y = 0, err = 2-2*radius1, e2;
  float K = 0, rad1 = 0, rad2 = 0;

  rad1 = radius1;
  rad2 = radius2;

  if (radius1 > radius2)
  {
    do {
      K = (float)(rad1/rad2);
      drawPixel(cx-x,cy+(uint16_t)(y/K),color);
      drawPixel(cx+x,cy+(uint16_t)(y/K),color);
      drawPixel(cx+x,cy-(uint16_t)(y/K),color);
      drawPixel(cx-x,cy-(uint16_t)(y/K),color);

      e2 = err;
      if (e2 <= y) {
        err += ++y*2+1;
        if (-x == y && e2 <= x) e2 = 0;
      }
      if (e2 > x) err += ++x*2+1;
    }
    while (x <= 0);
  }
  else
  {
    y = -radius2;
    x = 0;
    do {
      K = (float)(rad2/rad1);
      drawPixel(cx-(uint16_t)(x/K),cy+y,color);
      drawPixel(cx+(uint16_t)(x/K),cy+y,color);
      drawPixel(cx+(uint16_t)(x/K),cy-y,color);
      drawPixel(cx-(uint16_t)(x/K),cy-y,color);

      e2 = err;
      if (e2 <= x) {
        err += ++x*2+1;
        if (-y == x && e2 <= y) e2 = 0;
      }
      if (e2 > y) err += ++y*2+1;
    }
    while (y <= 0);
  }
}
  
// Draw a filled ellipse.
// cx: specifies the X position
// cy: specifies the Y position
// radius1: minor radius of ellipse.
// radius2: major radius of ellipse.
// fillcolor  : specifies the Color to use for Fill the Ellipse.
// bordercolor: specifies the Color to use for draw the Border from the Ellipse.
void VGA_T4::drawfilledellipse(int16_t cx, int16_t cy, int16_t radius1, int16_t radius2, vga_pixel fillcolor, vga_pixel bordercolor){
  int x = -radius1, y = 0, err = 2-2*radius1, e2;
  float K = 0, rad1 = 0, rad2 = 0;

  rad1 = radius1;
  rad2 = radius2;

  if (radius1 > radius2)
  {
    do
    {
      K = (float)(rad1/rad2);
      draw_v_line((cx+x), (cy-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1) , fillcolor);
      draw_v_line((cx-x), (cy-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1) , fillcolor);

      e2 = err;
      if (e2 <= y)
      {
        err += ++y*2+1;
        if (-x == y && e2 <= x) e2 = 0;
      }
      if (e2 > x) err += ++x*2+1;

    }
    while (x <= 0);
  }
  else
  {
    y = -radius2;
    x = 0;
    do
    {
      K = (float)(rad2/rad1);
      draw_h_line((cx-(uint16_t)(x/K)), (cy+y), (2*(uint16_t)(x/K) + 1) , fillcolor);
      draw_h_line((cx-(uint16_t)(x/K)), (cy-y), (2*(uint16_t)(x/K) + 1) , fillcolor);

      e2 = err;
      if (e2 <= x)
      {
        err += ++x*2+1;
        if (-y == x && e2 <= y) e2 = 0;
      }
      if (e2 > y) err += ++y*2+1;
    }
    while (y <= 0);
  }
  drawellipse(cx,cy,radius1,radius2,bordercolor);
}
  
//--------------------------------------------------------------
// Draw a Triangle.
// ax,ay, bx,by, cx,cy - the triangle points.
// color    - color of the triangle.
//--------------------------------------------------------------
void VGA_T4::drawtriangle(int16_t ax, int16_t ay, int16_t bx, int16_t by, int16_t cx, int16_t cy, vga_pixel color){
  drawline(ax , ay , bx , by , color);
  drawline(bx , by , cx , cy , color);
  drawline(cx , cy , ax , ay , color);
}
  
//--------------------------------------------------------------
// Draw a Filled Triangle.
// ax,ay, bx,by, cx,cy - the triangle points.
// fillcolor - specifies the Color to use for Fill the triangle.
// bordercolor - specifies the Color to use for draw the Border from the triangle.
//--------------------------------------------------------------
void VGA_T4::drawfilledtriangle(int16_t ax, int16_t ay, int16_t bx, int16_t by, int16_t cx, int16_t cy, vga_pixel fillcolor, vga_pixel bordercolor){
  float ma, mb, mc    ; //'gradient of the lines
  float start, finish ; //'draw a line from start to finish!
  float tempspace     ; //'temporary storage for swapping values...
  double x1,x2,x3      ;
  double y1,y2,y3      ;
  int16_t n           ;

  //' need to sort out ay, by and cy into order.. highest to lowest
  //'
  if(ay < by)
  {
    //'swap x's
    tempspace = ax;
    ax = bx;
    bx = tempspace;

    //'swap y's
    tempspace = ay;
    ay = by;
    by = tempspace;
  }

  if(ay < cy)
  {
    //'swap x's
    tempspace = ax;
    ax = cx;
    cx = tempspace;

    //'swap y's
    tempspace = ay;
    ay = cy;
    cy = tempspace;
  }

  if(by < cy)
  {
    //'swap x's
    tempspace = bx;
    bx = cx;
    cx = tempspace;

    //'swap y's
    tempspace = by;
    by = cy;
    cy = tempspace;
  }

  //' Finally - copy the values in order...

  x1 = ax; x2 = bx; x3 = cx;
  y1 = ay; y2 = by; y3 = cy;

  //'bodge if y coordinates are the same
  if(y1 == y2)  y2 = y2 + 0.01;
  if(y2 == y3)  y3 = y3 + 0.01;
  if(y1 == y3)  y3 = y3 + 0.01;

  ma = (x1 - x2) / (y1 - y2);
  mb = (x3 - x2) / (y2 - y3);
  mc = (x3 - x1) / (y1 - y3);

  //'from y1 to y2
  for(n = 0;n >= (y2 - y1);n--)
  {
    start = n * mc;
    finish = n * ma;
    drawline((int16_t)(x1 - start), (int16_t)(n + y1), (int16_t)(x1 + finish), (int16_t)(n + y1), fillcolor);
  }


  //'and from y2 to y3

  for(n = 0;n >= (y3 - y2);n--)
  {
    start = n * mc;
    finish = n * mb;
    drawline((int16_t)(x1 - start - ((y2 - y1) * mc)), (int16_t)(n + y2), (int16_t)(x2 - finish), (int16_t)(n + y2), fillcolor);
  }

  // draw the border color triangle
  drawtriangle(ax,ay,bx,by,cx,cy,bordercolor);
}


//--------------------------------------------------------------
//  Displays a Rectangle at a given Angle.
//  centerx			: specifies the center of the Rectangle.
//	centery
//  w,h 	: specifies the size of the Rectangle.
//	angle			: specifies the angle for drawing the rectangle
//  color	    	: specifies the Color to use for Fill the Rectangle.
//--------------------------------------------------------------
void VGA_T4::drawquad(int16_t centerx, int16_t centery, int16_t w, int16_t h, int16_t angle, vga_pixel color){
	int16_t	px[4],py[4];
	float	l;
	float	raddeg = 3.14159 / 180;
	float	w2 = w / 2.0;
	float	h2 = h / 2.0;
	float	vec = (w2*w2)+(h2*h2);
	float	w2l;
	float	pangle[4];

	l = sqrtf(vec);
	w2l = w2 / l;
	pangle[0] = acosf(w2l) / raddeg;
	pangle[1] = 180.0 - (acosf(w2l) / raddeg);
	pangle[2] = 180.0 + (acosf(w2l) / raddeg);
	pangle[3] = 360.0 - (acosf(w2l) / raddeg);
	px[0] = (int16_t)(calcco[((int16_t)(pangle[0]) + angle) % 360] * l + centerx);
	py[0] = (int16_t)(calcsi[((int16_t)(pangle[0]) + angle) % 360] * l + centery);
	px[1] = (int16_t)(calcco[((int16_t)(pangle[1]) + angle) % 360] * l + centerx);
	py[1] = (int16_t)(calcsi[((int16_t)(pangle[1]) + angle) % 360] * l + centery);
	px[2] = (int16_t)(calcco[((int16_t)(pangle[2]) + angle) % 360] * l + centerx);
	py[2] = (int16_t)(calcsi[((int16_t)(pangle[2]) + angle) % 360] * l + centery);
	px[3] = (int16_t)(calcco[((int16_t)(pangle[3]) + angle) % 360] * l + centerx);
	py[3] = (int16_t)(calcsi[((int16_t)(pangle[3]) + angle) % 360] * l + centery);
	// here we draw the quad
	drawline(px[0],py[0],px[1],py[1],color);
	drawline(px[1],py[1],px[2],py[2],color);
	drawline(px[2],py[2],px[3],py[3],color);
	drawline(px[3],py[3],px[0],py[0],color);
}
  
//--------------------------------------------------------------
//  Displays a filled Rectangle at a given Angle.
//  centerx			: specifies the center of the Rectangle.
//	centery
//  w,h 	: specifies the size of the Rectangle.
//	angle			: specifies the angle for drawing the rectangle
//  fillcolor    	: specifies the Color to use for Fill the Rectangle.
//  bordercolor  	: specifies the Color to use for draw the Border from the Rectangle.
//--------------------------------------------------------------
void VGA_T4::drawfilledquad(int16_t centerx, int16_t centery, int16_t w, int16_t h, int16_t angle, vga_pixel fillcolor, vga_pixel bordercolor){
	int16_t	px[4],py[4];
	float	l;
	float	raddeg = 3.14159 / 180;
	float	w2 = w / 2.0;
	float	h2 = h / 2.0;
	float	vec = (w2*w2)+(h2*h2);
	float	w2l;
	float	pangle[4];

	l = sqrtf(vec);
	w2l = w2 / l;
	pangle[0] = acosf(w2l) / raddeg;
	pangle[1] = 180.0 - (acosf(w2l) / raddeg);
	pangle[2] = 180.0 + (acosf(w2l) / raddeg);
	pangle[3] = 360.0 - (acosf(w2l) / raddeg);
	px[0] = (int16_t)(calcco[((int16_t)(pangle[0]) + angle) % 360] * l + centerx);
	py[0] = (int16_t)(calcsi[((int16_t)(pangle[0]) + angle) % 360] * l + centery);
	px[1] = (int16_t)(calcco[((int16_t)(pangle[1]) + angle) % 360] * l + centerx);
	py[1] = (int16_t)(calcsi[((int16_t)(pangle[1]) + angle) % 360] * l + centery);
	px[2] = (int16_t)(calcco[((int16_t)(pangle[2]) + angle) % 360] * l + centerx);
	py[2] = (int16_t)(calcsi[((int16_t)(pangle[2]) + angle) % 360] * l + centery);
	px[3] = (int16_t)(calcco[((int16_t)(pangle[3]) + angle) % 360] * l + centerx);
	py[3] = (int16_t)(calcsi[((int16_t)(pangle[3]) + angle) % 360] * l + centery);
	// We draw 2 filled triangle for made the quad
	// To be uniform we have to use only the Fillcolor
	drawfilledtriangle(px[0],py[0],px[1],py[1],px[2],py[2],fillcolor,fillcolor);
	drawfilledtriangle(px[2],py[2],px[3],py[3],px[0],py[0],fillcolor,fillcolor);
	// here we draw the BorderColor from the quad
	drawline(px[0],py[0],px[1],py[1],bordercolor);
	drawline(px[1],py[1],px[2],py[2],bordercolor);
	drawline(px[2],py[2],px[3],py[3],bordercolor);
	drawline(px[3],py[3],px[0],py[0],bordercolor);
}

//--------------------------------------------------------------
//  Displays a Polygon.
//  centerx			: are specified with PolySet.Center.x and y.
//	centery
//  cx              : Translate the polygon in x direction
//  cy              : Translate the polygon in y direction
//  bordercolor  	: specifies the Color to use for draw the Border from the polygon.
//  polygon points  : are specified with PolySet.Pts[n].x and y 
//  After the last polygon point , set PolySet.Pts[n + 1].x to 10000
//  Max number of point for the polygon is set by MaxPolyPoint previously defined.
//--------------------------------------------------------------
void VGA_T4::drawpolygon(int16_t cx, int16_t cy, vga_pixel bordercolor){
	uint8_t n = 1;
	while((PolySet.Pts[n].x < 10000) && (n < MaxPolyPoint)){
		drawline(PolySet.Pts[n].x + cx, 
	             PolySet.Pts[n].y + cy, 
				 PolySet.Pts[n - 1].x + cx , 
				 PolySet.Pts[n - 1].y + cy, 
				 bordercolor);
		n++;		
	}
	// close the polygon
	drawline(PolySet.Pts[0].x + cx, 
	         PolySet.Pts[0].y + cy, 
			 PolySet.Pts[n - 1].x + cx, 
			 PolySet.Pts[n - 1].y + cy, 
			 bordercolor);
}

//--------------------------------------------------------------
//  Displays a filled Polygon.
//  centerx			: are specified with PolySet.Center.x and y.
//	centery
//  cx              : Translate the polygon in x direction
//  cy              : Translate the polygon in y direction
//  fillcolor  	    : specifies the Color to use for filling the polygon.
//  bordercolor  	: specifies the Color to use for draw the Border from the polygon.
//  polygon points  : are specified with PolySet.Pts[n].x and y 
//  After the last polygon point , set PolySet.Pts[n + 1].x to 10000
//  Max number of point for the polygon is set by MaxPolyPoint previously defined.
//--------------------------------------------------------------
void VGA_T4::drawfullpolygon(int16_t cx, int16_t cy, vga_pixel fillcolor, vga_pixel bordercolor){
	int n,i,j,k,dy,dx;
	int y,temp;
	int a[MaxPolyPoint][2],xi[MaxPolyPoint];
	float slope[MaxPolyPoint];

    n = 0;

	while((PolySet.Pts[n].x < 10000) && (n < MaxPolyPoint)){
		a[n][0] = PolySet.Pts[n].x;
		a[n][1] = PolySet.Pts[n].y;
		n++;
	}

	a[n][0]=PolySet.Pts[0].x;
	a[n][1]=PolySet.Pts[0].y;

	for(i=0;i<n;i++)
	{
		dy=a[i+1][1]-a[i][1];
		dx=a[i+1][0]-a[i][0];

		if(dy==0) slope[i]=1.0;
		if(dx==0) slope[i]=0.0;

		if((dy!=0)&&(dx!=0)) /*- calculate inverse slope -*/
		{
			slope[i]=(float) dx/dy;
		}
	}

	for(y=0;y< 480;y++)
	{
		k=0;
		for(i=0;i<n;i++)
		{

			if( ((a[i][1]<=y)&&(a[i+1][1]>y))||
					((a[i][1]>y)&&(a[i+1][1]<=y)))
			{
				xi[k]=(int)(a[i][0]+slope[i]*(y-a[i][1]));
				k++;
			}
		}

		for(j=0;j<k-1;j++) /*- Arrange x-intersections in order -*/
			for(i=0;i<k-1;i++)
			{
				if(xi[i]>xi[i+1])
				{
					temp=xi[i];
					xi[i]=xi[i+1];
					xi[i+1]=temp;
				}
			}

		for(i=0;i<k;i+=2)
		{
			drawline(xi[i] + cx,y + cy,xi[i+1]+1 + cx,y + cy, fillcolor);
		}

	}

	// Draw the polygon outline
	drawpolygon(cx , cy , bordercolor);
}

//--------------------------------------------------------------
//  Displays a rotated Polygon.
//  centerx			: are specified with PolySet.Center.x and y.
//	centery
//  cx              : Translate the polygon in x direction
//  ct              : Translate the polygon in y direction
//  bordercolor  	: specifies the Color to use for draw the Border from the polygon.
//  polygon points  : are specified with PolySet.Pts[n].x and y 
//  After the last polygon point , set PolySet.Pts[n + 1].x to 10000
//  Max number of point for the polygon is set by MaxPolyPoint previously defined.
//--------------------------------------------------------------
void VGA_T4::drawrotatepolygon(int16_t cx, int16_t cy, int16_t Angle, vga_pixel fillcolor, vga_pixel bordercolor, uint8_t filled)
{
	Point2D 	SavePts[MaxPolyPoint];
	uint16_t	n = 0;
	int16_t		ctx,cty;
	float		raddeg = 3.14159 / 180;
	float		angletmp;
	float		tosquare;
	float		ptsdist;

	ctx = PolySet.Center.x;
	cty = PolySet.Center.y;
	
	while((PolySet.Pts[n].x < 10000) && (n < MaxPolyPoint)){
		// Save Original points coordinates
		SavePts[n] = PolySet.Pts[n];
		// Rotate and save all points
		tosquare = ((PolySet.Pts[n].x - ctx) * (PolySet.Pts[n].x - ctx)) + ((PolySet.Pts[n].y - cty) * (PolySet.Pts[n].y - cty));
		ptsdist  = sqrtf(tosquare);
		angletmp = atan2f(PolySet.Pts[n].y - cty,PolySet.Pts[n].x - ctx) / raddeg;
		PolySet.Pts[n].x = (int16_t)((cosf((angletmp + Angle) * raddeg) * ptsdist) + ctx);
		PolySet.Pts[n].y = (int16_t)((sinf((angletmp + Angle) * raddeg) * ptsdist) + cty);
		n++;
	}	
	
	if(filled != 0)
	  drawfullpolygon(cx , cy , fillcolor , bordercolor);
    else
	  drawpolygon(cx , cy , bordercolor);

	// Get the original points back;
	n=0;
	while((PolySet.Pts[n].x < 10000) && (n < MaxPolyPoint)){
		PolySet.Pts[n] = SavePts[n];
		n++;
	}	
}





/*******************************************************************
 Experimental GAME engine supporting:
 - Multiple tiles layers with independent scrolling
 - Sprites (MAX_SPRITES)
 - up to 256 redefinable tiles
 - up to 256 redefinable sprites
*******************************************************************/
struct Sprite_t {
  int x;
  int y;
  unsigned char index;
};

static vga_pixel * tilesbuffer __attribute__((aligned(32))) = NULL;
static vga_pixel * spritesbuffer __attribute__((aligned(32))) = NULL;
static unsigned char * tilesram __attribute__((aligned(32))) = NULL;
static Sprite_t * spritesdata __attribute__((aligned(32))) = NULL;
static int nb_layers = 0;
static int nb_tiles = 0;
static int nb_sprites = 0;
static int hscr[TILES_MAX_LAYERS];
static int vscr[TILES_MAX_LAYERS];
static int hscr_beg[TILES_MAX_LAYERS]={0,0};
static int hscr_end[TILES_MAX_LAYERS]={TILES_ROWS-1, TILES_ROWS-1};
static int hscr_mask=0;


static void drawSpr(unsigned char index, int x, int y) {
  if ((x + SPRITES_W) <= 0) return;
  if (x >= (fb_width-hscr_mask)) return;
  if ((y + SPRITES_H) <= 0) return;
  if (y >= fb_height) return;
  
  vga_pixel * src=&spritesbuffer[index*SPRITES_W*SPRITES_H];
  int i,j;
  vga_pixel pix;  
  for (j=0; j<SPRITES_H; j++)
  {
    vga_pixel * dst=&framebuffer[((j+y)*fb_stride)+x];
    for (i=0; i<SPRITES_W; i++)
    { 
      pix=*src++;
      if ( (!pix) || ((x+i) < 0) || ((x+i) > (fb_width-hscr_mask)) || ((y+j) < 0) || ((y+j) >= fb_height) ) dst++;
      else *dst++ = pix;
    }
  }
}


static void drawTile(unsigned char tile, int x, int y) {
  vga_pixel * src=&tilesbuffer[tile*TILES_W*TILES_H];
  int i,j;
  for (j=0; j<TILES_H; j++)
  {
    vga_pixel * dst=&framebuffer[((j+y)*fb_stride)+x];
    for (i=0; i<TILES_W; i++)
    { 
      *dst++ = *src++;
    }
  }
}

static void drawTileCropL(unsigned char tile, int x, int y) {
  vga_pixel * src=&tilesbuffer[tile*TILES_W*TILES_H];
  int i,j;
  vga_pixel pix;
  for (j=0; j<TILES_H; j++)
  {
    vga_pixel * dst=&framebuffer[((j+y)*fb_stride)+x];
    for (i=0; i<TILES_W; i++)
    { 
      pix=*src++;
      if ((x+i) < 0) dst++;
      else
        *dst++ = pix;
    }
  }
}

static void drawTileCropR(unsigned char tile, int x, int y) {
  vga_pixel * src=&tilesbuffer[tile*TILES_W*TILES_H];
  int i,j;
  vga_pixel pix;
  for (j=0; j<TILES_H; j++)
  {
    vga_pixel * dst=&framebuffer[((j+y)*fb_stride)+x];
    for (i=0; i<TILES_W; i++)
    { 
      pix = *src++;
      if ((x+i) > (fb_width-hscr_mask)) *dst++=0;
      else
        *dst++ = pix;
    }
  }
}

static void drawTransTile(unsigned char tile, int x, int y) {
  vga_pixel * src=&tilesbuffer[tile*TILES_W*TILES_H];
  vga_pixel pix;
  int i,j;
  for (j=0; j<TILES_H; j++)
  {
    vga_pixel * dst=&framebuffer[((j+y)*fb_stride)+x];
    for (i=0; i<TILES_W; i++)
    { 
      if ((pix=*src++)) *dst++ = pix;
      else dst++;
    }
  }
}

static void drawTransTileCropL(unsigned char tile, int x, int y) {
  vga_pixel * src=&tilesbuffer[tile*TILES_W*TILES_H];
  vga_pixel pix;
  int i,j;
  for (j=0; j<TILES_H; j++)
  {
    vga_pixel * dst=&framebuffer[((j+y)*fb_stride)+x];
    for (i=0; i<TILES_W; i++)
    { 
      pix=*src++;
      if ((x+i) < 0) dst++;
      else
        if (pix) *dst++ = pix;
        else dst++;
    }
  }
}

static void drawTransTileCropR(unsigned char tile, int x, int y) {
  vga_pixel * src=&tilesbuffer[tile*TILES_W*TILES_H];
  vga_pixel pix;
  int i,j;
  for (j=0; j<TILES_H; j++)
  {
    vga_pixel * dst=&framebuffer[((j+y)*fb_stride)+x];
    for (i=0; i<TILES_W; i++)
    { 
      if ((x+i) > (fb_width-hscr_mask)) src++;
      else
        if ((pix=*src++)) *dst++ = pix;
        else *dst++;
    }
  }
}



static void tileText(unsigned char index, int16_t x, int16_t y, const char * text, vga_pixel fgcolor, vga_pixel bgcolor, vga_pixel *dstbuffer, int dstwidth, int dstheight) {
  vga_pixel c;
  vga_pixel * dst;
  
  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];
    int l=y;
    for (int i=0;i<8;i++)
    {     
      unsigned char bits;
      dst=&dstbuffer[(index*dstheight+l)*dstwidth+x]; 
      bits = *charpt++;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      l++;
    }
    x +=8;
  } 
}

static void tileTextOverlay(int16_t x, int16_t y, const char * text, vga_pixel fgcolor) {
  vga_pixel c;
  vga_pixel * dst;
  
  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];
    int l=y;
    for (int i=0;i<8;i++)
    {     
      unsigned char bits;
      dst=&framebuffer[+l*fb_stride+x]; 
      bits = *charpt++;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      bits = bits >> 1;     
      if (bits&0x01) *dst++=fgcolor;
      else dst++;
      l++;
    }
    x +=8;
  } 
}
static const char * hex = "0123456789ABCDEF";

void VGA_T4::begin_gfxengine(int nblayers, int nbtiles, int nbsprites)
{
  // Try double buffering
  if (fb1 == NULL) {
    fb1 = (vga_pixel *)malloc(fb_stride*fb_height*sizeof(vga_pixel));
    memset((void*)fb1,0, fb_stride*fb_height*sizeof(vga_pixel)); 
  }
  if (fb1 != NULL) {
  	memset((void*)fb1,0, fb_stride*fb_height*sizeof(vga_pixel));
  	framebuffer = fb1;
  }

  nb_layers = nblayers;
  nb_tiles = nbtiles;
  nb_sprites = nbsprites;

  if (spritesbuffer == NULL) spritesbuffer = (vga_pixel*)malloc(SPRITES_W*SPRITES_H*sizeof(vga_pixel)*nb_sprites);
  if (tilesbuffer == NULL) tilesbuffer = (vga_pixel*)malloc(TILES_W*TILES_H*sizeof(vga_pixel)*nb_tiles); 
  if (tilesram == NULL) tilesram = (unsigned char*)malloc(TILES_COLS*TILES_ROWS*nb_layers); 
  if (spritesdata == NULL) spritesdata = (Sprite_t *)malloc(SPRITES_MAX*sizeof(Sprite_t)); 

  memset((void*)spritesbuffer,0, SPRITES_W*SPRITES_H*sizeof(vga_pixel)*nb_sprites); 
  memset((void*)tilesbuffer,0, TILES_W*TILES_H*sizeof(vga_pixel)*nb_tiles); 
  memset((void*)tilesram,0,TILES_COLS*TILES_ROWS*nb_layers); 

  /* Random test tiles */
  char numhex[3];
  for (int i=0; i<nb_tiles; i++)
  {
    int r = rand() % 0xBF + 0x40;
    int g = rand() % 0xBF + 0x40;
    int b = rand() % 0xBF + 0x40;
    if (i==0) {
      memset((void*)&tilesbuffer[TILES_W*TILES_H*sizeof(vga_pixel)*i],0, TILES_W*TILES_H*sizeof(vga_pixel));     
    }
    else {
      memset((void*)&tilesbuffer[TILES_W*TILES_H*sizeof(vga_pixel)*i],VGA_RGB(r,g,b), TILES_W*TILES_H*sizeof(vga_pixel)); 
      numhex[0] = hex[(i>>4) & 0xf];
      numhex[1] = hex[i & 0xf];
      numhex[2] = 0;
      if (TILES_W == 16 )tileText(i, 0, 0, numhex, VGA_RGB(0xff,0xff,0xff), VGA_RGB(0x40,0x40,0x40), tilesbuffer,TILES_W,TILES_H); 
    }
  }
  /* Random test sprites */
  for (int i=0; i<nb_sprites; i++)
  {
    int r = rand() % 0xBF + 0x40;
    int g = rand() % 0xBF + 0x40;
    int b = rand() % 0xBF + 0x40;
    if (i==0) {
      memset((void*)&spritesbuffer[SPRITES_W*SPRITES_H*sizeof(vga_pixel)*i],0, SPRITES_W*SPRITES_H*sizeof(vga_pixel));     
    }
    else {
      memset((void*)&spritesbuffer[SPRITES_W*SPRITES_H*sizeof(vga_pixel)*i],VGA_RGB(r,g,b), SPRITES_W*SPRITES_H*sizeof(vga_pixel)); 
      numhex[0] = hex[(i>>4) & 0xf];
      numhex[1] = hex[i & 0xf];
      numhex[2] = 0;
      tileText(i, 0, 0, numhex, VGA_RGB(0xff,0xff,0x00), VGA_RGB(0x00,0x00,0x00),spritesbuffer,SPRITES_W,SPRITES_H); 
    }
  } 
}


void VGA_T4::run_gfxengine()
{
  waitSync();
  
  if (fb1 != NULL) {
  	if (visible_framebuffer == fb0) {
  	  visible_framebuffer = fb1;
  	  framebuffer = fb0;
  	}
  	else {
  	  visible_framebuffer = fb0;
  	  framebuffer = fb1;
  	} 
  }
  
  unsigned char * tilept;

  // Layer 0
  for (int j=0; j<TILES_ROWS; j++)
  {
    tilept = &tilesram[j*TILES_COLS];
    if ( (j>=hscr_beg[0]) && (j<=hscr_end[0]) ) {     
      int modcol = (hscr[0] >> TILES_HBITS) % TILES_COLS;
      for (int i=0; i<TILES_COLS; i++)
      {
        (i == 0) ? drawTileCropL(tilept[modcol], (i<<TILES_HBITS) - (hscr[0] & TILES_HMASK), j*TILES_H) : 
          (i == (TILES_COLS-1))?drawTileCropR(tilept[modcol], (i<<TILES_HBITS) - (hscr[0] & TILES_HMASK), j*TILES_H) : 
            drawTile(tilept[modcol], (i<<TILES_HBITS) - (hscr[0] & TILES_HMASK), j*TILES_H);
        modcol++;
        modcol = modcol % TILES_COLS; 
      }
    }  
    else {
      for (int i=0; i<TILES_COLS; i++)
      {
        (i == (TILES_COLS-1)) ? drawTileCropR(tilept[i], (i<<TILES_HBITS), j*TILES_H) :
          drawTile(tilept[i], (i<<TILES_HBITS), j*TILES_H);
      }      
    }
  }

  // Other layers
  if (nb_layers > 1) {
    int lcount = 1;
    while (lcount < nb_layers) {
      for (int j=0; j<TILES_ROWS; j++)
      {
        tilept = &tilesram[(j+lcount*TILES_ROWS)*TILES_COLS];
        if ( (j>=hscr_beg[lcount]) && (j<=hscr_end[lcount]) ) {     
          int modcol = (hscr[lcount] >> TILES_HBITS) % TILES_COLS;
          for (int i=0; i<TILES_COLS; i++)
          {
            (i == 0) ? drawTransTileCropL(tilept[modcol], (i<<TILES_HBITS) - (hscr[lcount] & TILES_HMASK), j*TILES_H) : 
              (i == (TILES_COLS-1))?drawTransTileCropR(tilept[modcol], (i<<TILES_HBITS) - (hscr[lcount] & TILES_HMASK), j*TILES_H) : 
                drawTransTile(tilept[modcol], (i<<TILES_HBITS) - (hscr[lcount] & TILES_HMASK), j*TILES_H);
            modcol++;
            modcol = modcol % TILES_COLS; 
          }
        }          
        else {
          for (int i=0; i<TILES_COLS; i++)
          {
            drawTransTile(tilept[i], (i<<TILES_HBITS), j*TILES_H);
          }      
        }
      }
      lcount++;
    }  
  } 

/*
 static char * ro="01234567890123456789012345678901234567";
 for (int i=0; i<TILES_ROWS*2; i++)
 {
  tileTextOverlay(0, i*8, ro, VGA_RGB(0x00,0xff,0x00)); 
 }
*/

 for (int i=0; i<SPRITES_MAX; i++)
 {
   drawSpr(spritesdata[i].index, spritesdata[i].x, spritesdata[i].y);
 }

}

void VGA_T4::tile_data(unsigned char index, vga_pixel * data, int len)
{
  memcpy((void*)&tilesbuffer[index*TILES_W*TILES_H],(void*)data,len); 
}

void VGA_T4::sprite_data(unsigned char index, vga_pixel * data, int len)
{ 
  memcpy((void*)&spritesbuffer[index*SPRITES_W*SPRITES_H],(void*)data,len); 
}

void VGA_T4::sprite(int id , int x, int y, unsigned char index)
{
  if (id < SPRITES_MAX) {
    spritesdata[id].x = x;  
    spritesdata[id].y = y;  
    spritesdata[id].index = index;  
  }
}


void VGA_T4::sprite_hide(int id)
{
  if (id < SPRITES_MAX) {
    spritesdata[id].x = -16;  
    spritesdata[id].y = -16;  
    spritesdata[id].index = 0;  
  }  
}

void VGA_T4::tile_draw(int layer, int x, int y, unsigned char index)
{
  tilesram[(y+layer*TILES_ROWS)*TILES_COLS+x] = index;
}

void VGA_T4::tile_draw_row(int layer, int x, int y, unsigned char * data, int len)
{
  while (len--)
  {
    tilesram[(y+layer*TILES_ROWS)*TILES_COLS+x++] = *data++;
  }
}

void VGA_T4::tile_draw_col(int layer, int x, int y, unsigned char * data, int len)
{
  while (len--)
  {
    tilesram[(y++ +layer*TILES_ROWS)*TILES_COLS+x] = *data++;
  }
}

void VGA_T4::hscroll(int layer, int value)
{
  hscr[layer] = value;
}

void VGA_T4::vscroll(int layer, int value)
{
  vscr[layer] = value;
}

void VGA_T4::set_hscroll(int layer, int rowbeg, int rowend, int mask)
{
  hscr_beg[layer] = rowbeg;
  hscr_end[layer] = rowend;
  hscr_mask = mask+1;
}

void VGA_T4::set_vscroll(int layer, int colbeg, int colend, int mask)
{
  hscr_beg[layer] = colbeg;
  hscr_end[layer] = colend;
  hscr_mask = mask+1;
}


/*******************************************************************
 Experimental PWM interrupt based sound driver !!!
*******************************************************************/
#include "hardware/irq.h"
#include "hardware/pwm.h"

#define AUDIO_PIN 28

static bool fillfirsthalf = true;
static uint16_t cnt = 0;
static uint16_t sampleBufferSize = 0;

static void (*fillsamples)(short * stream, int len) = nullptr;

static uint32_t * i2s_tx_buffer;
static short * i2s_tx_buffer16;


static void SOFTWARE_isr() {
  if (fillfirsthalf) {
    fillsamples((short *)i2s_tx_buffer, sampleBufferSize);
  }  
  else { 
    fillsamples((short *)&i2s_tx_buffer[sampleBufferSize/2], sampleBufferSize);
  }
}

static void AUDIO_isr() {
  pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));
  long s = i2s_tx_buffer16[cnt++]; 
  s += i2s_tx_buffer16[cnt++];
  s = s/2 + 32767; 
  pwm_set_gpio_level(AUDIO_PIN, s >> 8);
  cnt = cnt & (sampleBufferSize*2-1);
  
  if (cnt == 0) {
    fillfirsthalf = false;
    SOFTWARE_isr();
  } 
  else if (cnt == sampleBufferSize) {
    fillfirsthalf = true;
    SOFTWARE_isr();
  }
}

void VGA_T4::begin_audio(int samplesize, void (*callback)(short * stream, int len))
{
  fillsamples = callback;
  i2s_tx_buffer =  (uint32_t*)malloc(samplesize*sizeof(uint32_t));

  if (i2s_tx_buffer == NULL) {
    printf("sound buffer could not be allocated!!!!!\n");
    return;  
  }
  memset((void*)i2s_tx_buffer,0, samplesize*sizeof(uint32_t));
  printf("sound buffer allocated\n");

  i2s_tx_buffer16 = (short*)i2s_tx_buffer;

  sampleBufferSize = samplesize;

  gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
  int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);
  // Setup PWM interrupt to fire when PWM cycle is complete
  pwm_clear_irq(audio_pin_slice);
  pwm_set_irq_enabled(audio_pin_slice, true);
  irq_set_exclusive_handler(PWM_IRQ_WRAP, AUDIO_isr);
  irq_set_enabled(PWM_IRQ_WRAP, true);

  // Setup PWM for audio output
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 5.5f);
  pwm_config_set_wrap(&config, 254);
  pwm_init(audio_pin_slice, &config, true);

  pwm_set_gpio_level(AUDIO_PIN, 0);
  printf("sound initialized\n");
}
 
void VGA_T4::end_audio()
{
  if (i2s_tx_buffer != NULL) {
    free(i2s_tx_buffer);
  }  
}



 

