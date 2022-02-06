#ifndef __SLOT_H
#define __SLOT_H

#ifdef TEENSYDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#include <stdio.h>
#endif


class Slot {
 public:
  virtual ~Slot() {};

  virtual void Reset() = 0; // for use at cold-boot

  virtual uint8_t readSwitches(uint8_t s) = 0;
  virtual void writeSwitches(uint8_t s, uint8_t v) = 0;

  virtual void loadROM(uint8_t *toWhere) = 0;
};

#endif
