#include "pico.h"
#include "pico/stdlib.h"

#include <stdio.h>
#include <stdlib.h>
#include "ga.h"
#include "crtc.h"
#include "cpc.h"


struct GAConfig gaConfig;

/**
 *  Some of the colours are duplicates, because select_pen_colour() uses the least significant
 *  5 bits to index into this array, so the duplicates prevent out-of-bounds read.
*/
struct RGB palette[32] = {
    {128, 128, 128},      // White
    {128, 128, 128},      // White
    {0, 255, 128},      // Sea Green
    {255, 255, 128},    // Pastel Yellow
    {0, 0, 128},        // Blue
    {255, 0, 128},      // Purple
    {0, 128, 128},       // Cyan
    {255, 128, 128},     // Pink
    {255, 0, 128},      // Purple
    {255, 255, 128},    // Pastel Yellow
    {255, 255, 0},     // Bright Yellow
    {255, 255, 255},   // Bright White
    {255, 0, 0},       // Bright Red
    {255, 0, 255},     // Bright Magenta
    {255, 128, 0},      // Orange
    {255, 128, 255},    // Pastel Magenta
    {0, 0, 128},        // Blue
    {0, 255, 128},      // Sea Green
    {0, 255, 0},       // Bright Green
    {0, 255, 255},     // Bright Cyan
    {0, 0, 0},         // Black
    {0, 0, 255},       // Bright Blue
    {0, 128, 0},        // Green
    {0, 128, 255},      // Sky Blue
    {128, 0, 128},       // Magenta
    {128, 255, 128},     // Pastel Green
    {128, 255, 0},      // Lime
    {128, 255, 255},    // Pastel Cyan
    {128, 0, 0},        // Red
    {128, 0, 255},      // Mauve
    {128, 128, 0},       // Yellow
    {128, 128, 255}      // Pastel Blue
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

char convertPixelToVGA(uint8_t r, uint8_t g, uint8_t b)
{
    return VGA_RGB(r,g,b);
}

void addressToPixels()
{
    // When HSYNC is active Gate-Array outputs the palette colour black
    if(gaConfig.hsyncActive)
    {
        // for(int j = 0; j < registers[3] & 0b1111; j++)
        // {
        //     *bitstream = convertPixelToVGA(gaConfig.penColours[19].R,
        //                                gaConfig.penColours[19].G,
        //                                gaConfig.penColours[19].B);
        //     *bitstream++;
        // }
        return;
    }

    // border
    if(gaConfig.vsyncActive || gaConfig.hsyncActive)
    {
        // for(int j = 0; j < 16; j++)
        // {
        //     *bitstream = convertPixelToVGA(gaConfig.penColours[0x10].R,
        //                                 gaConfig.penColours[0x10].G,
        //                                 gaConfig.penColours[0x10].B);
        //     *bitstream++;
        // }
        return;
    }

    for(int i = 0; i < 2; i++) 
    {
        uint16_t address = crtc_generateAddress() + i;
        printf("address from CRTC: %x \nRAM data: %x \n", address, RAM[address]);
        uint8_t encodedByte = RAM[address];
        uint8_t pixel0, pixel1, pixel2, pixel3;
        uint8_t* pixels = (uint8_t*) calloc(4, 8*sizeof(uint8_t));
        // if(address >= 0xC000)
        // {
            // printf("CRTC generated addr: %x \nRAM[address]: %x \n", address, encodedByte);
            switch(gaConfig.screenMode)
            {
                case 0:
                    pixel0 = (encodedByte & 0x80) >> 7 |
                             (encodedByte & 0x08) >> 2 |
                             (encodedByte & 0x20) >> 3 |
                             (encodedByte & 0x02) << 2;
                    pixel1 = (encodedByte & 0x40) >> 6 |
                             (encodedByte & 0x04) >> 1 |
                             (encodedByte & 0x10) >> 2 |
                             (encodedByte & 0x01) << 3;
                    pixels[0] = pixel0;
                    pixels[1] = pixel1;
                    

                    // bitstream[address - 0xC000]
                    // bitstream[address + 1 - 0xC000]
                    // need to consider each color for the 2 pixels
                    for(int pixelIdx = 0; pixelIdx < 2; pixelIdx++)
                    {
                        for(int color = 0; color < 4; color++)
                        {
                            bitstream[address + color - 0xC000] = convertPixelToVGA(gaConfig.penColours[pixels[pixelIdx]].R,
                                                                                    gaConfig.penColours[pixels[pixelIdx]].G,
                                                                                    gaConfig.penColours[pixels[pixelIdx]].B);
                        }
                    }
                    break;
                case 1:
                    pixel0 = (encodedByte & 0x80) >> 7 |
                             (encodedByte & 0x08) >> 2;
                    pixel1 = (encodedByte & 0x40) >> 6 |
                             (encodedByte & 0x04) >> 1;
                    pixel2 = (encodedByte & 0x02) |
                             (encodedByte & 0x20) >> 5;
                    pixel3 = (encodedByte & 0x10) >> 4 |
                             (encodedByte & 0x01) << 1;
                    pixels[0] = pixel0;
                    pixels[1] = pixel1;
                    pixels[2] = pixel2;
                    pixels[3] = pixel3;

                    for(int pixelIdx = 0; pixelIdx < 4; pixelIdx++)
                    {
                        for(int color = 0; color < 2; color++)
                        {
                            bitstream[address + color - 0xC000] = convertPixelToVGA(gaConfig.penColours[pixels[pixelIdx]].R,
                                                                                    gaConfig.penColours[pixels[pixelIdx]].G,
                                                                                    gaConfig.penColours[pixels[pixelIdx]].B);
                        }
                    }
                    break;
                case 2:
                    uint8_t pixel;
                    for (int color = 0; color < 8; color++)
                    {
                        pixel = (encodedByte >> 7 - color) & 1;
                        bitstream[address + color - 0xC000] = convertPixelToVGA(gaConfig.penColours[pixel].R,
                                                                                gaConfig.penColours[pixel].G,
                                                                                gaConfig.penColours[pixel].B);
                    }
                    break;
            // }
        }
        free(pixels);
    }
}

bool ga_step()
{
    bool interruptGenerated = updateInterrupts();
    // printf("Did the GA generate an interrupt? %d \n Interrupt counter: %d", interruptGenerated, gaConfig.interruptCounter);
    // if(memoryAddr != 0)
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
    
    if(!gaConfig.hsyncActive && isHSyncActive())
    {
        // Screen mode config, dictated by the least significant 2 bits.
        // Effective at next line.
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
        printf("Updated screen mode to mode %d", gaConfig.screenMode);
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

