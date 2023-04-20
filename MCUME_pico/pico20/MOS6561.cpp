#include "MOS6561.h"

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}

#include "pico_dsp.h"
typedef uint16_t Pixel;

#define WIN_W TFT_WIDTH
#define WIN_H TFT_HEIGHT


// definitions for easy access to registers
#define REG_COLPAGE_BASE      (0x9400) // 0x9400+0x200 = 0x9600 unexpanded

#define REG_SCRPAGE_LO        ((readWord(0x9002) & 0x80)<< 2) // 0x0200
#define REG_COLPAGE_LO        ((readWord(0x9002) & 0x80)<< 2) // 0x0200
#define REG_NB_COLUMNS        (readWord(0x9002) & 0x7F)

#define REG_RASTER_LO_RD      (0x9003) // 0x80 >> 7
#define REG_DOUBLE_HEIGHT     (readWord(0x9003) & 0x01)
#define REG_NB_ROWS           ((readWord(0x9003) & 0x7E) >> 1) //x2 => 46

#define REG_RASTER_HI_RD      (0x9004) // << 1 // 1E00 unexpanded?

#define REG_CHRMAP_PT         (readWord(0x9005) & 0x0F)
#define REG_SCRPAGE_HIGH      ((readWord(0x9005) & 0xF0) << 6) // 0x2C00

#define REG_AUXILIARY_COLOUR  ((readWord(0x900E) & 0xF0) >> 4)
#define REG_BORDER_COLOUR     (readWord(0x900F) & 0x7)
#define REG_BACKGROUND_COLOUR ((readWord(0x900F) & 0xF0) >> 4)

#define REG_HORI_OFFSET       ((readWord(0x9000) & 0x7F) 
#define REG_VERT_OFFSET       ((readWord(0x9001) & 0xFF) 

// C  R  H  V
// 16 16 +6 +16
// 22 23 0  0
// 24 28 -3 -9
// 25 30 -3 -12
// 27 33 -5 -19


static Pixel vicPalette[16];
static uint16_t remap[16] = {
  0x8000,
  0x8400,
  0x8800,
  0x8c00,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x1000,
  0x1400,
  0x1800,
  0x1c00
};

static Pixel linebuf[WIN_W];

MOS6561::MOS6561() : IC(), curRow(0), frameReady(true) {
	// Set clock speed
	this->setClockSpeed(1108000);
}

MOS6561::~MOS6561() {
}

void MOS6561::initialize() {
	// Initialize VIC palette
	vicPalette[0] = RGBVAL16(0, 0, 0);
	vicPalette[1] = RGBVAL16(255, 255, 255);
	vicPalette[2] = RGBVAL16((182), (31), (33));
	vicPalette[3] = RGBVAL16((77), (240), (255));
	vicPalette[4] = RGBVAL16((180), (63), (255));
	vicPalette[5] = RGBVAL16((68), (226), (55));
	vicPalette[6] = RGBVAL16((15), (87), (247));
	vicPalette[7] = RGBVAL16((220), (215), (27));
	vicPalette[8] = RGBVAL16((202), (84), (0));
	vicPalette[9] = RGBVAL16((233), (176), (114));
	vicPalette[10] = RGBVAL16((231), (146), (147));
	vicPalette[11] = RGBVAL16((154), (247), (253)); 
	vicPalette[12] = RGBVAL16((224), (159), (255));
	vicPalette[13] = RGBVAL16((143), (228), (147));
	vicPalette[14] = RGBVAL16((130), (144), (255));
	vicPalette[15] = RGBVAL16((229), (222), (133));
}


void MOS6561::renderBorder(uint16_t raster){
	if (raster < WIN_H) {
		Pixel  bcol = vicPalette[REG_BORDER_COLOUR];
		Pixel * dst = &linebuf[0];
		for (int x=0; x < WIN_W; x++) {
		  *dst++ = bcol;
		}
		emu_DrawLine16(&linebuf[0], WIN_W, WIN_H, raster);
	}
}


void MOS6561::renderLine(uint16_t raster, uint16_t row, uint8_t rowHeight, uint8_t chrLine) {
	int curRow = row;
	int nbRow = REG_NB_ROWS;

	if ( (rowHeight == 16) && (nbRow >= 23) ) {
		curRow = curRow/2;
		nbRow = nbRow/2;    
	}

	if (raster < WIN_H) 
	{
		int nbCol = REG_NB_COLUMNS;
		int bWidth = (WIN_W - nbCol*8)/2;
		#define bakcol 0
		#define borcol 1
		#define forcol 2
		#define auxcol 3
		Pixel cols[4];
		uint16_t screenPage = (REG_SCRPAGE_HIGH & ~0x2000) + REG_SCRPAGE_LO;
		uint8_t * charPointer = &vicmemory[screenPage + (curRow * nbCol)];
		uint16_t colourPage = REG_COLPAGE_BASE + REG_COLPAGE_LO;
		uint8_t * colPointer = &vicmemory[colourPage + (curRow * nbCol)];

		cols[borcol] = vicPalette[REG_BORDER_COLOUR];
		cols[bakcol] = vicPalette[REG_BACKGROUND_COLOUR];

		// Border Left
		Pixel * dst = &linebuf[0];
		for (int x=0; x < bWidth; x++) {
		  *dst++ = cols[borcol];
		}

		uint16_t chardefbase = remap[REG_CHRMAP_PT];
		for (int x = 0; x < nbCol; x +=1) 
		{
			uint16_t charpt = chardefbase + charPointer[x]*rowHeight + chrLine;
			if ( (charpt > 0x2000) && (charpt < 0x3000) )
			  charpt += 0x6000;
		  uint8_t characterByte = vicmemory[charpt];
		  uint8_t colour = colPointer[x] & 0x7;
		  uint8_t multiColour = colPointer[x] & 0x8;
		  cols[forcol] = vicPalette[colour];	  
		  if (!multiColour) {
			Pixel * dest = dst;
			for (int a = 0; a < 8; a++) {
			  if ((characterByte << a) & 0x80) {
				*dest++ = cols[forcol];
			  }
			  else {
				*dest++ = cols[bakcol];
			  }
			}
		  }
		  else {
			Pixel * dest = dst;
			cols[auxcol] = vicPalette[REG_AUXILIARY_COLOUR];	  
			for (int a = 0; a < 8; a += 2) {
				// Set colour
				Pixel col = cols[((characterByte << a) & 0xC0) >> 6];
				*dest++ = col;
				*dest++ = col;
			}
		  }
		  dst +=8;
		}

		// Border Right
		for (int x=0; x < bWidth; x++) {
		  *dst++ = cols[borcol];
		} 
		emu_DrawLine16(&linebuf[0], WIN_W, 1, raster);     
	}  
}
  
void MOS6561::renderRow(uint16_t raster, uint16_t row, uint8_t rowHeight) 
{
	int curRow = row;
	int nbRow = REG_NB_ROWS;

	//printf("rows %d of %d, cols=%d, rowHeight:%d\n", curRow, nbRow, nbCol, rowHeight);

	if ( (rowHeight == 16) && (nbRow >= 23) ) {
		curRow = curRow/2;
		nbRow = nbRow/2;    
	}

	if ((raster+curRow*rowHeight) < WIN_H) 
	{
		int nbCol = REG_NB_COLUMNS;
		int bWidth = (WIN_W - nbCol*8)/2;
		#define bakcol 0
		#define borcol 1
		#define forcol 2
		#define auxcol 3
		Pixel cols[4];
		uint16_t screenPage = (REG_SCRPAGE_HIGH & ~0x2000) + REG_SCRPAGE_LO;
		uint8_t * charPointer = &vicmemory[screenPage + (curRow * nbCol)];
		uint16_t colourPage = REG_COLPAGE_BASE + REG_COLPAGE_LO;
		uint8_t * colPointer = &vicmemory[colourPage + (curRow * nbCol)];

		cols[borcol] = vicPalette[REG_BORDER_COLOUR];
		cols[bakcol] = vicPalette[REG_BACKGROUND_COLOUR];

		for (int line=0; line < rowHeight; line++) {
			// Border Left
			Pixel *  dst = &linebuf[0];
			for (int x=0; x < bWidth; x++) {
			  *dst++ = cols[borcol];
			}

			uint16_t chardefbase = remap[REG_CHRMAP_PT];
			for (int x = 0; x < nbCol; x +=1) 
			{
				uint16_t charpt = chardefbase + charPointer[x]*rowHeight + line;
				if ( (charpt > 0x2000) && (charpt < 0x3000) )
				  charpt += 0x6000;
			  uint8_t characterByte = vicmemory[charpt];
			  uint8_t colour = colPointer[x] & 0x7;
			  uint8_t multiColour = colPointer[x] & 0x8;
			  cols[forcol] = vicPalette[colour];	  
			  if (!multiColour) {
					Pixel * dest = dst;
					for (int a = 0; a < 8; a++) {
					  if ((characterByte << a) & 0x80) {
							*dest++ = cols[forcol];
					  }
					  else {
							*dest++ = cols[bakcol];
					  }
					}
			  }
			  else {
					Pixel * dest = dst;
					cols[auxcol] = vicPalette[REG_AUXILIARY_COLOUR];	  
					for (int a = 0; a < 8; a += 2) {
						// Set colour
						Pixel col = cols[((characterByte << a) & 0xC0) >> 6];
						*dest++ = col;
						*dest++ = col;
					}
			  }
			  dst +=8;
			}

			// Border Right
			for (int x=0; x < bWidth; x++) {
			  *dst++ = cols[borcol];
			}
			emu_DrawLine16(&linebuf[0], WIN_W, 1, curRow*rowHeight+line+raster);      
		}
	}  
}


void MOS6561::renderFrame() {
	this->frameReady = false;
}

bool MOS6561::isFrameReady() {
	return this->frameReady;
}


void MOS6561::tick(int nbcycles) {
	// Increment raster counter 
	uint16_t raster = readWord(REG_RASTER_HI_RD) << 1 | readWord(REG_RASTER_LO_RD) >> 7;
	if (cycles % cyclesPerScanline == 0) {
		uint8_t rowHeight = (REG_DOUBLE_HEIGHT?16:8);
		if (raster <= lastScanline) {
			raster += 1;
		}
		else {
			raster = 0;	
			visScanlines = REG_NB_ROWS * rowHeight;
			if ( (rowHeight == 16) && (REG_NB_ROWS >= 23) ) {
				visScanlines = REG_NB_ROWS * 8;
			}
			firstVisibleScanline = (WIN_H-visScanlines)/2;
		}
		writeWord(REG_RASTER_LO_RD, (raster & 0x1) << 7 | (readWord(REG_RASTER_LO_RD) & 0x7F));
		writeWord(REG_RASTER_HI_RD, raster >> 1);
	
		if (visScanlines) {
			if (raster < firstVisibleScanline) {
				renderBorder(raster);
			}
			// If not blanking, update screen
			else if (raster >= firstVisibleScanline && raster < (firstVisibleScanline+visScanlines)) {
				// Update video  
				uint8_t chrLine = (raster-firstVisibleScanline) % rowHeight;
				//renderLine(raster, this->curRow, rowHeight, chrLine);
				if (chrLine == rowHeight-1) {				
					renderRow(firstVisibleScanline, this->curRow, rowHeight);
					this->curRow++;
					// End of screen reached
					if (this->curRow >= REG_NB_ROWS) {
						this->curRow = 0;
						frameReady = true;
					}
				}
			}
			else {
				renderBorder(raster);
			}
		}
	}

	// Increment cycle counter
	this->cycles += nbcycles;
}



