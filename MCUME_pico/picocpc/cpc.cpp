#include "pico.h"
#include "pico/stdlib.h"

#include "cpc.h"

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}
#include "processor/Z80.h"
#include "roms/rom464.h"
#include "crtc.h"
#include "ga.h"

/*
 * Declarations of instances of the RAM, VRAM, processor and other required components.
*/

uint8_t RAM[0x10000];   // 64k
uint8_t bitstream[0x4000]; // 16k video ram to be used by PIO.
static Z80 CPU;
extern struct GAConfig gateArray;

/*
 * Implementations of system-specific emuapi Init, Step etc. functions.
*/

void cpc_Init(void)
{

}

void cpc_Step(void)
{
    // probably where i will call crtc_step() and ga_step()
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

void OutZ80(word Port, byte Value)
{
    if(!(Port & 0x8000)) writeGA(Value);         // The Gate Array is selected when bit 15 is set to 0.
    if(!(Port & 0x4000)) writeCRTC(Port, Value); // The CRTC is selected when bit 14 is set to 0. 
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

byte InZ80(word Port)
{
    if(!(Port & 0x4000)) return readCRTC(Port); // The CRTC is selected when bit 14 is set to 0. 
    return 0xFF;
}

#define LOWER_ROM_END   0x4000
#define UPPER_ROM_BEGIN 0xC000

void WrZ80(word Addr, byte Value)
{
    RAM[Addr] = Value;
}

byte RdZ80(word Addr)
{
    if(Addr <= LOWER_ROM_END && gateArray.lowerROMEnable)
    {
        return gb_rom_464_0[Addr];
    }
    else if(Addr >= UPPER_ROM_BEGIN && gateArray.upperROMEnable)
    {
        return gb_rom_464_1[Addr];
    }
    else
    {
        return RAM[Addr];
    }
}