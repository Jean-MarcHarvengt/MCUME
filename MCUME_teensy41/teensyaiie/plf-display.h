#ifndef __PLF_DISPLAY_H
#define __PLF_DISPLAY_H

#include <stdint.h>
#include "physicaldisplay.h"

class UTFT;
class BIOS;

class PlfDisplay : public PhysicalDisplay {
  friend class BIOS;

 public:
  PlfDisplay();
  virtual ~PlfDisplay();
  
  virtual void blit(AiieRect r);
  virtual void redraw();

  virtual void drawDriveDoor(uint8_t which, bool isOpen);
  virtual void setDriveIndicator(uint8_t which, bool isRunning);
  virtual void drawBatteryStatus(uint8_t percent);
};

#endif
