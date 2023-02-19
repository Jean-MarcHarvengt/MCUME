#ifndef CRTC_H_
#define CRTC_H_

#ifndef PICO_H_
#include "pico.h"
#include "pico/stdlib.h"
#endif

extern uint8_t registers[16];
extern uint16_t memory_start_addr;
extern uint8_t selected_register;
extern uint8_t horizontal_count;
extern uint8_t char_line_count;
extern uint8_t scanline_count;
extern uint8_t vertical_adjust_count;

bool is_hsync_active();
bool is_vsync_active();
bool is_within_display();
uint16_t crtc_generate_addr();
void crtc_step();
void write_crt_controller(unsigned short address, uint8_t value);
uint8_t read_crt_controller(unsigned short address);

#endif