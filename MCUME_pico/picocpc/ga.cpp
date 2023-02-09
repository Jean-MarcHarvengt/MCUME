/**
 * Gate Array is connected to a 16Mhz clock signal, which is then divided to 4Mhz to feed the Z80, and 1Mhz for the CRTC. 
 * Gate Array's READY signal is connected to Z80's WAIT input for "memory contention". 
 * This means CPU execution is halted every time Gate Array and CPU accesses the memory at the same time. 
 * The reason for this is that both GA and the CPU share the same address/data bus for memory access.
 * 
 * The Gate Array is responsible for the display (colour palette, resolution, horizontal and vertical sync), interrupt generation 
 * and memory arrangement.
*/
#include "pico.h"
#include "pico/stdlib.h"
#include "ga.h"

struct GA_Config ga_config;
/**
 *  Some of these colours are duplicates, because select_pen_colour() uses the least significant
 *  5 bits to index into this array, so the duplicates prevent out-of-bounds read.
*/

struct RGB Palette[32] = {
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

 
void select_pen(register uint8_t value)
{
    switch(value >> 4)
    {
        case 0b01:
            // Select border.
            ga_config.pen_selected = 0x10;
            break;
        case 0b00:
            // Bits 0-3 dictate the pen number
            ga_config.pen_selected = value & 15;
            break;
    }
}

void select_pen_colour(register uint8_t value)
{
    // Bits 0-4 of "value" specify the hardware colour number from the hardware colour palette.
    // (i.e. which index into the Palette array of structs.)
    ga_config.pen_colors[ga_config.pen_selected] = Palette[value & 31];
}

void do_rom_bank_screen_cfg(register uint8_t value)
{
    // Screen mode config, dictated by the least significant 2 bits.
    switch(value & 3)
    {
        case 0b00:
            // mode 0
            // ga_config.ScreenMode = 0;
            break;
        case 0b01:
            // mode 1
            ga_config.screen_mode = 1;
            break;
        case 0b10:
            // mode 2
            // ga_config.ScreenMode = 2;
            break;
        case 0b11:
            // mode 3, unused.
            break;
    }

    // ROM enable flags.
    if ((value >> 2) & 0b1) ga_config.lower_rom_enable = false; else ga_config.lower_rom_enable = true;
    if ((value >> 3) & 0b1) ga_config.upper_rom_enable = false; else ga_config.upper_rom_enable = true;

    // Interrupt generation control.
    if ((value >> 4) & 0b1) ga_config.interrupt_delay = true; else ga_config.interrupt_delay = false;
}

/** Bit 7   Bit 6    Function
 *  --0--   --0--    Select pen
 *  --0--   --1--    Select colour of the selected pen
 *  --1--   --0--    Select screen mode, ROM configuration and interrupt control
 *  --1--   --1--    RAM memory management
*/
void write_ga(register uint8_t value)
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

