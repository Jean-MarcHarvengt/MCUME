#include "pico.h"
#include "pico/stdlib.h"
#include "processor/Z80.h"
#include "roms/rom464.h"

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}

#include "crtc.h"
#include "ga.h"

/*
 * Declarations of instances of the RAM, VRAM, processor and other required components.
*/

static byte ram[0x10000];   // 64k
static byte bitstream[0x4000]; // 16k video ram to be used by PIO.
static Z80 cpu;
extern struct GA_Config ga_config;

/*
 * Implementations of system-specific emuapi Init, Step etc. functions.
*/

void cpc_Init(void)
{

}

void cpc_Step(void)
{

}

void cpc_Start(char* filename)
{

}

void cpc_Input(int bClick)
{

}

/*
 * System-specific implementations of the Z80 instructions required by the portable Z80 emulator.
*/

void OutZ80(register word Port, register byte Value)
{
    if(!(Port & 0x8000)) write_ga(Value);         // The Gate Array is selected when bit 15 is set to 0.
    if(!(Port & 0x4000)) write_crtc(Port, Value); // The CRTC is selected when bit 14 is set to 0. 
    if(!(Port & 0x2000)) 
    {
        // upper rom bank number. ROM banking needs to be done regardless of CPC model
        // The Upper ROM Bank Number (in range of 0x00..0xFF) to be mapped to memory at 0xC000..0xFFFF

        // byte req_bank_number = Value & 15;
        // if(ga_config.upper_rom_enable)
        // {
        // }
    }                        
}

byte InZ80(register word Port)
{
    if(!(Port & 0x4000)) read_crtc(Port); // The CRTC is selected when bit 14 is set to 0. 
    return 0xFF;
}

#define LOWER_ROM_END   0x4000
#define UPPER_ROM_BEGIN 0xC000

void WrZ80(register word Addr, register byte Value)
{
    ram[Addr] = Value;
}

byte RdZ80(register word Addr)
{
    if(Addr <= LOWER_ROM_END && ga_config.lower_rom_enable)
    {
        return gb_rom_464_0[Addr];
    }
    else if(Addr >= UPPER_ROM_BEGIN && ga_config.upper_rom_enable)
    {
        return gb_rom_464_1[Addr];
    }
    else
    {
        return ram[Addr];
    }
}