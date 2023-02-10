#include "pico.h"
#include "pico/stdlib.h"

#include "crtc.h"

uint8_t registers[16] = {
    63,                 // horizontal_total
    40,                 // horitzontal_displayed
    46,                 // horizontal_sync_position
    142,                // horizontal_and_vertical_sync_widths
    38,                 // vertical_total
    0,                  // vertical_total_adjust
    25,                 // vertical_displayed
    30,                 // vertical_sync_position
    0,                  // interlace_and_skew
    7,                  // max_raster_address
    0,                  // cursor_start_raster
    0,                  // cursor_end_raster
    48,                 // display_start_addr_high
    0,                  // display_start_addr_low
    0,                  // cursor_addr_high
    0                   // cursor_addr_low
};
uint8_t selectedRegister = 0;
uint8_t horizontal_counter = 0;     // C0
uint8_t vertical_counter = 0;       // C4
uint8_t scanline_counter = 0;       // C9
uint8_t vertical_adjust_counter = 0;
uint16_t memory_address = 0;


void step()
{
    horizontal_counter++;
    if(horizontal_counter == registers[0])
    {
        // horizontal counter is equal to the Horizontal Total Register.
        horizontal_counter = 0;
        scanline_counter++;
    }

    if(scanline_counter == registers[9])
    {
        // The counter for Maximum Raster Address is equal to it.
        scanline_counter = 0;
        vertical_counter++;
    }

    if(vertical_counter == registers[4])
    {
        // The vertical counter reaches the Vertical Total register.
        if(vertical_adjust_counter == registers[5])
        {
            // TODO see how the vertical adjust counter is used by GA and rest of the system.
            vertical_adjust_counter = 0;
            vertical_counter = 0;
        }
    }
}

// TODO hsync and vsync functions.


void writeCRTC(unsigned short address, uint8_t value)
{
    switch(address & 0xFF00)
    {
        case 0xBC00: selectedRegister = value & 0b11111;
        case 0xBD00: registers[selectedRegister] = value;
    }
}

uint8_t readCRTC(unsigned short address)
{
    switch(address & 0xFF00)
    {
        case 0xBF00: return registers[selectedRegister];
    }
}
