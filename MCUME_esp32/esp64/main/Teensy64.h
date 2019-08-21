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
#ifndef Teensy64_h_
#define Teensy64_h_

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define F_CPU 240000000.0
#define F_BUS 240000000.0

#include "settings.h"

#define VERSION "09"
#define NTSC (!PAL)
#define USBHOST (!PS2KEYBOARD)

#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "driver/spi_master.h" // for SPI_SWAP_DATA_TX

inline unsigned long millis() {return (esp_timer_get_time()/1000);}

#include "ili9341_t3dma.h"
extern ILI9341_t3DMA tft;	

extern "C" {
  #include "emuapi.h"
}

void initMachine();
void resetMachine() __attribute__ ((noreturn));
void resetExternal();
unsigned loadFile(const char *filename);


#if PAL == 1
#define CRYSTAL       	17734475.0f
#define CLOCKSPEED      ( CRYSTAL / 18.0f) // 985248,61 Hz
#define CYCLESPERRASTERLINE 63
#define LINECNT         312 //Rasterlines
#define VBLANK_FIRST    300
#define VBLANK_LAST     15

#else
#define CRYSTAL       	14318180.0f
#define CLOCKSPEED      ( CRYSTAL / 14.0f) // 1022727,14 Hz
#define CYCLESPERRASTERLINE 64
#define LINECNT       	263 //Rasterlines
#define VBLANK_FIRST    13
#define VBLANK_LAST     40
#endif

#define LINEFREQ      			(CLOCKSPEED / CYCLESPERRASTERLINE) //Hz
#define REFRESHRATE       		(LINEFREQ / LINECNT) //Hz
#define LINETIMER_DEFAULT_FREQ (1000000.0f/LINEFREQ)


#define MCU_C64_RATIO   ((float)F_CPU / CLOCKSPEED) //MCU Cycles per C64 Cycle
#define US_C64_CYCLE    (1000000.0f / CLOCKSPEED) // Duration (µs) of a C64 Cycle

#define AUDIOSAMPLERATE     (LINEFREQ * 2)// (~32kHz)

#define ISR_PRIORITY_RASTERLINE   255



#if 0
#define WRITE_ATN_CLK_DATA(value) { \
    digitalWriteFast(PIN_SERIAL_ATN, (~value & 0x08));\//PTA13 IEC ATN 3
digitalWriteFast(PIN_SERIAL_CLK, (~value & 0x10)); \ //PTA14 IEC CLK 4
digitalWriteFast(PIN_SERIAL_DATA, (~value & 0x20)); \ //PTA15 IEC DATA 5
}
#define READ_CLK_DATA() \
  ((digitalReadFast(PIN_SERIAL_CLK) << 6) | \
   (digitalReadFast(PIN_SERIAL_DATA) << 7))

#else
#define WRITE_ATN_CLK_DATA(value) {}
#define READ_CLK_DATA() (0)
#endif

#include "output_dac.h"
#include "cpu.h"
#endif