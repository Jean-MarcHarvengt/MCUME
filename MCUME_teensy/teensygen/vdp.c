
#include "shared.h"
#include "vcnt.h"
#include "hcnt.h"
#include "hvc.h"

/* Pack and unpack CRAM data */
#define PACK_CRAM(d)    ((((d)&0xE00)>>9)|(((d)&0x0E0)>>2)|(((d)&0x00E)<<5))
#define UNPACK_CRAM(d)  ((((d)&0x1C0)>>5)|((d)&0x038)<<2|(((d)&0x007)<<9))

/* Mark a pattern as dirty */
#define MARK_BG_DIRTY(addr)                                     \
{                                                               \
    int name = (addr >> 5) & 0x7FF;                             \
    if(bg_name_dirty[name] == 0)                                \
    {                                                           \
        bg_name_list[bg_list_index] = name;                     \
        bg_list_index += 1;                                     \
    }                                                           \
    bg_name_dirty[name] |= (1 << ((addr >> 2) & 0x07));         \
}

/* Tables that define the playfield layout */
uint8 shift_table[] = {6, 7, 0, 8};
uint8 col_mask_table[] = {0x1F, 0x3F, 0x1F, 0x7F};
uint16 row_mask_table[] = {0x0FF, 0x1FF, 0x2FF, 0x3FF};
uint32 y_mask_table[] = {0x1FC0, 0x1FC0, 0x1F80, 0x1F00};

uint8 sat[0x400];               /* Internal copy of sprite attribute table */
//uint8 vram[0x10000];            /* Video RAM (64Kx8) */
uint8 cram[0x80];               /* On-chip color RAM (64x9) */
uint8 vsram[0x80];              /* On-chip vertical scroll RAM (40x11) */
uint8 reg[0x20];                /* Internal VDP registers (23x8) */
uint16 addr;                    /* Address register */
uint16 addr_latch;              /* Latched A15, A14 of address */
uint8 code;                     /* Code register */
uint8 pending;                  /* Pending write flag */
uint16 buffer;                  /* Read buffer */
uint16 status;                  /* VDP status flags */
uint16 ntab;                    /* Name table A base address */
uint16 ntbb;                    /* Name table B base address */
uint16 ntwb;                    /* Name table W base address */
uint16 satb;                    /* Sprite attribute table base address */
uint16 hscb;                    /* Horizontal scroll table base address */
uint16 sat_base_mask;           /* Base bits of SAT */
uint16 sat_addr_mask;           /* Index bits of SAT */
uint8 is_color_dirty;           /* 1= One or more colors have changed */
uint8 color_dirty[0x40];        /* 1= This color is dirty */
uint8 border;                   /* Border color index */
uint8 is_border_dirty;          /* 1= The border color has changed */
//uint8 bg_name_dirty[0x800];     /* 1= This pattern is dirty */
//uint16 bg_name_list[0x800];     /* List of modified pattern indices */
uint16 bg_list_index;           /* # of modified patterns in list */
//uint8 bg_pattern_cache[0x80000];/* Cached and flipped patterns */
uint8 playfield_shift;          /* Width of planes A, B (in bits) */
uint8 playfield_col_mask;       /* Vertical scroll mask */
uint16 playfield_row_mask;      /* Horizontal scroll mask */
uint32 y_mask;                  /* Name table Y-index bits mask */
int hint_pending;               /* 0= Line interrupt is pending */
int vint_pending;               /* 1= Frame interrupt is pending */
int counter;                    /* Raster counter */
int dma_fill;                   /* 1= DMA fill has been requested */
int im2_flag;                   /* 1= Interlace mode 2 is being used */
int frame_end;                  /* End-of-frame (IRQ line) */
int v_counter;                  /* VDP scan line counter */
int v_update;                  /* 1= VC was updated by a ctrl or HV read */
void (*color_update)(int index, uint16 data);

/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown functions                                          */
/*--------------------------------------------------------------------------*/

void vdp_init(void)
{
}

void vdp_reset(void)
{
    memset(sat, 0, sizeof(sat));
    memset(vram, 0, VRAM_SIZE);
    memset(cram, 0, sizeof(cram));
    memset(vsram, 0, sizeof(vsram));
    memset(reg, 0, sizeof(reg));

    addr = addr_latch = code = pending = buffer = status = 0;
    ntab = ntbb = ntwb = satb = hscb = 0;
    sat_base_mask = 0xFE00;
    sat_addr_mask = 0x01FF;

    /* Mark all colors as dirty to force a palette update */
    is_color_dirty = 1;
    memset(color_dirty, 1, 0x40);
    border = 0x00;
    is_border_dirty = 1;

    memset(bg_name_dirty, 0, BGNAMEDIRTY_SIZE);
    memset(bg_name_list, 0, BGNAME_SIZE);
    bg_list_index = 0;
    memset(bg_pattern_cache, 0, BGPATTERN_CACH_SIZE);

    playfield_shift = 6;
    playfield_col_mask = 0x1F;
    playfield_row_mask = 0x0FF;
    y_mask = 0x1FC0;

    hint_pending = vint_pending = 0;
    counter = 0;
    frame_end = 0xE0;
    v_counter = v_update = 0;

    /* Initialize viewport */
    gbitmap.viewport.x = 0x20;
    gbitmap.viewport.y = 0x20;
    gbitmap.viewport.w = 256;
    gbitmap.viewport.h = 224;
    gbitmap.viewport.oh = 256;
    gbitmap.viewport.ow = 224;
    gbitmap.viewport.changed = 1;
}

void vdp_shutdown(void)
{
}


/*--------------------------------------------------------------------------*/
/* Memory access functions                                                  */
/*--------------------------------------------------------------------------*/

void vdp_ctrl_w(uint16 data)
{
    if(pending == 0)
    {
        if((data & 0xC000) == 0x8000)
        {
            uint8 r = (data >> 8) & 0x1F;
            uint8 d = (data >> 0) & 0xFF;
            vdp_reg_w(r, d);
        }
        else
        {
            pending = 1;
        }

        addr = ((addr_latch & 0xC000) | (data & 0x3FFF)) & 0xFFFF;
        code = ((code & 0x3C) | ((data >> 14) & 0x03)) & 0x3F;
    }
    else
    {
        /* Clear pending flag */
        pending = 0;

        /* Update address and code registers */
        addr = ((addr & 0x3FFF) | ((data & 3) << 14)) & 0xFFFF;
        code = ((code & 0x03) | ((data >> 2) & 0x3C)) & 0x3F;

        /* Save address bits A15 and A14 */
        addr_latch = (addr & 0xC000);

        if((code & 0x20) && (reg[1] & 0x10))
        {
            switch(reg[23] & 0xC0)
            {
                case 0x00: /* V bus to VDP DMA */
                case 0x40: /* V bus to VDP DMA */
                    dma_vbus();
                    break;

                case 0x80: /* VRAM fill */
                    dma_fill = 1;
                    break;

                case 0xC0: /* VRAM copy */
                    dma_copy();
                    break;
            }
        }
    }
}

uint16 vdp_ctrl_r(void)
{
#if 0
    int cycles = m68k_cycles_run();
    uint16 temp = status;

    /* Clear pending flag */
    pending = 0;

    /* VBlank flag is set when the screen is disabled */
    if((reg[1] & 0x40) == 0x00)
    {
        temp |= 0x0008;
    }

    /* Clear collision flag on reads */
    status &= ~0x0020;

    /* Set HBlank flag based on H counter */
    if(reg[12] & 1)
    {
        int hc = cycle2hc40[(cycles % 487)];
        if((hc >= 0xB6) && (hc <= 0xFF)) temp |= 0x0004;
    }
    else
    {
        int hc = cycle2hc32[(cycles % 487)];
        if((hc >= 0x93) && (hc <= 0xFF)) temp |= 0x0004;
    }

    /* Flag FIFO as empty */
    temp |= 0x0200;     

    /* Clear unused bits */
    temp &= 0x03FF;  

    return (temp);
#else
    uint16 temp = (0x4e71 & 0xFC00);
    pending = 0;
    status &= ~0x0020; // clear sprite hit flag on reads
    status |= 0x0200;  // fifo empty
    status ^= 0x0004; /* hack (red zone) */
    temp |= (status & 0x03BF); // clear spr over
    return (temp);
#endif
}


void vdp_data_w(uint16 data)
{
    /* Clear pending flag */
    pending = 0;

    switch(code & 0x0F)
    {
        case 0x01: /* VRAM */
            /* Byte-swap data if A0 is set */
            if(addr & 1) data = (data >> 8) | (data << 8);

            /* Copy SAT data to the internal SAT */
            if((addr & sat_base_mask) == satb)
            {
                *(uint16 *)&sat[addr & sat_addr_mask] = data;
            }

            /* Only write unique data to VRAM */
            if(data != *(uint16 *)&vram[addr & 0xFFFE])
            {
                /* Write data to VRAM */
                *(uint16 *)&vram[addr & 0xFFFE] = data;

                /* Update the pattern cache */
                MARK_BG_DIRTY(addr);
            }
            break;

        case 0x03: /* CRAM */
            {
                uint16 *p = (uint16 *)&cram[(addr & 0x7E)];
                data &= 0x0EEE;
                if(data != *p)
                {
                    int index = (addr >> 1) & 0x3F;
                    *p = PACK_CRAM(data);

                    if((index & 0x0F) != 0x00)
                    {
                        color_dirty[index] = is_color_dirty = 1;
                    }

                    if(index == border)
                    {
                        is_border_dirty = 1;
                        if(color_update)
                        {
                            color_update(0x00, *p);
                            color_update(0x40, *p);
                            color_update(0x80, *p);
                        }
                    }

                    if(color_update) color_update(index, *p);
                }
            }
            break;
          
        case 0x05: /* VSRAM */
            *(uint16 *)&vsram[(addr & 0x7E)] = data;
            break;
    }

    /* Bump address register */
    addr += reg[15];

    if(dma_fill)
    {
        int length = (reg[20] << 8 | reg[19]) & 0xFFFF;
        if(!length) length = 0x10000;

        do {
            vram[(addr & 0xFFFF)] = (data >> 8) & 0xFF;
            MARK_BG_DIRTY(addr);
            addr += reg[15];
        } while(--length);
        dma_fill = 0;
    }
}


uint16 vdp_data_r(void)
{
    uint16 temp = 0;

    /* Clear pending flag */
    pending = 0;

    switch(code & 0x0F)
    {
        case 0x00: /* VRAM */
            temp = *(uint16 *)&vram[(addr & 0xFFFE)];
            break;

        case 0x08: /* CRAM */
            temp = *(uint16 *)&cram[(addr & 0x7E)];
            temp = UNPACK_CRAM(temp);
            break;

        case 0x04: /* VSRAM */
            temp = *(uint16 *)&vsram[(addr & 0x7E)];
            break;
    }

    /* Bump address register */
    addr += reg[15];

    /* return data */
    return (temp);
}


/*
    The reg[] array is updated at the *end* of this function, so the new
    register data can be compared with the previous data.
*/
void vdp_reg_w(uint8 r, uint8 d)
{
    switch(r)
    {
        case 0x00: /* CTRL #1 */

            if(hint_pending)
            {
                if(d & 0x10)
                {
                    m68k_set_irq(4);
                }
                else
                {
                    /* Cancel pending level 4 interrupt */
                    m68k_set_irq(0);
                }
            }
            break;

        case 0x01: /* CTRL #2 */

            if(vint_pending)
            {
                if(d & 0x20)
                {
                    m68k_set_irq(6);
                }
                else
                {
                    /* Cancel pending level 6 interrupt */
                    m68k_set_irq(0);
                }
            }

            /* Change the frame timing */
            frame_end = (d & 8) ? 0xF0 : 0xE0;

            /* Check if the viewport height has actually been changed */
            if((reg[1] & 8) != (d & 8))
            {                
                /* Update the height of the viewport */
                gbitmap.viewport.oh = gbitmap.viewport.h;
                gbitmap.viewport.h = (d & 8) ? 240 : 224;
                gbitmap.viewport.changed = 1;                
            }
            break;

        case 0x02: /* NTAB */
            ntab = (d << 10) & 0xE000;
            break;

        case 0x03: /* NTWB */
            ntwb = (d << 10) & 0xF800;
            if(reg[12] & 1) ntwb &= 0xF000;
            break;

        case 0x04: /* NTBB */
            ntbb = (d << 13) & 0xE000;
            break;

        case 0x05: /* SATB */
            sat_base_mask = (reg[12] & 1) ? 0xFC00 : 0xFE00;
            sat_addr_mask = (reg[12] & 1) ? 0x03FF : 0x01FF;
            satb = (d << 9) & sat_base_mask;
            break;

        case 0x07:
            d &= 0x3F;

            /* Check if the border color has actually changed */
            if(border != d)
            {
                /* Mark the border color as modified */
                border = d;
                is_border_dirty = 1;
                if(color_update)
                {
                    color_update(0x00, *(uint16 *)&cram[(border << 1)]);
                    color_update(0x40, *(uint16 *)&cram[(border << 1)]);
                    color_update(0x80, *(uint16 *)&cram[(border << 1)]);
                }
            }
            break;

        case 0x0C:

            /* Check if the viewport width has actually been changed */
            if((reg[0x0C] & 1) != (d & 1))
            {
                /* Update the width of the viewport */
                gbitmap.viewport.ow = gbitmap.viewport.w;
                gbitmap.viewport.w = (d & 1) ? 320 : 256;
                gbitmap.viewport.changed = 1;
            }

            /* See if the S/TE mode bit has changed */
            if((reg[0x0C] & 8) != (d & 8))
            {
                int i;
                reg[0x0C] = d;

                /* Update colors */
                if(color_update)
                {
                    for(i = 0; i < 0x40; i += 1)
                    {
                        color_update(i, *(uint16 *)&cram[i << 1]);                    
                    }
                    color_update(0x00, *(uint16 *)&cram[border << 1]);
                    color_update(0x40, *(uint16 *)&cram[border << 1]);
                    color_update(0x80, *(uint16 *)&cram[border << 1]);
                }
                /* Flush palette */
                is_color_dirty = is_border_dirty = 1;
                memset(color_dirty, 1, 0x40);                    
            }

            /* Check interlace mode 2 setting */
            im2_flag = ((d & 0x06) == 0x06) ? 1 : 0;

            /* The following register updates check this value */
            reg[0x0C] = d;

            /* Update display-dependant registers */
            vdp_reg_w(0x03, reg[0x03]);
            vdp_reg_w(0x05, reg[0x05]);

            break;

        case 0x0D: /* HSCB */
            hscb = (d << 10) & 0xFC00;
            break;

        case 0x10: /* Playfield size */
            playfield_shift = shift_table[(d & 3)];
            playfield_col_mask = col_mask_table[(d & 3)];
            playfield_row_mask = row_mask_table[(d >> 4) & 3];
            y_mask = y_mask_table[(d & 3)];
            break;
    }

    /* Write new register value */
    reg[r] = d;
}


uint16 vdp_hvc_r(void)
{
    int cycles = m68k_cycles_run();
    uint8 *hctab = (reg[12] & 1) ? cycle2hc40 : cycle2hc32;
    int vc = vc28[v_counter];
    int hc = hctab[(cycles % 487)];
    return (vc << 8 | hc);
}

/*
    VRAM to VRAM copy
    Read byte from VRAM (source), write to VRAM (addr),
    bump source and add r15 to addr.

    - see how source addr is affected
      (can it make high source byte inc?)
*/
void dma_copy(void)
{
    int length = (reg[20] << 8 | reg[19]) & 0xFFFF;
    int source = (reg[22] << 8 | reg[21]) & 0xFFFF;
    if(!length) length = 0x10000;

    do {
        uint8 temp = vram[source];
        vram[addr] = temp;
        MARK_BG_DIRTY(addr);
        source = (source + 1) & 0xFFFF;
        addr = (addr + reg[15]) & 0xFFFF;
    } while (--length);

    reg[19] = (length >> 0) & 0xFF;
    reg[20] = (length >> 8) & 0xFF;
}


void dma_vbus(void)
{
    uint32 base, source = ((reg[23] & 0x7F) << 17 | reg[22] << 9 | reg[21] << 1) & 0xFFFFFE;
    uint32 length = (reg[20] << 8 | reg[19]) & 0xFFFF;

    if(!length) length = 0x10000;
    base = source;

    do {
        uint16 temp = vdp_dma_r(source);
        source += 2;
        source = ((base & 0xFE0000) | (source & 0x1FFFF));
        vdp_data_w(temp);
    } while (--length);

    reg[19] = (length >> 0) & 0xFF;
    reg[20] = (length >> 8) & 0xFF;

    reg[21] = (source >> 1) & 0xFF;
    reg[22] = (source >> 9) & 0xFF;
    reg[23] = (reg[23] & 0x80) | ((source >> 17) & 0x7F);
}


void vdp_test_w(uint16 value)
{
}


