#include "shared.h"

/* Background drawing function */
static void (*render_bg)(int line);

/* Pointer to output buffer */
static byte *linebuf;

/* Internal buffer for drawing non 8-bit displays */
static byte internal_buffer[0x100];

/* Pattern cache */
//static byte cache[0x20000];

/* Dirty pattern info */
byte svram_dirty[0x200];
byte sis_vram_dirty;

/* Pixel look-up table */
//#include "genlut.h"
#include "smslut.h"
//static byte lut[0x10000];


/* Attribute expansion table */
static UINT32 atex[4] =
{
    0x00000000,
    0x10101010,
    0x20202020,
    0x30303030,
};

/* Display sizes */
static int vp_vstart;
static int vp_vend;
static int vp_hstart;
static int vp_hend;


/* Macros to access memory 32-bits at a time (from MAME's drawgfx.c) */

#ifdef ALIGN_DWORD

static __inline__ UINT32 read_dword(void *address)
{
    if ((UINT32)address & 3)
	{
#ifdef LSB_FIRST  /* little endian version */
        return ( *((byte *)address) +
                (*((byte *)address+1) << 8)  +
                (*((byte *)address+2) << 16) +
                (*((byte *)address+3) << 24) );
#else             /* big endian version */
        return ( *((byte *)address+3) +
                (*((byte *)address+2) << 8)  +
                (*((byte *)address+1) << 16) +
                (*((byte *)address)   << 24) );
#endif
	}
	else
        return *(UINT32 *)address;
}


static __inline__ void write_dword(void *address, UINT32 data)
{
    if ((UINT32)address & 3)
	{
#ifdef LSB_FIRST
            *((byte *)address) =    data;
            *((byte *)address+1) = (data >> 8);
            *((byte *)address+2) = (data >> 16);
            *((byte *)address+3) = (data >> 24);
#else
            *((byte *)address+3) =  data;
            *((byte *)address+2) = (data >> 8);
            *((byte *)address+1) = (data >> 16);
            *((byte *)address)   = (data >> 24);
#endif
		return;
  	}
  	else
        *(UINT32 *)address = data;
}
#else
#define read_dword(address) *(UINT32 *)address
#define write_dword(address,data) *(UINT32 *)address=data
#endif


/****************************************************************************/


/* Initialize the rendering data */
void render_init(void)
{
    render_reset();
}


/* Reset the rendering data */
void render_reset(void)
{
    int i;

    /* Clear display bitmap */
//    memset(sbitmap.data, 0, sbitmap.pitch * sbitmap.height);

    /* Clear palette */
    for(i = 0; i < PALETTE_SIZE; i += 1)
    {
        palette_sync(i);
    }

    /* Invalidate pattern cache */
    sis_vram_dirty = 1;
    memset(svram_dirty, 1, 0x200);
    memset(cache, 0, CACHE_SIZE);

    /* Set up viewport size */
    if(IS_GG)
    {
        vp_vstart = 24;
        vp_vend   = 168;
        vp_hstart = 6;
        vp_hend   = 26;
    }
    else
    {
        vp_vstart = 0;
        vp_vend   = 192;
        vp_hstart = 0;
        vp_hend   = 32;
    }

    /* Pick render routine */
    render_bg = IS_GG ? render_bg_gg : render_bg_sms;
}


/* Draw a line of the display */
void render_line(int line)
{
    /* Ensure we're within the viewport range */
    if((line < vp_vstart) || (line >= vp_vend)) return;

    /* Point to current line in output buffer */
    linebuf = &internal_buffer[0];

    /* Update pattern cache */
    update_cache();

    /* Blank line */
    if( (!(vdp.reg[1] & 0x40)) || (((vdp.reg[2] & 1) == 0) && (IS_SMS)))
    {
        memset(linebuf + (vp_hstart << 3), BACKDROP_COLOR, BMP_WIDTH);
    }
    else
    {
        /* Draw background */
        render_bg(line);

        /* Draw sprites */
        render_obj(line);

        /* Blank leftmost column of display */
        if(vdp.reg[0] & 0x20)
        {
            memset(linebuf, BACKDROP_COLOR, 8);
        }
    }

    emu_DrawLine(&internal_buffer[BMP_X_OFFSET], BMP_WIDTH , BMP_HEIGHT, line);
}


/* Draw the Master System background */
void render_bg_sms(int line)
{
    int locked = 0;
    int v_line = (line + vdp.reg[9]) % 224;
    int v_row  = (v_line & 7) << 3;
    int hscroll = ((vdp.reg[0] & 0x40) && (line < 0x10)) ? 0 : (0x100 - vdp.reg[8]);
    int column = vp_hstart;
    UINT16 attr;
    UINT16 *nt = (UINT16 *)&vdp.vram[vdp.ntab + ((v_line >> 3) << 6)];
    int nt_scroll = (hscroll >> 3);
    int shift = (hscroll & 7);
    UINT32 atex_mask;
    UINT32 *cache_ptr;
    UINT32 *linebuf_ptr = (UINT32 *)&linebuf[0 - shift];

    /* Draw first column (clipped) */
    if(shift)
    {
        int x, c, a;

        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        a = (attr >> 7) & 0x30;

        for(x = shift; x < 8; x += 1)
        {
            c = cache[((attr & 0x7FF) << 6) | (v_row) | (x)];
            linebuf[(0 - shift) + (x)  ] = ((c) | (a));
        }

        column += 1;
    }

    /* Draw a line of the background */
    for(; column < vp_hend; column += 1)
    {
        /* Stop vertical scrolling for leftmost eight columns */
        if((vdp.reg[0] & 0x80) && (!locked) && (column >= 24))
        {
            locked = 1;
            v_row = (line & 7) << 3;
            nt = (UINT16 *)&vdp.vram[((vdp.reg[2] << 10) & 0x3800) + ((line >> 3) << 6)];
        }

        /* Get name table attribute word */
        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        /* Expand priority and palette bits */
        atex_mask = atex[(attr >> 11) & 3];

        /* Point to a line of pattern data in cache */
        cache_ptr = (UINT32 *)&cache[((attr & 0x7FF) << 6) | (v_row)];
        
        /* Copy the left half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1)] , read_dword( &cache_ptr[0] ) | (atex_mask));

        /* Copy the right half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1) | (1)], read_dword( &cache_ptr[1] ) | (atex_mask));
    }

    /* Draw last column (clipped) */
    if(shift)
    {
        int x, c, a;

        char *p = &linebuf[(0 - shift)+(column << 3)];

        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        a = (attr >> 7) & 0x30;

        for(x = 0; x < shift; x += 1)
        {
            c = cache[((attr & 0x7FF) << 6) | (v_row) | (x)];
            p[x] = ((c) | (a));
        }
    }
}


/* Draw the Game Gear background */
void render_bg_gg(int line)
{
    int v_line = (line + vdp.reg[9]) % 224;
    int v_row  = (v_line & 7) << 3;
    int hscroll = (0x100 - vdp.reg[8]);
    int column;
    UINT16 attr;
    UINT16 *nt = (UINT16 *)&vdp.vram[vdp.ntab + ((v_line >> 3) << 6)];
    int nt_scroll = (hscroll >> 3);
    UINT32 atex_mask;
    UINT32 *cache_ptr;
    UINT32 *linebuf_ptr = (UINT32 *)&linebuf[0 - (hscroll & 7)];

    /* Draw a line of the background */
    for(column = vp_hstart; column <= vp_hend; column += 1)
    {
        /* Get name table attribute word */
        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        /* Expand priority and palette bits */
        atex_mask = atex[(attr >> 11) & 3];

        /* Point to a line of pattern data in cache */
        cache_ptr = (UINT32 *)&cache[((attr & 0x7FF) << 6) | (v_row)];

        /* Copy the left half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1)] , read_dword( &cache_ptr[0] ) | (atex_mask));

        /* Copy the right half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1) | (1)], read_dword( &cache_ptr[1] ) | (atex_mask));
    }
}


/* Draw sprites */
void render_obj(int line)
{
    int i;

    /* Sprite count for current line (8 max.) */
    int count = 0;

    /* Sprite dimensions */
    int width = 8;
    int height = (vdp.reg[1] & 0x02) ? 16 : 8;

    /* Pointer to sprite attribute table */
    byte *st = (byte *)&vdp.vram[vdp.satb];

    /* Adjust dimensions for double size sprites */
    if(vdp.reg[1] & 0x01)
    {
        width *= 2;
        height *= 2;
    }

    /* Draw sprites in front-to-back order */
    for(i = 0; i < 64; i += 1)
    {
        /* Sprite Y position */
        int yp = st[i];

        /* End of sprite list marker? */
        if(yp == 208) return;

        /* Actual Y position is +1 */
        yp += 1;

        /* Wrap Y coordinate for sprites > 240 */
        if(yp > 240) yp -= 256;

        /* Check if sprite falls on current line */
        if((line >= yp) && (line < (yp + height)))
        {
            byte *linebuf_ptr;

            /* Width of sprite */
            int start = 0;
            int end = width;

            /* Sprite X position */
            int xp = st[0x80 + (i << 1)];

            /* Pattern name */
            int n = st[0x81 + (i << 1)];

            /* Bump sprite count */
            count += 1;

            /* Too many sprites on this line ? */
            if((vdp.limit) && (count == 9)) return;

            /* X position shift */
            if(vdp.reg[0] & 0x08) xp -= 8;

            /* Add MSB of pattern name */
            if(vdp.reg[6] & 0x04) n |= 0x0100;

            /* Mask LSB for 8x16 sprites */
            if(vdp.reg[1] & 0x02) n &= 0x01FE;

            /* Point to offset in line buffer */
            linebuf_ptr = (byte *)&linebuf[xp];

            /* Clip sprites on left edge */
            if(xp < 0)
            {
                start = (0 - xp);
            }

            /* Clip sprites on right edge */
            if((xp + width) > 256)        
            {
                end = (256 - xp);
            }

            /* Draw double size sprite */
            if(vdp.reg[1] & 0x01)
            {
                int x;
                byte *cache_ptr = (byte *)&cache[(n << 6) | (((line - yp) >> 1) << 3)];

                /* Draw sprite line */
                for(x = start; x < end; x += 1)
                {
                    /* Source pixel from cache */
                    byte sp = cache_ptr[(x >> 1)];
    
                    /* Only draw opaque sprite pixels */
                    if(sp)
                    {
                        /* Background pixel from line buffer */
                        byte bg = linebuf_ptr[x];
    
                        /* Look up result */
                        linebuf_ptr[x] = lut[(bg << 8) | (sp)]; /* + COL_OFFSET; */
    
                        /* Set sprite collision flag */
                        if(bg & 0x40) vdp.status |= 0x20;
                    }
                }
            }
            else /* Regular size sprite (8x8 / 8x16) */
            {
                int x;
                byte *cache_ptr = (byte *)&cache[(n << 6) | ((line - yp) << 3)];

                /* Draw sprite line */
                for(x = start; x < end; x += 1)
                {
                    /* Source pixel from cache */
                    byte sp = cache_ptr[x];
    
                    /* Only draw opaque sprite pixels */
                    if(sp)
                    {
                        /* Background pixel from line buffer */
                        byte bg = linebuf_ptr[x];
    
                        /* Look up result */
                        linebuf_ptr[x] = lut[(bg << 8) | (sp)]; /* + COL_OFFSET; */
    
                        /* Set sprite collision flag */
                        if(bg & 0x40) vdp.status |= 0x20;
                    }
                }
            }
        }
    }
}


/* Update pattern cache with modified tiles */
void update_cache(void)
{
    int i, x, y, c;
    int b0, b1, b2, b3;
    int i0, i1, i2, i3;

    if(!sis_vram_dirty) return;
    sis_vram_dirty = 0;

    for(i = 0; i < 0x200; i += 1)
    {
        if(svram_dirty[i])
        {
            svram_dirty[i] = 0;

            for(y = 0; y < 8; y += 1)
            {
                b0 = vdp.vram[(i << 5) | (y << 2) | (0)];
                b1 = vdp.vram[(i << 5) | (y << 2) | (1)];
                b2 = vdp.vram[(i << 5) | (y << 2) | (2)];
                b3 = vdp.vram[(i << 5) | (y << 2) | (3)];

                for(x = 0; x < 8; x += 1)
                {
                    i0 = (b0 >> (7 - x)) & 1;
                    i1 = (b1 >> (7 - x)) & 1;
                    i2 = (b2 >> (7 - x)) & 1;
                    i3 = (b3 >> (7 - x)) & 1;

                    c = (i3 << 3 | i2 << 2 | i1 << 1 | i0);

                    cache[0x00000 | (i << 6) | ((y  ) << 3) | (x  )] = c;
                    cache[0x08000 | (i << 6) | ((y  ) << 3) | (7-x)] = c;
                    cache[0x10000 | (i << 6) | ((7-y) << 3) | (x  )] = c;
                    cache[0x18000 | (i << 6) | ((7-y) << 3) | (7-x)] = c;
                }
            }
        }
    }
}

/* Update a palette entry */
void palette_sync(int index)
{
    int r, g, b;

    if(IS_GG)
    {
        r = ((vdp.cram[(index << 1) | 0] >> 1) & 7) << 5;
        g = ((vdp.cram[(index << 1) | 0] >> 5) & 7) << 5;
        b = ((vdp.cram[(index << 1) | 1] >> 1) & 7) << 5;
    }
    else
    {
        r = ((vdp.cram[index] >> 0) & 3) << 6;
        g = ((vdp.cram[index] >> 2) & 3) << 6;
        b = ((vdp.cram[index] >> 4) & 3) << 6;
    }

    emu_SetPaletteEntry(r,g,b,index);

    sbitmap.pal.dirty[index] = sbitmap.pal.update = 1;
}





