#ifndef _H_IC
#define _H_IC

#include <stdint.h>

/*
Simple base class for integrated circuits.
*/
class IC {
public:
	IC();
	~IC();

	// Getters
	uint32_t getCycles();
	uint32_t getClockSpeed();

	// Setters
	void setClockSpeed(uint32_t clock);
protected:
	// Derived classes handle incrementing this counter
	uint32_t cycles;

	// Determines the maximum frequenzy an IC can operate in hertz
	uint32_t clockSpeed;
};

#endif