#include "processor/Z80.h"
#include "roms/rom464.h"
#include "emuapi.h"
#include "crtc.h"
#include "ga.h"

/*
 * The implementations of the Z80 instructions required by the portable Z80 emulator.
 * These implementations are system-specific.
*/
void OutZ80(register word Port, register byte Value)
{
    if(!(Port & 0x8000)) write_ga(Port, Value); // The Gate Array is selected when bit 15 is set to 0.
    if(!(Port & 0x4000)) write_crtc(Port, Value); // The CRTC is selected when bit 14 is set to 0. 
    return;
}

byte InZ80(register word Port)
{
    if(!(Port & 0x4000)) read_crtc(Port); // The CRTC is selected when bit 14 is set to 0. 
    return 0xFF;
}

void WrZ80(register word Addr,register byte Value)
{
    return;
}

byte RdZ80(register word Addr)
{
    return 0xFF;
}

void PatchZ80(register Z80 *R)
{
    return;
}

word LoopZ80(register Z80 *R)
{
    return 0xFFFF;
}

// void JumpZ80(word PC)
// {
//     return;
// }

/*
 * Non-Z80 stuff.
*/