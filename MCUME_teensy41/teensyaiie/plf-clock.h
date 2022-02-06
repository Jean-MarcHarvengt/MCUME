#ifndef __TEENSYCLOCK_H
#define __TEENSYCLOCK_H

#include <stdint.h>
#include <stdio.h>

#include "slot.h"
#include "applemmu.h"

class PlfClock : public Slot {
 public:
  PlfClock(AppleMMU *mmu);
  virtual ~PlfClock();

  virtual void Reset();

  virtual uint8_t readSwitches(uint8_t s);
  virtual void writeSwitches(uint8_t s, uint8_t v);

  virtual void loadROM(uint8_t *toWhere);

 private:
  AppleMMU *mmu;
};

#endif
