#include "pico.h"
#include "pico/stdlib.h"

#include <stdio.h>
#include <stdlib.h>
#include "ga.h"
#include "crtc.h"
#include "cpc.h"

struct GAConfig ga_config;

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
    
    //printf("vsync_delay_count: %d \n", ga_config.vsync_delay_count);

    if(ga_config.vsync_delay_count == 2)
    {
        if(ga_config.interrupt_counter >= 32)
        {
            //printf("Interrupt generated! \n");
            interrupt_generated = true;
        }
        ga_config.interrupt_counter = 0;
    }

    return interrupt_generated;
}

char ga_rgb_to_vga(uint8_t r, uint8_t g, uint8_t b)
{
    return VGA_RGB(r,g,b);
}

void address_to_pixels()
{
    // When HSYNC is active Gate-Array outputs the palette colour black
    // if(ga_config.hsync_active)
    // {
    //     *(bitstream) = ga_rgb_to_vga(ga_config.pen_colours[19].R,
    //                                 ga_config.pen_colours[19].G,
    //                                 ga_config.pen_colours[19].B);
    //     return;
    // }

    // // border
    // if(ga_config.vsync_active || ga_config.hsync_active)
    // {
    //     *(bitstream) = ga_rgb_to_vga(ga_config.pen_colours[0x10].R,
    //                                 ga_config.pen_colours[0x10].G,
    //                                 ga_config.pen_colours[0x10].B);
    //     return;
    // }

    for(int i = 0; i < 2; i++) 
    {
        uint16_t address = crtc_generate_addr() + i;
        // printf("address from CRTC: %x \nRAM data: %x \n", address, RAM[address]);
        uint8_t encodedByte = RAM[address];
        uint8_t pixel0, pixel1, pixel2, pixel3;
        uint8_t* pixels = (uint8_t*) calloc(4, 8*sizeof(uint8_t));
        if(address >= 0xC000)
        {
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
                    pixels[0] = pixel0;
                    pixels[1] = pixel1;
                    for(int pixelIdx = 0; pixelIdx < 2; pixelIdx++)
                    {
                        for(int color = 0; color < 4; color++)
                        {
                            bitstream[address + color - 0xC000] = ga_rgb_to_vga(ga_config.pen_colours[pixels[pixelIdx]].R,
                                                                                ga_config.pen_colours[pixels[pixelIdx]].G,
                                                                                ga_config.pen_colours[pixels[pixelIdx]].B);
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
                            bitstream[address + color - 0xC000] = ga_rgb_to_vga(ga_config.pen_colours[pixels[pixelIdx]].R,
                                                                                ga_config.pen_colours[pixels[pixelIdx]].G,
                                                                                ga_config.pen_colours[pixels[pixelIdx]].B);
                        }
                    }
                    break;
                case 2:
                    uint8_t pixel;
                    for (int color = 0; color < 8; color++)
                    {
                        pixel = (encodedByte >> 7 - color) & 1;
                        bitstream[address + color - 0xC000] = ga_rgb_to_vga(ga_config.pen_colours[pixel].R,
                                                                            ga_config.pen_colours[pixel].G,
                                                                            ga_config.pen_colours[pixel].B);
                    }
                    break;
            }
            free(pixels);
        }
        
    }
}

bool ga_step()
{
    bool interrupt_generated = update_interrupts();
    //if(is_within_display())
    //{
        address_to_pixels();
    //}
    
    ga_config.hsync_active = is_hsync_active();
    ga_config.vsync_active = is_vsync_active();

    return interrupt_generated;
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
    ga_config.pen_colours[ga_config.pen_selected] = palette[value & 31];
}

void rom_and_screen_mgmt(uint8_t value)
{
    
    if(!ga_config.hsync_active && is_hsync_active())
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
    if ((value >> 4) & 0b1) {
        ga_config.interrupt_counter = 0;
    }
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

