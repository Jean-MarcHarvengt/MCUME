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

#define CYCLES_PER_FRAME 19968
#define NBLINES          312
#define CYCLES_PER_STEP  CYCLES_PER_FRAME/NBLINES

/*
 * Declarations of instances of the RAM, VRAM, processor and other required components.
*/

char osROM[0x4000];
char basicROM[0x4000];
uint8_t RAM[0x10000];      // 64k
unsigned char* bitstream = 0; // 16k video ram to be used by PIO.
static Z80 CPU;
bool interruptGenerated = false;


/*
 * Implementations of system-specific emuapi Init, Step etc. functions.
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

void cpc_Start(char* filename)
{
    
    // Interrupts disabled, PC=0, enable low rom, disable high rom, gate array scanline counter = 0, write 0xC0 into GA.
    // set memory to 0.

    CPU.IFF = 0;
    CPU.PC.W = 0x0000;
    CPU.IRequest = INT_NONE;
    gaConfig.lowerROMEnable = true;
    gaConfig.upperROMEnable = false;
    gaConfig.interruptCounter = 0;
    writeGA(0xC0);
    // writeGA(0x89);
    // CPU.PC.W = 0x580;
}

void cpc_Step(void)
{
    // printf("Enter step\n");
    // RunZ80(&CPU);
    //printf("Current program counter: %d \n", CPU.PC.W);
    for(int i = 0; i < NBLINES; i++)
        {   
            ExecZ80(&CPU, CYCLES_PER_STEP);
            for(int j = 0; j < CYCLES_PER_STEP; j++)
            {    
                crtc_step();
                interruptGenerated = ga_step();
                if(interruptGenerated)
                {
                    printf("Interrupting! Jumping to \n");
                    IntZ80(&CPU, INT_IRQ); 
                    gaConfig.interruptCounter &= 0x1F;
                }
            }    
            emu_DrawLine8(bitstream, WIDTH, HEIGHT, i);
            //printf("index %d: %c \n", i, bitstream + i);
        }
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

// word LoopZ80(Z80 *R)
// {

//     if(interruptGenerated)
//     {
//         return INT_IRQ;
//     }
//     else
//     {
//         return INT_NONE;
//     }
// }

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
    
    //printf("Write %x at %x \n", Value, Addr);
}

void PatchZ80(Z80 *R)
{
    // do nothing
}

byte RdZ80(word Addr)
{
    if(Addr <= LOWER_ROM_END && gaConfig.lowerROMEnable)
    {
        // printf("At program counter %x, Z80 read from address %x in OS ROM\n", CPU.PC.W, Addr);
        return gb_rom_464_0[Addr];
    }
    else if(Addr >= UPPER_ROM_BEGIN && gaConfig.upperROMEnable)
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