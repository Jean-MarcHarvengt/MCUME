#include "pico.h"
#include "pico/stdlib.h"

#include "ga.h"

struct GAConfig gaConfig;
/**
 *  Some of these colours are duplicates, because select_pen_colour() uses the least significant
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

// TODO: Add a step() function, or something, that reads from the crtc's hsync and vsync and generates the actual
// pixel data based on its config data.
void step()
{
    return;
}
 
void select_pen(uint8_t value)
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

void select_pen_colour(uint8_t value)
{
    // Bits 0-4 of "value" specify the hardware colour number from the hardware colour palette.
    // (i.e. which index into the Palette array of structs.)
    gaConfig.penColours[gaConfig.penSelected] = palette[value & 31];
}

void do_rom_bank_screen_cfg(uint8_t value)
{
    // Screen mode config, dictated by the least significant 2 bits.
    switch(value & 3)
    {
        case 0b00:
            // mode 0
            // ga_config.screen_mode = 0;
            break;
        case 0b01:
            // mode 1
            gaConfig.screenMode = 1;
            break;
        case 0b10:
            // mode 2
            // ga_config.screen_mode = 2;
            break;
        case 0b11:
            // mode 3, unused.
            break;
    }

    // ROM enable flags.
    if ((value >> 2) & 0b1) gaConfig.lowerROMEnable = false; else gaConfig.lowerROMEnable = true;
    if ((value >> 3) & 0b1) gaConfig.upperROMEnable = false; else gaConfig.upperROMEnable = true;

    // Interrupt generation control.
    if ((value >> 4) & 0b1) gaConfig.interruptDelay = true; else gaConfig.interruptDelay = false;
}

/** Bit 7   Bit 6    Function
 *  --0--   --0--    Select pen
 *  --0--   --1--    Select colour of the selected pen
 *  --1--   --0--    Select screen mode, ROM configuration and interrupt control
 *  --1--   --1--    RAM memory management
*/
void write_ga(uint8_t value)
{
    switch(value >> 6)
    {
        case 0b00:
            select_pen(value);
            break;
        case 0b01:
            select_pen_colour(value);
            break;
        case 0b10:
            do_rom_bank_screen_cfg(value);
            break;
        case 0b11:
            // This would be RAM banking but it is not available on the CPC464.
            break;
    }
}

