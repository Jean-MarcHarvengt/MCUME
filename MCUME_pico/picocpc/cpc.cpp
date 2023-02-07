#include "processor/Z80.h"
#include "roms/rom464.h"
#include "emuapi.h"
#include "crtc.h"
#include "ga.h"

/*
 * Declarations of the RAM, VRAM, processor instances.
*/
static byte z80RAM[0x10000];   // 64k
static byte screenRAM[0x4000]; // 16k
static Z80 cpu;

/*
 * Implementation of cpc.h which is used by emuapi.h to define emu_Init() etc.
*/

void cpc_Init(void)
{
    return;
}

void cpc_Step()
{
    return;
}

void cpc_Start(char* filename)
{
    return;
}

void cpc_Input(int bClick)
{
    return;
}

/*
 * The implementations of the Z80 instructions required by the portable Z80 emulator.
 * These implementations are system-specific.
*/
void OutZ80(register word Port, register byte Value)
{
    if(!(Port & 0x8000)) write_ga(Port, Value);   // The Gate Array is selected when bit 15 is set to 0.
    if(!(Port & 0x4000)) write_crtc(Port, Value); // The CRTC is selected when bit 14 is set to 0. 
    if(!(Port & 0x2000)) ;                        // upper rom bank number. ROM banking needs to be done regardless of CPC model
}

byte InZ80(register word Port)
{
    if(!(Port & 0x4000)) read_crtc(Port); // The CRTC is selected when bit 14 is set to 0. 
    return 0xFF;
}

#define RAM_BASE 0x4000 // The ROM takes up 0x0000-0x4000. From 0x4000-0x10000 is the RAM.

/**
 * Write the byte Value into the address Addr. If Addr is less than RAM_BASE,
 * then we do not write anything as it would be written into the ROM.
*/
void WrZ80(register word Addr, register byte Value)
{
    if(Addr >= RAM_BASE)
    {
        z80RAM[Addr - RAM_BASE] = Value;
    }
}

byte RdZ80(register word Addr)
{
    if(Addr < RAM_BASE)
    {
        return gb_rom_464_0[Addr];
    }
    else
    {
        return z80RAM[Addr - RAM_BASE];
    }
}