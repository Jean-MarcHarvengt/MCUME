#include "globals.h"

FileManager *g_filemanager = NULL;
Cpu *g_cpu = NULL;
VM *g_vm = NULL;
PhysicalDisplay *g_display = NULL;
PhysicalKeyboard *g_keyboard = NULL;
PhysicalSpeaker *g_speaker = NULL;
PhysicalPaddles *g_paddles = NULL;
PhysicalPrinter *g_printer = NULL;
uint16_t g_battery = 0;
uint16_t g_charge = 0;
int16_t g_volume = 15;
int8_t g_displayType = 3; // FIXME m_perfectcolor
uint8_t g_joyTrimX = 127;
uint8_t g_joyTrimY = 127;
uint8_t g_joySpeed = 5;
uint8_t g_screenSync = true;
bool biosRequest = false;
