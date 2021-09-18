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

#ifndef Teensy64_pla_h_
#define Teensy64_pla_h_

#define CONSTROM const

#define MEM_BASIC_ROM 0xA000
#define MEM_CHARSET_ROM 0xD000
#define MEM_KERNAL_ROM  0xE000

#define MEM_VIC     0xD000
#define MEM_VICCOLOR  0xD800
#define MEM_SID     0xD400
#define MEM_CIA1    0xDC00
#define MEM_CIA2    0xDD00

//C64 Memory/Device access (PLA)

/* READ */
typedef uint8_t (*r_ptr_t)( uint32_t address ); //Funktionspointer auf uint8_t foo(uint16_t address);
typedef r_ptr_t rarray_t[256];          //Array von Funktionspointern
typedef rarray_t * r_rarr_ptr_t;        //Pointer auf Array von Funktionspointern

/* WRITE */
typedef void (*w_ptr_t)( uint32_t address, uint8_t value ); //Funktionspointer auf void foo( uint16_t address, uint8_t value );
typedef w_ptr_t warray_t[256];                //Array von Funktionspointern
typedef warray_t * w_rarr_ptr_t;              //Pointer auf Array von Funktionspointern


void resetPLA(void);

#endif
