#ifndef __APPLEKEYBOARD_H
#define __APPLEKEYBOARD_H

#include <stdint.h>

#include "vmkeyboard.h"
#include "applemmu.h"
extern void biosInterrupt();

class AppleKeyboard : public VMKeyboard {
 public:
  AppleKeyboard(AppleMMU *m);
  virtual ~AppleKeyboard();

  virtual void keyDepressed(uint8_t k);
  virtual void keyReleased(uint8_t k);
  virtual void keyDepressed(uint8_t k, uint8_t m);
  virtual void keyReleased(uint8_t k, uint8_t m);
  virtual void setButton(uint8_t index, bool val);
  virtual void setButtons(bool b0, bool b1, bool b2);
  virtual bool getAnnunciator(uint8_t index);
  virtual void maintainKeyboard(uint32_t cycleCount);

 protected:
  bool isVirtualKey(uint8_t kc);
  uint8_t translateKeyWithModifiers(uint8_t k);
  uint8_t translateKeyWithModifiers(uint8_t k, uint8_t m);

 private:
  AppleMMU *mmu;

  bool capsLockEnabled;

  // This is a trade-off. I'm choosing speed over RAM size. If we need
  // to reclaim RAM, we can get some bytes here at the expense of speed.

  // These are flags for whether or not each of the individual keys are 
  // down, so that we can repeat appropriately. We're tracking state
  // of all of the keys because of special modifier key situations.
  // It's lazily using 256 bytes instead of whatever 62 we'd actually need.
  bool keysDown[256];
  bool anyKeyIsDown;

  // While one - and only one - key is down, we repeat keypresses 
  // after about "534 to 801 milliseconds" (UTA2E, p. 7-15); and then 
  // while repeating, we send that keypress (reset keystrobe) about 15 
  // times a second (every 66667-ish CPU cycles).
  //
  // startRepeatTimer is the time (in CPU clock cycles) when we will
  // start repeating the key that's currently down (note: rollover 
  // happens every 4925 seconds because it's a 32-bit counter, which means 
  // that roughly once every 82 minutes it's possible that a key will begin 
  // repeating early).
  //
  // keyThatIsRepeating is set to the actual key pressed.
  // repeatTimer is the cpu cycle count at which we would repeat again.
  // (It also has the rollover problem once every 82 minutes.)
 
  uint32_t startRepeatTimer;
  uint8_t keyThatIsRepeating;
  uint32_t repeatTimer;
};

#endif
