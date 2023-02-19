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
#include "crtc.h"
#include "ga.h"
#include "roms/rom464.h"

#define WIDTH            320
#define HEIGHT           200
#define CYCLES_PER_FRAME 19968 //79872
#define NBLINES          312
#define CYCLES_PER_SCANLINE  CYCLES_PER_FRAME/NBLINES
#define LOWER_ROM_END   0x4000
#define UPPER_ROM_BEGIN 0xC000

// Declarations of instances of the RAM, VRAM, processor and other required components.

uint8_t RAM[0x10000];      // 64k
unsigned char* bitstream = 0; // 16k video ram to be used by PIO.
static Z80 CPU;
bool interrupt_generated = false;

// Implementations of system-specific emuapi Init, Step etc. functions. 

/**
 * Creates initial emulation state (i.e. sets up color palette, clears memory etc.)
*/
void cpc_Init(void)
{

    for(int i = 0; i < PALETTE_SIZE; i++)
    {
        emu_SetPaletteEntry(palette[i].R, palette[i].G, palette[i].B, i);
    }
    if (bitstream == 0) bitstream = (unsigned char *)emu_Malloc(WIDTH*HEIGHT);

    ResetZ80(&CPU, CYCLES_PER_FRAME);
    memset(RAM, 0, sizeof(RAM));
}

/**
 * Starts the emulator by setting the initial program counter and emulates initial hardware state.
*/
void cpc_Start(char* filename)
{
    CPU.PC.W = 0x0000;
    ga_config.lower_rom_enable = true;
    ga_config.upper_rom_enable = false;
    ga_config.interrupt_counter = 0;
    // write_gate_array(0xC0);
}

/**
 * Steps through emulation and renders the screen.
*/
void cpc_Step(void)
{
    int total_cycles = 0;
    int cycles_left = 0;
    int cycles_taken = 0;

    for(int i = 0; i < NBLINES; i++)
    {   // TODO find a way to call crtc and ga step after each instruction, not after
        // executing everything in the scanline (not guaranteed to be 80000 cycles anyway).
        
        cycles_left = ExecZ80(&CPU, 1);
        cycles_taken += 1;
        while(cycles_left < 0)
        {
            cycles_left -= ExecZ80(&CPU, 1);
            cycles_taken += 1;
        }

        for(int j = 0; j < cycles_taken; j++)
        {
            crtc_step();
            interrupt_generated = ga_step();
            if(interrupt_generated)
            {
                //printf("Interrupting! Jumping to \n");
                IntZ80(&CPU, INT_IRQ);
                ga_config.interrupt_counter &= 0x1F;
            }
        }
        
        emu_DrawLine8(bitstream, WIDTH, HEIGHT, i);
        cycles_taken = 0;
    }
}

/**
 * Input handler.
*/
void cpc_Input(int bClick)
{

}


// System-specific implementations of the Z80 instructions required by the portable Z80 emulator.

void OutZ80(word Port, byte Value)
{
    if(!(Port & 0x8000)) write_gate_array(Value);           // The Gate Array is selected when bit 15 is set to 0.
    if(!(Port & 0x4000)) write_crt_controller(Port, Value); // The CRTC is selected when bit 14 is set to 0. 
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
    if(!(Port & 0x4000)) return read_crt_controller(Port); // The CRTC is selected when bit 14 is set to 0. 
    return 0xFF;
}

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
    if(Addr <= LOWER_ROM_END && ga_config.lower_rom_enable)
    {
        // printf("At program counter %x, Z80 read from address %x in OS ROM\n", CPU.PC.W, Addr);
        return gb_rom_464_0[Addr];
    }
    else if(Addr >= UPPER_ROM_BEGIN && ga_config.upper_rom_enable)
    {
        // printf("At program counter %x, Z80 read from address %x in BASIC ROM\n", CPU.PC.W, Addr);
        return gb_rom_464_1[Addr - 0xC000];
    }
    else
    {
        // printf("At program counter %x, Z80 read from address %x in RAM\n", CPU.PC.W, Addr);
        return RAM[Addr];
    }
}