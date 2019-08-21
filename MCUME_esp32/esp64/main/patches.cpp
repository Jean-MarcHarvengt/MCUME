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

#include "patches.h"


#define DIRECTORY ROMSDIR + "/\0"

static char filename[64];
static char buffer[2];

extern char * menuSelection(void);

void patchLOAD(void) {

int device;
int secondaryAddress;
uint16_t addr,size;

	//printf("Patched LOAD\n");
	device = cpu.RAM[0xBA];
	if (device != 1) {
		//Jump to unpatched original address:
		cpu.pc = rom_kernal[cpu.pc - 0xe000 + 1] * 256 + rom_kernal[cpu.pc - 0xe000];
		return;
	};


#if XXX
	if (cpu.RAM[cpu.RAM[0xBC] * 256 + cpu.RAM[0xBB]] == '$' && cpu.RAM[0xB7] == 1) {
		//Directoy listing with LOAD "$"
		printf("Listing of ");
		printf(DIRECTORY);
		printf("\n");
		file = SD.open(DIRECTORY);
		int blocks, start, len;
		addr = cpu.RAM[0x2C] * 256 + cpu.RAM[0x2B];

		/*first line of BASIC listing */
		start = addr;
		cpu.RAM[addr++] = (start + 30) & 0xff;
		cpu.RAM[addr++] = (start + 30) >> 8;
		blocks = 0;
		cpu.RAM[addr++] = blocks & 0xff;
		cpu.RAM[addr++] = blocks >> 8;

		const char title[] = "\x12\"TEENSY64        \" FB " VERSION;
		strcpy((char * )&cpu.RAM[addr], title);
		addr = start + 30;

		while (true) {
			entry = file.openNextFile();
			if (! entry) {
				// no more files
				break;
			}
			int offset;
			if (!entry.isDirectory()) {

				/* Listing to BASIC-RAM */
				start = addr;
				offset = 0;

				//pointer to next line:
				cpu.RAM[addr++] = (start + 32) & 0xff;
				cpu.RAM[addr++] = (start + 32) >> 8;

				//# of blocks
				blocks = ceil((float)entry.size()/256.0f);
				cpu.RAM[addr++] = blocks & 0xff;
				cpu.RAM[addr++] = blocks >> 8;

				if (blocks < 100)   { cpu.RAM[addr++] = ' '; offset++;}
				if (blocks < 10)    { cpu.RAM[addr++] = ' '; offset++; }
			 	cpu.RAM[addr++] = ' ';

				//filename:
				cpu.RAM[addr++] = '"';
				char *s = (char * )&cpu.RAM[addr];
				entry.getName(s, 17);
				while(*s) {*s = toupper(*s); s++;}
				//strcpy((char * )&cpu.RAM[addr], entry.name());
				len = strlen((char * )&cpu.RAM[addr]);

				if (len > 16) len = 16;
				addr += len;
				cpu.RAM[addr++] = '"';

				//fill with space
				while ((addr-start) < (32)) { cpu.RAM[addr++] = ' ';}

				//display "PRG"
				addr = start + 23 + offset;
				cpu.RAM[addr++] = ' ';
				cpu.RAM[addr++] = 'P';
				cpu.RAM[addr++] = 'R';
				cpu.RAM[addr++] = 'G';

				//line-ending
				cpu.RAM[start+31] = 0;
				addr = start + 32;

				/* Listing to serial console */
				itoa (blocks,filename,10);
				len = strlen(filename);
				while (len < 4) { strcat(filename," "); len++; };
				strcat(filename, "\"");
				char nbuf[18] = {0};
				entry.getName(nbuf, 17);
				strcat(filename, nbuf);
				//strcat(filename, entry.getName());
				strcat(filename, "\"");
				len = strlen(filename);
				while (len < 18+4) { strcat(filename," "); len++; };
				strcat(filename," PRG   ");
				//printf(filename);

			}
			entry.close();
		}
		file.close();

        /*add last line to BASIC listing*/
		start = addr;
		cpu.RAM[addr++] = (start + 32) & 0xff;
		cpu.RAM[addr++] = (start + 32) >> 8;
		//# of blocks. todo : determine free space on sd card
		blocks = 65535;
		cpu.RAM[addr++] = blocks & 0xff;
		cpu.RAM[addr++] = blocks >> 8;
		if (blocks < 100) { cpu.RAM[addr++] = ' ';}
		if (blocks < 10)  { cpu.RAM[addr++] = ' ';}
		const char blockfree[] = "BLOCKS FREE.";

		strcpy((char * )&cpu.RAM[addr], blockfree);
		len = strlen(blockfree);
		addr += len;
		while ((addr-start) < (32)) { cpu.RAM[addr++] = ' ';}
		cpu.RAM[start+31] = 0;
		cpu.RAM[start+32] = 0;
		cpu.RAM[start+33] = 0;

		cpu.y = 0x49; //Offset for "LOADING"
		cpu.pc = 0xF12B; //Print and return
		return;
	} // end directory listing
#endif

	//$B7    : Length of file name or disk command
	//$BB-$BC: Pointer to current file name or disk command
    memset(filename,0,sizeof(filename));
    if ( cpu.RAM[0xB7] == 0) {
		strcpy(filename,menuSelection());
    }
    else {
		strncpy(filename, (char*)&cpu.RAM[cpu.RAM[0xBC] * 256 + cpu.RAM[0xBB]], cpu.RAM[0xB7] );
    }     
	secondaryAddress = cpu.RAM[0xB9];

	printf("%s,%d,%d:", filename, device, secondaryAddress);


	if (emu_FileOpen(filename) == 0) {
		printf("not found.\n");
		cpu.pc = 0xf530; //Jump to $F530
		return;
	}

	size = emu_FileSize(filename);
	emu_FileOpen(filename);
	emu_FileRead(buffer, 2);
	addr = buffer[1] * 256 + buffer[0];
	emu_FileRead((char*)&cpu.RAM[addr], size - 2);
	emu_FileClose();

	cpu.RAM[0xAF] = (addr + size - 2) & 0xff;
	cpu.RAM[0xAE] = (addr + size - 2) / 256;

	cpu.y = 0x49; //Offset for "LOADING"
	cpu.pc = 0xF12B; //Print and return
	printf("loaded.\n");

	return;
}

void patchSAVE(void) {
#ifdef XXX
int device;
int secondaryAddress;
uint16_t addr,size;

	Serial.println("Patched SAVE");
	device = cpu.RAM[0xBA];
	if (device != 1) {
		//Jump to unpatched original address:
		cpu.pc = rom_kernal[cpu.pc - 0xe000 + 1] * 256 + rom_kernal[cpu.pc - 0xe000];
		return;
	};

	if (!SDinitialized) {
		cpu.pc = 0xF707; //Device not present error
		Serial.println("SD Card not initialized");
		return;
	}

	if( !SD.exists(DIRECTORY) && SD.mkdir(DIRECTORY) ) {
		cpu.pc = 0xF707; //Device not present error
		Serial.println("SD: Could not create " DIRECTORY);
	}

	//$B7    : Length of file name or disk command
	//$BB-$BC: Pointer to current file name or disk command
	memset(filename,0,sizeof(filename));
	strcpy(filename, DIRECTORY);
	strncat(filename, (char*)&cpu.RAM[cpu.RAM[0xBC] * 256 + cpu.RAM[0xBB]], cpu.RAM[0xB7] );

	secondaryAddress = cpu.RAM[0xB9];

	Serial.print(filename);
	Serial.print(",");
	Serial.print(device);
	Serial.print(",");
	Serial.print(secondaryAddress);
	Serial.print(":");

	addr = cpu.RAM[cpu.a + 1] * 256 + cpu.RAM[cpu.a];
	size = (cpu.y * 256 + cpu.x) - addr;

	buffer[0] = addr & 0xff;
	buffer[1] = addr >> 8;

	if (SD.exists(filename)) SD.remove(filename);
	file = SD.open(filename, FILE_WRITE);
	if (!file) {
		Serial.println ("not possible.");
		cpu.pc = 0xf530; //Jump to $F530
		return;
	}
	file.write(buffer, 2);
	file.write(&cpu.RAM[addr], size);
	file.close();

	if (cpu.RAM[0x9D] & 128) {
		uint16_t pushval = 0xF68D;
		cpu.RAM[BASE_STACK + cpu.sp] = (pushval >> 8) & 0xFF;
		cpu.RAM[BASE_STACK + ((cpu.sp - 1) & 0xFF)] = pushval & 0xFF;
		cpu.sp -= 2;

		cpu.y = 0x51;
		cpu.pc = 0xF12F;
	} else {
		cpu.pc = 0xF68D;
	}

	Serial.println("saved.");
	return;
#endif	
}
