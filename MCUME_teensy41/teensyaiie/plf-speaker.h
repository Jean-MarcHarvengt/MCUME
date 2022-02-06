#ifndef __PLF_SPEAKER_H
#define __PLF_SPEAKER_H

#include "physicalspeaker.h"

class PlfSpeaker : public PhysicalSpeaker {
 public:
  PlfSpeaker(uint8_t pinNum);
  virtual ~PlfSpeaker();

  virtual void toggle();
  virtual void maintainSpeaker(uint32_t c);

  virtual void beginMixing();
  virtual void mixOutput(uint8_t v);

 private:
  uint8_t speakerPin;

  bool toggleState;
  bool needsToggle;

  uint32_t mixerValue;
  uint8_t numMixed;
};

#endif
