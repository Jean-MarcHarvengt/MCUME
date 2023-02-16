#ifndef GA_H_
#define GA_H_

#ifndef PICO_H_
#include "pico.h"
#include "pico/stdlib.h"
#endif

extern void writeGA(uint8_t value);
bool ga_step();
void selectPen(uint8_t value);
void selectPenColour(uint8_t value);
void romBanking(uint8_t value);

#define PEN_NUMBER 4  // Mode 0 has 16 pens, mode 1 has 4 pens and mode 2 has 2 pens.
#define VGA_RGB(r,g,b)   ( (((r>>5)&0x07)<<5) | (((g>>5)&0x07)<<2) | (((b>>6)&0x3)<<0) )

struct RGB {
    uint8_t R, G, B;
};

struct GAConfig {
    uint8_t penSelected = 0;
    RGB penColours[PEN_NUMBER];
    uint8_t screenMode = 1; // 0 -> 160x200 16c; 1 -> 320x200 4c; 2 -> 640x200 2c; 3 (undocumented) -> 160x200 4c.
    bool upperROMEnable = true;
    bool lowerROMEnable = true;
    uint8_t interruptCounter = 0;
    uint8_t vsyncDelayCount = 0;
    bool hsyncActive = false;
    bool vsyncActive = false;
    uint8_t ramBanking = 0; // unused in CPC 464
};

extern struct RGB palette[32];

#endif
