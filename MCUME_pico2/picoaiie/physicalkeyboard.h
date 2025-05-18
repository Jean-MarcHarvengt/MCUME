#ifndef __PHYSICALKEYBOARD_H
#define __PHYSICALKEYBOARD_H

#include <stdint.h>

#include "vmkeyboard.h"

#define ESC 0x1B
#define DEL 0x7F
#define RET 0x0D
#define TAB 0x09
#define LARR 0x08 // control-H
#define RARR 0x15 // control-U
#define DARR 0x0A
#define UARR 0x0B

// Virtual keys
#define _CTRL  0x81
#define LSHFT 0x82
#define RSHFT 0x83
#define LOCK  0x84 // caps lock
#define LA    0x85 // left (open) apple, aka paddle0 button
#define RA    0x86 // right (closed) apple aka paddle1 button
#define SYSRQ 0x87

//Panel keys
#define LJOY  	0x88
#define RJOY  	0x89
#define UJOY  	0x8A
#define DJOY  	0x8B
#define BUT1  	0x8C
#define BUT2  	0x8D
#define JOYKEY 	0x8E
#define NOTEKEY	0x8F
#define LWHTKEY	0x90
#define UKEY  	0x91
#define RWHTKEY 0x92
#define ENTKEY	0x93
#define LKEY	0x94
#define DKEY	0x95
#define RKEY	0x96

//Special keys
#define JOY2	0x97

//USB modifiers
#define USB_LEFT_CTRL   0x01
#define USB_LEFT_SHIFT  0x02
#define USB_LEFT_ALT    0x04
#define USB_LEFT_GUI    0x08
#define USB_RIGHT_CTRL  0x10
#define USB_RIGHT_SHIFT 0x20
#define USB_RIGHT_ALT   0x40
#define USB_RIGHT_GUI   0x80

#define JOY_MODE_ANA_ABS   	0
#define JOY_MODE_ANA_REL   	1
#define JOY_MODE_JOYPORT1   2
#define JOY_MODE_JOYPORT2   3

class PhysicalKeyboard {
 public:
  PhysicalKeyboard(VMKeyboard *k) { this->vmkeyboard = k; }
  virtual ~PhysicalKeyboard() {};

  virtual void maintainKeyboard() = 0;
  virtual bool kbhit() = 0;
  virtual uint8_t read() = 0;
  //Key joystick
  virtual void startReading() = 0;
  virtual uint8_t getMapping(uint8_t key) = 0;
  virtual void setMapping(uint8_t key, uint8_t val) = 0;
  virtual void setJoymode(uint8_t mode) = 0;
  virtual void setAnnunciators() = 0;
  virtual void setCaps(bool enabled) = 0;

  virtual void onPress(int unicode);
  virtual void onRelease(int unicode);
  virtual void pressedKey(uint8_t key, uint8_t mod);
  virtual void releasedKey(uint8_t key, uint8_t mod);
 protected:
  VMKeyboard *vmkeyboard;
};

#endif
