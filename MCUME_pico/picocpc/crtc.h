#ifndef CRTC_H_
#define CRTC_H_

#ifndef PICO_H_
#include "pico.h"
#include "pico/stdlib.h"
#endif

void writeCRTC(unsigned short address, uint8_t value);
uint8_t readCRTC(unsigned short address);

#endif