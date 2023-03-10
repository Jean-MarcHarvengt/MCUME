#ifndef GA_H_
#define GA_H_

#ifndef PICO_H_
#include "pico.h"
#include "pico/stdlib.h"
#endif

extern void write_gate_array(uint8_t value);
bool ga_step();
void select_pen(uint8_t value);
void select_pen_colour(uint8_t value);
void rom_and_screen_mgmt(uint8_t value);

#define PEN_NUMBER 17  // Mode 0 has 16 pens, mode 1 has 4 pens and mode 2 has 2 pens.

struct RGB {
    int R, G, B;
};

struct GAConfig {
    uint8_t pen_selected = 0;
    uint8_t pen_colours[PEN_NUMBER];
    uint8_t screen_mode = 1; // 0 -> 160x200 16c; 1 -> 320x200 4c; 2 -> 640x200 2c; 3 (undocumented) -> 160x200 4c.
    bool upper_rom_enable = true;
    bool lower_rom_enable = true;
    uint8_t interrupt_counter = 0;
    uint8_t vsync_delay_count = 0;
    bool hsync_active = false;
    bool vsync_active = false;
    uint8_t ram_banking = 0; // unused in CPC 464
    bool wait_signal = false;
};

extern struct RGB firmware_palette[27];
extern uint8_t hardware_colours[32];

#endif
