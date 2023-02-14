#include "pico.h"
#include "pico/stdlib.h"

#include <stdio.h>
#include "ga.h"
#include "crtc.h"
#include "cpc.h"

struct GAConfig gaConfig;

/**
 *  Some of the colours are duplicates, because select_pen_colour() uses the least significant
 *  5 bits to index into this array, so the duplicates prevent out-of-bounds read.
*/
struct RGB palette[32] = {
    {50, 50, 50},      // White
    {50, 50, 50},      // White
    {0, 100, 50},      // Sea Green
    {100, 100, 50},    // Pastel Yellow
    {0, 0, 50},        // Blue
    {100, 0, 50},      // Purple
    {0, 50, 50},       // Cyan
    {100, 50, 50},     // Pink
    {100, 0, 50},      // Purple
    {100, 100, 50},    // Pastel Yellow
    {100, 100, 0},     // Bright Yellow
    {100, 100, 100},   // Bright White
    {100, 0, 0},       // Bright Red
    {100, 0, 100},     // Bright Magenta
    {100, 50, 0},      // Orange
    {100, 50, 100},    // Pastel Magenta
    {0, 0, 50},        // Blue
    {0, 100, 50},      // Sea Green
    {0, 100, 0},       // Bright Green
    {0, 100, 100},     // Bright Cyan
    {0, 0, 0},         // Black
    {0, 0, 100},       // Bright Blue
    {0, 50, 0},        // Green
    {0, 50, 100},      // Sky Blue
    {50, 0, 50},       // Magenta
    {50, 100, 50},     // Pastel Green
    {50, 100, 0},      // Lime
    {50, 100, 100},    // Pastel Cyan
    {50, 0, 0},        // Red
    {50, 0, 100},      // Mauve
    {50, 50, 0},       // Yellow
    {50, 50, 100}      // Pastel Blue
};

bool updateInterrupts()
{
    bool interrupt_generated = false;

    if(gaConfig.hsyncActive && !isHSyncActive())
    {
        // falling edge of CRTC hsync signal.
        gaConfig.interruptCounter++;
        gaConfig.vsyncDelayCount++;

        if(gaConfig.interruptCounter == 52)
        {
            gaConfig.interruptCounter = 0;
            interrupt_generated = true;
        }
    }

    if(!gaConfig.vsyncActive && isVSyncActive())
    {
        // A VSYNC triggers a delay action of 2 HSYNCs in the GA, at the 
        // completion of which the scan line count in the GA is compared to 32. 
        gaConfig.vsyncDelayCount = 0;
    }
    
    //printf("vsyncDelayCount: %d \n", gaConfig.vsyncDelayCount);

    if(gaConfig.vsyncDelayCount == 2)
    {
        if(gaConfig.interruptCounter >= 32)
        {
            interrupt_generated = true;
        }
        gaConfig.interruptCounter = 0;
    }

    return interrupt_generated;
}

void addressToPixels()
{
    for(int i = 0; i < 2; i++) 
    {
        uint16_t address = crtc_generateAddress() + i;
        uint8_t encodedByte = RAM[address];
        if(RAM[address] != 0 && address >= 0xC000)
        {
            printf("CRTC generated addr: %x \nRAM[address]: %x \n", address, encodedByte);
            switch(gaConfig.screenMode)
            {
                case 0:
                    bitstream[address - 0xC000] = (encodedByte & 0x80) >> 7 |
                                                (encodedByte & 0x08) >> 2 |
                                                (encodedByte & 0x20) >> 3 |
                                                (encodedByte & 0x02) << 2;
                    bitstream[address + 1 - 0xC000] = (encodedByte & 0x40) >> 6 |
                                                    (encodedByte & 0x04) >> 1 |
                                                    (encodedByte & 0x10) >> 2 |
                                                    (encodedByte & 0x01) << 3;

                    // TODO may not be correct, needs testing.
                    break;
                case 1:
                    bitstream[address - 0xC000] = (encodedByte & 0x80) >> 7 |
                                                (encodedByte & 0x08) >> 2;
                    bitstream[address + 1 - 0xC000] = (encodedByte & 0x40) >> 6 |
                                                    (encodedByte & 0x04) >> 1;
                    bitstream[address + 2 - 0xC000] = (encodedByte & 0x02) |
                                                    (encodedByte & 0x20) >> 5;
                    bitstream[address + 3 - 0xC000] = (encodedByte & 0x10) >> 4 |
                                                    (encodedByte & 0x01) << 1;
                    break;
                case 2:
                    for (int j = 0; j < 8; j++)
                    {
                        bitstream[address + j - 0xC000] = (encodedByte >> 7 - j) & 1;
                    }
                    break;
            }
        }
        
    }
}

bool ga_step()
{
    // TODO add proper GA responses to hsync and vsync signals.
    bool interruptGenerated = updateInterrupts();
    addressToPixels();
    
    gaConfig.hsyncActive = isHSyncActive();
    gaConfig.vsyncActive = isVSyncActive();

    return interruptGenerated;
}
 
void selectPen(uint8_t value)
{
    switch(value >> 4)
    {
        case 0b01:
            // Select border.
            gaConfig.penSelected = 0x10;
            break;
        case 0b00:
            // Bits 0-3 dictate the pen number
            gaConfig.penSelected = value & 15;
            break;
    }
}

void selectPenColour(uint8_t value)
{
    // Bits 0-4 of "value" specify the hardware colour number from the hardware colour palette.
    // (i.e. which index into the Palette array of structs.)
    gaConfig.penColours[gaConfig.penSelected] = palette[value & 31];
}

void ROMMgmtAndScreenCfg(uint8_t value)
{
    // Screen mode config, dictated by the least significant 2 bits.
    if(!gaConfig.hsyncActive)
    {
        switch(value & 3)
        {
            case 0b00:
                // mode 0
                gaConfig.screenMode = 0;
                break;
            case 0b01:
                // mode 1
                gaConfig.screenMode = 1;
                break;
            case 0b10:
                // mode 2
                gaConfig.screenMode = 2;
                break;
            case 0b11:
                // mode 3, unused.
                break;
        }
    }
    
    // ROM enable flags.
    if ((value >> 2) & 0b1) gaConfig.lowerROMEnable = false; else gaConfig.lowerROMEnable = true;
    if ((value >> 3) & 0b1) gaConfig.upperROMEnable = false; else gaConfig.upperROMEnable = true;

    // Interrupt delay control.
    
    if ((value >> 4) & 0b1) {
        gaConfig.interruptCounter = 0;
    }
}

/** Bit 7   Bit 6    Function
 *  --0--   --0--    Select pen
 *  --0--   --1--    Select colour of the selected pen
 *  --1--   --0--    Select screen mode, ROM configuration and interrupt control
 *  --1--   --1--    RAM memory management
*/
void writeGA(uint8_t value)
{
    switch(value >> 6)
    {
        case 0b00:
            selectPen(value);
            break;
        case 0b01:
            selectPenColour(value);
            break;
        case 0b10:
            ROMMgmtAndScreenCfg(value);
            break;
        case 0b11:
            // This would be RAM banking but it is not available on the CPC464.
            break;
    }
}

