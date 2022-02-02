#include "IC.h"

IC::IC() : cycles(0) {

}

IC::~IC() {

}

uint32_t IC::getCycles() {
	return this->cycles;
}

uint32_t IC::getClockSpeed() {
	return this->clockSpeed;
}

void IC::setClockSpeed(uint32_t clock) {
	this->clockSpeed = clock;
}