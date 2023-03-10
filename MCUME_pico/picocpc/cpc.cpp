#include "pico.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "cpc.h"

#include <cstring>
#include <stdlib.h>

extern "C" {
#include "emuapi.h"
#include "platform_config.h"
}
#ifndef CHIPS_IMPL
 #define CHIPS_IMPL
#endif
#include "z80.h"
#include "crtc.h"
#include "ga.h"
#include "roms/rom464.h"

#define WIDTH            320 
#define HEIGHT           200 
#define NBLINES          312
#define LOWER_ROM_END   0x4000
#define UPPER_ROM_BEGIN 0xC000

// Declarations of instances of the RAM, VRAM, processor and other required components.

uint8_t RAM[0x10000];         // 64k
unsigned char* bitstream = 0; // 16k video ram to be used by PIO.
static z80_t CPU;
uint64_t pins;
bool interrupt_generated = false;
int position = 0;
int width_count = 0;
int x, y = 0;

// Helper functions

void write_to_bitstream(char pixel)
{
    // this populates the bitstream.

    x = position % WIDTH;
    y = position / WIDTH;

    bitstream[x + y * WIDTH] = VGA_RGB(firmware_palette[hardware_colours[pixel]].R,
                                       firmware_palette[hardware_colours[pixel]].G,
                                       firmware_palette[hardware_colours[pixel]].B); 
    
    position++;
    
    if(position == WIDTH * HEIGHT)
    {
        position = 0;
        vsync_wait = true;
    }
    
}

static void display_screen()
{
    emu_DrawScreen(bitstream, WIDTH, HEIGHT, WIDTH);
    emu_DrawVsync();
}

char read_z80(uint16_t Addr)
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

void write_z80(uint16_t Addr, uint8_t Value)
{
    RAM[Addr] = Value;
}

void out_z80(uint16_t Port, uint8_t Value)
{
    if(!(Port & 0x8000)) write_gate_array(Value);           // The Gate Array is selected when bit 15 is set to 0.
    if(!(Port & 0x4000)) write_crt_controller(Port, Value); // The CRTC is selected when bit 14 is set to 0. 
    // if(!(Port & 0x2000)) 
    // {
    //     // upper rom bank number. ROM banking needs to be done regardless of CPC model
    //     // The Upper ROM Bank Number (in range of 0x00..0xFF) to be mapped to memory at 0xC000..0xFFFF

    //     // byte req_bank_number = Value & 15;
    //     // if(ga_config.upper_rom_enable)
    //     // {
    //     // }
    // }                        
}

uint8_t in_z80(uint16_t Port)
{
    if(!(Port & 0x4000)) return read_crt_controller(Port); // The CRTC is selected when bit 14 is set to 0. 
    return 0xFF;
}

/**
 * Creates initial emulation state (i.e. sets up color palette, clears memory etc.)
*/
void cpc_Init(void)
{
    for(int i = 0; i < 32; i++)
    {
        emu_SetPaletteEntry(firmware_palette[hardware_colours[i]].R, 
                            firmware_palette[hardware_colours[i]].G, 
                            firmware_palette[hardware_colours[i]].B, 
                            hardware_colours[i]);
    }
    if (bitstream == 0) bitstream = (unsigned char *)emu_Malloc(WIDTH*HEIGHT);

    pins = z80_init(&CPU);
    memset(RAM, 0, sizeof(RAM));
    //vsync_wait = true;
}

/**
 * Starts the emulator by setting the initial program counter and emulates initial hardware state.
*/
void cpc_Start(char* filename)
{

}

/**
 * Steps through emulation and renders the screen.
*/
void cpc_Step(void)
{
    // if not (z80 wait and ga wait) then tick, otherwise stall.
    // or rather the tick will be a stall if both waits are asserted, i think that's better.
    bool interrupt_acknowledged = false;
    pins = z80_tick(&CPU, pins);

    if (pins & Z80_MREQ) 
    {
        const uint16_t addr = Z80_GET_ADDR(pins);
        if (pins & Z80_RD) 
        {
            uint8_t data = read_z80(addr);
            Z80_SET_DATA(pins, data);
        }
        else if (pins & Z80_WR) 
        {
            uint8_t data = Z80_GET_DATA(pins);
            write_z80(addr, data);
        }
    }
    else if (pins & Z80_IORQ) 
    {
        const uint16_t port = Z80_GET_ADDR(pins);
        if (pins & Z80_M1) 
        {
            // an interrupt acknowledge cycle, depending on the emulated system,
            // put either an instruction byte, or an interrupt vector on the data bus
            Z80_SET_DATA(pins, 0x0038);
            interrupt_acknowledged = true;
        }
        else if (pins & Z80_RD) 
        {
            // handle IO input request at port
            in_z80(port);
        }
        else if (pins & Z80_WR) 
        {
            // handle IO output request at port
            uint8_t data = Z80_GET_DATA(pins);
            out_z80(port, data);
        }
    }

    crtc_step();
    interrupt_generated = ga_step();
    if(ga_config.wait_signal)
    {
        // printf("Waiting in the next cycle.\n");
        pins = pins | Z80_WAIT;
    }
    else
    {
        // printf("Not waiting in the next cycle.\n");
        pins = pins & ~Z80_WAIT;
    }

    if(interrupt_generated)
    {
        // To request an interrupt, or inject a wait state just set the respective pin
        // (Z80_INT, Z80_NMI, Z80_WAIT), don't forget to clear the pin again later (the
        // details on when those pins are set and cleared depend heavily on the
        // emulated system).

        // request an interrupt from the CPU
        // TODO how to set the Z80_INT pin?
        pins = pins | Z80_INT;
    }

    if(interrupt_acknowledged)
    {
        pins = pins & ~Z80_INT;
        ga_config.interrupt_counter &= 0x1f;
    }

    if(!vsync_wait)
    {
        display_screen();
        vsync_wait = true;
    }
}

void cpc_Input(int bClick)
{

}