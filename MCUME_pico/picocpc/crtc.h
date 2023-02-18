#ifndef CRTC_H_
#define CRTC_H_

#ifndef PICO_H_
#include "pico.h"
#include "pico/stdlib.h"
#endif

extern uint8_t registers[16];
extern uint16_t memoryAddr;
extern uint8_t selectedRegister;
extern uint8_t horizontalCount;
extern uint8_t characterLineCount;
extern uint8_t scanlineCount;
extern uint8_t verticalAdjustCount;

bool isHSyncActive();
bool isVSyncActive();
bool isWithinDisplay();
uint16_t crtc_generateAddress();
void crtc_step();
void writeCRTC(unsigned short address, uint8_t value);
uint8_t readCRTC(unsigned short address);

#endif