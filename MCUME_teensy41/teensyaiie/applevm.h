#ifndef __APPLEVM_H
#define __APPLEVM_H

#include "cpu.h"
#include "appledisplay.h"
#include "diskii.h"
#include "vmkeyboard.h"

#undef TEENSYDUINO
#ifdef TEENSYDUINO
#include "teensy-clock.h"
#else
#include "plf-clock.h"
#endif

#include "vm.h"
class AppleVM : public VM {
 public:
  AppleVM();
  virtual ~AppleVM();

  void cpuMaintenance(uint32_t cycles);

  virtual void Reset();
  void Monitor();

  virtual void triggerPaddleInCycles(uint8_t paddleNum,uint16_t cycleCount);

  const char *DiskName(uint8_t drivenum);
  void ejectDisk(uint8_t drivenum, bool drawIt = true);
  void insertDisk(uint8_t drivenum, const char *filename, bool drawIt = true);

  virtual VMKeyboard *getKeyboard();

  DiskII *disk6;
 protected:
  VMKeyboard *keyboard;
#ifdef TEENSYDUINO
  TeensyClock *teensyClock;
#else
  PlfClock *plfClock;
#endif
};


#endif
