#ifndef __PHYSICALSPEAKER_H
#define __PHYSICALSPEAKER_H

#include <stdint.h>

class PhysicalSpeaker {
 public:
  virtual ~PhysicalSpeaker() {}

  virtual void toggle() = 0;
  virtual void maintainSpeaker(uint32_t c) = 0;
  virtual void beginMixing() = 0;
  virtual void mixOutput(uint8_t v) = 0;

};

#endif
