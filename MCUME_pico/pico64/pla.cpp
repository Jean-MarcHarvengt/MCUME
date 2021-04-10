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

#include <string.h>
#include "roms.h"
#include "cpu.h"

#include "pla.h"
#include "vic.h"
#include "cia1.h"
#include "cia2.h"


extern CONSTROM rarray_t PLA_READ[8];
extern CONSTROM warray_t PLA_WRITE[8];

uint8_t r_ram(uint32_t address)		{ return cpu.RAM[address]; }
uint8_t r_bas(uint32_t address)		{ return rom_basic[address & (sizeof(rom_basic)-1)]; } //BASIC ROM
uint8_t r_ker(uint32_t address)		{ return rom_kernal[address & (sizeof(rom_kernal)-1)]; } //KERNAL ROM
uint8_t r_chr(uint32_t address)		{ return rom_characters[address & (sizeof(rom_characters)-1)]; } //CHARACTER ROM
uint8_t r_vic(uint32_t address)		{ return vic_read(address); }
#ifdef HAS_SND      
uint8_t r_sid(uint32_t address)		{ return playSID.getreg(address & 0x1F);}
#else
uint8_t r_sid(uint32_t address)		{ return 0;}
#endif
uint8_t r_col(uint32_t address)		{ return cpu.vic.COLORRAM[address & 0x3FF]; }
uint8_t r_cia1(uint32_t address)	{ return cia1_read(address); }
uint8_t r_cia2(uint32_t address)	{ return cia2_read(address); }
uint8_t r_crtL(uint32_t address)	{ return cpu.cartrigeLO[address & 0x1fff]; } //Cartrige Low ($8000) 
uint8_t r_crtH(uint32_t address)	{ return cpu.cartrigeHI[address & 0x1fff]; } 
uint8_t r_nul(uint32_t address)		{ return 0;} //No RAM for Ultimax-cartrige
uint8_t r_rnd(uint32_t address)		{ return 255;} //Random for $DE00-$DFFF

void w_ram( uint32_t address, uint8_t value )	{ 
    cpu.RAM[address ]=value; 
}
void w_ramz( uint32_t address, uint8_t value )	{ 
	cpu.RAM[address]=value;  //zeropage
	if (address==1) {	//6510 Port
		value &= 0x07;
		cpu.plamap_r = (rarray_t*)&PLA_READ[value];
		cpu.plamap_w = (warray_t*)&PLA_WRITE[value];
	  } 
	} 
void w_vic( uint32_t address, uint8_t value )	{ vic_write(address, value); }
void w_col( uint32_t address, uint8_t value )	{ cpu.vic.COLORRAM[address & 0x3FF] = value & 0x0F;}
#ifdef HAS_SND      
void w_sid( uint32_t address, uint8_t value )	{ playSID.setreg(address & 0x1F, value); }
#else
void w_sid( uint32_t address, uint8_t value )	{ }
#endif
void w_cia1( uint32_t address, uint8_t value )	{ cia1_write(address, value); }
void w_cia2( uint32_t address, uint8_t value )	{ cia2_write(address, value); }

/*
    LORAM (bit 0) can generally be thought of as a control line which banks
  the 8K byte BASIC ROM in and out of the microprocessor address space.
  Normally, this line is HIGH for BASIC operation. If this line is
  programmed LOW, the BASIC ROM will disappear from the memory map and be
  replaced by 8K bytes of RAM from $A000-$BFFF.

    HIRAM (bit 1) can generally be thought of as a control line which banks
  the 8K byte KERNAL ROM in and out of the microprocessor address space.
  Normally, this line is HIGH for BASIC operation. If this line is
  programmed LOW, the KERNAL ROM will disappear from the memory map and be
  replaced by 8K bytes of RAM from $E000-$FFFF.

    CHAREN (bit 2) is used only to bank the 4K byte character generator ROM
  in or out of the microprocessor address space. From the processor point
  of view, the character ROM occupies the same address space as the I/O
  devices ($D000-$DFFF). When the CHAREN line is set to 1 (as is normal),
  the I/O devices appear in the microprocessor address space, and the
  character ROM is not accessable. When the CHAREN bit is cleared to 0, the
  character ROM appears in the processor address space, and the I/O devices
  are not accessible. (The microprocessor only needs to access the
  character ROM when downloading the character set from ROM to RAM. Special
  care is needed for this... see the section on PROGRAMMABLE CHARACTERS in
  the GRAPHICS chapter). CHAREN can be overridden by other control lines in
  certain memory configurations. CHAREN will have no effect on any memory
  configuration without I/O devices. RAM will appear from $D000-$DFFF
  instead.
  */
  
  /*  
       Bit+-------------+-----------+------------+
       210| $8000-$BFFF |$D000-$DFFF|$E000-$FFFF |
  +---+---+-------------+-----------+------------+
  | 7 |111| Cart.+Basic |    I/O    | Kernal ROM |
  +---+---+-------------+-----------+------------+
  | 6 |110|     RAM     |    I/O    | Kernal ROM |
  +---+---+-------------+-----------+------------+
  | 5 |101|     RAM     |    I/O    |    RAM     |
  +---+---+-------------+-----------+------------+
  | 4 |100|     RAM     |    RAM    |    RAM     |
  +---+---+-------------+-----------+------------+
  | 3 |011| Cart.+Basic | Char. ROM | Kernal ROM |
  +---+---+-------------+-----------+------------+
  | 2 |010|     RAM     | Char. ROM | Kernal ROM |
  +---+---+-------------+-----------+------------+
  | 1 |001|     RAM     | Char. ROM |    RAM     |
  +---+---+-------------+-----------+------------+
  | 0 |000|     RAM     |    RAM    |    RAM     |
  +---+---+-------------+-----------+------------+
       |||
 /CharEn|/LoRam
        |
      /HiRam
 */	  

CONSTROM 
rarray_t PLA_READ[8] = {
	//charen hiram loram
	/* 000 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 001 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	
    /* A0 */ //  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */ //  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	
	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 010 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 011 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 100 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 101 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
		
	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 110 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 111 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker}
};

CONSTROM 
warray_t PLA_WRITE[8] = {
	/* 000 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* D0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram},
	
	/* 001 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* D0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram},	
	
	/* 010 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
    /* D0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,},	
	
	/* 011 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
    /* D0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,		
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,},	
	
	/* 100 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* D0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram},	
	
	/* 101 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* D0 */   w_vic,  w_vic,  w_vic,  w_vic,  w_sid,  w_sid,  w_sid,  w_sid,  w_col,  w_col,  w_col,  w_col,  w_cia1, w_cia2, w_ram,  w_ram,
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram},	
	
	/* 110 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* D0 */   w_vic,  w_vic,  w_vic,  w_vic,  w_sid,  w_sid,  w_sid,  w_sid,  w_col,  w_col,  w_col,  w_col,  w_cia1, w_cia2, w_ram,  w_ram,
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram},	
	
	/* 111 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {w_ramz, w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 10 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 20 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 30 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 40 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 50 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 60 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 70 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* 80 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* 90 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* A0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* B0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,

	/* C0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* D0 */   w_vic,  w_vic,  w_vic,  w_vic,  w_sid,  w_sid,  w_sid,  w_sid,  w_col,  w_col,  w_col,  w_col,  w_cia1, w_cia2, w_ram,  w_ram,
	/* E0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,
	/* F0 */   w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram,  w_ram}	
};

/*
Normal   8kB cartridge at $8000       (ROML):      GAME = 1, EXROM = 0
Normal  16kB cartridge at $8000/$a000 (ROML,ROMH): GAME = 0, EXROM = 0
Ultimax 16kB cartridge at $8000/$e000 (ROML,ROMH): GAME = 0, EXROM = 1
*/

CONSTROM 
rarray_t PLA_READ_CARTRIGE_10[8] = { 
	
	//Normal   8kB cartridge at $8000       (ROML):      GAME = 1, EXROM = 0
		
	//charen hiram loram
	/* 000 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 001 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 010 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 011 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 100 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 101 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 110 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 111 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker}
};

CONSTROM 
rarray_t PLA_READ_CARTRIGE_00[8] = { //GAME = 0 EXROM = 0

	//Normal  16kB cartridge at $8000/$a000 (ROML,ROMH): GAME = 0, EXROM = 0
	
	//charen hiram loram
	/* 000 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 001 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 010 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* B0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 011 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* B0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  r_chr,  	//chr
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 100 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* B0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 101 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,
	/* B0 */   r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,  r_bas,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* F0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram},

	/* 110 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 90 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* A0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* B0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker},

	/* 111 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 20 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 30 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 40 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 50 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 60 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 70 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* B0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,

	/* C0 */   r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_ram,  r_ram,
	/* E0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,
	/* F0 */   r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker,  r_ker}
};

CONSTROM 
rarray_t PLA_READ_CARTRIGE_01[8] = { 
	
	//Ultimax 16kB cartridge at $8000/$e000 (ROML,ROMH): GAME = 0, EXROM = 1
		
	//charen hiram loram
	/* 000 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},

	/* 001 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},

	/* 010 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},

	/* 011 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},

	/* 100 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},

	/* 101 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},

	/* 110 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},

	/* 111 */
	/*        	0   	1   	2   	3   	4   	5   	6   	7   	8	   9   		A   	B   	C   	D   	E   	F*/
	/* 00 */  {r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,  r_ram,
	/* 10 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 20 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 30 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 40 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 50 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 60 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* 70 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* 80 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* 90 */   r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL, r_crtL,
	/* A0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* B0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,

	/* C0 */   r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,  r_nul,
	/* D0 */   r_vic,  r_vic,  r_vic,  r_vic,  r_sid,  r_sid,  r_sid,  r_sid,  r_col,  r_col,  r_col,  r_col,  r_cia1, r_cia2, r_nul/*?*/,  r_nul/*?*/,
	/* E0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH,
	/* F0 */   r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH, r_crtH},
};



void resetPLA(void) {
	
	// Initialize RAM
	unsigned i = 0;
	const char pattern1 = 0x00;
	const char pattern2 = 0xff;
	const char patternLength = 0x40;
	
	while (i <= (sizeof(cpu.RAM) - patternLength * 2)) {
		memset(&cpu.RAM[i], pattern1, patternLength);
		i += patternLength;
		memset(&cpu.RAM[i], pattern2, patternLength);
		i += patternLength;
	};

	cpu.RAM[0] = 0x2F;
	cpu.RAM[1] = 0x1F;	

/* Cartriges : 
Normal   8kB cartridge at $8000       (ROML):      GAME = 1, EXROM = 0
Normal  16kB cartridge at $8000/$a000 (ROML,ROMH): GAME = 0, EXROM = 0
Ultimax 16kB cartridge at $8000/$e000 (ROML,ROMH): GAME = 0, EXROM = 1
*/
#if 1 //No Cartrige
    cpu._game = 1;
	cpu._exrom = 1;
#else //TODO...
    cpu._game = 0;
	cpu._exrom = 0;	
#endif	

	if (cpu._game == 1 && cpu._exrom==0) {
		cpu.plamap_r = (rarray_t*)&PLA_READ_CARTRIGE_10[0x07];
		cpu.plamap_w = (warray_t*)&PLA_WRITE[0x07];		
	}
	else 
	if (cpu._game == 0 && cpu._exrom==0) {
		cpu.plamap_r = (rarray_t*)&PLA_READ_CARTRIGE_00[0x07];
		cpu.plamap_w = (warray_t*)&PLA_WRITE[0x07];		
	}
	else 
	if (cpu._game == 0 && cpu._exrom==1) {
		cpu.plamap_r = (rarray_t*)&PLA_READ_CARTRIGE_00[0x07];
		cpu.plamap_w = (warray_t*)&PLA_WRITE[0x07];		
	}	
	else { //C64 without Cartridge
		cpu.plamap_r = (rarray_t*)&PLA_READ[0x07];
		cpu.plamap_w = (warray_t*)&PLA_WRITE[0x07];
	}
}
