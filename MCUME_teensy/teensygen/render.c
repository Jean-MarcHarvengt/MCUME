/*
        only update window clip on window change (?)
        fix leftmost window/nta render and window bug
        sprite masking isn't right in sonic/micromachines 2, but
        seems ok in galaxy force 2
*/


#include "shared.h"

#ifdef ALIGN_LONG

/* Or change the names if you depend on these from elsewhere.. */
#undef READ_LONG
#undef WRITE_LONG

static __inline__ uint32 READ_LONG(void *address)
{
    if ((uint32)address & 3)
	{
#ifdef LSB_FIRST  /* little endian version */
        return ( *((uint8 *)address) +
                (*((uint8 *)address+1) << 8)  +
                (*((uint8 *)address+2) << 16) +
                (*((uint8 *)address+3) << 24) );
#else             /* big endian version */
        return ( *((uint8 *)address+3) +
                (*((uint8 *)address+2) << 8)  +
                (*((uint8 *)address+1) << 16) +
                (*((uint8 *)address)   << 24) );
#endif  /* LSB_FIRST */
	}
	else
        return *(uint32 *)address;
}

static __inline__ void WRITE_LONG(void *address, uint32 data)
{
    if ((uint32)address & 3)
	{
#ifdef LSB_FIRST
            *((uint8 *)address) =    data;
            *((uint8 *)address+1) = (data >> 8);
            *((uint8 *)address+2) = (data >> 16);
            *((uint8 *)address+3) = (data >> 24);
#else
            *((uint8 *)address+3) =  data;
            *((uint8 *)address+2) = (data >> 8);
            *((uint8 *)address+1) = (data >> 16);
            *((uint8 *)address)   = (data >> 24);
#endif /* LSB_FIRST */
		return;
  	}
  	else
        *(uint32 *)address = data;
}

#endif  /* ALIGN_LONG */

#ifdef ALIGN_LONG
/* Draw a single 8-pixel column */
#define DRAW_COLUMN(ATTR, LINE) \
    atex = atex_table[(ATTR >> 13) & 7]; \
    src = (uint32 *)&bg_pattern_cache[(ATTR & 0x0FFF) << 6 | (LINE)]; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++; \
    ATTR >>= 16; \
    atex = atex_table[(ATTR >> 13) & 7]; \
    src = (uint32 *)&bg_pattern_cache[(ATTR & 0x0FFF) << 6 | (LINE)]; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++;


/* Draw a single 16-pixel column */
#define DRAW_COLUMN_IM2(ATTR, LINE) \
    atex = atex_table[(ATTR >> 13) & 7]; \
    offs = (ATTR & 0x03FF) << 7 | (ATTR & 0x0800) << 6 | (LINE); \
    if(ATTR & 0x1000) offs ^= 0x40; \
    src = (uint32 *)&bg_pattern_cache[offs]; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++; \
    ATTR >>= 16; \
    atex = atex_table[(ATTR >> 13) & 7]; \
    offs = (ATTR & 0x03FF) << 7 | (ATTR & 0x0800) << 6 | (LINE); \
    if(ATTR & 0x1000) offs ^= 0x40; \
    src = (uint32 *)&bg_pattern_cache[offs]; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++; \
    WRITE_LONG(dst, READ_LONG(src) | atex); \
    dst++; \
    src++; 

#else

#define DRAW_COLUMN(ATTR, LINE) \
    atex = atex_table[(ATTR >> 13) & 7]; \
    src = (uint32 *)&bg_pattern_cache[(ATTR & 0x0FFF) << 6 | (LINE)]; \
    *dst++ = (*src++ | atex); \
    *dst++ = (*src++ | atex); \
    ATTR >>= 16; \
    atex = atex_table[(ATTR >> 13) & 7]; \
    src = (uint32 *)&bg_pattern_cache[(ATTR & 0x0FFF) << 6 | (LINE)]; \
    *dst++ = (*src++ | atex); \
    *dst++ = (*src++ | atex);

#define DRAW_COLUMN_IM2(ATTR, LINE) \
    atex = atex_table[(ATTR >> 13) & 7]; \
    offs = (ATTR & 0x03FF) << 7 | (ATTR & 0x0800) << 6 | (LINE); \
    if(ATTR & 0x1000) offs ^= 0x40; \
    src = (uint32 *)&bg_pattern_cache[offs]; \
    *dst++ = (*src++ | atex); \
    *dst++ = (*src++ | atex); \
    ATTR >>= 16; \
    atex = atex_table[(ATTR >> 13) & 7]; \
    offs = (ATTR & 0x03FF) << 7 | (ATTR & 0x0800) << 6 | (LINE); \
    if(ATTR & 0x1000) offs ^= 0x40; \
    src = (uint32 *)&bg_pattern_cache[offs]; \
    *dst++ = (*src++ | atex); \
    *dst++ = (*src++ | atex);



#endif /* ALIGN_LONG */



/*
    gcc complains about this:
        *lb++ = table[(*lb << 8) |(*src++ | palette)]; 
    .. claiming the result on lb is undefined.
    So we manually advance lb and use constant offsets into the line buffer.
*/
#define DRAW_SPRITE_TILE \
    lb[0] = table[(lb[0] << 8) |(*src++ | palette)]; \
    lb[1] = table[(lb[1] << 8) |(*src++ | palette)]; \
    lb[2] = table[(lb[2] << 8) |(*src++ | palette)]; \
    lb[3] = table[(lb[3] << 8) |(*src++ | palette)]; \
    lb[4] = table[(lb[4] << 8) |(*src++ | palette)]; \
    lb[5] = table[(lb[5] << 8) |(*src++ | palette)]; \
    lb[6] = table[(lb[6] << 8) |(*src++ | palette)]; \
    lb[7] = table[(lb[7] << 8) |(*src++ | palette)]

/* Pixel creation macros, input is four bits each */

/* 5:6:5 RGB */
#define MAKE_PIXEL_16(r,g,b) ((r) << 12 | (g) << 7 | (b) << 1)

/* Clip data */
static clip_t clip[2];

/* Attribute expansion table */
static const uint32 atex_table[] = {
    0x00000000, 0x10101010, 0x20202020, 0x30303030,
    0x40404040, 0x50505050, 0x60606060, 0x70707070
};

/* Sprite name look-up table */
static uint8 name_lut[0x400];

/* Sprite line buffer data */
static uint8 object_index_count;

struct {
    uint16 ypos;
    uint16 xpos;
    uint16 attr;
    uint8 size;
    uint8 index;
}object_info[20];

/* Pixel look-up tables and table base address */
static uint8 *lut[5];
#include "genlut.h"

/* 16-bit pixel remapping data */
static uint16 pixel_16[0x100];
static uint16 pixel_16_lut[3][0x200];

/* Line buffers */
static uint8 tmp_buf[0x400];                   /* Temporary buffer */
static uint8 bg_buf[0x400];                    /* Merged background buffer */
static uint8 nta_buf[0x400];                   /* Plane A / Window line buffer */
static uint8 ntb_buf[0x400];                   /* Plane B line buffer */
static uint8 obj_buf[0x400];                   /* Object layer line buffer */

//static uint16 line_buf[0x400];



/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown routines                                           */
/*--------------------------------------------------------------------------*/

int render_init(void)
{
    int i;

    /* Pixel look-up tables */
    lut[0] = (uint8 *)(lut_base);
    for(i = 1; i < LUT_MAX; i += 1)
    {
        lut[i] = lut[0] + (i * LUT_SIZE);
    }

    /* Make pixel data tables */
    for(i = 0; i < 0x200; i += 1)
    {
        int r, g, b;
        
        r = (i >> 6) & 7;
        g = (i >> 3) & 7;
        b = (i >> 0) & 7;

        pixel_16_lut[0][i] = MAKE_PIXEL_16(r,g,b);
        pixel_16_lut[1][i] = MAKE_PIXEL_16(r<<1,g<<1,b<<1);
        pixel_16_lut[2][i] = MAKE_PIXEL_16(r|8,g|8,b|8);
    }

    /* Set up color update function */
    color_update = color_update_16;

    /* Make sprite name look-up table */
    make_name_lut();

    return (1);
}

void make_name_lut(void)
{
    int col, row;
    int vcol, vrow;
    int width, height;
    int flipx, flipy;
    int i, name;

    memset(name_lut, 0, sizeof(name_lut));

    for(i = 0; i < 0x400; i += 1)
    {
        vcol = col = i & 3;
        vrow = row = (i >> 2) & 3;
        height = (i >> 4) & 3;
        width = (i >> 6) & 3;
        flipx = (i >> 8) & 1;
        flipy = (i >> 9) & 1;

        if(flipx)
            vcol = (width - col);
        if(flipy)
            vrow = (height - row);

        name = vrow + (vcol * (height + 1));

        if((row > height) || col > width)
            name = -1;

        name_lut[i] = name;        
    }
}



void render_reset(void)
{
    memset(&clip, 0, sizeof(clip));

    memset(bg_buf, 0, sizeof(bg_buf));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    memset(nta_buf, 0, sizeof(nta_buf));
    memset(ntb_buf, 0, sizeof(ntb_buf));
    memset(obj_buf, 0, sizeof(obj_buf));

    memset(&pixel_16, 0, sizeof(pixel_16));
}


void render_shutdown(void)
{
}

/*--------------------------------------------------------------------------*/
/* Line render function                                                     */
/*--------------------------------------------------------------------------*/

void render_line(int line)
{
//    uint8 *lb = (gbitmap.remap == 1) ? tmp_buf : &gbitmap.data[(line * gbitmap.pitch)];
    uint8 *lb = tmp_buf;

    if((reg[1] & 0x40) == 0x00)
    {
        /* Use the overscan color to clear the screen */
        memset(&lb[gbitmap.viewport.x], 0x40 | border, gbitmap.viewport.w);
    }
    else
    {
        update_bg_pattern_cache();
        window_clip(line);

        if(im2_flag)
        {
            render_ntx_im2(0, line, nta_buf);
            render_ntx_im2(1, line, ntb_buf);
        }
        else
        {
            if(reg[0x0B] & 4)
            {
                render_ntx_vs(0, line, nta_buf);
                render_ntx_vs(1, line, ntb_buf);
            }
            else
            {
                render_ntx(0, line, nta_buf);
                render_ntx(1, line, ntb_buf);
            }
        }

        if(im2_flag)
              render_ntw_im2(line, nta_buf);
        else
              render_ntw(line, nta_buf);

        if(reg[12] & 8)
        {
            merge(&nta_buf[0x20], &ntb_buf[0x20], &bg_buf[0x20], lut[2], (reg[12] & 1) ? 320 : 256);
            memset(&obj_buf[0x20], 0, (reg[12] & 1) ? 320 : 256);

            if(im2_flag)
                render_obj_im2(line, obj_buf, lut[3]);
            else
                render_obj(line, obj_buf, lut[3]);

            merge(&obj_buf[0x20], &bg_buf[0x20], &lb[0x20], lut[4], (reg[12] & 1) ? 320 : 256);
        }
        else
        {
            merge(&nta_buf[0x20], &ntb_buf[0x20], &lb[0x20], lut[0], (reg[12] & 1) ? 320 : 256);
            if(im2_flag)
                render_obj_im2(line, lb, lut[1]);
            else
                render_obj(line, lb, lut[1]);
        }
    }

    if(reg[0] & 0x20)
    {
        memset(&lb[gbitmap.viewport.x], 0x40 | border, 0x08);
    }


    int width = (reg[12] & 1) ? 320 : 256;
    


    //remap_16(lb+0x20, line_buf, pixel_16, width);
    //emu_DrawLine16(line_buf, width ,256, line); 
    remap_16(lb+0x20, emu_LineBuffer(line), pixel_16, width);
}
/*--------------------------------------------------------------------------*/
/* Window rendering                                                         */
/*--------------------------------------------------------------------------*/

void render_ntw(int line, uint8 *buf)
{
    int column, v_line, width;
    uint32 *nt, *src, *dst, atex, atbuf;

    v_line = (line & 7) << 3;
    width = (reg[12] & 1) ? 7 : 6;

    nt = (uint32 *)&vram[ntwb | ((line >> 3) << width)];
    dst = (uint32 *)&buf[0x20 + (clip[1].left << 4)];

    for(column = clip[1].left; column < clip[1].right; column += 1)
    {
        atbuf = nt[column];
        DRAW_COLUMN(atbuf, v_line)
    }
}

void render_ntw_im2(int line, uint8 *buf)
{
    int column, v_line, width;
    uint32 *nt, *src, *dst, atex, atbuf, offs;

    v_line = ((line & 7) << 1 | ((status >> 4) & 1)) << 3;
    width = (reg[12] & 1) ? 7 : 6;

    nt = (uint32 *)&vram[ntwb | ((line >> 3) << width)];
    dst = (uint32 *)&buf[0x20 + (clip[1].left << 4)];

    for(column = clip[1].left; column < clip[1].right; column += 1)
    {
        atbuf = nt[column];
        DRAW_COLUMN_IM2(atbuf, v_line)
    }
}

/*--------------------------------------------------------------------------*/
/* Background plane rendering                                               */
/*--------------------------------------------------------------------------*/

void render_ntx(int which, int line, uint8 *buf)
{
    int column;
    int start, end;
    int index;
    int shift;
    int nametable_row_mask = (playfield_col_mask >> 1);
    int v_line;
    uint32 atex, atbuf, *src, *dst;
    uint16 xascroll, xbscroll, xscroll;
    int y_scroll;
    uint32 *nt;
    int vsr_shift;
    uint32 *vs;
    uint16 table;




    table = (which) ? ntbb : ntab;

    get_hscroll(line, &xascroll, &xbscroll);
    xscroll = (which) ? xbscroll : xascroll;

    shift = (xscroll & 0x0F);
    index = ((playfield_col_mask + 1) >> 1) - ((xscroll >> 4) & nametable_row_mask);

    if(which)
    {
        start = 0;
        end = (reg[0x0C] & 1) ? 20 : 16;
    }
    else
    {
// Looks correct if clip[0].left has 1 subtracted
// Otherwise window has gap between endpoint and where the first normal
// nta column starts

        if(clip[0].enable == 0) return;
        start = clip[0].left;
        end = clip[0].right;
        index = (index + clip[0].left) & nametable_row_mask;
    }

    vsr_shift = (which) ? 16 : 0;
    vs = (uint32 *)&vsram[0];

    y_scroll = (vs[0] >> vsr_shift) & 0xFFFF;

//    int dy = line & 0x7; 
//    int rev_line = (line & 0x1f8) + (dy^7);

    y_scroll = (line + (y_scroll & 0x3FF)) & playfield_row_mask;

    int dy = y_scroll & 0x7; 
    int y_scroll2 = (y_scroll & 0x1f8) + (dy^7);




    v_line = (y_scroll & 7) << 3;
    int v_line2 = (y_scroll2 & 7) << 3;
    
    nt = (uint32 *)&vram[table + (((y_scroll >> 3) << playfield_shift) & y_mask)];

    if(shift)
    {
        dst = (uint32 *)&buf[0x20-(0x10-shift)];
        atbuf = nt[(index-1) & nametable_row_mask];
        //DRAW_COLUMN(atbuf, v_line)
       // JMH
        if (atbuf & 0x1000) {
            DRAW_COLUMN(atbuf, v_line2);
        }
        else  {
            DRAW_COLUMN(atbuf, v_line);
        }  
     }
    buf = (buf + 0x20 + shift);
    dst = (uint32 *)&buf[start<<4];



    for(column = start; column < end; column += 1, index += 1)
    {
        atbuf = nt[index & nametable_row_mask];
        // JMH
        if (atbuf & 0x1000) {
            DRAW_COLUMN(atbuf, v_line2);
        }
        else  
        {
            DRAW_COLUMN(atbuf, v_line);
        }  
    }

}


void render_ntx_im2(int which, int line, uint8 *buf)
{
    int column;
    int start, end;
    int index;
    int shift;
    int nametable_row_mask = (playfield_col_mask >> 1);
    int v_line;
    uint32 atex, atbuf, *src, *dst;
    uint16 xascroll, xbscroll, xscroll;
    int y_scroll;
    uint32 *nt;
    int vsr_shift;
    uint32 *vs;
    uint16 table;
    uint32 offs;

    table = (which) ? ntbb : ntab;

    get_hscroll(line, &xascroll, &xbscroll);
    xscroll = (which) ? xbscroll : xascroll;

    shift = (xscroll & 0x0F);
    index = ((playfield_col_mask + 1) >> 1) - ((xscroll >> 4) & nametable_row_mask);

    if(which)
    {
        start = 0;
        end = (reg[0x0C] & 1) ? 20 : 16;
    }
    else
    {
        if(clip[0].enable == 0) return;
        start = clip[0].left;
        end = clip[0].right;
        index = (index + clip[0].left) & nametable_row_mask;
    }

    vsr_shift = (which) ? 16 : 0;
    vs = (uint32 *)&vsram[0];

    y_scroll = (vs[0] >> vsr_shift) & 0xFFFF;
    y_scroll = (line + ((y_scroll >> 1) & 0x3FF)) & playfield_row_mask;
    v_line = (((y_scroll & 7) << 1) | ((status >> 4) & 1)) << 3;
    nt = (uint32 *)&vram[table + (((y_scroll >> 3) << playfield_shift) & y_mask)];

    if(shift)
    {
        dst = (uint32 *)&buf[0x20-(0x10-shift)];
        atbuf = nt[(index-1) & nametable_row_mask];
        DRAW_COLUMN_IM2(atbuf, v_line)
    }
    buf = (buf + 0x20 + shift);
    dst = (uint32 *)&buf[start<<4];

    for(column = start; column < end; column += 1, index += 1)
    {
        atbuf = nt[index & nametable_row_mask];
        DRAW_COLUMN_IM2(atbuf, v_line)
    }
}


void render_ntx_vs(int which, int line, uint8 *buf)
{
    int column;
    int start, end;
    int index;
    int shift;
    int nametable_row_mask = (playfield_col_mask >> 1);
    int v_line;
    uint32 atex, atbuf, *src, *dst;
    uint16 xascroll, xbscroll, xscroll;
    int y_scroll;
    uint32 *nt;
    int vsr_shift;
    uint32 *vs;
    uint16 table;

    table = (which) ? ntbb : ntab;

    get_hscroll(line, &xascroll, &xbscroll);
    xscroll = (which) ? xbscroll : xascroll;
    shift = (xscroll & 0x0F);
    index = ((playfield_col_mask + 1) >> 1) - ((xscroll >> 4) & nametable_row_mask);

    if(which)
    {
        start = 0;
        end = (reg[0x0C] & 1) ? 20 : 16;
    }
    else
    {
        if(clip[0].enable == 0) return;
        start = clip[0].left;
        end = clip[0].right;
        index = (index + clip[0].left) & nametable_row_mask;
    }

    vsr_shift = (which) ? 16 : 0;
    vs = (uint32 *)&vsram[0];
    end = (reg[0x0C] & 1) ? 20 : 16;

    if(shift)
    {
        dst = (uint32 *)&buf[0x20-(0x10-shift)];
        y_scroll = (line & playfield_row_mask);
        v_line = (y_scroll & 7) << 3;
        nt = (uint32 *)&vram[table + (((y_scroll >> 3) << playfield_shift) & y_mask)];
        atbuf = nt[(index-1) & nametable_row_mask];
        DRAW_COLUMN(atbuf, v_line)
    }

    buf = (buf + 0x20 + shift);
    dst = (uint32 *)&buf[start << 4];

    for(column = start; column < end; column += 1, index += 1)
    {
        y_scroll = (vs[column] >> vsr_shift) & 0xFFFF;
        y_scroll = (line + (y_scroll & 0x3FF)) & playfield_row_mask;
        v_line = (y_scroll & 7) << 3;
        nt = (uint32 *)&vram[table + (((y_scroll >> 3) << playfield_shift) & y_mask)];
        atbuf = nt[index & nametable_row_mask];
        DRAW_COLUMN(atbuf, v_line)
    }
}
/*--------------------------------------------------------------------------*/
/* Helper functions (cache update, hscroll, window clip)                    */
/*--------------------------------------------------------------------------*/

void update_bg_pattern_cache(void)
{
    int i;
    uint8 x, y, c;
    uint16 name;

    if(!bg_list_index) return;

    for(i = 0; i < bg_list_index; i += 1)
    {
        name = bg_name_list[i];
        bg_name_list[i] = 0;

        for(y = 0; y < 8; y += 1)
        {
            if(bg_name_dirty[name] & (1 << y))
            {
                uint8 *dst = &bg_pattern_cache[name << 6];
                uint32 bp = *(uint32 *)&vram[(name << 5) | (y << 2)];

                for(x = 0; x < 8; x += 1)
                {
                    c = (bp >> ((x ^ 3) << 2)) & 0x0F;
                    dst[0x00000 | (y << 3) | (x)] = (c);
                    dst[0x20000 | (y << 3) | (x ^ 7)] = (c);
                    //dst[0x40000 | ((y ^ 7) << 3) | (x)] = (c);
                    //dst[0x60000 | ((y ^ 7) << 3) | (x ^ 7)] = (c);
                }
            }
        }
        bg_name_dirty[name] = 0;
    }
    bg_list_index = 0;
}

void get_hscroll(int line, uint16 *scrolla, uint16 *scrollb)
{
    switch(reg[11] & 3)
    {
        case 0: /* Full-screen */
            *scrolla = *(uint16 *)&vram[hscb + 0];
            *scrollb = *(uint16 *)&vram[hscb + 2];
            break;

        case 1: /* First 8 lines */
            *scrolla = *(uint16 *)&vram[hscb + ((line & 7) << 2) + 0];
            *scrollb = *(uint16 *)&vram[hscb + ((line & 7) << 2) + 2];
            break;

        case 2: /* Every 8 lines */
            *scrolla = *(uint16 *)&vram[hscb + ((line & ~7) << 2) + 0];
            *scrollb = *(uint16 *)&vram[hscb + ((line & ~7) << 2) + 2];
            break;

        case 3: /* Every line */
            *scrolla = *(uint16 *)&vram[hscb + (line << 2) + 0];
            *scrollb = *(uint16 *)&vram[hscb + (line << 2) + 2];
            break;
    }

    *scrolla &= 0x03FF;
    *scrollb &= 0x03FF;
}

void window_clip(int line)
{
    /* Window size and invert flags */
    int hp = (reg[17] & 0x1F);
    int hf = (reg[17] >> 7) & 1;
    int vp = (reg[18] & 0x1F) << 3;
    int vf = (reg[18] >> 7) & 1;

    /* Display size  */
    int sw = (reg[12] & 1) ? 20 : 16;

    /* Clear clipping data */
    memset(&clip, 0, sizeof(clip));

    /* Check if line falls within window range */
    if(vf == (line >= vp))
    {
        /* Window takes up entire line */
        clip[1].right = sw;
        clip[1].enable = 1;
    }
    else
    {
        /* Perform horizontal clipping; the results are applied in reverse
           if the horizontal inversion flag is set */
        int a = hf;
        int w = hf ^ 1;

        if(hp)
        {
            if(hp > sw)
            {
                /* Plane W takes up entire line */
                clip[w].right = sw;
                clip[w].enable = 1;
            }
            else
            {
                /* Window takes left side, Plane A takes right side */
                clip[w].right = hp;
                clip[a].left = hp;
                clip[a].right = sw;
                clip[0].enable = clip[1].enable = 1;
            }
        }
        else
        {
            /* Plane A takes up entire line */
            clip[a].right = sw;
            clip[a].enable = 1;
        }
    }
}







/*--------------------------------------------------------------------------*/
/* Remap functions                                                          */
/*--------------------------------------------------------------------------*/

/*
void remap_8(uint8 *src, uint8 *dst, uint8 *table, int length)
{
    int count;
    for(count = 0; count < length; count += 1)
    {
        *dst++ = table[*src++];
    }
}
*/

void remap_16(uint8 *src, uint16 *dst, uint16 *table, int length)
{
    int count;
    for(count = 0; count < length; count += 1)
    {
        *dst++ = table[*src++];
    }
}


/*--------------------------------------------------------------------------*/
/* Merge functions                                                          */
/*--------------------------------------------------------------------------*/

void merge(uint8 *srca, uint8 *srcb, uint8 *dst, uint8 *table, int width)
{
    int i;
    for(i = 0; i < width; i += 1)
    {
        uint8 a = srca[i];
        uint8 b = srcb[i];
        uint8 c = table[(b << 8) | (a)];
        dst[i] = c;
    }
}

void color_update_16(int index, uint16 data)
{
    if(reg[12] & 8)
    {
        pixel_16[0x00 | index] = pixel_16_lut[0][data];
        pixel_16[0x40 | index] = pixel_16_lut[1][data];
        pixel_16[0x80 | index] = pixel_16_lut[2][data];
    }
    else
    {
        uint16 temp = pixel_16_lut[1][data];
        pixel_16[0x00 | index] = temp;
        pixel_16[0x40 | index] = temp;
        pixel_16[0x80 | index] = temp;
    }
}


/*--------------------------------------------------------------------------*/
/* Object render functions                                                  */
/*--------------------------------------------------------------------------*/

void parse_satb(int line)
{
    static uint8 sizetab[] = {8, 16, 24, 32};
    uint8 *p, *q, link = 0;
    uint16 ypos;

    int count;
    int height;

    int limit = (reg[12] & 1) ? 20 : 16;
    int total = (reg[12] & 1) ? 80 : 64;

    object_index_count = 0;

    for(count = 0; count < total; count += 1)
    {
        q = &sat[link << 3];
        p = &vram[satb + (link << 3)];

        ypos = *(uint16 *)&q[0];

        if(im2_flag)
            ypos = (ypos >> 1) & 0x1FF;
        else
            ypos &= 0x1FF;

        height = sizetab[q[3] & 3];

        if((line >= ypos) && (line < (ypos + height)))
        {
            object_info[object_index_count].ypos = *(uint16 *)&q[0];
            object_info[object_index_count].xpos = *(uint16 *)&p[6];

            // using xpos from internal satb stops sprite x
            // scrolling in bloodlin.bin,
            // but this seems to go against the test prog
//          object_info[object_index_count].xpos = *(uint16 *)&q[6];
            object_info[object_index_count].attr = *(uint16 *)&p[4];
            object_info[object_index_count].size = q[3];
            object_info[object_index_count].index = count;

            object_index_count += 1;

            if(object_index_count == limit)
            {
                if(vint_pending == 0)
                    status |= 0x40;
                return;
            }
        }

        link = q[2] & 0x7F;
        if(link == 0) break;
    }
}

void render_obj(int line, uint8 *buf, uint8 *table)
{
    uint16 ypos;
    uint16 attr;
    uint16 xpos;
    uint8 sizetab[] = {8, 16, 24, 32};
    uint8 size;
    uint8 *src;

    int count;
    int pixellimit = (reg[12] & 1) ? 320 : 256;
    int pixelcount = 0;
    int width;
    int height;
    int v_line;
    int column;
    int sol_flag = 0;
    int left = 0x80;
    int right = 0x80 + ((reg[12] & 1) ? 320 : 256);

    uint8 *s, *lb;
    uint16 name, index;
    uint8 palette;

    int attr_mask, nt_row;

    if(object_index_count == 0) return;

    for(count = 0; count < object_index_count; count += 1)
    {
        size = object_info[count].size & 0x0f;
        xpos = object_info[count].xpos;
        xpos &= 0x1ff;

        width = sizetab[(size >> 2) & 3];

        if(xpos != 0) sol_flag = 1;
        else
        if(xpos == 0 && sol_flag) return;

        if(pixelcount > pixellimit) return;
        pixelcount += width;

        if(((xpos + width) >= left) && (xpos < right))
        {
            ypos = object_info[count].ypos;
            ypos &= 0x1ff;

            attr = object_info[count].attr;
//            attr_mask = (attr & 0x1800);
            attr_mask = (attr & 0x800);

            height = sizetab[size & 3];
            palette = (attr >> 9) & 0x70;

            v_line = (line - ypos);
            nt_row = (v_line >> 3) & 3;

            if (attr & 0x1000) {
                v_line = ((v_line & 7)^0x07) << 3;
            }
            else {
                v_line = ((v_line & 7)) << 3;                
            }

            name = (attr & 0x07FF);
            s = &name_lut[((attr >> 3) & 0x300) | (size << 4) | (nt_row << 2)];

            lb = (uint8 *)&buf[0x20 + (xpos - 0x80)];

            width >>= 3;
            for(column = 0; column < width; column += 1, lb+=8)
            {
                index = attr_mask | ((name + s[column]) & 0x07FF);
                src = &bg_pattern_cache[(index << 6) | (v_line)];
                DRAW_SPRITE_TILE;
            }
        }
    }
}

void render_obj_im2(int line, uint8 *buf, uint8 *table)
{
    uint16 ypos;
    uint16 attr;
    uint16 xpos;
    uint8 sizetab[] = {8, 16, 24, 32};
    uint8 size;
    uint8 *src;

    int count;
    int pixellimit = (reg[12] & 1) ? 320 : 256;
    int pixelcount = 0;
    int width;
    int height;
    int v_line;
    int column;
    int sol_flag = 0;
    int left = 0x80;
    int right = 0x80 + ((reg[12] & 1) ? 320 : 256);

    uint8 *s, *lb;
    uint16 name, index;
    uint8 palette;
    uint32 offs;

    int attr_mask, nt_row;

    if(object_index_count == 0) return;

    for(count = 0; count < object_index_count; count += 1)
    {
        size = object_info[count].size & 0x0f;
        xpos = object_info[count].xpos;
        xpos &= 0x1ff;

        width = sizetab[(size >> 2) & 3];

        if(xpos != 0) sol_flag = 1;
        else
        if(xpos == 0 && sol_flag) return;

        if(pixelcount > pixellimit) return;
        pixelcount += width;

        if(((xpos + width) >= left) && (xpos < right))
        {
            ypos = object_info[count].ypos;
            ypos = (ypos >> 1) & 0x1ff;

            attr = object_info[count].attr;
//            attr_mask = (attr & 0x1800);
            attr_mask = (attr & 0x800);

            height = sizetab[size & 3];
            palette = (attr >> 9) & 0x70;

            v_line = (line - ypos);
            nt_row = (v_line >> 3) & 3;
            v_line = ((((v_line & 7)/*^0x07*/) << 1) | ((status >> 4) & 1)) << 3;            

            name = (attr & 0x03FF);
            s = &name_lut[((attr >> 3) & 0x300) | (size << 4) | (nt_row << 2)];

            lb = (uint8 *)&buf[0x20 + (xpos - 0x80)];

            width >>= 3;
            for(column = 0; column < width; column += 1, lb+=8)
            {
                index = (name + s[column]) & 0x3ff;
                offs = index << 7 | attr_mask << 6 | v_line;
                if(attr & 0x1000) offs ^= 0x40;
                src = &bg_pattern_cache[offs];
                DRAW_SPRITE_TILE;
            }
        }
    }
}

