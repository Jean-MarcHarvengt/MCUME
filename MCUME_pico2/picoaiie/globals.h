#include <stdint.h>

#include "filemanager.h"
#include "cpu.h"
#include "vm.h"
#include "physicaldisplay.h"
#include "physicalkeyboard.h"
#include "physicalspeaker.h"
#include "physicalpaddles.h"
#include "physicalprinter.h"

extern FileManager *g_filemanager;
extern Cpu *g_cpu;
extern VM *g_vm;
extern PhysicalDisplay *g_display;
extern PhysicalKeyboard *g_keyboard;
extern PhysicalSpeaker *g_speaker;
extern PhysicalPaddles *g_paddles;
extern PhysicalPrinter *g_printer;
extern uint16_t g_battery;
extern uint16_t g_charge;
extern int16_t g_volume;
extern int8_t g_displayType;
extern uint8_t g_joyTrimX;
extern uint8_t g_joyTrimY;
extern uint8_t g_joySpeed;
extern uint8_t g_screenSync;
extern bool biosRequest;
