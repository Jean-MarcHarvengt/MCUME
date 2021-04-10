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

/* choose your "display"-type */

#if 1 // color display (default)

// MACROS moved to vic implementations

#elif 0 // B&W TV for real retro feeling. Looks great (ILI 9341 only)
#define PALETTE(r,g,b) ( ((((int)( 0.299f * r + 0.587f * g + 0.114f * b )) & 0xF8) << 8 ) | \
						( (((int) ( 0.299f * r + 0.587f * g + 0.114f * b )) & 0xFC) << 3 ) | \
						( (((int) ( 0.299f * r + 0.587f * g + 0.114f * b )) & 0xFF) >> 3 ))


#elif 0 // green display (ILI 9341 only)
#define PALETTE(r,g,b) ( ( 0 ) | \
						( (((int) ( 0.299f * r + 0.587f * g + 0.114f * b )) & 0xFC) << 3 ) | \
						( 0 ))
// TODO: amber display
#endif



/* chose one of these palettes:
   VGA is 256 colors only (rrrgggbb) , subtle differences might not be visible
*/

#if 1 // "Deekay" (default)
static const uint16_t palette[16] = {
	PALETTE(0x00,0x00,0x00), PALETTE(0xff,0xff,0xff), PALETTE(0x88,0x20,0x00), PALETTE(0x68,0xd0,0xa8), // black, white, red, cyan,
	PALETTE(0xa8,0x38,0xa0), PALETTE(0x50,0xb8,0x18), PALETTE(0x18,0x10,0x90), PALETTE(0xf0,0xe8,0x58), // purple, green, blue, yellow
	PALETTE(0xa0,0x48,0x00), PALETTE(0x47,0x2b,0x1b), PALETTE(0xc8,0x78,0x70), PALETTE(0x48,0x48,0x48), // orange, brown, lightred, grey1,
	PALETTE(0x80,0x80,0x80), PALETTE(0x98,0xff,0x98), PALETTE(0x50,0x90,0xd0), PALETTE(0xb8,0xb8,0xb8)  // grey2, lightgreen, lightblue, grey3
};

#elif 0 // VICE vice.vpl
static const uint16_t palette[16] = {
	PALETTE(0x00,0x00,0x00), PALETTE(0xFD,0xFE,0xFC), PALETTE(0xBE,0x1a,0x24), PALETTE(0x30,0xe6,0xc6), // black, white, red, cyan,
	PALETTE(0xb4,0x1a,0xe2), PALETTE(0x1f,0xd2,0x1e), PALETTE(0x21,0x1b,0xae), PALETTE(0xdf,0xf6,0x0a), // purple, green, blue, yellow
	PALETTE(0xb8,0x41,0x04), PALETTE(0x6a,0x33,0x04), PALETTE(0xfe,0x4a,0x57), PALETTE(0x42,0x45,0x40), // orange, brown, lightred, grey1,
	PALETTE(0x70,0x74,0x6f), PALETTE(0x59,0xfe,0x59), PALETTE(0x5f,0x53,0xfe), PALETTE(0xa4,0xa7,0xa2)  // grey2, lightgreen, lightblue, grey3
};


#elif 0 // "PEPTO" http://www.pepto.de/projects/colorvic/2001/
static const uint16_t palette[16] = {
  0x0000, 0xFFFF, 0x69A5, 0x7536, 0x69F0, 0x5C68, 0x314F, 0xBE2D,// black, white, red, cyan, purple, green, blue, yellow
  0x6A64, 0x41C0, 0x9B2B, 0x4228, 0x6B6D, 0x9E90, 0x6AF6, 0x94B2 // orange, brown, lightred, grey1, grey2, lightgreen, lightblue, grey3
};


#elif 0 // "GODOT" http://www.godot64.de/german/hpalet.htm
static const uint16_t palette[16] = {
	PALETTE(0x00,0x00,0x00), PALETTE(0xff,0xff,0xff), PALETTE(0x88,0x00,0x00), PALETTE(0xaa,0xff,0xee), // black, white, red, cyan,
	PALETTE(0xcc,0x44,0xcc), PALETTE(0x00,0xcc,0x55), PALETTE(0x00,0x00,0xaa), PALETTE(0xee,0xee,0x77), // purple, green, blue, yellow
	PALETTE(0xdd,0x88,0x55), PALETTE(0x66,0x44,0x00), PALETTE(0xff,0x77,0x77), PALETTE(0x33,0x33,0x33), // orange, brown, lightred, grey1,
	PALETTE(0x77,0x77,0x77), PALETTE(0xaa,0xff,0x66), PALETTE(0x00,0x88,0xff), PALETTE(0xbb,0xbb,0xbb)  // grey2, lightgreen, lightblue, grey3
};


#elif 0 // "FRODO"
static const uint16_t palette[16] = {
	PALETTE(0x00,0x00,0x00), PALETTE(0xff,0xff,0xff), PALETTE(0xcc,0x00,0x00), PALETTE(0x00,0xff,0xcc), // black, white, red, cyan,
	PALETTE(0xff,0x00,0xff), PALETTE(0x00,0xcc,0x00), PALETTE(0x00,0x00,0xcc), PALETTE(0xff,0xff,0x00), // purple, green, blue, yellow
	PALETTE(0xff,0x88,0x00), PALETTE(0x88,0x44,0x00), PALETTE(0xff,0x88,0x88), PALETTE(0x44,0x44,0x44), // orange, brown, lightred, grey1,
	PALETTE(0x88,0x88,0x88), PALETTE(0x88,0xff,0x88), PALETTE(0x88,0x88,0xff), PALETTE(0xcc,0xcc,0xcc)  // grey2, lightgreen, lightblue, grey3
};


#elif 0 //RGB (full saturated colors - only good for testing)
static const uint16_t palette[16] = {
	PALETTE(0x00,0x00,0x00), PALETTE(0xff,0xff,0xff), PALETTE(0xff,0x00,0x00), PALETTE(0x00,0xff,0xff), // black, white, red, cyan,
	PALETTE(0xff,0x00,0xff), PALETTE(0x00,0xff,0x00), PALETTE(0x00,0x00,0xff), PALETTE(0xff,0xff,0x00), // purple, green, blue, yellow
	PALETTE(0xff,0x80,0x00), PALETTE(0x80,0x40,0x00), PALETTE(0xff,0x80,0x80), PALETTE(0x40,0x40,0x40), // orange, brown, lightred, grey1,
	PALETTE(0x80,0x80,0x80), PALETTE(0x80,0xff,0x80), PALETTE(0x80,0x80,0xff), PALETTE(0xc0,0xc0,0xc0)  // grey2, lightgreen, lightblue, grey3
};
#endif

#undef BW
