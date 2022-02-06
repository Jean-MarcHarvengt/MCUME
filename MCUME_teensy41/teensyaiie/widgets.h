#ifndef __WIDGETS_H
#define __WIDGETS_H

#ifdef TEENSYDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#endif

class Widgets {
 public:
  Widgets();
  ~Widgets();
  
  void drawBattery(int16_t x, int16_t y, uint16_t color);
  void drawBatteryText();
  void drawCaptionText(uint8_t style, uint16_t x, uint16_t y, const char* str);
  void drawKey (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, const char* str);
  void drawButton (uint16_t x, uint16_t y, uint16_t color, const char* str);
  void drawStick (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, const char* str0, const char* str1, const char* str2, const char* str3);
  
 private:
  int16_t battX;
  int16_t battY;
  int16_t battColor;
  
};

#endif
