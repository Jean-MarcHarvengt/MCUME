/**
 * What 6845 primarily does is to generate CRT signals such as Horizontal Sync and Vertical Sync as well as 
 * video memory addresses to fetch pixel data. 
 * Gate Array works together in sync with CRTC, and is responsible for converting the pixel data in memory 
 * into video signals through an R2R DAC.
*/
#include "pico.h"
#include "pico/stdlib.h"

struct Registers {
    uint8_t HorizontalTotal = 63;
    uint8_t HorizontalDisplayed = 40;
    uint8_t HorizontalSyncPosition = 46;
    uint8_t HorizontalAndVerticalSyncWidths = 142;
    uint8_t VerticalTotal = 38;
    uint8_t VerticalTotalAdjust = 0;
    uint8_t VerticalDisplayed = 25;
    uint8_t VerticalSyncPosition = 30;
    uint8_t InterlaceAndSkew = 0;
    uint8_t MaximumRasterAddress = 7;
    uint8_t CursorStartRaster = 0;
    uint8_t CursorEndRaster = 0;
    uint8_t DisplayStartAddressHigh = 48;
    uint8_t DisplayStartAddressLow = 0;
    uint8_t CursorAddressHigh = 0;
    uint8_t CursorAddressLow = 0;
    uint8_t LightPenAddressHigh = 0; // Read only
    uint8_t LightPenAddressLow = 0; // Read only 
};

void write_crtc(unsigned short address, unsigned short value)
{

    return;
}

void read_crtc(unsigned short address)
{
    switch(address & 0xFF00)
    {
        case 0xBF00: break;
    }
    return;
}