#include "widgets.h"

#include "globals.h"

Widgets::Widgets()
{
}

Widgets::~Widgets()
{
}

void Widgets::drawBatteryText() {
  /*
  uint16_t back = BLACK;
  char buf[10];
  if (g_charge > 15) {
	sprintf(buf, "Charge"); //charging
	back = BLUE;
  }
  else {
    if (g_battery > 1000) {
      int bat = g_battery;
	  if (bat > 4200) bat = 4200;
	  if (bat < 3000) bat = 3000;
	  if (bat > 3400) back = DARK_GREEN; //Good
	  else if (bat > 3200) back = BROWN; //Low
      else back = ORANGE; //Expired
	  bat = map(bat, 3000, 4200, 0, 100);
	  sprintf(buf, "%d%%", bat);
	}
	else {
	  sprintf(buf, "Full"); //Fully charged (off)
	  back = DARK_BLUE;
	}
  }
  */
  //g_display->fillRoundRect(battX, battY, 53, 20, 5, back);
  //g_display->drawRoundRect(battX, battY, 53, 20, 5, battColor);
  //g_display->drawRoundRect(battX+52, battY+7, 5, 6, 2, battColor);

  //g_display->setBackground(back);
  //g_display->drawString(M_NORMAL, battX+2, battY+4, "      ");	
  //g_display->drawString(M_NORMAL, (battX+27) - (strlen(buf) * 4), battY+4, buf);	
  //g_display->setBackground(DARK_BLUE);
}

void Widgets::drawBattery(int16_t x, int16_t y, uint16_t color) {
  battX = x;
  battY = y;
  battColor = color;
  drawBatteryText();
}

//Caption 1 or 2 chars long
void Widgets::drawCaptionText(uint8_t style, uint16_t x, uint16_t y, const char* str) {
  //uint8_t len = strlen(str)>1?4:0;	
  //g_display->drawString(style, x-len, y, str);	
}
void Widgets::drawKey (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, const char* str) {
  //g_display->drawRoundRect(x, y, w, h, 2, color);
  //g_display->fillRoundRect(x+2, y+2, w-4, h-4, 4, BLACK);
  //g_display->drawRoundRect(x+2, y+2, w-4, h-4, 4, color);
  //drawCaptionText(M_HIGHLIGHT, x+12, y+5, str);	
}
void Widgets::drawButton (uint16_t x, uint16_t y, uint16_t color, const char* str) {
  //g_display->drawCircle(x+14, y+14, 14, color);
  //g_display->fillCircle(x+14, y+14, 12, BLACK);
  //g_display->drawCircle(x+14, y+14, 12, color);
  //drawCaptionText(M_HIGHLIGHT, x+10, y+8, str);	
}
void Widgets::drawStick (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, const char* str0, const char* str1, const char* str2, const char* str3) {
  //g_display->fillRoundRect(x, y, w, h, 6, BLACK);
  //g_display->drawRoundRect(x, y, w, h, 6, color);
  //g_display->drawCircle(x+(w/2), y+(h/2), 3, color);
  //uint8_t len = strlen(str0)>1?8:0;	
  //g_display->drawString(M_HIGHLIGHT, x+(w/2)-14-len, y+(h/2)-6, str0);	
  //g_display->drawString(M_HIGHLIGHT, x+(w/2)+6, y+(h/2)-6, str1);	
  //drawCaptionText(M_HIGHLIGHT, x+(w/2)-3, y+(h/2)-16, str2);	
  //drawCaptionText(M_HIGHLIGHT, x+(w/2)-3, y+(h/2)+4, str3);	
}


