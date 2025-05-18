#ifndef __PLF_KEYBOARD_H
#define __PLF_KEYBOARD_H

#include "physicalkeyboard.h"

//void onPress(int unicode);
//void onRelease(int unicode);

class PlfKeyboard : public PhysicalKeyboard {
 public:
  PlfKeyboard(VMKeyboard *k);
  virtual ~PlfKeyboard();

  // Interface used by the VM...
  virtual void maintainKeyboard();

  // Interface used by the BIOS...
  virtual bool kbhit();
  virtual uint8_t read();

    //Key joystick
  virtual void startReading();
  virtual void setJoymode(uint8_t mode);
  virtual void setAnnunciators();
  virtual void setCaps(bool enabled);
  
  virtual void onPress(int unicode);
  virtual void onRelease(int unicode);
  virtual void pressedKey(uint8_t key, uint8_t mod);
  virtual void releasedKey(uint8_t key, uint8_t mod);

  //Panel mapping
  virtual uint8_t getMapping(uint8_t key);
  virtual void setMapping(uint8_t key, uint8_t val);
};

#endif
