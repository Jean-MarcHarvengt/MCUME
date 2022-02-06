#include <string.h> // memset
//#include <TimeLib.h>

#include "plf-clock.h"
#include "applemmu.h" // for FLOATING

/*
 *  http://apple2online.com/web_documents/prodos_technical_notes.pdf
 *
 * When ProDOS calls a clock card, the card deposits an ASCII string
 * in the GETLN input buffer in the form: 07,04,14,22,46,57. The
 * string translates as the following:
 * 
 * 07 = the month, July
 * 04 = the day of the week (00 = Sun)
 * 14 = the date (00 to 31)
 * 22 = the hour (00 to 23)
 * 46 = the minute (00 to 59)
 * 57 = the second (00 to 59)
*/

static void timeToProDOS(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute,
			 uint8_t proDOStimeOut[4])
{
  proDOStimeOut[0] = ((year % 100) << 1) | (month >> 3);
  proDOStimeOut[1] = ((month & 0x0F) << 5) | (day & 0x1F);
  proDOStimeOut[2] = hour & 0x1F;
  proDOStimeOut[3] = minute & 0x3F;
}

PlfClock::PlfClock(AppleMMU *mmu)
{
  this->mmu = mmu;
}

PlfClock::~PlfClock()
{
}

void PlfClock::Reset()
{
}

uint8_t PlfClock::readSwitches(uint8_t s)
{
  // When any switch is read, we'll put the current time in the prodos time buffer

  // JMH tmElements_t tm;
  // JMH breakTime(now(), tm);

  // Put the date/time in the official ProDOS buffer
  uint8_t prodosOut[4];
  // JMH timeToProDOS(tm.Year, tm.Month, tm.Day, tm.Hour, tm.Minute, prodosOut);
  timeToProDOS(1985, 12, 02, 10, 45, prodosOut);
  mmu->write(0xBF90, prodosOut[0]);
  mmu->write(0xBF91, prodosOut[1]);
  mmu->write(0xBF92, prodosOut[2]);
  mmu->write(0xBF93, prodosOut[3]);

  // and also generate a date/time that contains seconds, but not a
  // year, which it also consumes
  char ts[18];
  // JMH sprintf(ts, "%.2d,%.2d,%.2d,%.2d,%.2d,%.2d", 
	//  tm.Month,
	//  tm.Wday - 1, // Sunday should be 0, not 1
	//  tm.Day,
	//  tm.Hour,
	//  tm.Minute,
	//  tm.Second);
  sprintf(ts, "%.2d,%.2d,%.2d,%.2d,%.2d,%.2d", 
    2,
    4 - 1, // Sunday should be 0, not 1
    12,
    10,
    45,
    0);
  uint8_t i = 0;
  while (ts[i]) {
    mmu->write(0x200 + i, ts[i] | 0x80);
    i++;
  }

  return FLOATING;
}

void PlfClock::writeSwitches(uint8_t s, uint8_t v)
{
  //  printf("unimplemented write to the clock - 0x%X\n", v);
}

// FIXME: this assumes slot #7
void PlfClock::loadROM(uint8_t *toWhere)
{
  memset(toWhere, 0xEA, 256); // fill the page with NOPs

  // ProDOS only needs these 4 bytes to recognize that a clock is present
  toWhere[0x00] = 0x08; // PHP
  toWhere[0x02] = 0x28; // PLP
  toWhere[0x04] = 0x58; // CLI
  toWhere[0x06] = 0x70; // BVS

  // Pad out those bytes so they will return control well. The program
  // at c700 becomes
  //
  //   C700: PHP  ; push to stack
  //         NOP  ; filler (filled in by memory clear)
  //         PLP  ; pop from stack
  //         RTS  ; return
  //         CLI  ; required to detect driver, but not used
  //         NOP  ; filled in by memory clear
  //         BVS  ; required to detect driver, but not used

  toWhere[0x03] = 0x60; // RTS

  // And it needs a small routing here to read/write it:
  // 0x08: read
  toWhere[0x08] = 0x4C; // JMP $C710
  toWhere[0x09] = 0x10;
  toWhere[0x0A] = 0xC7;

  // 0x0b: write
  toWhere[0x0B] = 0x8D; // STA $C0F0 (slot 7's first switch)
  toWhere[0x0C] = 0xF0;
  toWhere[0x0D] = 0xC0;
  toWhere[0x0E] = 0x60; // RTS

  // simple read
  toWhere[0x10] = 0xAD; // LDA $C0F0 (slot 7's first switch)
  toWhere[0x11] = 0xF0;
  toWhere[0x12] = 0xC0;
  toWhere[0x13] = 0x60; // RTS
}

