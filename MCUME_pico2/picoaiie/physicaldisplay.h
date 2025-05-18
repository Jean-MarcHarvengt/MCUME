#ifndef __PHYSICALDISPLAY_H
#define __PHYSICALDISPLAY_H

#include <string.h> // strncpy

#include "vmdisplay.h" // FIXME: for AiieRect


class PhysicalDisplay {
 public:
  PhysicalDisplay() { overlayMessage[0] = '\0'; }
  virtual ~PhysicalDisplay() {};

  virtual void redraw() = 0; // total redraw, assuming nothing
  virtual void blit(AiieRect r) = 0;   // redraw just the VM display area

  virtual void drawDriveDoor(uint8_t which, bool isOpen) = 0;
  virtual void setDriveIndicator(uint8_t which, bool isRunning) = 0;
  virtual void drawBatteryStatus(uint8_t percent) = 0;

  virtual void debugMsg(const char *msg) { strncpy(overlayMessage, msg, sizeof(overlayMessage)); }

 protected:
  char overlayMessage[40];
};

#endif
