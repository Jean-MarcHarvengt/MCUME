#include "pico.h"
#include "pico/stdlib.h"

#include <stdio.h>
#include <stdlib.h>
#include "ga.h"
#include "crtc.h"
#include "cpc.h"

struct GAConfig ga_config;
bool vsync_wait;
uint8_t microsecond_count_ga = 0;

/**
 *  Some of the colours are duplicates, because select_pen_colour() uses the least significant
 *  5 bits to index into this array, so the duplicates prevent out-of-bounds read.
*/
uint8_t hardware_colours[32] = {
    13, 
    13, 
    19, 
    25, 
    1,  
    7,  
    10, 
    16, 
    7,  
    25, 
    24, 
    26, 
    6,  
    8,  
    15, 
    17, 
    1,  
    19,
    18, 
    20, 
    0,  
    2,  
    9,  
    11,
    4,
    22,
    21,
    23,
    3,
    5,
    12,
    14
};
struct RGB firmware_palette[27] = {
    {0, 0, 0}, // 0
    {0, 0, 128}, // 1
    {0, 0, 255}, // 2
    {128, 0, 0}, // 3
    {128, 0, 128}, // 4
    {128, 0, 255}, // 5
    {255, 0, 0}, // 6
    {255, 0, 128}, // 7
    {255, 0, 255}, // 8
    {0, 128, 0}, // 9
    {0, 128, 128}, // 10
    {0, 128, 255}, // 11
    {128, 128, 0}, // 12
    {128, 128, 128}, // 13
    {128, 128, 255}, // 14
    {255, 128, 0}, // 15
    {255, 128, 128}, // 16
    {255, 128, 255}, // 17
    {0, 255, 0}, // 18
    {0, 255, 128}, // 19
    {0, 255, 255}, // 20
    {128, 255, 0}, // 21
    {128, 255, 128}, // 22
    {128, 255, 255}, // 23
    {255, 255, 0}, // 24
    {255, 255, 128}, // 25
    {255, 255, 255} // 26
};

bool update_interrupts()
{
    bool interrupt_generated = false;
    if(ga_config.hsync_active && !is_hsync_active())
    {
        // falling edge of CRTC hsync signal.
        ga_config.interrupt_counter++;
        ga_config.vsync_delay_count++;

        if(ga_config.interrupt_counter == 52)
        {
            //printf("Interrupt generated! \n");
            ga_config.interrupt_counter = 0;
            interrupt_generated = true;
        }
    }

    if(!ga_config.vsync_active && is_vsync_active())
    {
        // A VSYNC triggers a delay action of 2 HSYNCs in the GA, at the 
        // completion of which the scan line count in the GA is compared to 32. 
        ga_config.vsync_delay_count = 0;
    }
    
    if(ga_config.vsync_delay_count == 2)
    {
        interrupt_generated = ga_config.interrupt_counter >= 32;
        ga_config.interrupt_counter = 0;
    }

    return interrupt_generated;
}

void address_to_pixels()
{
    if(!ga_config.vsync_active && is_vsync_active())
    {
        vsync_wait = false;
        // return;
    }

    // this gets rid of the horizontal scrolling, but "locomotive software ltd" doesnt show up.
    // if(is_hsync_active() || is_vsync_active())
    // {
    //     return;
    // }

    if(!is_within_display())
    {
        return;
    }

    for(int i = 0; i < 2; i++) 
    {
        uint16_t address = crtc_generate_addr() + i;
        uint8_t encodedByte = RAM[address];
        uint8_t pixel0, pixel1, pixel2, pixel3;
        switch(ga_config.screen_mode)
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
                write_to_bitstream(ga_config.pen_colours[pixel0]);                
                write_to_bitstream(ga_config.pen_colours[pixel1]);
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
                write_to_bitstream(ga_config.pen_colours[pixel0]);
                write_to_bitstream(ga_config.pen_colours[pixel1]);
                write_to_bitstream(ga_config.pen_colours[pixel2]);
                write_to_bitstream(ga_config.pen_colours[pixel3]);
                break;
            case 2:
                uint8_t pixel;
                for (int color = 0; color < 8; color++)
                {
                    pixel = (encodedByte >> 7 - color) & 1;
                    write_to_bitstream(ga_config.pen_colours[pixel]);
                }
                break;
        } 
    }
}

bool ga_step()
{
    ga_config.wait_signal = microsecond_count_ga == 1;

    if(microsecond_count_ga == 3)
    {
        bool interrupt_generated = update_interrupts();
        address_to_pixels();
        
        ga_config.hsync_active = is_hsync_active();
        ga_config.vsync_active = is_vsync_active();
        microsecond_count_ga = (microsecond_count_ga + 1) % 4;
        return interrupt_generated;
    }
    else
    {
        microsecond_count_ga = (microsecond_count_ga + 1) % 4;
        return false;  
    }
}
 
void select_pen(uint8_t value)
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

void select_pen_colour(uint8_t value)
{
    // Bits 0-4 of "value" specify the hardware colour number from the hardware colour palette.
    // (i.e. which index into the Palette array of structs.)
    ga_config.pen_colours[ga_config.pen_selected] = value & 31;
}

void rom_and_screen_mgmt(uint8_t value)
{
    if(ga_config.hsync_active && !is_hsync_active())
    {
        // Screen mode config, dictated by the least significant 2 bits.
        // Effective at next line.
        switch(value & 3)
        {
            case 0b00:
                // mode 0
                ga_config.screen_mode = 0;
                break;
            case 0b01:
                // mode 1
                ga_config.screen_mode = 1;
                break;
            case 0b10:
                // mode 2
                ga_config.screen_mode = 2;
                break;
            case 0b11:
                // mode 3, unused.
                break;
        }
    }
    
    // ROM enable flags.
    if ((value >> 2) & 0b1) ga_config.lower_rom_enable = false; else ga_config.lower_rom_enable = true;
    if ((value >> 3) & 0b1) ga_config.upper_rom_enable = false; else ga_config.upper_rom_enable = true;

    // Interrupt delay control.
    if ((value >> 4) & 0b1) ga_config.interrupt_counter = 0;
}

/** Bit 7   Bit 6    Function
 *  --0--   --0--    Select pen
 *  --0--   --1--    Select colour of the selected pen
 *  --1--   --0--    Select screen mode, ROM configuration and interrupt control
 *  --1--   --1--    RAM memory management
*/
void write_gate_array(uint8_t value)
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
            rom_and_screen_mgmt(value);
            break;
        case 0b11:
            // This would be RAM banking but it is not available on the CPC464.
            break;
    }
}

