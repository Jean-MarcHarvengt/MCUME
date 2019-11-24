/*
    Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003  Charles Mac Donald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "shared.h"

//uint8 work_ram[0x10000];    /* 68K work RAM */
//uint8 zram[0x2000];         /* Z80 work RAM */
uint8 zbusreq;              /* /BUSREQ from Z80 */
uint8 zreset;               /* /RESET to Z80 */
uint8 zbusack;              /* /BUSACK to Z80 */
uint8 zirq;                 /* /IRQ to Z80 */
uint32 zbank;               /* Address of Z80 bank window */
uint8 gen_running;

/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown functions                                          */
/*--------------------------------------------------------------------------*/

void gen_init(void)
{
    sound_init();
    memset(&snd, 0, sizeof(snd));

    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();
    error("PC:%08X\tSP:%08X\n", m68k_get_reg(NULL,M68K_REG_PC), m68k_get_reg(NULL,M68K_REG_SP));
    gen_running = 1;
}

void gen_reset(void)
{
    /* Clear RAM */
    memset(work_ram, 0, WORK_RAM_SIZE);
    memset(zram, 0, Z_RAM_SIZE);

    gen_running = 1;
    zreset  = 0;    /* Z80 is reset */
    zbusreq = 0;    /* Z80 has control of the Z bus */
    zbusack = 1;    /* Z80 is busy using the Z bus */
    zbank   = 0;    /* Assume default bank is 000000-007FFF */
    zirq    = 0;    /* No interrupts occuring */

    io_reset();

    /* Reset the 68000 emulator */
    m68k_pulse_reset();
    error("PC:%08X\tSP:%08X\n", m68k_get_reg(NULL,M68K_REG_PC), m68k_get_reg(NULL,M68K_REG_SP));
    z80_reset(0);
    z80_set_irq_callback(z80_irq_callback);
}

void gen_shutdown(void)
{
    int i;

    /* Print 68K and Z80 state */
    error("\n");
    error("\nPC:%08X\tSP:%08X\tSR:%04X\n", m68k_get_reg(NULL,M68K_REG_PC), m68k_get_reg(NULL,M68K_REG_SP), m68k_get_reg(NULL, M68K_REG_SR));
    for(i=0;i<8;i++)
    {
        error("D%d:%08X\tA%d:%08X\n", 
            i, m68k_get_reg(NULL, M68K_REG_D0+i),
            i, m68k_get_reg(NULL, M68K_REG_A0+i));
    }

    error("\n");
    error("PC:%04X\tSP:%04X\n", z80_get_reg(Z80_PC), z80_get_reg(Z80_SP));
    error("AF:%04X\tAF:%04X\n", z80_get_reg(Z80_AF), z80_get_reg(Z80_AF2));
    error("BC:%04X\tBC:%04X\n", z80_get_reg(Z80_BC), z80_get_reg(Z80_BC2));
    error("DE:%04X\tDE:%04X\n", z80_get_reg(Z80_DE), z80_get_reg(Z80_DE2));
    error("HL:%04X\tHL:%04X\n", z80_get_reg(Z80_HL), z80_get_reg(Z80_HL2));
    error("IX:%04X\tIY:%04X\n", z80_get_reg(Z80_IX), z80_get_reg(Z80_IY));
    error("\n");
}

/*--------------------------------------------------------------------------*/
/* Bus controller chip functions                                            */
/*--------------------------------------------------------------------------*/

int gen_busack_r(void)
{
    return (zbusack & 1);
}

void gen_busreq_w(int state)
{
    zbusreq = (state & 1);
    zbusack = 1 ^ (zbusreq & zreset);

    if(zbusreq == 0 && zreset == 1)
    {
        z80_execute(32);
    }
}

void gen_reset_w(int state)
{
    zreset = (state & 1);
    zbusack = 1 ^ (zbusreq & zreset);

    if(zreset == 0)
    {
        if(snd.enabled)
        {
            YM2612ResetChip(0);
        }

        z80_reset(0);
        z80_set_irq_callback(z80_irq_callback);
    }
}


void gen_bank_w(int state)
{
    zbank = ((zbank >> 1) | ((state & 1) << 23)) & 0xFF8000;
}



int z80_irq_callback(int param)
{
    zirq = 0;
    z80_set_irq_line(0, CLEAR_LINE);
    return 0xFF;
}

int vdp_int_ack_callback(int int_level)
{
    switch(int_level)
    {
        case 4: 
            hint_pending = 0;
            break;

        case 6: 
            status &= ~0x0080;
            vint_pending = 0;
            break;
    }

    return M68K_INT_ACK_AUTOVECTOR;
}

