#ifdef TEENSYDUINO
#include <Arduino.h>
#define println(x) Serial.println(x)
#else
#include <stdio.h>
#include <unistd.h>
#define println(x) {}
#endif

#include "applemmu.h"
#include "applemmu-rom.h"
#include "physicalspeaker.h"
#include "cpu.h"

#include "globals.h"

// apple //e memory map

/*
page 0x00: zero page (straight ram)
page 0x01: stack (straight ram)
page 0x02: 
page 0x03: 
text/lores page 1: 0x0400 - 0x7FF
text/lores page 2: 0x0800 - 0xBFF
pages 0x0C - 0x1F: straight ram
hires page 1: pages 0x20 - 0x3F
hires page 2: pages 0x40 - 0x5F
pages 0x60 - 0xBF: straight ram
page 0xc0: I/O switches
pages 0xc1 - 0xcf: slot ROMs
pages 0xd0 - 0xdf: Basic ROM
pages 0xe0 - 0xff: monitor ROM
*/

AppleMMU::AppleMMU(AppleDisplay *display)
{
  anyKeyDown = false;
  keyboardStrobe = 0x00;

  isOpenApplePressed = false;
  isClosedApplePressed = false;
  isButton2Pressed = false;

  for (int8_t i=0; i<=7; i++) {
    slots[i] = NULL;
  }

  for (int8_t i=0; i<3; i++) {
    annunciators[i] = false;
  }
  
  allocateMemory();

  this->display = display;
  this->display->setSwitches(&switches);
  resetRAM(); // initialize RAM, load ROM
}

AppleMMU::~AppleMMU()
{
  delete display;
  // FIXME: clean up the memory we allocated
}

void AppleMMU::Reset()
{
  resetRAM();
  resetDisplay(); // sets the switches properly
}

uint8_t AppleMMU::read(uint16_t address)
{
  if (address >= 0xC000 &&
      address <= 0xC0FF) {
    return readSwitches(address);
  }

  // If C800-CFFF isn't latched to a slot ROM, and we try to 
  // access a slot's memory space from C100-C7FF, then we need 
  // to latch in the slot's ROM.
  if (slotLatch == -1 && address >= 0xc100 && address <= 0xc7ff) {
    slotLatch = (address >> 8) & 0x07;
    if (slotLatch == 3 && slot3rom) {
      // Back off: UTA2E p. 5-28: don't latch in slot 3 ROM while 
      // the slot3rom flag is enabled
      // fixme
      slotLatch = 3;
    } else {
      updateMemoryPages();
    }
  }

  // If we access CFFF, that unlatches slot ROM.
  if (address == 0xCFFF) {
    slotLatch = -1;
    updateMemoryPages();
  }

  uint8_t res = readPages[address >> 8][address & 0xFF];

  return res;
}

// Bypass MMU and read directly from a given page - also bypasses switches
uint8_t AppleMMU::readDirect(uint16_t address, uint8_t fromPage)
{
  return ramPages[address >> 8][fromPage][address & 0xFF];
}

// Bypass MMU and write directly to a given page - also bypasses switches
void AppleMMU::writeDirect(uint16_t address, uint8_t fromPage, uint8_t val)
{
  ramPages[address >> 8][fromPage][address & 0xFF] = val;
}

void AppleMMU::write(uint16_t address, uint8_t v)
{
  if (address >= 0xC000 &&
      address <= 0xC0FF) {
    return writeSwitches(address, v);
  }

  // Don't allow writes to ROM
  // Hard ROM, I/O, slots, whatnot
  if (address >= 0xC100 && address <= 0xCFFF)
    return;
  // Bank-switched ROM/RAM areas
  if (address >= 0xD000 && address <= 0xFFFF && !writebsr) {
    return;
  }

  writePages[address >> 8][address & 0xFF] = v;

  if (address >= 0x400 &&
      address <= 0x7FF) {

    // If it's text mode, or mixed mode, or lores graphics mode, then update.
    if ((switches & S_TEXT) || (switches & S_MIXED) || (!(switches & S_HIRES))) {
      // Force a redraw
      display->modeChange();
    }
    return;
  }

  if (address >= 0x2000 &&
      address <= 0x5FFF) {
    if (switches & S_HIRES) {
      // Force a redraw
      display->modeChange();
    }
  }
}

// FIXME: this is no longer "MMU", is it?
void AppleMMU::resetDisplay()
{
  updateMemoryPages();
  display->modeChange();
}

void AppleMMU::handleMemorySwitches(uint16_t address, uint16_t lastSwitch)
{
  // many of these are spelled out here: 
  // http://apple2.org.za/gswv/a2zine/faqs/csa2pfaq.html
  switch (address) {

    // These are write-only and perform no action on read

  case 0xC000: // CLR80STORE
    switches &= ~S_80STORE;
    break;
  case 0xC001: // SET80STORE
    switches |= S_80STORE;
    break;
  case 0xC002: // CLRAUXRD read from main 48k RAM
    auxRamRead = false;
    break;
  case 0xC003: // SETAUXRD read from aux/alt 48k
    auxRamRead = true;
    break;
  case 0xC004: // CLRAUXWR write to main 48k RAM
    auxRamWrite = false;
    break;
  case 0xC005: // SETAUXWR write to aux/alt 48k
    auxRamWrite = true;
    break;
  case 0xC006: // CLRCXROM use ROM on cards
    intcxrom = false;
    break;
  case 0xC007: // SETCXROM use internal ROM
    intcxrom = true;
    break;
  case 0xC008: // CLRAUXZP use main zero page, stack, LC
    altzp = false;
    break;
  case 0xC009: // SETAUXZP use alt zero page, stack, LC
    altzp = true;
    break;
  case 0xC00A: // CLRC3ROM use internal slot 3 ROM
    slot3rom = false;
    break;
  case 0xC00B: // SETC3ROM use external slot 3 ROM
    slot3rom = true;
    break;

    // Registers C080 - C08F control bank switching.
  case 0xC080:
  case 0xC081:
  case 0xC082:
  case 0xC083:
  case 0xC084:
  case 0xC085:
  case 0xC086:
  case 0xC087:
  case 0xC088:
  case 0xC089:
  case 0xC08A:
  case 0xC08B:
  case 0xC08C:
  case 0xC08D:
  case 0xC08E:
  case 0xC08F:

    // Per ITA2E, p. 286:
    // (address & 0x08) controls whether or not we are selecting from bank2. Per table 8-2,
    // bank2 is active if address & 0x08 is zero. So if the bit is on, it's bank 1.
    bank2 = (address & 0x08) ? false : true;

    // (address & 0x04) is unused.

    // (address & 0x02) is read-select: if it is set the same as
    // (address & 0x01) then readbsr is true.
    readbsr = ((address & 0x02) >> 1) == (address & 0x01);

    // (address & 0x01) is write-select: if 1, we write BSR RAM; if 0, we write ROM.
    // But it's a little more complicated than readbsr.
    // Per UTA2E p. 5-23:
    //   "Writing to high RAM is enabled when the HRAMWRT' soft switch
    //   is reset. ... It is reset by even read access or any write
    //   access in the $C08X range. HRAMWRT' is reset by odd read
    //   access in the $C08X range when PRE-WRITE is set. It is set by
    //   even access in the CC08X range. Any other type of access
    //   causes HRAMWRT' to hold its current state."

    if (address & 0x01) {
      if (preWriteFlag)
	writebsr = 1;
      // Per UTA2E, p. 5-23: any other preWriteFlag leaves writebsr unchanged.
    } else {
      writebsr = false;
    }

    break;
  }

  updateMemoryPages();
}

// many (most? all?) switches are documented here:
//   http://apple2.org.za/gswv/a2zine/faqs/csa2pfaq.html

uint8_t AppleMMU::readSwitches(uint16_t address)
{
  static uint16_t lastReadSwitch = 0x0000;
  static uint16_t thisReadSwitch = 0x0000;

  lastReadSwitch = thisReadSwitch;
  thisReadSwitch = address;

  // If this is a read for any of the slot switches, and we have
  // hardware in that slot, then return its result.
  if (address >= 0xC090 && address <= 0xC0FF) {
    for (uint8_t i=1; i<=7; i++) {
      if (address >= (0xC080 | (i << 4)) &&
	  address <= (0xC08F | (i << 4))) {
	if (slots[i]) {
	  return slots[i]->readSwitches(address & ~(0xC080 | (i<<4)));
	}
	else
	  return FLOATING;
      }
    }
  }

  switch (address) {
  case 0xC010:
    // consume the keyboard strobe flag
    keyboardStrobe &= 0x7F;
    return (anyKeyDown ? 0x80 :  0x00);

  case 0xC080:
  case 0xC081:
  case 0xC082:
  case 0xC083:
  case 0xC084:
  case 0xC085:
  case 0xC086:
  case 0xC087:
  case 0xC088:
  case 0xC089:
  case 0xC08A:
  case 0xC08B:
  case 0xC08C:
  case 0xC08D:
  case 0xC08E:
  case 0xC08F:
    // but read does affect these, same as write
    handleMemorySwitches(address, lastReadSwitch);

    // UTA2E, p. 5-23: preWrite is set by odd read access, and reset
    // by even read access
    preWriteFlag = (address & 0x01);

    break;

  case 0xC00C: // CLR80VID disable 80-col video mode
    if (switches & S_80COL) {
      switches &= ~S_80COL;
      resetDisplay();
    }
    break;
  case 0xC00D: // SET80VID enable 80-col video mode
    if (!(switches & S_80COL)) {
      switches |= S_80COL;
      resetDisplay();
    }
    break;

  case 0xC00E: // CLRALTCH use main char set - norm LC, flash UC
    switches &= ~S_ALTCH;
    break;
  case 0xC00F: // SETALTCH use alt char set - norm inverse, LC; no flash
    switches |= S_ALTCH;
    break;


  case 0xC011: // RDLCBNK2
    return bank2 ? 0x80 : 0x00;
  case 0xC012: // RDLCRAM
    return readbsr ? 0x80 : 0x00;
  case 0xC013: // RDRAMRD
    return auxRamRead ? 0x80 : 0x00;
  case 0xC014: // RDRAMWR
    return auxRamWrite ? 0x80 : 0x00;
  case 0xC015: // RDCXROM
    return intcxrom ? 0x80 : 0x00;
  case 0xC016: // RDAUXZP
    return altzp ? 0x80 : 0x00;
  case 0xC017: // RDC3ROM
    return slot3rom ? 0x80 : 0x00;

  case 0xC018: // RD80COL
    return (switches & S_80STORE) ? 0x80 : 0x00;
  case 0xC019: // RDVBLBAR -- vertical blanking, for 4550 cycles of every 17030
    // Should return 0 for 4550 of 17030 cycles. Since we're not really 
    // running full speed video, instead, I'm returning 0 for 4096 (2^12)
    // of every 16384 (2^14) cycles; the math is easier.
    if ((g_cpu->cycles & 0x3000) == 0x3000) {
      return 0x00;
    } else {
      return 0xFF; // FIXME: is 0xFF correct? Or 0x80?
    }
  case 0xC01A: // RDTEXT
    return ( (switches & S_TEXT) ? 0x80 : 0x00 );
  case 0xC01B: // RDMIXED
    return ( (switches & S_MIXED) ? 0x80 : 0x00 );
  case 0xC01C: // RDPAGE2
    return ( (switches & S_PAGE2) ? 0x80 : 0x00 );
  case 0xC01D: // RDHIRES
    return ( (switches & S_HIRES) ? 0x80 : 0x00 );
  case 0xC01E: // RDALTCH
    return ( (switches & S_ALTCH) ? 0x80 : 0x00 );
  case 0xC01F: // RD80VID
    return ( (switches & S_80COL) ? 0x80 : 0x00 );


  case 0xC030: // SPEAKER
    g_speaker->toggle();
    break;

  case 0xC050: // CLRTEXT
    if (switches & S_TEXT) {
      switches &= ~S_TEXT;
      resetDisplay();
    }
    return FLOATING;
  case 0xC051: // SETTEXT
    if (!(switches & S_TEXT)) {
      switches |= S_TEXT;
      resetDisplay();
    }
    return FLOATING;
  case 0xC052: // CLRMIXED
    if (switches & S_MIXED) {
      switches &= ~S_MIXED;
      resetDisplay();
    }
    return FLOATING;
  case 0xC053: // SETMIXED
    if (!(switches & S_MIXED)) {
      switches |= S_MIXED;
      resetDisplay();
    }
    return FLOATING;

  case 0xC054: // PAGE1
    if (switches & S_PAGE2) {
      switches &= ~S_PAGE2;
      if (!(switches & S_80COL)) {
	resetDisplay();
      } else {
	updateMemoryPages();
      }
    }
    return FLOATING;

  case 0xC055: // PAGE2
    if (!(switches & S_PAGE2)) {
      switches |= S_PAGE2;
      if (!(switches & S_80COL)) {
	resetDisplay();
      } else {
	updateMemoryPages();
      }
    }
    return FLOATING;

  case 0xC056: // CLRHIRES
    if (switches & S_HIRES) {
      switches &= ~S_HIRES;
      resetDisplay();
    }
    return FLOATING;
  case 0xC057: // SETHIRES
    if (!(switches & S_HIRES)) {
      switches |= S_HIRES;
      resetDisplay();
    }
    return FLOATING;

  case 0xC058: // annunciator 0 off
    annunciators[0] = false;
	g_keyboard->setAnnunciators();
    return FLOATING;

  case 0xC059: // annunciator 0 on
    annunciators[0] = true;
	g_keyboard->setAnnunciators();
    return FLOATING;

  case 0xC05A: // annunciator 1 off
    annunciators[1] = false;
	g_keyboard->setAnnunciators();
    return FLOATING;

  case 0xC05B: // annunciator 1 on
    annunciators[1] = true;
	g_keyboard->setAnnunciators();
    return FLOATING;

  case 0xC05C: // annunciator 2 off
    annunciators[2] = false;
	g_keyboard->setAnnunciators();
    return FLOATING;

  case 0xC05D: // annunciator 2 on
    annunciators[2] = true;
	g_keyboard->setAnnunciators();
    return FLOATING;

  case 0xC05E: // DHIRES ON
    if (!(switches & S_DHIRES)) {
      switches |= S_DHIRES;
      resetDisplay();
    }
    return FLOATING;

  case 0xC05F: // DHIRES OFF
    if (switches & S_DHIRES) {
      switches &= ~S_DHIRES;
      resetDisplay();
    }
    return FLOATING;

    // paddles
  case 0xC061: // OPNAPPLE
    return isOpenApplePressed ? 0x80 : 0x00;
  case 0xC062: // CLSAPPLE
    return isClosedApplePressed ? 0x80 : 0x00;
  case 0xC063: // Button 2
    return isButton2Pressed ? 0x80 : 0x00;
    
  case 0xC070: // PDLTRIG
    // It doesn't matter if we update readPages or writePages, because 0xC0 
    // has only one page.
    readPages[0xC0][0x64] = readPages[0xC0][0x65] = 0xFF;    
    g_keyboard->startReading();
    g_paddles->startReading();
    return FLOATING;
  }

  if (address >= 0xc000 && address <= 0xc00f) {
    // This is the keyboardStrobe support referenced in the switch statement above.
    return keyboardStrobe;
  }

  return readPages[address >> 8][address & 0xFF];
}

void AppleMMU::writeSwitches(uint16_t address, uint8_t v)
{
  // fixme: combine these with the last read switch
  static uint16_t lastWriteSwitch = 0x0000;
  static uint16_t thisWriteSwitch = 0x0000;
  lastWriteSwitch = thisWriteSwitch;
  thisWriteSwitch = address;

  // If this is a write for any of the slot switches, and we have
  // hardware in that slot, then return its result.
  if (address >= 0xC090 && address <= 0xC0FF) {
    for (uint8_t i=1; i<=7; i++) {
      if (address >= (0xC080 | (i << 4)) &&
	  address <= (0xC08F | (i << 4))) {
	if (slots[i]) {
	  slots[i]->writeSwitches(address & ~(0xC080 | (i<<4)), v);
	  return;
	}
      }
    }
  }

  switch (address) {
  case 0xC010:
  case 0xC011: // Per Understanding the Apple //e, p. 7-3:
  case 0xC012: //   a write to any $C01x address causes 
  case 0xC013: //   a clear of the keyboard strobe.
  case 0xC014:
  case 0xC015:
  case 0xC016:
  case 0xC017:
  case 0xC018:
  case 0xC019:
  case 0xC01A:
  case 0xC01B:
  case 0xC01C:
  case 0xC01D:
  case 0xC01E:
  case 0xC01F:
    keyboardStrobe &= 0x7F;
    return;

  case 0xC050: // graphics mode
    if (switches & S_TEXT) {
      switches &= ~S_TEXT;
      resetDisplay();
    }
    return;
     
  case 0xC051:
    if (!(switches & S_TEXT)) {
      switches |= S_TEXT;
      resetDisplay();
    }
    return;

  case 0xC052: // "no mixed"
    if (switches & S_MIXED) {
      switches &= ~S_MIXED;
      resetDisplay();
    }
    return;

  case 0xC053: // "mixed"
    if (!(switches & S_MIXED)) {
      switches |= S_MIXED;
      resetDisplay();
    }
    return;

  case 0xC054: // page2 off
    if (switches & S_PAGE2) {
      switches &= ~S_PAGE2;
      if (!(switches & S_80COL)) {
	resetDisplay();
      } else {
	updateMemoryPages();
      }
    }
    return;

  case 0xC055: // page2 on
    if (!(switches & S_PAGE2)) {
      switches |= S_PAGE2;
      if (!(switches & S_80COL)) {
	resetDisplay();
      } else {
	updateMemoryPages();
      }
    }
    return;

  case 0xC056: // hires off
    if (switches & S_HIRES) {
      switches &= ~S_HIRES;
      resetDisplay();
    }
    return;

  case 0xC057: // hires on
    if (!(switches & S_HIRES)) {
      switches |= S_HIRES;
      resetDisplay();
    }
    return;

  case 0xC058: // annunciator 0 off
    annunciators[0] = false;
	g_keyboard->setAnnunciators();
    return;

  case 0xC059: // annunciator 0 on
    annunciators[0] = true;
	g_keyboard->setAnnunciators();
    return;

  case 0xC05A: // annunciator 1 off
    annunciators[1] = false;
	g_keyboard->setAnnunciators();
    return;

  case 0xC05B: // annunciator 1 on
    annunciators[1] = true;
	g_keyboard->setAnnunciators();
    return;

  case 0xC05C: // annunciator 2 off
    annunciators[2] = false;
	g_keyboard->setAnnunciators();
    return;

  case 0xC05D: // annunciator 2 on
    annunciators[2] = true;
	g_keyboard->setAnnunciators();
    return;

  case 0xC05E: // DHIRES ON
    if (!(switches & S_DHIRES)) {
      switches |= S_DHIRES;
      resetDisplay();
    }
    return;

  case 0xC05F: // DHIRES OFF
    if (switches & S_DHIRES) {
      switches &= ~S_DHIRES;
      resetDisplay();
    }
    return;

    // paddles
  case 0xC070:
    g_keyboard->startReading();
    g_paddles->startReading();
    writePages[0xC0][0x64] = writePages[0xC0][0x65] = 0xFF;
    break;

  case 0xC080:
  case 0xC081:
  case 0xC082:
  case 0xC083:
  case 0xC084:
  case 0xC085:
  case 0xC086:
  case 0xC087:
  case 0xC088:
  case 0xC089:
  case 0xC08A:
  case 0xC08B:
  case 0xC08C:
  case 0xC08D:
  case 0xC08E:
  case 0xC08F:
    // UTA2E, p. 5-23: preWrite is reset by any write access to these
    preWriteFlag = 0;
    // fall through...
  case 0xC000:
  case 0xC001:
  case 0xC002:
  case 0xC003:
  case 0xC004:
  case 0xC005:
  case 0xC006:
  case 0xC007:
  case 0xC008:
  case 0xC009:
  case 0xC00A:
  case 0xC00B:
    handleMemorySwitches(address, lastWriteSwitch);
    break;

  case 0xC00C: // CLR80VID disable 80-col video mode
    if (switches & S_80COL) {
      switches &= ~S_80COL;
      resetDisplay();
    }
    break;
  case 0xC00D: // SET80VID enable 80-col video mode
    if (!(switches & S_80COL)) {
      switches |= S_80COL;
      resetDisplay();
    }
    break;

  case 0xC00E: // CLRALTCH use main char set - norm LC, flash UC
    switches &= ~S_ALTCH;
    break;
  case 0xC00F: // SETALTCH use alt char set - norm inverse, LC; no flash
    switches |= S_ALTCH;
    break;
  }
}

void AppleMMU::keyboardInput(uint8_t v)
{
  keyboardStrobe = v | 0x80;
  anyKeyDown = true;
}

void AppleMMU::setKeyDown(bool isTrue)
{
  anyKeyDown = isTrue;
}

void AppleMMU::triggerPaddleTimer(uint8_t paddle)
{
  writePages[0xC0][0x64 + paddle] = 0x00;
}

void AppleMMU::resetRAM()
{
  switches = S_TEXT;

  // Per UTA2E, p. 5-23:
  // When a system reset occurs, all MMU soft switches are reset (turned off).
  bank2 = false;
  auxRamRead = auxRamWrite = false;
  readbsr = writebsr = false;
  altzp = false;

  intcxrom = false;
  slot3rom = false;

  slotLatch = -1;

  preWriteFlag = false;

  // Clear all the pages
  for (uint8_t i=0; i<0xFF; i++) {
    for (uint8_t j=0; j<5; j++) {
      if (ramPages[i][j]) {
	for (uint16_t k=0; k<0x100; k++) {
	  ramPages[i][j][k] = 0;
	}
      }
    }
    // and set our expectation of what we're reading from/writing to
    readPages[i] = writePages[i] = ramPages[i][0];
  }

  // Load system ROM
  for (uint16_t i=0x80; i<=0xFF; i++) {
    for (uint16_t k=0; k<0x100; k++) {
      uint16_t idx = ((i-0x80) << 8) | k;
#ifdef TEENSYDUINO
      uint8_t v = pgm_read_byte(&romData[idx]);
#else
      uint8_t v = romData[idx];
#endif
      for (int j=0; j<5; j++) {
	// For the ROM section from 0xc100 .. 0xcfff, we load in to 
	// an alternate page space (INTCXROM).

	if (i >= 0xc1 && i <= 0xcf) {
	  // If we want to convince the VM we've got 128k of RAM, we 
	  // need to load C3 ROM in page 0 (but not 1, meaning there's 
	  // a board installed); and C800.CFFF in both page [0] and [1]
	  // (meaning there's an extended 80-column ROM available, 
	  // that is also physically in the slot).
	  // Everything else goes in page [1].
	  if (i == 0xc3) 
	    ramPages[i][0][k] = v;
	  else if (i >= 0xc8)
	    ramPages[i][0][k] = ramPages[i][1][k] = v;
	  else
	    ramPages[i][1][k] = v;
	} else {
	  // Everything else goes in page 0.
	  ramPages[i][0][k] = v;
	}
      }
    }
  }

  // have each slot load its ROM
  for (uint8_t slotnum = 0; slotnum <= 7; slotnum++) {
    if (slots[slotnum]) {
      slots[slotnum]->loadROM(ramPages[0xC0 + slotnum][0]);
    }
  }

  // update the memory read/write flags &c. Not strictly necessary, if
  // we're really setting all the RAM flags to the right default
  // settings above - but better safe than sorry?
  updateMemoryPages();
}

void AppleMMU::setSlot(int8_t slotnum, Slot *peripheral)
{
  slots[slotnum] = peripheral;
  if (slots[slotnum]) {
    slots[slotnum]->loadROM(ramPages[0xC0 + slotnum][0]);
  }
}

void AppleMMU::allocateMemory()
{
  for (uint16_t i=0; i<0xC0; i++) {
    for (uint8_t j=0; j<2; j++) {
      ramPages[i][j] = (uint8_t *)malloc(0x100);
	  if (ramPages[i][j] == NULL) println("MMU Out of memory!");
    }
    for (uint8_t j=2; j<5; j++) {
      ramPages[i][j] = NULL;
    }
    readPages[i] = ramPages[i][0];
    writePages[i] = ramPages[i][0];
  }
  for (uint16_t i=0xC0; i<0x100; i++) {
    for (uint8_t j=0; j<4; j++) {
      ramPages[i][j] = (uint8_t *)malloc(0x100);
	  if (ramPages[i][j] == NULL) println("MMU Out of memory!");
    }
    for (uint8_t j=4; j<5; j++) {
      ramPages[i][j] = NULL;
    }
    readPages[i] = ramPages[i][0];
    writePages[i] = ramPages[i][0];
  }
}

void AppleMMU::updateMemoryPages()
{
  if (auxRamRead) {
    for (uint8_t idx = 0x02; idx < 0xc0; idx++) {
      readPages[idx] = ramPages[idx][1];
    }
  } else {
    for (uint8_t idx = 0x02; idx < 0xc0; idx++) {
      readPages[idx] = ramPages[idx][0];
    }
  }

  if (auxRamWrite) {
    for (uint8_t idx = 0x02; idx < 0xc0; idx++) {
      writePages[idx] = ramPages[idx][1];
    }
  } else {
    for (uint8_t idx = 0x02; idx < 0xc0; idx++) {
      writePages[idx] = ramPages[idx][0];
    }
  }

  if (switches & S_80STORE) {
    // When S_80STORE is on, we switch 400-800 and 2000-4000 based on S_PAGE2.
    // The behavior is different based on whether HIRESON/OFF is set.
    if (switches & S_PAGE2) {
      // Regardless of HIRESON/OFF, pages 0x400-0x7ff are switched on S_PAGE2
      for (uint8_t idx = 0x04; idx < 0x08; idx++) {
	readPages[idx] = ramPages[idx][1];
	writePages[idx] = ramPages[idx][1];
      }

      // but 2000-3fff switches based on S_PAGE2 only if HIRES is on.

    // HIRESOFF: 400-7ff doesn't switch based on read/write flags
    //           b/c it switches based on S_PAGE2 instead
    // HIRESON: 400-800, 2000-3fff doesn't switch
    //          b/c they switch based on S_PAGE2 instead

      // If HIRES is on, then we honor the PAGE2 setting; otherwise, we don't
      for (uint8_t idx = 0x20; idx < 0x40; idx++) {
	readPages[idx] = ramPages[idx][(switches & S_HIRES) ? 1 : 0];
	writePages[idx] = ramPages[idx][(switches & S_HIRES) ? 1 : 0];
      }
    } else {
      for (uint8_t idx = 0x04; idx < 0x08; idx++) {
	readPages[idx] = ramPages[idx][0];
	writePages[idx] = ramPages[idx][0];
      }
      for (uint8_t idx = 0x20; idx < 0x40; idx++) {
	readPages[idx] = ramPages[idx][0];
	writePages[idx] = ramPages[idx][0];
      }
    }
  }

  if (intcxrom) {
    for (uint8_t idx = 0xc1; idx < 0xd0; idx++) {
      readPages[idx] = ramPages[idx][1];
    }
  } else {
    for (uint8_t idx = 0xc1; idx < 0xd0; idx++) {
      readPages[idx] = ramPages[idx][0];
    }
    if (slot3rom) {
      readPages[0xc3] = ramPages[0xc3][1];
      for (int i=0xc8; i<=0xcf; i++) {
	readPages[i] = ramPages[i][1];
      }
    }
  }

  // If slotLatch is set (!= -1), then we are mapping 2k of ROM
  // for a given peripheral to C800..CFFF.
  if (slotLatch != -1) {
    // FIXME: the only peripheral we support this with right now is 
    // the 80-column card.
    if (slotLatch == 3) {
      for (int i=0xc8; i <= 0xcf; i++) {
	readPages[i] = ramPages[i][1];
      }
    }
  }

  // set zero-page & stack pages based on altzp flag
  if (altzp) {
    for (uint8_t idx = 0x00; idx < 0x02; idx++) {
      readPages[idx] = ramPages[idx][1];
      writePages[idx] = ramPages[idx][1];
    }
  } else {
    for (uint8_t idx = 0x00; idx < 0x02; idx++) {
      readPages[idx] = ramPages[idx][0];
      writePages[idx] = ramPages[idx][0];
    }
  }

  // Set bank-switched ram reading from readbsr & bank2
  if (readbsr) {
    // 0xD0 - 0xE0 has 4 possible banks:
    if (!bank2) {
      // Bank 1 RAM: either in main RAM (1) or in the extended memory
      // card (3):
      for (uint8_t idx = 0xd0; idx < 0xe0; idx++) {
	readPages[idx] = ramPages[idx][altzp ? 3 : 1];
      }
    } else {
      // Bank 2 RAM: either in main RAM (2) or in the extended memory
      // card (4):
      for (uint8_t idx = 0xd0; idx < 0xe0; idx++) {
	readPages[idx] = ramPages[idx][altzp ? 4 : 2];
      }
    }
    // ... but 0xE0 - 0xFF has just the motherboard RAM (1) and
    // extended memory card RAM (2):
    for (uint16_t idx = 0xe0; idx < 0x100; idx++) {
      readPages[idx] = ramPages[idx][altzp ? 2 : 1];
    }
  } else {
    // Built-in ROM
    for (uint16_t idx = 0xd0; idx < 0x100; idx++) {
      readPages[idx] = ramPages[idx][0];
    }
  }

  if (writebsr) {
    if (!bank2) {
      for (uint8_t idx = 0xd0; idx < 0xe0; idx++) {
	writePages[idx] = ramPages[idx][altzp ? 3 : 1];
      }
    } else {
      for (uint8_t idx = 0xd0; idx < 0xe0; idx++) {
	writePages[idx] = ramPages[idx][altzp ? 4 : 2];
      }
    }
    for (uint16_t idx = 0xe0; idx < 0x100; idx++) {
      writePages[idx] = ramPages[idx][altzp ? 2 : 1];
    }
  } else {
    for (uint16_t idx = 0xd0; idx < 0x100; idx++) {
      writePages[idx] = ramPages[idx][0];
    }
  }
}
