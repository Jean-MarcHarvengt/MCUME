/**
 * Gate Array is connected to a 16Mhz clock signal, which is then divided to 4Mhz to feed the Z80, and 1Mhz for the CRTC. 
 * Gate Array's READY signal is connected to Z80's WAIT input for "memory contention". 
 * This means CPU execution is halted every time Gate Array and CPU accesses the memory at the same time. 
 * The reason for this is that both GA and the CPU share the same address/data bus for memory access.
*/
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