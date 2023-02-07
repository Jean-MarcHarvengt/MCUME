void write_ga(register uint16_t address, register uint8_t value);
void select_pen(register uint16_t address, register uint8_t value);
void select_pen_colour(register uint16_t address, register uint8_t value);
void rom_banking(register uint16_t address, register uint8_t value);

struct { unsigned char R,G,B;} Palette[27] = {
    { 0, 0, 0},       // Black
    { 0, 0, 128},     // Blue
    { 0, 0, 255},     // Bright blue
    { 128, 0, 0},     // Red
    { 128, 0, 128},   // Magenta
    { 128, 0, 255},   // Mauve
    { 255, 0, 0},     // Bright Red
    { 255, 0, 128},   // Purple
    { 255, 0, 255},   // Bright Magenta
    { 0, 128, 0},     // Green
    { 0, 128, 128},   // Cyan
    { 0, 128, 255},   // Sky Blue
    { 128, 128, 0},   // Yellow
    { 128, 128, 128}, // White
    { 128, 128, 255}, // Pastel Blue
    { 255, 128, 0},   // Orange
    { 255, 128, 128}, // Pink
    { 255, 128, 255}, // Pastel Magenta
    { 0, 255, 0},     // Bright Green
    { 0, 255, 128},   // Sea Green
    { 0, 255, 255},   // Bright Cyan
    { 128, 255, 0},   // Lime
    { 128, 255, 128}, // Pastel Green 
    { 128, 255, 255}, // Pastel Cyan
    { 255, 255, 0},   // Bright Yellow
    { 255, 255, 128}, // Pastel Yellow
    { 255, 255, 255}, // Bright White
};

#define PEN_NUMBER 4  // Mode 0 has 16 pens, mode 1 has 4 pens and mode 2 has 2 pens.

struct Registers {
    uint8_t PaletteIndex = 0;
    uint8_t PaletteData[PEN_NUMBER];
    uint8_t ScreenModeAndROMConfig = 0;
    uint8_t RAMBanking = 0; // unused in CPC 464
};