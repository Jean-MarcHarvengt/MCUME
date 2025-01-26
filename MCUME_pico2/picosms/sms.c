#include "shared.h"

/* SMS context */
t_sms sms;
t_input input;       /* Controller input */

/* Run the virtual console emulation for one frame */
void sms_frame(int skip_render)
{
    /* Take care of hard resets */
    if(input.system & INPUT_HARD_RESET)
    {
        system_reset();
    }

    /* Debounce pause key */
    if(input.system & INPUT_PAUSE)
    {
        if(!sms.paused)
        {
            sms.paused = 1;

            z80_set_nmi_line(ASSERT_LINE);
            z80_set_nmi_line(CLEAR_LINE);
        }
    }
    else
    {
         sms.paused = 0;
    }

    if(smssnd.log) smssnd.callback(0x00);

    for(vdp.line = 0; vdp.line < 262; vdp.line += 1)
    {
        /* Handle VDP line events */
        vdp_run();

        /* Draw the current frame */
        if(!skip_render) render_line(vdp.line);

        /* Run the Z80 for a line */
        z80_execute(227);
    }
}

void audio_play_sample(int16 *bufl, int16 *bufr, int length)
{
    length = length/2;

    if(smssnd.enabled) 
    {
        int count;

        SN76496Update(0, smssnd.psg_buffer, length, sms.psg_mask);
        if(sms.use_fm) YM3812UpdateOne(ym3812, smssnd.fm_buffer, length);

        for(count = 0; count < length; count += 1)
        {
            signed short left   = 0;
            signed short right  = 0;
            if (sms.use_fm) left = right = smssnd.fm_buffer[count];
            left  += smssnd.psg_buffer[0][count];
            right += smssnd.psg_buffer[1][count];
            *bufl++ = left;
            *bufl++ = right;
        }
    }
     
}



void sms_init(void)
{
#if PSX
    z80_set_context((void *)0x1F800000);
#endif
    cpu_reset();
    sms_reset();
}


void sms_reset(void)
{
    memset(&input, 0, sizeof(t_input));

    /* Clear SMS context */
    memset(sms.dummy, 0, 0x2000);
    memset(sms.ram, 0, 0x2000);
    memset(sms.sram, 0, 0x8000);
    sms.paused = sms.save = sms.port_3F = sms.port_F2 = sms.irq = 0x00;
    sms.psg_mask = 0xFF;

    /* Load memory maps with default values */
    cpu_readmap[0] = 0x0000;
    cpu_readmap[1] = 0x2000;
    cpu_readmap[2] = 0x4000;
    cpu_readmap[3] = 0x6000;
    cpu_readmap[4] = 0x0000;
    cpu_readmap[5] = 0x2000;
    cpu_readmap[6] = sms.ram;            
    cpu_readmap[7] = sms.ram;

    cpu_writemap[0] = sms.dummy;         
    cpu_writemap[1] = sms.dummy;
    cpu_writemap[2] = sms.dummy;         
    cpu_writemap[3] = sms.dummy;
    cpu_writemap[4] = sms.dummy;         
    cpu_writemap[5] = sms.dummy;
    cpu_writemap[6] = sms.ram;           
    cpu_writemap[7] = sms.ram;

    sms.fcr[0] = 0x00;
    sms.fcr[1] = 0x00;
    sms.fcr[2] = 0x01;
    sms.fcr[3] = 0x00;
}


/* Reset Z80 emulator */
void cpu_reset(void)
{
    z80_reset(0);
    z80_set_irq_callback(sms_irq_callback);
}


/* Write to memory */
void cpu_writemem16(int address, int data)
{
    cpu_writemap[(address >> 13)][(address & 0x1FFF)] = data;
    if(address >= 0xFFFC) sms_mapper_w(address & 3, data);
}


/* Write to an I/O port */
void cpu_writeport(int port, int data)
{
    switch(port & 0xFF)
    {
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            break;

        case 0x06: /* GG STEREO */
            if(smssnd.log) {
            smssnd.callback(0x04);
            smssnd.callback(data);
            }
            sms.psg_mask = (data & 0xFF);
            break;

        case 0x7E: /* SN76489 PSG */
        case 0x7F:
            if(smssnd.log) {
            smssnd.callback(0x03);
            smssnd.callback(data);
            }
            if(smssnd.enabled) SN76496Write(0, data);
            break;

        case 0xBE: /* VDP DATA */
            vdp_data_w(data);
            break;

        case 0xBD: /* VDP CTRL */ 
        case 0xBF:
            vdp_ctrl_w(data);
            break;

        case 0xF0: /* YM2413 */
        case 0xF1:
            if(smssnd.log) {
            smssnd.callback((port & 1) ? 0x06 : 0x05);
            smssnd.callback(data);
            }
            if(smssnd.enabled && sms.use_fm) ym2413_write(0, port & 1, data);
            break;

        case 0xF2: /* YM2413 DETECT */
            if(sms.use_fm) sms.port_F2 = (data & 1);
            break;

        case 0x3F: /* TERRITORY CTRL. */
             sms.port_3F = ((data & 0x80) | (data & 0x20) << 1) & 0xC0;
            if(sms.country == TYPE_DOMESTIC) sms.port_3F ^= 0xC0;
            break;
    }
}


/* Read from an I/O port */
int cpu_readport(int port)
{
    byte temp = 0xFF;

    switch(port & 0xFF)
    {
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            return (0x00);
    
        case 0x7E: /* V COUNTER */
            return (vdp_vcounter_r());
            break;
    
        case 0x7F: /* H COUNTER */
            return (vdp_hcounter_r());
            break;
    
        case 0x00: /* INPUT #2 */
            temp = 0xFF;
            if(input.system & INPUT_START) temp &= ~0x80;
            if(sms.country == TYPE_DOMESTIC) temp &= ~0x40;
            return (temp);
    
        case 0xC0: /* INPUT #0 */  
        case 0xDC:
            temp = 0xFF;
            if(input.pad[0] & INPUT_UP)      temp &= ~0x01;
            if(input.pad[0] & INPUT_DOWN)    temp &= ~0x02;
            if(input.pad[0] & INPUT_LEFT)    temp &= ~0x04;
            if(input.pad[0] & INPUT_RIGHT)   temp &= ~0x08;
            if(input.pad[0] & INPUT_BUTTON2) temp &= ~0x10;
            if(input.pad[0] & INPUT_BUTTON1) temp &= ~0x20;
            if(input.pad[1] & INPUT_UP)      temp &= ~0x40;
            if(input.pad[1] & INPUT_DOWN)    temp &= ~0x80;
            return (temp);
    
        case 0xC1: /* INPUT #1 */
        case 0xDD:
            temp = 0xFF;
            if(input.pad[1] & INPUT_LEFT)    temp &= ~0x01;
            if(input.pad[1] & INPUT_RIGHT)   temp &= ~0x02;
            if(input.pad[1] & INPUT_BUTTON2) temp &= ~0x04;
            if(input.pad[1] & INPUT_BUTTON1) temp &= ~0x08;
            if(input.system & INPUT_SOFT_RESET) temp &= ~0x10;
            return ((temp & 0x3F) | (sms.port_3F & 0xC0));

        case 0xBE: /* VDP DATA */
            return (vdp_data_r());
    
        case 0xBD:
        case 0xBF: /* VDP CTRL */
            return (vdp_ctrl_r());

        case 0xF2: /* YM2413 DETECT */
            if(sms.use_fm) return (sms.port_F2);
            break;
    }
    return (0xFF);     
}


void sms_mapper_w(int address, int data)
{
    /* Calculate ROM page index */
    byte page = (data % cart.pages);

    /* Save frame control register data */
    sms.fcr[address] = data;

    switch(address)
    {
        case 0:
            if(data & 8)
            {
                sms.save = 1;
                /* Page in ROM */
                cpu_readmap[4]  = &sms.sram[(data & 4) ? 0x4000 : 0x0000];
                cpu_readmap[5]  = &sms.sram[(data & 4) ? 0x6000 : 0x2000];
                cpu_writemap[4] = &sms.sram[(data & 4) ? 0x4000 : 0x0000];
                cpu_writemap[5] = &sms.sram[(data & 4) ? 0x6000 : 0x2000];
            }
            else
            {
                /* Page in RAM */
                cpu_readmap[4]  = ((sms.fcr[3] % cart.pages) << 14) + 0x0000;
                cpu_readmap[5]  = ((sms.fcr[3] % cart.pages) << 14) + 0x2000;
                cpu_writemap[4] = sms.dummy;
                cpu_writemap[5] = sms.dummy;
            }
            break;

        case 1:
            cpu_readmap[0] = (page << 14) + 0x0000;
            cpu_readmap[1] = (page << 14) + 0x2000;
            break;

        case 2:
            cpu_readmap[2] = (page << 14) + 0x0000;
            cpu_readmap[3] = (page << 14) + 0x2000;
            break;

        case 3:
            if(!(sms.fcr[0] & 0x08))
            {
                cpu_readmap[4] = (page << 14) + 0x0000;
                cpu_readmap[5] = (page << 14) + 0x2000;
            }
            break;
    }
}


int sms_irq_callback(int param)
{
    return (0xFF);
}
