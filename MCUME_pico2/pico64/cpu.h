/*
	Copyright Frank Bösing, 2017

	This file is part of Teensy64.

    Teensy64 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Teensy64 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Teensy64.  If not, see <http://www.gnu.org/licenses/>.

    Diese Datei ist Teil von Teensy64.

    Teensy64 ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Teensy64 wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

*/

#ifndef Teensy64_cpu_h_
#define Teensy64_cpu_h_

//#include <arm_math.h>


#define INLINE 			static inline __attribute__((always_inline))
#define INLINEOP 		static inline __attribute__((always_inline, flatten))
#define OPCODE			static

#define RAMSIZE 		65536	//Bytes


#include "Teensy64.h"
#include "roms.h"
#include "patches.h"
#include "timerutil.h"
#include "pla.h"
#include "vic.h"
#include "keyboard.h"
#include "cia1.h"
#include "cia2.h"


//#include <reSID.h>
//extern AudioPlaySID  playSID;
//extern AudioOutputAnalog audioout;

#define BASE_STACK     0x100

struct tio  {
	uint32_t gpioa, gpiob, gpioc, gpiod, gpioe;
}__attribute__((packed, aligned(4)));

struct tcpu {
  uint32_t exactTimingStartTime;
  uint8_t exactTiming;

  //6502 CPU registers
  uint8_t sp, a, x, y, cpustatus;
  uint8_t penaltyop, penaltyaddr;
  uint8_t nmi;
  uint16_t pc;

  //helper variables
  uint16_t reladdr;
  uint16_t ea;

  uint16_t lineCyclesAbs; //for debug
  unsigned ticks;
  unsigned lineCycles;
  unsigned long lineStartTime;

  r_rarr_ptr_t plamap_r; //Memory-Mapping read
  w_rarr_ptr_t plamap_w; //Memory-Mapping write
  uint8_t _exrom:1, _game:1;
  uint8_t nmiLine;
  uint8_t swapJoysticks;

  tvic vic;
  tcia cia1;
  tcia cia2;

  union {
	uint8_t RAM[RAMSIZE];
	uint16_t RAM16[RAMSIZE/2];
	uint32_t RAM32[RAMSIZE/4];
  };



  uint8_t cartrigeLO[1]; //TODO
  uint8_t cartrigeHI[1]; //TODO

};

extern struct tio io;
extern struct tcpu cpu;

void cpu_reset();
void cpu_nmi();
void cpu_clearNmi();
void cpu_clock(int cycles);
void cpu_setExactTiming();
void cpu_disableExactTiming();

void cia_clockt(int ticks);

#define CORE_PIN0_PORT	io.gpiob
#define CORE_PIN1_PORT	io.gpiob
#define CORE_PIN2_PORT	io.gpiod
#define CORE_PIN3_PORT	io.gpioa
#define CORE_PIN4_PORT	io.gpioa
#define CORE_PIN5_PORT	io.gpiod
#define CORE_PIN6_PORT	io.gpiod
#define CORE_PIN7_PORT	io.gpiod
#define CORE_PIN8_PORT	io.gpiod
#define CORE_PIN9_PORT	io.gpioc
#define CORE_PIN10_PORT	io.gpioc
#define CORE_PIN11_PORT	io.gpioc
#define CORE_PIN12_PORT	io.gpioc
#define CORE_PIN13_PORT	io.gpioc
#define CORE_PIN14_PORT	io.gpiod
#define CORE_PIN15_PORT	io.gpioc
#define CORE_PIN16_PORT	io.gpiob
#define CORE_PIN17_PORT	io.gpiob
#define CORE_PIN18_PORT	io.gpiob
#define CORE_PIN19_PORT	io.gpiob
#define CORE_PIN20_PORT	io.gpiod
#define CORE_PIN21_PORT	io.gpiod
#define CORE_PIN22_PORT	io.gpioc
#define CORE_PIN23_PORT	io.gpioc
#define CORE_PIN24_PORT	io.gpioe
#define CORE_PIN25_PORT	io.gpioa
#define CORE_PIN26_PORT	io.gpioa
#define CORE_PIN27_PORT	io.gpioa
#define CORE_PIN28_PORT	io.gpioa
#define CORE_PIN29_PORT	io.gpiob
#define CORE_PIN30_PORT	io.gpiob
#define CORE_PIN31_PORT	io.gpiob
#define CORE_PIN32_PORT	io.gpiob
#define CORE_PIN33_PORT	io.gpioe
#define CORE_PIN34_PORT	io.gpioe
#define CORE_PIN35_PORT	io.gpioc
#define CORE_PIN36_PORT	io.gpioc
#define CORE_PIN37_PORT	io.gpioc
#define CORE_PIN38_PORT	io.gpioc
#define CORE_PIN39_PORT	io.gpioa
#define CORE_PIN40_PORT	io.gpioa
#define CORE_PIN41_PORT	io.gpioa
#define CORE_PIN42_PORT	io.gpioa
#define CORE_PIN43_PORT	io.gpiob
#define CORE_PIN44_PORT	io.gpiob
#define CORE_PIN45_PORT	io.gpiob
#define CORE_PIN46_PORT	io.gpiob
#define CORE_PIN47_PORT	io.gpiod
#define CORE_PIN48_PORT	io.gpiod
#define CORE_PIN49_PORT	io.gpiob
#define CORE_PIN50_PORT	io.gpiob
#define CORE_PIN51_PORT	io.gpiod
#define CORE_PIN52_PORT	io.gpiod
#define CORE_PIN53_PORT	io.gpiod
#define CORE_PIN54_PORT	io.gpiod
#define CORE_PIN55_PORT	io.gpiod
#define CORE_PIN56_PORT	io.gpioe
#define CORE_PIN57_PORT	io.gpioe
#define CORE_PIN58_PORT	io.gpioe
#define CORE_PIN59_PORT	io.gpioe
#define CORE_PIN60_PORT	io.gpioe
#define CORE_PIN61_PORT	io.gpioe
#define CORE_PIN62_PORT	io.gpioe
#define CORE_PIN63_PORT	io.gpioe

static inline uint8_t gpioRead(uint8_t pin) __attribute__((always_inline, unused));
static inline uint8_t gpioRead(uint8_t pin)
{
	/*
	if (__builtin_constant_p(pin)) {
		if (pin == 0) {
			return (CORE_PIN0_PORT & CORE_PIN0_BITMASK) ? 1 : 0;
		} else if (pin == 1) {
			return (CORE_PIN1_PORT & CORE_PIN1_BITMASK) ? 1 : 0;
		} else if (pin == 2) {
			return (CORE_PIN2_PORT & CORE_PIN2_BITMASK) ? 1 : 0;
		} else if (pin == 3) {
			return (CORE_PIN3_PORT & CORE_PIN3_BITMASK) ? 1 : 0;
		} else if (pin == 4) {
			return (CORE_PIN4_PORT & CORE_PIN4_BITMASK) ? 1 : 0;
		} else if (pin == 5) {
			return (CORE_PIN5_PORT & CORE_PIN5_BITMASK) ? 1 : 0;
		} else if (pin == 6) {
			return (CORE_PIN6_PORT & CORE_PIN6_BITMASK) ? 1 : 0;
		} else if (pin == 7) {
			return (CORE_PIN7_PORT & CORE_PIN7_BITMASK) ? 1 : 0;
		} else if (pin == 8) {
			return (CORE_PIN8_PORT & CORE_PIN8_BITMASK) ? 1 : 0;
		} else if (pin == 9) {
			return (CORE_PIN9_PORT & CORE_PIN9_BITMASK) ? 1 : 0;
		} else if (pin == 10) {
			return (CORE_PIN10_PORT & CORE_PIN10_BITMASK) ? 1 : 0;
		} else if (pin == 11) {
			return (CORE_PIN11_PORT & CORE_PIN11_BITMASK) ? 1 : 0;
		} else if (pin == 12) {
			return (CORE_PIN12_PORT & CORE_PIN12_BITMASK) ? 1 : 0;
		} else if (pin == 13) {
			return (CORE_PIN13_PORT & CORE_PIN13_BITMASK) ? 1 : 0;
		} else if (pin == 14) {
			return (CORE_PIN14_PORT & CORE_PIN14_BITMASK) ? 1 : 0;
		} else if (pin == 15) {
			return (CORE_PIN15_PORT & CORE_PIN15_BITMASK) ? 1 : 0;
		} else if (pin == 16) {
			return (CORE_PIN16_PORT & CORE_PIN16_BITMASK) ? 1 : 0;
		} else if (pin == 17) {
			return (CORE_PIN17_PORT & CORE_PIN17_BITMASK) ? 1 : 0;
		} else if (pin == 18) {
			return (CORE_PIN18_PORT & CORE_PIN18_BITMASK) ? 1 : 0;
		} else if (pin == 19) {
			return (CORE_PIN19_PORT & CORE_PIN19_BITMASK) ? 1 : 0;
		} else if (pin == 20) {
			return (CORE_PIN20_PORT & CORE_PIN20_BITMASK) ? 1 : 0;
		} else if (pin == 21) {
			return (CORE_PIN21_PORT & CORE_PIN21_BITMASK) ? 1 : 0;
		} else if (pin == 22) {
			return (CORE_PIN22_PORT & CORE_PIN22_BITMASK) ? 1 : 0;
		} else if (pin == 23) {
			return (CORE_PIN23_PORT & CORE_PIN23_BITMASK) ? 1 : 0;
		} else if (pin == 24) {
			return (CORE_PIN24_PORT & CORE_PIN24_BITMASK) ? 1 : 0;
		} else if (pin == 25) {
			return (CORE_PIN25_PORT & CORE_PIN25_BITMASK) ? 1 : 0;
		} else if (pin == 26) {
			return (CORE_PIN26_PORT & CORE_PIN26_BITMASK) ? 1 : 0;
		} else if (pin == 27) {
			return (CORE_PIN27_PORT & CORE_PIN27_BITMASK) ? 1 : 0;
		} else if (pin == 28) {
			return (CORE_PIN28_PORT & CORE_PIN28_BITMASK) ? 1 : 0;
		} else if (pin == 29) {
			return (CORE_PIN29_PORT & CORE_PIN29_BITMASK) ? 1 : 0;
		} else if (pin == 30) {
			return (CORE_PIN30_PORT & CORE_PIN30_BITMASK) ? 1 : 0;
		} else if (pin == 31) {
			return (CORE_PIN31_PORT & CORE_PIN31_BITMASK) ? 1 : 0;
		} else if (pin == 32) {
			return (CORE_PIN32_PORT & CORE_PIN32_BITMASK) ? 1 : 0;
		} else if (pin == 33) {
			return (CORE_PIN33_PORT & CORE_PIN33_BITMASK) ? 1 : 0;
		} else if (pin == 34) {
			return (CORE_PIN34_PORT & CORE_PIN34_BITMASK) ? 1 : 0;
		} else if (pin == 35) {
			return (CORE_PIN35_PORT & CORE_PIN35_BITMASK) ? 1 : 0;
		} else if (pin == 36) {
			return (CORE_PIN36_PORT & CORE_PIN36_BITMASK) ? 1 : 0;
		} else if (pin == 37) {
			return (CORE_PIN37_PORT & CORE_PIN37_BITMASK) ? 1 : 0;
		} else if (pin == 38) {
			return (CORE_PIN38_PORT & CORE_PIN38_BITMASK) ? 1 : 0;
		} else if (pin == 39) {
			return (CORE_PIN39_PORT & CORE_PIN39_BITMASK) ? 1 : 0;
		} else if (pin == 40) {
			return (CORE_PIN40_PORT & CORE_PIN40_BITMASK) ? 1 : 0;
		} else if (pin == 41) {
			return (CORE_PIN41_PORT & CORE_PIN41_BITMASK) ? 1 : 0;
		} else if (pin == 42) {
			return (CORE_PIN42_PORT & CORE_PIN42_BITMASK) ? 1 : 0;
		} else if (pin == 43) {
			return (CORE_PIN43_PORT & CORE_PIN43_BITMASK) ? 1 : 0;
		} else if (pin == 44) {
			return (CORE_PIN44_PORT & CORE_PIN44_BITMASK) ? 1 : 0;
		} else if (pin == 45) {
			return (CORE_PIN45_PORT & CORE_PIN45_BITMASK) ? 1 : 0;
		} else if (pin == 46) {
			return (CORE_PIN46_PORT & CORE_PIN46_BITMASK) ? 1 : 0;
		} else if (pin == 47) {
			return (CORE_PIN47_PORT & CORE_PIN47_BITMASK) ? 1 : 0;
		} else if (pin == 48) {
			return (CORE_PIN48_PORT & CORE_PIN48_BITMASK) ? 1 : 0;
		} else if (pin == 49) {
			return (CORE_PIN49_PORT & CORE_PIN49_BITMASK) ? 1 : 0;
		} else if (pin == 50) {
			return (CORE_PIN50_PORT & CORE_PIN50_BITMASK) ? 1 : 0;
		} else if (pin == 51) {
			return (CORE_PIN51_PORT & CORE_PIN51_BITMASK) ? 1 : 0;
		} else if (pin == 52) {
			return (CORE_PIN52_PORT & CORE_PIN52_BITMASK) ? 1 : 0;
		} else if (pin == 53) {
			return (CORE_PIN53_PORT & CORE_PIN53_BITMASK) ? 1 : 0;
		} else if (pin == 54) {
			return (CORE_PIN54_PORT & CORE_PIN54_BITMASK) ? 1 : 0;
		} else if (pin == 55) {
			return (CORE_PIN55_PORT & CORE_PIN55_BITMASK) ? 1 : 0;
		} else if (pin == 56) {
			return (CORE_PIN56_PORT & CORE_PIN56_BITMASK) ? 1 : 0;
		} else if (pin == 57) {
			return (CORE_PIN57_PORT & CORE_PIN57_BITMASK) ? 1 : 0;
		} else if (pin == 58) {
			return (CORE_PIN58_PORT & CORE_PIN58_BITMASK) ? 1 : 0;
		} else if (pin == 59) {
			return (CORE_PIN59_PORT & CORE_PIN59_BITMASK) ? 1 : 0;
		} else if (pin == 60) {
			return (CORE_PIN60_PORT & CORE_PIN60_BITMASK) ? 1 : 0;
		} else if (pin == 61) {
			return (CORE_PIN61_PORT & CORE_PIN61_BITMASK) ? 1 : 0;
		} else if (pin == 62) {
			return (CORE_PIN62_PORT & CORE_PIN62_BITMASK) ? 1 : 0;
		} else if (pin == 63) {
			return (CORE_PIN63_PORT & CORE_PIN63_BITMASK) ? 1 : 0;
		} else {
			return 0;
		}
	} else {
		Serial.println("Warning: Pin# not constant");
		return digitalRead(pin);
	}
	*/
	return 0;

}

#endif