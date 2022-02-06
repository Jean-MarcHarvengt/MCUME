#include <ctype.h> // isgraph
#include <stdlib.h> // calloc
#include <string.h> // strlen
#include "appledisplay.h"
#include "applemmu.h" // for switch constants

#include "font.h"

/* Fourpossible Hi-Res color-drawing modes..
   MONOCHROME: show all the pixels, but only in green;
   BLACKANDWHITE: monochrome, but use B&W instead of B&G;
   NTSCLIKE: reduce the resolution to 140 pixels wide, similar to how an NTSC monitor would blend it
   PERFECTCOLOR: as the Apple RGB monitor shows it, which means you can't have a solid color field
*/



#define extendDirtyRect(x,y) {    \
    if (dirtyRect.left > x) {     \
      dirtyRect.left = x;         \
    }                             \
    if (dirtyRect.right < x) {    \
      dirtyRect.right = x;        \
    }                             \
    if (dirtyRect.top > y) {      \
      dirtyRect.top = y;          \
    }                             \
    if (dirtyRect.bottom < y) {   \
      dirtyRect.bottom = y;       \
    }                             \
}

#if DISPLAYRUN == 512

#define drawPixel(c, x, y) {                                     \
    uint16_t idx = (((y) << 9) + (x)) >> 1;			 \
    if ((x) & 1) {                                               \
      videoBuffer[idx] = (videoBuffer[idx] & 0xF0) | (c);	 \
    } else {                                                     \
      videoBuffer[idx] = (videoBuffer[idx] & 0x0F) | ((c) << 4); \
    }                                                            \
  }

#define draw2Pixels(cAB, x, y) { \
    videoBuffer[(((y) <<9) + (x)) >> 1] = cAB;	\
  }


#else

#define drawPixel(c, x, y) {                                     \
    uint16_t idx = ((y) * DISPLAYRUN + (x)) / 2;	         \
    if ((x) & 1) {                                               \
      videoBuffer[idx] = (videoBuffer[idx] & 0xF0) | (c);	 \
    } else {                                                     \
      videoBuffer[idx] = (videoBuffer[idx] & 0x0F) | ((c) << 4); \
    }                                                            \
  }

#define draw2Pixels(cAB, x, y) { \
    videoBuffer[((y) * DISPLAYRUN + (x)) /2] = cAB; \
  }

#endif


#define DrawLoresPixelAt(c, x, y) {     \
  uint8_t pixel = c & 0x0F;             \
  for (uint8_t y2 = 0; y2<4; y2++) {    \
    for (int8_t x2 = 6; x2>=0; x2--) {  \
      drawPixel(pixel, x*7+x2, y*8+y2); \
    }                                   \
  }                                     \
  pixel = (c >> 4);                     \
  for (uint8_t y2 = 4; y2<8; y2++) {    \
    for (int8_t x2 = 6; x2>=0; x2--) {  \
      drawPixel(pixel, x*7+x2, y*8+y2); \
    }                                   \
  }                                     \
}

#include "globals.h"

AppleDisplay::AppleDisplay(uint8_t *vb) : VMDisplay(vb)
{
  this->switches = NULL;
  this->dirty = true;
  this->dirtyRect.left = this->dirtyRect.top = 0;
  this->dirtyRect.right = 279;
  this->dirtyRect.bottom = 191;

  textColor = g_displayType == m_monochrome?c_green:c_white;
}

AppleDisplay::~AppleDisplay()
{
}

bool AppleDisplay::deinterlaceAddress(uint16_t address, uint8_t *row, uint8_t *col)
{
  if (address >= 0x800 && address < 0xC00) {
    address -= 0x400;
  }

  uint8_t block = (address >> 7) - 0x08;
  uint8_t blockOffset = (address & 0x00FF) - ((block & 0x01) ? 0x80 : 0x00);
  if (blockOffset < 0x28) {
    *row = block;
    *col = blockOffset;
  } else if (blockOffset < 0x50) {
    *row = block + 8;
    *col = blockOffset - 0x28;
  } else {
    *row = block + 16;
    *col = blockOffset - 0x50;
  }

  return true;
}

// calculate x/y pixel offsets from a memory address.
// Note that this is the first of 7 pixels that will be affected by this write;
// we'll need to update all 7 starting at this x.
bool AppleDisplay::deinterlaceHiresAddress(uint16_t address, uint8_t *row, uint16_t *col)
{
  // each row is 40 bytes, for 7 pixels each, totalling 128
  // pixels wide.
  // They are grouped in to 3 "runs" of 40-byte blocks, where 
  // each group is 64 lines after the one before.

  // Then repeat at +400, +800, +c00, +1000, +1400, +1800, +1c00 for
  // the other 7 pixels tall.

  // Repeat the whole shebang at +0x80, +0x100, +0x180, ... to +280
  // for each 8-pixel tall group.

  // There are 8 bytes at the end of each run that we ignore. Skip them.
  if ((address & 0x07f) >= 0x78 &&
      (address & 0x7f) <= 0x7f) {
    *row = 255;
    *col = 65535;
    return false;
  }

  *row = ((address & 0x380) >> 4) +
    ((address & 0x1c00)>>10) + 
    64 * ((address & 0x7f) / 40);

  *col = ((address & 0x7f) % 40) * 7;

  return true;
}

// return a pointer to the right glyph, and set *invert appropriately
const unsigned char *AppleDisplay::xlateChar(uint8_t c, bool *invert)
{
  if (c <= 0x3F) {
    // 0-3f: inverted @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_ !"#$%&'()*+,-./0123456789:;<=>?
    // (same w/o mousetext, actually)
    *invert = true;
    return &ucase_glyphs[c * 8];
  } else if (c <= 0x5F) {
    // 40-5f: normal mousetext
    // (these are flashing @ABCDEFG..[\]^_ when not in mousetext mode)
    if ((*switches) & S_ALTCH) {
      *invert = false;
      return &mousetext_glyphs[(c - 0x40) * 8];
    } else {
      *invert = true;
      return &ucase_glyphs[(c - 0x40) * 8];
    }
  } else if (c <= 0x7F) {
    // 60-7f: inverted   `abcdefghijklmnopqrstuvwxyz{|}~*
    // (these are flashing (sp)!"#$%...<=>? when not in mousetext)
    if ((*switches) & S_ALTCH) {
      *invert = true;
      return &lcase_glyphs[(c - 0x60) * 8];
    } else {
      *invert = true;
      return &ucase_glyphs[((c-0x60) + 0x20) * 8];
    }
  } else if (c <= 0xBF) {
    // 80-BF: normal @ABCD... <=>? in both character sets
    *invert = false;
    return &ucase_glyphs[(c - 0x80) * 8];
  } else if (c <= 0xDF) {
    // C0-DF: normal @ABCD...Z[\]^_ in both character sets
    *invert = false;
    return &ucase_glyphs[(c - 0xC0) * 8];
  } else {
    // E0-  : normal `abcdef... in both character sets
    *invert = false;
    return &lcase_glyphs[(c - 0xE0) * 8];
  }

  /* NOTREACHED */
}
  
inline void AppleDisplay::Draw14DoubleHiresPixelsAt(uint16_t addr)
{
  // We will consult 4 bytes (2 in main, 2 in aux) for any single-byte
  // write. Align to the first byte in that series based on what
  // address we were given...
  addr &= ~0x01;

  // Figure out the position of that address on the "normal" hires screen
  uint8_t row;
  uint16_t col;
  deinterlaceHiresAddress(addr, &row, &col);
  if (row >= 160 && 
      ((*switches) & S_MIXED)) {
    // displaying text, so don't have to draw this line
    return;
  }

  // Make sure it's a valid graphics area, not a dead hole
  if (col <= 280 && row <= 192) {
    // Grab the 4 bytes we care about
    uint8_t b1A = mmu->readDirect(addr, 0);
    uint8_t b2A = mmu->readDirect(addr+1, 0);
    uint8_t b1B = mmu->readDirect(addr, 1);
    uint8_t b2B = mmu->readDirect(addr+1, 1);

    // Construct the 28 bit wide bitstream, like we do for the simpler 14 Hires pixel draw
    uint32_t bitTrain = b2A & 0x7F;
    bitTrain <<= 7;
    bitTrain |= (b2B & 0x7F);
    bitTrain <<= 7;
    bitTrain |= (b1A & 0x7F);
    bitTrain <<= 7;
    bitTrain |= (b1B & 0x7F);
    
    // Now we pop groups of 4 bits off the bottom and draw our
    // NTSC-style-only color.  The display for this project only has
    // 320 columns, so it's silly to try to do 560 columns of
    // monochrome; and likewise, we can't do "perfect" representation
    // of shifted color pixels. So NTSC it is, and we'll draw two screen 
    // pixels for every color.

    for (int8_t xoff = 0; xoff < 14; xoff += 2) {
      drawPixel(bitTrain & 0x0F, col+xoff, row);
      drawPixel(bitTrain & 0x0F, col+xoff+1, row);

      bitTrain >>= 4;
    }
  }
}


// Whenever we change a byte, it's possible that it will have an affect on the byte next to it - 
// because between two bytes there is a shared bit.
// FIXME: what happens when the high bit of the left doesn't match the right? Which high bit does 
// the overlap bit get?
inline void AppleDisplay::Draw14HiresPixelsAt(uint16_t addr)
{
  uint8_t row;
  uint16_t col;

  deinterlaceHiresAddress(addr, &row, &col);
  if (row >= 160 && 
      ((*switches) & S_MIXED)) {
    return;
  }

  if (col <= 280 && row <= 192) {
    /*
      The high bit only selects the color palette.

      There are only really two bits here, and they can be one of six colors.
      
      color    highbit even    odd    restriction
      black       x      0x80,0x00
      green       0    0x2A    0x55    odd only
      violet      0    0x55    0x2A    even only
      white       x      0xFF,0x7F    
      orange      1    0xAA    0xD5    odd only
      blue        1    0xD5    0xAA    even only

      in other words, we can look at the pixels in pairs and we get

      00 black
      01 green/orange
      10 violet/blue
      11 white

      When the horizontal byte number is even, we ignore the last
      bit. When the horizontal byte number is odd, we use that dropped
      bit.

      So each even byte turns in to 3 bits; and each odd byte turns in
      to 4. Our effective output is therefore 140 pixels (half the 
      actual B&W resolution).

      (Note that I swap 0x02 and 0x01 below, because we're running the
      bit train backward, so the bits are reversed.)
     */

    uint8_t b1 = mmu->read(addr);
    uint8_t b2 = mmu->read(addr+1);

    // Used for color modes...
    bool highBitOne = (b1 & 0x80);
    bool highBitTwo = (b2 & 0x80);

    uint16_t bitTrain = (b1 & 0x7F) | ((b2 & 0x7F) << 7);

    for (int8_t xoff = 0; xoff < 14; xoff += 2) {

      if (g_displayType == m_monochrome) {
	draw2Pixels(((bitTrain & 0x01 ? c_green : c_black) << 4) |
		    (bitTrain & 0x02 ? c_green : c_black),
		    col+xoff, row);
      } else if (g_displayType == m_blackAndWhite) {
	draw2Pixels(((bitTrain & 0x01 ? c_white : c_black) << 4) |
		    (bitTrain & 0x02 ? c_white : c_black),
		    col+xoff, row);
      } else if (g_displayType == m_ntsclike) {
	// Use the NTSC-like color mode, where we're only 140 pixels wide.
	
	bool highBitSet = (xoff >= 7 ? highBitTwo : highBitOne);
	uint8_t color;
	switch (bitTrain & 0x03) {
	case 0x00:
	  color = c_black;
	  break;
	case 0x02:
	  color = (highBitSet ? c_orange : c_green);
	  break;
	case 0x01:
	  color = (highBitSet ? c_medblue : c_purple);
	  break;
	case 0x03:
	  color = c_white;
	  break;
	}
	
	draw2Pixels( (color << 4) | color, col+xoff, row );
      } else {
	// Use the "perfect" color mode, like the Apple RGB monitor showed.
	bool highBitSet = (xoff >= 7 ? highBitTwo : highBitOne);
	uint8_t color;
	switch (bitTrain & 0x03) {
	case 0x00:
	  color = c_black;
	  break;
	case 0x02:
	  color = (highBitSet ? c_orange : c_green);
	  break;
	case 0x01:
	  color = (highBitSet ? c_medblue : c_purple);
	  break;
	case 0x03:
	  color = c_white;
	  break;
	}
	
	uint16_t twoColors;
	
	if (color == c_black || color == c_white || bitTrain & 0x01) {
	  twoColors = color;
	} else {
	  twoColors = c_black;
	}
	twoColors <<= 4;
	
	if (color == c_black || color == c_white || bitTrain & 0x02) {
	  twoColors |= color;
	} else {
	  twoColors |= c_black;
	}
	draw2Pixels(twoColors, col+xoff, row);
      }
      bitTrain >>= 2;
    }
  }
}

void AppleDisplay::redraw80ColumnText(uint8_t startingY)
{
  uint8_t row, col;
  col = -1; // will force us to deinterlaceAddress()
  bool invert;
  const uint8_t *cptr;

  // FIXME: is there ever a case for 0x800, like in redraw40ColumnText?
  uint16_t start = 0x400;

  // Every time through this loop, we increment the column. That's going to be correct most of the time.
  // Sometimes we'll get beyond the end (40 columns), and wind up on another line 8 rows down.
  // Sometimes we'll get beyond the end, and we'll wind up in unused RAM.
  // But this is an optimization (for speed) over just calling DrawCharacter() for every one.
  for (uint16_t addr = start; addr <= start + 0x3FF; addr++,col++) {
    if (col > 39 || row > 23) {
      // Could be blanking space; we'll try to re-confirm...
      deinterlaceAddress(addr, &row, &col);      
    }

    // Only draw onscreen locations
    if (row >= startingY && col <= 39 && row <= 23) {
      // Even characters are in bank 0 ram. Odd characters are in bank
      // 1 ram.  Technically, this would need 560 columns to work
      // correctly - and I don't have that, so it's going to be a bit
      // wonky.
      // 
      // First pass: draw two pixels on top of each other, clearing
      // only if both are black. This would be blocky but probably
      // passable if it weren't for the fact that characters are 7
      // pixels wide, so we wind up sharing a half-pixel between two
      // characters. So we'll render these as 3-pixel-wide characters
      // and make sure they always even-align the drawing on the left
      // side so we don't overwrite every other one on the left or
      // right side.

      // Draw the first of two characters
      cptr = xlateChar(mmu->readDirect(addr, 1), &invert);
      for (uint8_t y2 = 0; y2<8; y2++) {
	uint8_t d = *(cptr + y2);
	for (uint8_t x2 = 0; x2 <= 7; x2+=2) {
	  uint16_t basex = ((col * 2) * 7) & 0xFFFE; // even aligned
	  bool pixelOn = ( (d & (1<<x2)) | (d & (1<<(x2+1))) );
	  if (pixelOn) {
	    uint8_t val = (invert ? c_black : textColor);
	    drawPixel(val, (basex+x2)/2, row*8+y2);
	  } else {
	    uint8_t val = (invert ? textColor : c_black);
	    drawPixel(val, (basex+x2)/2, row*8+y2);
	  }
	}
      }

      // Draw the second of two characters
      cptr = xlateChar(mmu->readDirect(addr, 0), &invert);
      for (uint8_t y2 = 0; y2<8; y2++) {
	uint8_t d = *(cptr + y2);
	for (uint8_t x2 = 0; x2 <= 7; x2+=2) {
	  uint16_t basex = ((col * 2 + 1) * 7) & 0xFFFE; // even aligned -- +1 for the second character
	  bool pixelOn = ( (d & (1<<x2)) | (d & (1<<(x2+1))) );
	  if (pixelOn) {
	    uint8_t val = (invert ? c_black : textColor);
	    drawPixel(val, (basex+x2)/2, row*8+y2);
	  } else {
	    uint8_t val = (invert ? textColor : c_black);
	    drawPixel(val, (basex+x2)/2, row*8+y2);
	  }
	}
      }
    }
  }
}

void AppleDisplay::redraw40ColumnText(uint8_t startingY)
{
  bool invert;

  uint16_t start = ((*switches) & S_PAGE2) ? 0x800 : 0x400;
  uint8_t row, col;
  col = -1; // will force us to deinterlaceAddress()
  
  // Every time through this loop, we increment the column. That's going to be correct most of the time.
  // Sometimes we'll get beyond the end (40 columns), and wind up on another line 8 rows down.
  // Sometimes we'll get beyond the end, and we'll wind up in unused RAM.
  // But this is an optimization (for speed) over just calling DrawCharacter() for every one.
  for (uint16_t addr = start; addr <= start + 0x3FF; addr++,col++) {
    if (col > 39 || row > 23) {
      // Could be blanking space; we'll try to re-confirm...
      deinterlaceAddress(addr, &row, &col);      
    }

    // Only draw onscreen locations
    if (row >= startingY && col <= 39 && row <= 23) {
      const uint8_t *cptr = xlateChar(mmu->read(addr), &invert);
      
      for (uint8_t y2 = 0; y2<8; y2++) {
	uint8_t d = *(cptr + y2);
	for (uint8_t x2 = 0; x2 < 7; x2++) {
	  if (d & 1) {
	    uint8_t val = (invert ? c_black : textColor);
	    drawPixel(val, col*7+x2, row*8+y2);
	  } else {
	    uint8_t val = (invert ? textColor : c_black);
	    drawPixel(val, col*7+x2, row*8+y2);
	  }
	  d >>= 1;
	}
      }
    }
  }
}

void AppleDisplay::redrawHires()
{
  uint16_t start = ((*switches) & S_PAGE2) ? 0x4000 : 0x2000;
  if ((*switches) & S_80STORE) {
    // Apple IIe, technical nodes #3: 80STORE must be OFF to display Page 2
    start = 0x2000;
  }

  // FIXME: check MIXED & don't redraw the lower area if it's set
  for (uint16_t addr = start; addr <= start + 0x1FFF; addr+=2) {
    if ((*switches) & S_DHIRES) {
      // FIXME: inline & optimize
      Draw14DoubleHiresPixelsAt(addr);
    } else {
      // FIXME: inline & optimize
      Draw14HiresPixelsAt(addr);
    }
  }
}

void AppleDisplay::redrawLores()
{
  // FIXME: can make more efficient by checking S_MIXED for lower bound
  
  if (((*switches) & S_80COL) && ((*switches) & S_DHIRES)) {
    for (uint16_t addr = 0x400; addr <= 0x400 + 0x3ff; addr++) {
      uint8_t row, col;
      deinterlaceAddress(addr, &row, &col);
      if (col <= 39 && row <= 23) {
	Draw80LoresPixelAt(mmu->readDirect(addr, 0), col, row, 1);
	Draw80LoresPixelAt(mmu->readDirect(addr, 1), col, row, 0);
      }
    }
  } else {
    uint16_t start = ((*switches) & S_PAGE2) ? 0x800 : 0x400;
    for (uint16_t addr = start; addr <= start + 0x3FF; addr++) {
      uint8_t row, col;
      deinterlaceAddress(addr, &row, &col);
      if (col <= 39 && row <= 23) {
	DrawLoresPixelAt(mmu->read(addr), col, row);
      }
    }
  }
}

void AppleDisplay::modeChange()
{
  dirty = true;
}

void AppleDisplay::Draw80LoresPixelAt(uint8_t c, uint8_t x, uint8_t y, uint8_t offset)
{
  // Just like 80-column text, this has a minor problem; we're taking
  // a 7-pixel-wide space and dividing it in half. Here I'm drawing
  // every other column 1 pixel narrower (the ">= offset" in the for
  // loop condition).
  //
  // Make those ">= 0" and change the "*7" to "*8" and you've got
  // 320-pixel-wide slightly distorted but cleaner double-lores...

  if (!offset) {
    // The colors in every other column are swizzled. Un-swizzle.
    c = ((c & 0x77) << 1) | ((c & 0x88) >> 3);
  }
  uint8_t pixel = c & 0x0F;
  for (uint8_t y2 = 0; y2<4; y2++) {
    for (int8_t x2 = 3; x2>=offset; x2--) {
      drawPixel(pixel, x*7+x2+offset*3, y*8+y2);
    }
  }

  pixel = (c >> 4);
  for (uint8_t y2 = 4; y2<8; y2++) {
    for (int8_t x2 = 3; x2>=offset; x2--) {
      drawPixel(pixel, x*7+x2+offset*3, y*8+y2);
    }
  }
}

void AppleDisplay::setSwitches(uint16_t *switches)
{
  dirty = true;
  dirtyRect.left = 0;
  dirtyRect.right = 279;
  dirtyRect.top = 0;
  dirtyRect.bottom = 191;

  this->switches = switches;
}

AiieRect AppleDisplay::getDirtyRect()
{
  return dirtyRect;
}

bool AppleDisplay::needsRedraw()
{
  if (dirty) {
    // Figure out what graphics mode we're in and redraw it in its entirety.

    if ((*switches) & S_TEXT) {
      if ((*switches) & S_80COL) {
	redraw80ColumnText(0);
      } else {
	redraw40ColumnText(0);
      }
      
      return true;
    }

    // Not text mode - what mode are we in?
    if ((*switches) & S_HIRES) {
      redrawHires();
    } else {
      redrawLores();
    }

    // Mixed graphics modes: draw text @ bottom
    if ((*switches) & S_MIXED) {
      if ((*switches) & S_80COL) {
	redraw80ColumnText(20);
      } else {
	redraw40ColumnText(20);
      }
    }
  }

  return dirty;
}

void AppleDisplay::didRedraw()
{
  dirty = false;
}

void AppleDisplay::displayTypeChanged()
{
  textColor = g_displayType == m_monochrome?c_green:c_white;
}
