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

extern struct Registers regs;

/** Bit 7   Bit 6    Function
 *  --0--   --0--    Select pen
 *  --0--   --1--    Select colour of the selected pen
 *  --1--   --0--    Select screen mode, ROM configuration and interrupt control
 *  --1--   --1--    RAM memory management (Note: This is not available on the CPC 464)
*/
void write_ga(register uint16_t address, register uint8_t value)
{
    switch(address >> 6)
    {
        case 0x00:
            select_pen(address, value);
            break;
        case 0x01:
            select_pen_colour(address, value);
            break;
        case 0x10:
            rom_banking(address, value);
            break;
        case 0x11:
            // This would be RAM banking but we are not concerned with that yet.
            break;
    }
}

// TODO delete address argument? 
void select_pen(register uint16_t address, register uint8_t value)
{
    switch(value >> 4)
    {
        case 0x01:
            // Select border. TODO implement
            break;
        case 0x00:
            // Bits 0-3 dictate the pen number
            regs.PaletteIndex = value >> 4;
    }
}
void select_pen_colour(register uint16_t address, register uint8_t value)
{
    // Bits 4 to 0 specify the hardware colour number from the hardware colour palette.
    regs.PaletteData[regs.PaletteIndex] = value & 0x000FFFFF;
}

void rom_banking(register uint16_t address, register uint8_t value)
{
    return;
}
