#include "pico.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "cpc.h"

#include <cstring>

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

uint8_t RAM[0x10000];      // 64k
uint8_t bitstream[0x4000]; // 16k video ram to be used by PIO.
static Z80 CPU;
bool interruptGenerated = false;


/*
 * Implementations of system-specific emuapi Init, Step etc. functions.
*/
#define CYCLES_PER_FRAME 80000

#define NBLINES (1) //(48+192+56+16) //(32+256+32)
#define CYCLES_PER_STEP (CYCLES_PER_FRAME/NBLINES)

void cpc_Init(void)
{
    for(int i = 0; i < PALETTE_SIZE; i++)
    {
        // R, G and B are not absolute RGB, they are percentegaes. TODO change that.
        emu_SetPaletteEntry(palette[i].R, palette[i].G, palette[i].B, i);
    }

    //memset(RAM, 0, sizeof(RAM));

    //ResetZ80(&CPU, CYCLES_PER_FRAME);
}

void cpc_Step(void)
{
    // printf("Enter step\n");

    // TODO I don't think I am actually loading the ROM and having the CPU start executing w/e's there.
    
    for(int i = 0; i<CYCLES_PER_FRAME; i++)
    {
        ExecZ80(&CPU, 1);

        crtc_step();
        interruptGenerated = ga_step();
        if(interruptGenerated)
        {
            CPU.IRequest = INT_RST38; 
            gaConfig.interruptCounter &= 0x1F;
        }
        else
        {
            CPU.IRequest = INT_NONE;
        }
        // printf("Executed %d steps in hardware\n", 256);
    }

}

void cpc_Start(char* filename)
{

}

void cpc_Input(int bClick)
{

}

void displayScreen()
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

void PatchZ80(Z80 *R)
{
    // do nothing
}

byte RdZ80(word Addr)
{
    if(Addr <= LOWER_ROM_END && gaConfig.lowerROMEnable)
    {
        // printf("Reading from the OS ROM.\n");
        return gb_rom_464_0[Addr];
    }
    else if(Addr >= UPPER_ROM_BEGIN && gaConfig.upperROMEnable)
    {
        // printf("Reading from BASIC's ROM.\n");
        return gb_rom_464_1[Addr - 0xC000];
    }
    else
    {
        // printf("Reading from RAM.\n");
        return RAM[Addr];
    }
}