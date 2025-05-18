#include "applekeyboard.h"
#include "physicalkeyboard.h" // for LA/RA constants

#include "applemmu.h"

#include "globals.h"

// How many CPU cycles before we begin repeating a key?
#define STARTREPEAT 700000
// How many CPU cycles between repeats of a key?
#define REPEATAGAIN 66667

AppleKeyboard::AppleKeyboard(AppleMMU *m)
{
  this->mmu = m;

  for (uint16_t i=0; i<sizeof(keysDown); i++) {
    keysDown[i] = false;
  }
  anyKeyIsDown = false;
  startRepeatTimer = 0;
  repeatTimer = 0;

  capsLockEnabled = true;
}

AppleKeyboard::~AppleKeyboard()
{
}

bool AppleKeyboard::isVirtualKey(uint8_t kc)
{
  if (kc >= 0x81 && kc <= 0x97) {
    return true;
  }
  return false;
}

// apply the apple keymap.
// FIXME: easier with an array, but is that better?
uint8_t AppleKeyboard::translateKeyWithModifiers(uint8_t k)
{
  // tolower, so we know what we're working with...
  if (k >= 'A' && k <= 'Z') {
    k = k - 'A' + 'a';
  }

  if (keysDown[_CTRL]) {
    if (k >= 'a' && k <= 'z') {
      return k - 'a' + 1;
    }
    // FIXME: any other control keys honored on the //e keyboard?
  }

  if (capsLockEnabled && k >= 'a' && k <= 'z') {
    return k - 'a' + 'A';
  }

  if (keysDown[LSHFT] || keysDown[RSHFT]) {
    if (k >= 'a' && k <= 'z') {
      return k - 'a' + 'A';
    }
    switch (k) {
    case '1':
      return '!';
    case '2':
      return '@';
    case '3':
      return '#';
    case '4':
      return '$';
    case '5':
      return '%';
    case '6':
      return '^';
    case '7':
      return '&';
    case '8':
      return '*';
    case '9':
      return '(';
    case '0':
      return ')';
    case '-':
      return '_';
    case '=':
      return '+';
    case '[':
      return '{';
    case ']':
      return '}';
    case '\\':
      return '|';
    case '`':
      return '~';
    case ';':
      return ':';
    case '\'':
      return '"';
    case ',':
      return '<';
    case '.':
      return '>';
    case '/':
      return '?';
    }
    // FIXME: what the heck is it? I guess we don't need to shift it?
  }

  // And if we fall through, then just return it as-is
  return k;
}

// apply the apple keymap.
// FIXME: easier with an array, but is that better?
uint8_t AppleKeyboard::translateKeyWithModifiers(uint8_t k, uint8_t m)
{
  // tolower, so we know what we're working with...
  if (k >= 'A' && k <= 'Z') {
    k = k - 'A' + 'a';
  }

  if (m & (USB_LEFT_CTRL | USB_RIGHT_CTRL | USB_LEFT_GUI | USB_RIGHT_GUI)) {
    if (k >= 'a' && k <= 'z') {
      return k - 'a' + 1;
    }
    // FIXME: any other control keys honored on the //e keyboard?
  }

  if (capsLockEnabled && k >= 'a' && k <= 'z') {
    return k - 'a' + 'A';
  }

  if (m & (USB_LEFT_SHIFT | USB_RIGHT_SHIFT)) {
    if (k >= 'a' && k <= 'z') {
      return k - 'a' + 'A';
    }
    switch (k) {
    case '1':
      return '!';
    case '2':
      return '@';
    case '3':
      return '#';
    case '4':
      return '$';
    case '5':
      return '%';
    case '6':
      return '^';
    case '7':
      return '&';
    case '8':
      return '*';
    case '9':
      return '(';
    case '0':
      return ')';
    case '-':
      return '_';
    case '=':
      return '+';
    case '[':
      return '{';
    case ']':
      return '}';
    case '\\':
      return '|';
    case '`':
      return '~';
    case ';':
      return ':';
    case '\'':
      return '"';
    case ',':
      return '<';
    case '.':
      return '>';
    case '/':
      return '?';
    }
    // FIXME: what the heck is it? I guess we don't need to shift it?
  }

  // And if we fall through, then just return it as-is
  return k;
}

void AppleKeyboard::keyDepressed(uint8_t k)
{
  keysDown[k] = true;  

  // If it's not a virtual key, then set the anyKeyDown flag
  // (the VM will see this as a keyboard key)
  if (!isVirtualKey(k)) {
    if (!anyKeyIsDown) {
      mmu->setKeyDown(true);
      anyKeyIsDown = true;
    }
    keyThatIsRepeating = translateKeyWithModifiers(k);
    startRepeatTimer = g_cpu->cycles + STARTREPEAT;
    mmu->keyboardInput(keyThatIsRepeating);
  } else if (k == LA) {
    // Special handling: apple keys
    mmu->isOpenApplePressed = true;
    return;
  } else if (k == RA) {
    // Special handling: apple keys
    mmu->isClosedApplePressed = true;
    return;
  } else if (k == JOY2) {
    // Special handling: apple keys
    mmu->isButton2Pressed = true;
    return;
  } else if (k == LOCK) {
    // Special handling: caps lock
    capsLockEnabled = !capsLockEnabled;
    g_keyboard->setCaps(capsLockEnabled);
    return;
  }
}

void AppleKeyboard::keyReleased(uint8_t k)
{
  keysDown[k] = false;  

  // Special handling: apple keys
  if (k == LA) {
    mmu->isOpenApplePressed = false;
    return;
  }
  if (k == RA) {
    mmu->isClosedApplePressed = false;
    return;
  }
  if (k == JOY2) {
    mmu->isButton2Pressed = false;
    return;
  }
  if (k == LOCK) {
    // Nothing to do when the caps lock key is released.
    return;
  }

  if (anyKeyIsDown) {
    anyKeyIsDown = false;
    for (uint16_t i=0; i<sizeof(keysDown); i++) {
      if (keysDown[i] && !isVirtualKey(i)) {
	anyKeyIsDown = true;
	break;
      }
    }
    if (!anyKeyIsDown) {
      mmu->setKeyDown(false);
    }
  }  
}

void AppleKeyboard::keyDepressed(uint8_t k, uint8_t m)
{
  keysDown[k] = true;  

  // If it's not a virtual key, then set the anyKeyDown flag
  // (the VM will see this as a keyboard key)
  if (k && !isVirtualKey(k)) {
    if (!anyKeyIsDown) {
      mmu->setKeyDown(true);
      anyKeyIsDown = true;
    }
    keyThatIsRepeating = translateKeyWithModifiers(k, m);
    startRepeatTimer = g_cpu->cycles + STARTREPEAT;
    mmu->keyboardInput(keyThatIsRepeating);
  } else if (k == LA) {
    // Special handling: apple keys
    mmu->isOpenApplePressed = true;
    return;
  } else if (k == RA) {
    // Special handling: apple keys
    mmu->isClosedApplePressed = true;
    return;
  } else if (k == JOY2) {
    // Special handling: apple keys
    mmu->isButton2Pressed = true;
    return;
  } else if (k == LOCK) {
    // Special handling: caps lock
    capsLockEnabled = !capsLockEnabled;
    g_keyboard->setCaps(capsLockEnabled);
    return;
  } else if (k == SYSRQ) {
    // Special handling: System request
    biosRequest = true;
    return;
  }
}

void AppleKeyboard::keyReleased(uint8_t k, uint8_t m)
{
  keysDown[k] = false;  

  // Special handling: apple keys
  if (k == LA) {
    mmu->isOpenApplePressed = false;
    return;
  }
  if (k == RA) {
    mmu->isClosedApplePressed = false;
    return;
  }
  if (k == JOY2) {
    mmu->isButton2Pressed = false;
    return;
  }
  if (k == LOCK) {
    // Nothing to do when the caps lock key is released.
    return;
  }

  if (anyKeyIsDown) {
    anyKeyIsDown = false;
    for (uint16_t i=0; i<sizeof(keysDown); i++) {
      if (keysDown[i] && !isVirtualKey(i)) {
	anyKeyIsDown = true;
	break;
      }
    }
    if (!anyKeyIsDown) {
      mmu->setKeyDown(false);
    }
  }  
}

bool AppleKeyboard::getAnnunciator(uint8_t index){
  return mmu->annunciators[index];	
}

void AppleKeyboard::setButton(uint8_t index, bool val){
  if (index == 0) mmu->isOpenApplePressed = val;
  else if (index == 1) mmu->isClosedApplePressed = val;
  else if (index == 2) mmu->isButton2Pressed = val;  
}

void AppleKeyboard::setButtons(bool b0, bool b1, bool b2) {
  mmu->isOpenApplePressed = b0;
  mmu->isClosedApplePressed = b1;
  mmu->isButton2Pressed = b2;
}

void AppleKeyboard::maintainKeyboard(uint32_t cycleCount)
{
  if (anyKeyIsDown) {
    if (startRepeatTimer) {
      if (cycleCount >= startRepeatTimer) {
	// waiting to start repeating
	startRepeatTimer = 0;
	repeatTimer = 0;
	// Will fall through...
      } else {
	// Don't fall through; not time to start repeating yet
	return;
      }
    }
    
    // already repeating; keep it up
    if (cycleCount >= repeatTimer) {
      mmu->keyboardInput(keyThatIsRepeating);
      repeatTimer = cycleCount + REPEATAGAIN;
    }
  }
}
