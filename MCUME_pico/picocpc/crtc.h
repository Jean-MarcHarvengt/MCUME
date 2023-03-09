#ifndef CRTC_H_
#define CRTC_H_

#ifndef PICO_H_
#include "pico.h"
#include "pico/stdlib.h"
#endif

extern int registers[16];
extern uint16_t memory_start_addr;
extern uint8_t selected_register;
extern uint8_t horizontal_count;
extern uint8_t char_line_count;
extern uint8_t scanline_count;
extern uint8_t vertical_adjust_count;
extern uint8_t microsec_count_crtc;

extern bool is_hsync_active();
extern bool is_vsync_active();
extern bool is_within_display();
extern uint16_t crtc_generate_addr();
extern void crtc_step();
extern void write_crt_controller(unsigned short address, uint8_t value);
extern uint8_t read_crt_controller(unsigned short address);

#endif