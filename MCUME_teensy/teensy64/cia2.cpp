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

#include "cpu.h"
#include "cia2.h"
#include <string.h>

#define DEBUGCIA2 0
#define RTCDEBUG 0

#define decToBcd(x)	( ( (uint8_t) (x) / 10 * 16) | ((uint8_t) (x) % 10) )
#define bcdToDec(x) ( ( (uint8_t) (x) / 16 * 10) | ((uint8_t) (x) % 16) )
#define tod()		(cpu.cia2.TODfrozen?cpu.cia2.TODfrozenMillis:(int)((millis() - cpu.cia2.TOD) % 86400000l))

void cia2_setAlarmTime() {
  cpu.cia2.TODAlarm = (cpu.cia2.W[0x08] + cpu.cia2.W[0x09] * 10l + cpu.cia2.W[0x0A] * 600l + cpu.cia2.W[0x0B] * 36000l);
}

void cia2_write(uint32_t address, uint8_t value) {

  address &= 0x0F;

  switch (address) {

    case 0x00 : {
      if ((~value & 0x38)) {
        cpu_setExactTiming();
      }

	  WRITE_ATN_CLK_DATA(value);


      cpu.vic.bank = ((~value) & 0x03) * 16384;
      vic_adrchange();
      cpu.cia2.R[address] = value;
	  }
      break;

    case 0x01 :
	   break;//Data PORTB

    case 0x04 : {
        cpu.cia2.W[address] = value;
      }
	  break; //Timer A LO
    case 0x05 : {
        cpu.cia2.W[address] = value;
        if ((cpu.cia2.R[0x0E] & 0x01) == 0) cpu.cia2.R[address] = value;
      }
	  break; //Timer A HI
    case 0x06 : {
        cpu.cia2.W[address] = value;
      }
	  break; //Timer B LO
    case 0x07 : {
        cpu.cia2.W[address] = value;
        if ((cpu.cia2.R[0x0F] & 0x01) == 0) cpu.cia2.R[address] = value;
      }
      break; //Timer B HI

    //RTC
    case 0x08 : {
        if ((cpu.cia2.R[0x0f] & 0x80) > 0) {
          value &= 0x0f;
          cpu.cia2.W[address] = value;
          cia2_setAlarmTime();

#if RTCDEBUG
          Serial.print("CIA 2 Set Alarm TENTH:");
          Serial.println(value, HEX);
#endif

        } else {
          value &= 0x0f;
          cpu.cia2.TODstopped = 0;
          //Translate set Time to TOD:
          cpu.cia2.TOD = (int)(millis() % 86400000l) -
						(value * 100 + cpu.cia2.R[0x09] * 1000l + cpu.cia2.R[0x0A] * 60000l + cpu.cia2.R[0x0B] * 3600000l);
#if RTCDEBUG
          Serial.print("CIA 2 Set TENTH:");
          Serial.println(value, HEX);
          Serial.print("CIA 2 TOD (millis):");
          Serial.println(cpu.cia2.TOD);
#endif
        }
      }
      break; //TOD-Tenth
    case 0x09 : {
        if ((cpu.cia2.R[0x0f] & 0x80) > 0) {
          cpu.cia2.W[address] = bcdToDec(value);
          cia2_setAlarmTime();
#if RTCDEBUG
          Serial.print("CIA 2 Set Alarm SEC:");
          Serial.println(value, HEX);
#endif

        } else {
          cpu.cia2.R[address] = bcdToDec(value);
#if RTCDEBUG
          Serial.print("CIA 2 Set SEC:");
          Serial.println(value, HEX);
#endif

        }
      }
      break; //TOD-Secs
    case 0x0A : {
        if ((cpu.cia2.R[0x0f] & 0x80) > 0) {
          cpu.cia2.W[address] = bcdToDec(value);
          cia2_setAlarmTime();
#if RTCDEBUG
          Serial.print("CIA 2 Set Alarm MIN:");
          Serial.println(value, HEX);
#endif

        } else {
          cpu.cia2.R[address] = bcdToDec(value);
#if RTCDEBUG
          Serial.print("CIA 2 Set MIN:");
          Serial.println(value, HEX);
#endif

        }
      }
	  break; //TOD-Minutes
    case 0x0B : {
        if ((cpu.cia2.R[0x0f] & 0x80) > 0) {
          cpu.cia2.W[address] = bcdToDec(value & 0x1f) + (value & 0x80 ? 12 : 0);
          cia2_setAlarmTime();
#if RTCDEBUG
          Serial.print("CIA 2 Set Alarm HRS:");
          Serial.println(value, HEX);
#endif

        } else {
          cpu.cia2.R[address] = bcdToDec(value & 0x1f) + (value & 0x80 ? 12 : 0);
          cpu.cia2.TODstopped = 1;
#if RTCDEBUG
          Serial.print("CIA 2 Set HRS:");
          Serial.println(value, HEX);
#endif
        }
      }
	  break; //TOD-Hours
    case 0x0C : {
        cpu.cia2.R[address] = value;
        //Fake IRQ
        cpu.cia2.R[0x0d] |= 8 | ((cpu.cia2.W[0x0d] & 0x08) << 4);
        cpu_nmi();
      }
      break;
    case 0x0D : {
        if ((value & 0x80) > 0) {
          cpu.cia2.W[address] |= value & 0x1f;
          //ggf NMItriggern
          if (cpu.cia2.R[address] & cpu.cia2.W[address] & 0x1f) {
            cpu.cia2.R[address] |= 0x80;
            cpu_nmi();
          };
        } else {
          cpu.cia2.W[address] &= ~value;
        }
      }
      break;

    case 0x0E : {
        cpu.cia2.R[address] = value & ~0x10;
        if ((value & 0x10) > 0) {
          cpu.cia2.R16[0x04 / 2] = cpu.cia2.W16[0x04 / 2];
        }
      }
      break;
    case 0x0F : {
        cpu.cia2.R[address] = value & ~0x10;
        if ((value & 0x10) > 0) {
          cpu.cia2.R16[0x06 / 2] = cpu.cia2.W16[0x06 / 2];
        }
      }
	  break;
    default : {
        cpu.cia2.R[address] = value;/*if (address ==0) {Serial.print(value);Serial.print(" ");}*/
      }
	  break;
  }

#if DEBUGCIA2
  Serial.printf("%x CIA2: W %x %x\n", cpu.pc, address, value);
#endif
}

uint8_t cia2_read(uint32_t address) {
  uint8_t ret;

  address &= 0x0F;

  switch (address) {

    case 0x00 : {
        ret = (cpu.cia2.R[address] & 0x3f) | READ_CLK_DATA();
        if ((~ret & 0x3f)) {
          cpu_setExactTiming();
        }
		
        break;
      }

    //RTC
    case 0x08: {
        ret = tod() % 1000 / 10;
        cpu.cia2.TODfrozen = 0;
      };

#if RTCDEBUG
      Serial.print("CIA 2 Read TENTH:");
      Serial.println(ret, HEX);
#endif

      break;  //Bit 0..3: Zehntelsekunden im BCD-Format ($0-$9) Bit 4..7: immer 0
    case 0x09: {
        ret = decToBcd(tod() / 1000 % 60);
      };
      //Serial.println( tod() / 100);
#if RTCDEBUG
      Serial.print("CIA 2 Read SEC:");
      Serial.println(ret, HEX);
#endif

      break;        //Bit 0..3: Einersekunden im BCD-Format ($0-$9) Bit 4..6: Zehnersekunden im BCD-Format ($0-$5) Bit 7: immer 0
    case 0x0A: {
        ret = decToBcd(tod() / (1000 * 60) % 60);
      };
#if RTCDEBUG
      Serial.print("CIA 2 Read MIN:");
      Serial.println(ret, HEX);
#endif

      break;    //Bit 0..3: Einerminuten im BCD-Format( $0-$9) Bit 4..6: Zehnerminuten im BCD-Format ($0-$5) Bit 7: immer 0
    case 0x0B: {
        //Bit 0..3: Einerstunden im BCD-Format ($0-$9) Bit 4: Zehnerstunden im BCD-Format ($0-$1) // Bit 7: Unterscheidung AM/PM, 0=AM, 1=PM
        //Lesen aus diesem Register friert alle TOD-Register ein (TOD läuft aber weiter), bis Register 8 (TOD 10THS) gelesen wird.
        cpu.cia2.TODfrozen = 0;
        cpu.cia2.TODfrozenMillis = tod();
        cpu.cia2.TODfrozen = 1;
#if RTCDEBUG
        Serial.print("CIA 2 FrozenMillis:");
        Serial.println(cpu.cia2.TODfrozenMillis);
#endif
        ret = cpu.cia2.TODfrozenMillis / (1000 * 3600) % 24;
        if (ret >= 12)
          ret = 128 | decToBcd(ret - 12);
        else
          ret = decToBcd(ret);
      };
#if RTCDEBUG
      Serial.print("CIA 2 Read HRS:");
      Serial.println(ret, HEX);
#endif

      break;

    case 0x0D: {
        ret = cpu.cia2.R[address] & 0x9f;
        cpu.cia2.R[address] = 0;
        cpu_clearNmi();
      }; break;
    default: ret = cpu.cia2.R[address]; break;
  }

#if DEBUGCIA2
  Serial.printf("%x CIA2: R %x %x\n", cpu.pc, address, ret);
#endif
  return ret;
}

#if 0
void cia2_clock(int clk) {

  uint32_t cnta, cntb, cra, crb;

  //Timer A
  cra = cpu.cia2.R[0x0e];
  crb = cpu.cia2.R[0x0f];

  if (( cra & 0x21) == 0x01) {
    cnta = cpu.cia2.R[0x04] | cpu.cia2.R[0x05] << 8;
    cnta -= clk;
    if (cnta > 0xffff) { //Underflow
      cnta = cpu.cia2.W[0x04] | cpu.cia2.W[0x05] << 8; // Reload Timer
      if (cra & 0x08) { // One Shot
        cpu.cia2.R[0x0e] &= 0xfe; //Stop timer
      }

      //Interrupt:
      cpu.cia2.R[0x0d] |= 1 | /* (cpu.cia2.W[0x0d] & 0x01) |*/ ((cpu.cia2.W[0x0d] & 0x01) << 7);

      if ((crb & 0x61) == 0x41) { //Timer B counts underflows of Timer A
        cntb = cpu.cia2.R[0x06] | cpu.cia2.R[0x07] << 8;
        cntb--;
        if (cntb > 0xffff) { //underflow
          cpu.cia2.R[0x04] = cnta & 0x0f;
          cpu.cia2.R[0x05] = cnta >> 8;
          goto underflow_b;
        }
      }
    }

    cpu.cia2.R[0x04] = cnta & 0x0f;
    cpu.cia2.R[0x05] = cnta >> 8;

  }

  //Timer B
  if (( crb & 0x61) == 0x01) {
    cntb = cpu.cia2.R[0x06] | cpu.cia2.R[0x07] << 8;
    cntb -= clk;
    if (cntb > 0xffff) { //underflow
underflow_b:
      cntb = cpu.cia2.W[0x06] | cpu.cia2.W[0x07] << 8; // Reload Timer
      if (crb & 0x08) { // One Shot
        cpu.cia2.R[0x0f] &= 0xfe; //Stop timer
      }

      //Interrupt:
      cpu.cia2.R[0x0d] |= 2 | /*(cpu.cia2.W[0x0d] & 0x02) | */ ((cpu.cia2.W[0x0d] & 0x02) << 6);
    }

    cpu.cia2.R[0x06] = cntb & 0x0f;
    cpu.cia2.R[0x07] = cntb >> 8;

  }
  if (cpu.cia2.R[0x0d] & 0x80) cpu_nmi();
}

#else

void cia2_clock(int clk) {

  int32_t t;
  uint32_t regFEDC = cpu.cia2.R32[0x0C / 4];

  // TIMER A
  //if (((cpu.cia2.R[0x0E] & 0x01)>0) && ((cpu.cia2.R[0x0E] & 0x20)==0)) {
  //if ((regFEDC & 0x210000)==0x10000) {
  if (((regFEDC >> 16) & 0x21) == 0x1) {

    t = cpu.cia2.R16[0x04 / 2];

    if (clk > t) { //underflow
      t = cpu.cia2.W16[0x04 / 2] - (clk - t); //neu
      regFEDC |= 0x00000100;
      if ((regFEDC & 0x00080000)) regFEDC &= 0xfffeffff;
    }
    else {
      t -= clk;
    }

    cpu.cia2.R16[0x04 / 2] = t;
  }


  // TIMER B
  //TODO : Prüfen ob das funktioniert
  if ( regFEDC & 0x01000000 ) {
    //uint16_t quelle = (cpu.cia2.R[0x0F]>>5) & 0x03;
    if ((regFEDC & 0x60000000) == 0x40000000) {

      if (regFEDC & 0x00000100) //unterlauf TimerA?
        clk = 1;
      else
        goto tend;
    }

    t = cpu.cia2.R16[0x06 / 2];

    if (clk > t) { //underflow
      t = cpu.cia2.W16[0x06 / 2] - (clk - t); //Neu
      regFEDC |= 0x00000200;
      if ((regFEDC & 0x08000000)) regFEDC &= 0xfeffffff;
    } else {
      t -= clk;
    }
    cpu.cia2.R16[0x06 / 2] = t;

  }

tend:


  // INTERRUPT ?
  if ( regFEDC & cpu.cia2.W32[0x0C / 4] & 0x0f00 ) {
    regFEDC |= 0x8000;
    cpu.cia2.R32[0x0C / 4] = regFEDC;
  }
  cpu.cia2.R32[0x0C / 4] = regFEDC;
}
#endif

void cia2_checkRTCAlarm() { // call every 1/10 sec minimum
  if ((int)(millis() - cpu.cia2.TOD) % 86400000l / 100 == cpu.cia2.TODAlarm) {
    // Serial.print("CIA2 RTC interrupt");
    // Interrupt
    cpu.cia2.R[0x0d] |= 0x4 | (cpu.cia2.W[0x0d] & 4) << 5;
  }
}

void resetCia2(void) {
  memset((uint8_t*)&cpu.cia2.R, 0, sizeof(cpu.cia2.R));
  cpu.cia2.R[0x04] = 0xff;
  cpu.cia2.R[0x05] = 0xff;
  cpu.cia2.R[0x06] = 0xff;
  cpu.cia2.R[0x07] = 0xff;

  //pinMode(PIN_SERIAL_ATN, OUTPUT_OPENDRAIN);  //ATN OUT (CIA2 PA3 OUT)
  //pinMode(PIN_SERIAL_CLK, OUTPUT_OPENDRAIN);  //CLK   (CIA2 PA6:IN PA4: OUT)
  //pinMode(PIN_SERIAL_DATA, OUTPUT_OPENDRAIN); //DATA  (CIA2 PA7:IN PA5: OUT)
  //digitalWriteFast(PIN_SERIAL_ATN, 1);
  //digitalWriteFast(PIN_SERIAL_CLK, 1);
  //digitalWriteFast(PIN_SERIAL_DATA, 1);

}
