#include "pico.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "crtc.h"

int registers[16] = {
    63,                 // horizontal_total
    40,                 // horitzontal_displayed
    46,                 // horizontal_sync_position
    142,                // horizontal_and_vertical_sync_widths -> VVVVHHHH, so bits 0-3 correspond to hsync width, 4-7 to vsync.
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

uint8_t selected_register = 0;
uint8_t horizontal_count = 0;
uint8_t char_line_count = 0;
uint8_t scanline_count = 0;
uint8_t vertical_adjust_count = 0;
uint16_t memory_start_addr = 0;
uint8_t microsec_count_crtc = 0;


void crtc_step()
{
    if(microsec_count_crtc == 3)
    {
        horizontal_count++;

        if(horizontal_count > registers[0])
        {
            // horizontal counter is equal to the Horizontal Total Register.
            horizontal_count = 0;
            scanline_count++;
        }

        if(scanline_count > registers[9])
        {
            // The counter for Maximum Raster Address is equal to it.
            // The height of a character is 8 rasters, so when we reach 8 rasters we increment the char line count.
        
            scanline_count = 0;
            char_line_count++;
        }

        if(char_line_count > registers[4])
        {
            // The vertical counter reaches the Vertical Total register.
            char_line_count = 0;
        }

        if(char_line_count == 0 && horizontal_count == 0)
        {
            memory_start_addr = ((uint16_t) registers[12] << 8) | registers[13];
        }

    }
    microsec_count_crtc = (microsec_count_crtc + 1) % 4;
}

uint16_t crtc_generate_addr()
{
    // Video address is created as follows:
    // Bit 0: Always 0
    // Bits 1-10: From bits 0-9 of memory_start_addr
    // Bits 11-13: Bits 0-2 of scanline_count
    // Bits 14 and 15: Bits 12 and 13 of memory_start_addr.
    uint16_t current_memory_addr = memory_start_addr + (registers[1] * char_line_count) + horizontal_count;

    uint16_t bits_0_10 = (current_memory_addr & 0b0000001111111111) << 1;
    uint16_t bits_11_13 = (scanline_count & 0b0000000000000111) << 11;
    uint16_t bits_14_15 = (current_memory_addr & 0b0011000000000000) << 2;
    return bits_14_15 | bits_11_13 | bits_0_10;
}

bool is_within_display()
{
    return horizontal_count < registers[1] && char_line_count < registers[6];
}

bool is_hsync_active()
{
    // HSYNC is active if the horizontal counter is in the 
    // "horizontal_and_vertical_sync_widths"-defined width starting from the horizontal_sync_position register.
    return horizontal_count >= registers[2] &&
           horizontal_count < registers[2] + (registers[3] & 0b1111);
}

bool is_vsync_active()
{
    const uint8_t char_height = registers[9] + 1;
    int8_t char_lines_counted = (int8_t) char_line_count - registers[7];
    return char_height * char_lines_counted >= 0 && 
           char_height * char_lines_counted <= 16;
}

void write_crt_controller(unsigned short address, uint8_t value)
{
    switch(address & 0xFF00)
    {
        case 0xBC00: selected_register = value & 0b11111;break;
        case 0xBD00: registers[selected_register] = value;break;
    }
}

uint8_t read_crt_controller(unsigned short address)
{
    switch(address & 0xFF00)
    {
        case 0xBF00: return registers[selected_register];
    }
    return 0;
}
