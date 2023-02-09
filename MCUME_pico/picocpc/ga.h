extern void write_ga(register uint8_t value);
void select_pen(register uint8_t value);
void select_pen_colour(register uint8_t value);
void rom_banking(register uint8_t value);

#define PEN_NUMBER 4  // Mode 0 has 16 pens, mode 1 has 4 pens and mode 2 has 2 pens.

struct RGB {
    uint8_t R, G, B;
};

struct GA_Config {
    uint8_t pen_selected = 0;
    RGB pen_colors[PEN_NUMBER];
    uint8_t screen_mode = 1; // 0 -> 160x200 16c; 1 -> 320x200 4c; 2 -> 640x200 2c; 3 (undocumented) -> 160x200 4c.
    bool upper_rom_enable = false;
    bool lower_rom_enable = false;
    bool interrupt_delay = false;
    uint8_t ram_banking = 0; // unused in CPC 464
};
