#ifndef _H_MOS6561
#define	_H_MOS6561

#include <stdint.h>

#include "IC.h"
#include "MOS6502Memory.h"

/*
References, 
http://vic-20.appspot.com/docs/6561.txt
http://vic-20.appspot.com/docs/vicregs.txt
http://vic-20.appspot.com/docs/6561help.txt
*/

class MOS6561 : public IC {
public:
	MOS6561();
	~MOS6561();

	// Interface
	void tick(int cycles);
	void initialize();

	// Getters, mostly depend on memory
	bool isFrameReady();
	void renderFrame();
	void renderBorder(uint16_t raster);
	void renderRow(uint16_t raster, uint16_t row, uint8_t rowHeight);
	void renderLine(uint16_t raster, uint16_t row, uint8_t rowHeight, uint8_t chrLine);
private:
	// Used as counters
	uint8_t curRow;
	uint16_t firstVisibleScanline;
	uint16_t visScanlines;

	// Determines if a full frame has been drawn
	bool frameReady;
	
	static const uint16_t lastScanline = 312;
public:
	static const int cyclesPerScanline = 71;
	
	static const uint16_t Audio1Register = 0x900A;
	static const uint16_t Audio2Register = 0x900B;
	static const uint16_t Audio3Register = 0x900C;
	static const uint16_t Audio4Register = 0x900D;
	static const uint16_t AudioVolRegister = 0x900E;
};

#endif