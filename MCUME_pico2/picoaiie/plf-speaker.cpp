#include "plf-speaker.h"

#include "globals.h"

PlfSpeaker::PlfSpeaker(uint8_t pinNum) : PhysicalSpeaker()
{
  toggleState = false;
  needsToggle = false;
  speakerPin = pinNum;
  mixerValue = numMixed = 0;
}

PlfSpeaker::~PlfSpeaker()
{
}

void PlfSpeaker::toggle()
{
  needsToggle = true;
}

void PlfSpeaker::maintainSpeaker(uint32_t c)
{
  if (needsToggle) {
    toggleState = !toggleState;
    needsToggle = false;
  }

  mixerValue += (toggleState ? 0x1FF : 0x00);

  mixerValue >>= (16-g_volume);

}

void PlfSpeaker::beginMixing()
{
  mixerValue = 0;
  numMixed = 0;
}

void PlfSpeaker::mixOutput(uint8_t v)
{
  mixerValue += v;
  numMixed++;
}

