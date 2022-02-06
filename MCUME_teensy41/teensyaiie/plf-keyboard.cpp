#include "plf-keyboard.h"
#include "globals.h"
//#include <Keypad.h>
#include "RingBuf.h"

#define byte unsigned char
const byte ROWS = 3;
const byte COLS = 7;

//Panel keys and joystick
static unsigned char keys[ROWS][COLS] = {
  { BUT1, LJOY, UJOY, RJOY, DJOY },
  { BUT2, ENTKEY, LKEY, DKEY, RKEY },
  { JOYKEY, NOTEKEY, LWHTKEY, UKEY, RWHTKEY }
};

uint8_t rowsPins[ROWS] = { 24, 25, 26 };
uint8_t colsPins[COLS] = { 27, 28, 29, 30, 31 };
//Keypad keypad(makeKeymap(keys), rowsPins, colsPins, ROWS, COLS);

RingBuf bufferUsb(10); // 10 keys should be plenty, right?


static uint8_t panelBIOS[15] = { LARR, RARR, UARR, DARR, RET, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9' };

uint8_t panelMap[15] = { LJOY, RJOY, UJOY, DJOY, LA, RA, 'j', 'k', '1', UARR, '2', RET, LARR, DARR, RARR };

//PS2 101 key keyboard layout
const int ps2KeyLayout[102] = 
{
    0,   0,   0,   0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
  'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
  'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', RET, ESC,
  DEL, TAB, ' ', '-', '=', '[', ']','\\', '?', ';','\'', '`', ',', '.',
  '/',LOCK,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  SYSRQ, 0,   0,   0,   0,   0,   0,   0,   0,RARR,LARR,DARR,UARR,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,  LA,  RA
};

static uint8_t shiftedNumber[] = { '<', // ,
				   '_', // -
				   '>', // .
				   '?', // /
				   ')', // 0
				   '!', // 1
				   '@', // 2
				   '#', // 3
				   '$', // 4
				   '%', // 5
				   '^', // 6
				   '&', // 7
				   '*', // 8
				   '(', // 9
				   0,   // (: is not a key)
				   ':'  // ;
};

//bool buffered;
bool leftShiftPressed;
bool rightShiftPressed;
bool ctrlPressed;
bool capsLock;
bool leftApplePressed;
bool rightApplePressed;
int8_t numPressed;
bool buffered;
VMKeyboard *vmKeyboard;

int16_t paddle0;
int16_t paddle1;
uint8_t currentJoyMode = JOY_MODE_ANA_ABS;

PlfKeyboard::PlfKeyboard(VMKeyboard *k) : PhysicalKeyboard(k)
{
  //keypad.setDebounceTime(5);

  //myusb.begin();
  //keyboard1.attachPress(onPress);
  //keyboard1.attachRelease(onRelease);
  //keyboard1.rawOnly(true);

  leftShiftPressed = false;
  rightShiftPressed = false;
  ctrlPressed = false;
  capsLock = true;
  leftApplePressed = false;
  rightApplePressed = false;
  buffered = false;
  vmKeyboard = k;
  
  numPressed = 0;
  paddle0 = 127;
  paddle1 = 127;
}

PlfKeyboard::~PlfKeyboard()
{
}

uint8_t PlfKeyboard::getMapping(uint8_t key) {
  return panelMap[key];	
}

void PlfKeyboard::setMapping(uint8_t key, uint8_t val) {
  panelMap[key] = val;	
}


void PlfKeyboard::onPress(int unicode)
{
  /*
  uint8_t modifiers = 0; //keyboard1.getModifiers();
  uint8_t key = unicode & 0x7f; //keyboard1.getOemKey();
  if (key > 101) key = 101;
  if (buffered) {
	  pressedKey(ps2KeyLayout[key], modifiers);
  } else {
	  vmKeyboard->keyDepressed(ps2KeyLayout[key], modifiers);
  }
  */
  if (unicode <= 0x7f) {
    int8_t modifiers = 0;
    vmKeyboard->keyDepressed(unicode & 0x7f, modifiers);    
  }
  else {
    vmKeyboard->keyDepressed(unicode);    
  }
}

void PlfKeyboard::onRelease(int unicode)
{
  /*
  uint8_t key = unicode & 0x7f; //keyboard1.getOemKey();
  uint8_t modifiers = 0; //keyboard1.getModifiers();
  if (key > 101) key = 101;
  if (buffered) {
	  releasedKey(ps2KeyLayout[key], modifiers);
  } else {
	  vmKeyboard->keyReleased(ps2KeyLayout[key], modifiers);
  }
  */
  if (unicode <= 0x7f) {
    int8_t modifiers = 0;
    vmKeyboard->keyReleased(unicode & 0x7f, modifiers);    
  }
  else {
    vmKeyboard->keyReleased(unicode);    
  }
}

void PlfKeyboard::pressedKey(uint8_t key, uint8_t mod)
{
  numPressed++;
  if (key != SYSRQ && key & 0x80) {
    // it's a modifier key.
    switch (key) {
    case _CTRL:
      ctrlPressed = 1;
      break;
    case LSHFT:
      leftShiftPressed = 1;
      break;
    case RSHFT:
      rightShiftPressed = 1;
      break;
    case LOCK:
      capsLock = !capsLock;
      break;
    case LA:
      leftApplePressed = 1;
      break;
    case RA:
      rightApplePressed = 1;
      break;
    }
    return;
  }

  if (key == ' ' || key == DEL || key == ESC || key == RET || key == TAB || key == SYSRQ) {
	  //Serial.println((int)key);

    bufferUsb.addByte(key);
    return;
  }

  if (key >= 'a' &&
      key <= 'z') {
    if (ctrlPressed) {
      bufferUsb.addByte(key - 'a' + 1);
      return;
    }
    if (leftShiftPressed || rightShiftPressed || capsLock) {
      bufferUsb.addByte(key - 'a' + 'A');
      return;
    }
    bufferUsb.addByte(key);
    return;
  }

  // FIXME: can we control-shift?
  if (key >= ',' && key <= ';') {
    if (leftShiftPressed || rightShiftPressed) {
      bufferUsb.addByte(shiftedNumber[key - ',']);
      return;
    }
    bufferUsb.addByte(key);
    return;
  }

  if (leftShiftPressed || rightShiftPressed) {
    uint8_t ret = 0;
    switch (key) {
    case '=':
      ret = '+';
      break;
    case '[':
      ret = '{';
      break;
    case ']':
      ret = '}';
      break;
    case '\\':
      ret = '|';
      break;
    case '\'':
      ret = '"';
      break;
    case '`':
      ret = '~';
      break;
    }
    if (ret) {
      bufferUsb.addByte(ret);
      return;
    }
  }

  // Everything else falls through.
  bufferUsb.addByte(key);
}

void PlfKeyboard::releasedKey(uint8_t key, uint8_t mod)
{
  numPressed--;
  if (key & 0x80) {
    // it's a modifier key.
    switch (key) {
    case _CTRL:
      ctrlPressed = 0;
      break;
    case LSHFT:
      leftShiftPressed = 0;
      break;
    case RSHFT:
      rightShiftPressed = 0;
      break;
    case LA:
      leftApplePressed = 0;
      break;
    case RA:
      rightApplePressed = 0;
      break;
    }
  }
}

bool PlfKeyboard::kbhit()
{
  //USB keyboard
  if (!buffered) {
    bufferUsb.clear();
    buffered = true;
  }
  //myusb.Task();

  //Front panel keys
  /*
  if (keypad.getKeys()) {
    for (int i=0; i<LIST_MAX; i++) {
      if ( keypad.key[i].stateChanged ) {
        switch (keypad.key[i].kstate) {
		  case PRESSED:
			pressedKey(panelBIOS[keypad.key[i].kchar - 0x88], 0);
			break;
		  case RELEASED:
			releasedKey(panelBIOS[keypad.key[i].kchar - 0x88], 0);
			break;
		  case HOLD:
		  case IDLE:
			break;
		}
      }
    }
  }
  */

  // For debugging: also allow USB serial to act as a keyboard
  //if (Serial.available()) {
  //  bufferUsb.addByte(Serial.read());
  //}

  return bufferUsb.hasData();
}

uint8_t PlfKeyboard::read()
{
  if (bufferUsb.hasData()) {
    return bufferUsb.consumeByte();
  }

  return 0;
}

//Set joystick mode to analog, joyport1 or joyport2
void PlfKeyboard::setJoymode(uint8_t mode) {
  currentJoyMode = mode;
  if (mode == JOY_MODE_ANA_ABS || mode == JOY_MODE_ANA_REL)
	//Regular button interface
	vmKeyboard->setButtons(false, false, false);
  else if (mode == JOY_MODE_JOYPORT1 || mode == JOY_MODE_JOYPORT2) 
	//Joyport inputs are active low
    vmKeyboard->setButtons(true, true, true);
}

//Joyport stick switches
bool stick_up = false;
bool stick_down = false;
bool stick_left = false;
bool stick_right = false;
bool stick_trig = false;

//Set joystick input buttons based on annunciator outputs
// Emulates joyport circuit 
// http://lukazi.blogspot.nl/2009/04/game-controller-atari-joysticks.html
void updateJoyport() {
  if (currentJoyMode != JOY_MODE_ANA_ABS && currentJoyMode != JOY_MODE_ANA_REL) {
    if ((!vmKeyboard->getAnnunciator(0) && currentJoyMode == JOY_MODE_JOYPORT1) || 	//Joystick 1 enabled 
		(vmKeyboard->getAnnunciator(0) && currentJoyMode == JOY_MODE_JOYPORT2)) { 	//Joystick 2 enabled
      vmKeyboard->setButton(0, !stick_trig);		//Button
      if (!vmKeyboard->getAnnunciator(1)) { 		//Joystick L/R or U/D
        vmKeyboard->setButton(1, !stick_left);
        vmKeyboard->setButton(2, !stick_right); 
	  } else {
        vmKeyboard->setButton(1, !stick_up);
        vmKeyboard->setButton(2, !stick_down);
	  }
    }	
    else vmKeyboard->setButtons(true, true, true);	//No joystick
  }
}

//Called from MMU when game port annunciators are changed
void PlfKeyboard::setAnnunciators() {
  updateJoyport();
}

void updateRelStick() {
  if (currentJoyMode == JOY_MODE_ANA_REL) {
    //Relative Joystick Emulation
	if (stick_left) { 
	  paddle0 -= g_joySpeed;
	  if (paddle0 < 0) paddle0 = 0;
    }
    else if (stick_right) {
	  paddle0 += g_joySpeed;
	  if (paddle0 > 255) paddle0 = 255;
    }
    if (stick_up) {
	  paddle1 -= g_joySpeed;
	  if (paddle1 < 0) paddle1 = 0;
    }
    else if (stick_down) {
	  paddle1 += g_joySpeed;
	  if (paddle1 > 255) paddle1 = 255;
    }
  }
}

//Joystick emulation
// Returns true if key was handled by stick 
bool pressStick(uint8_t key) {
  //Joyport emulation
  if (currentJoyMode != JOY_MODE_ANA_ABS) {
    if (key == LJOY) {
	  stick_left = true;
	  stick_right = false;
	  updateJoyport();
      return true;
    }
    if (key == RJOY) {
	  stick_left = false;
	  stick_right = true;
	  updateJoyport();
	  return true;
    }
    if (key == UJOY) {
	  stick_up = true;
	  stick_down = false;
	  updateJoyport();
	  return true;
    }
    if (key == DJOY) {
	  stick_up = false;
	  stick_down = true;
	  updateJoyport();
	  return true;
    }
    if (key == LA && currentJoyMode != JOY_MODE_ANA_REL) { 
	  stick_trig = true;
	  updateJoyport();
	  return true;
    }
    if (key == RA && currentJoyMode != JOY_MODE_ANA_REL) { 
	  return true;
    }
  } else {
	//Absolute Analog stick emulation
    if (key == LJOY) { 
	  paddle0 = 0;
	  return true;
    }
    if (key == RJOY) {
	  paddle0 = 255;
	  return true;
    }
    if (key == UJOY) {
	  paddle1 = 0;
	  return true;
    }
    if (key == DJOY) {
	  paddle1 = 255;
	  return true;
    }
  }
  return false;
}

bool releaseStick(uint8_t key) {
  //Joyport emulation
  if (currentJoyMode != JOY_MODE_ANA_ABS) {
    if (key == LJOY || key == RJOY) {
	  stick_left = false;
	  stick_right = false;
	  updateJoyport();
      return true;
    }
    if (key == UJOY || key == DJOY) {
	  stick_up = false;
	  stick_down = false;
	  updateJoyport();
      return true;
    }
    if (key == LA && currentJoyMode != JOY_MODE_ANA_REL) { 
	  stick_trig = false;
	  updateJoyport();
	  return true;
    }
    if (key == RA && currentJoyMode != JOY_MODE_ANA_REL) { 
	  return true;
    }
  } else {
	//Absolute Analog stick emulation
    if (key == LJOY || key == RJOY) {
	    paddle0 = g_joyTrimX;
	    return true;
    }
    if (key == UJOY || key == DJOY) {
	    paddle1 = g_joyTrimY;
	    return true;
    }
  }
  return false;
}

bool capsLed = false;
// This is a non-buffered interface to the physical keyboard, as used
// by the VM.
void PlfKeyboard::maintainKeyboard()
{
	static bool oldCapsLed = false;
	if (oldCapsLed != capsLed) {
		oldCapsLed = capsLed;
        //if (keyboard1.connected()) keyboard1.capsLock(capsLed);
	}
//	Serial.println("maintain");
  buffered = false;
  //myusb.Task();
  
  updateRelStick();
/*
  if (keypad.getKeys()) {
    for (int i=0; i<LIST_MAX; i++) {
      if ( keypad.key[i].stateChanged ) {
		uint8_t kchar = panelMap[keypad.key[i].kchar - 0x88];
        switch (keypad.key[i].kstate) {
		  case PRESSED:
		    if (!pressStick(kchar))
			  vmkeyboard->keyDepressed(kchar);
			break;
		  case RELEASED:
		    if (!releaseStick(kchar))
			  vmkeyboard->keyReleased(kchar);
			break;
		  case HOLD:
		  case IDLE:
			break;
		}
      }
    }
  }
*/
  // For debugging: also allow USB serial to act as a keyboard
  /*
  if (Serial.available()) {
    int c = Serial.read();
    vmkeyboard->keyDepressed(c);
    vmkeyboard->keyReleased(c);
  }
  */
}

void PlfKeyboard::startReading()
{
  g_vm->triggerPaddleInCycles(0, 12 * paddle0);
  g_vm->triggerPaddleInCycles(1, 12 * paddle1);
}

void PlfKeyboard::setCaps(bool enabled) {
  capsLed = enabled;
  //Serial.print("Set Leds:");
  //Serial.println(enabled?"On":"Off");
}
