#include "shared.h"


/* Bit 0 : BG enable, Bit 1 : OBJ enable */
int plane_enable = -1;

/* VCE color data to 16-bit pixel table */
uint16 pixel[2][0x100];

/* Precalculated 16-bit pixel values */
uint16 pixel_lut[0x200];

/* Pointer to 8 or 16-bit version of render_line() */
void (*render_line)(int line) = NULL;

/* List of precalculated sprite data */
t_sprite sprite_list[0x40];

/* Bitplane to packed pixel lookup table */
//uint32 bp_lut[0x10000];
#include "bp_lut.h"

/* Used sprite data */
uint8 used_sprite_list[0x40];
uint8 used_sprite_index;


extern void emu_DrawLine16(unsigned short *src, int width , int height, int line); 
extern void emu_printf(char * text);



/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown functions                                          */
/*--------------------------------------------------------------------------*/

int render_init(void)
{
    int i, j;

    /* Make VCE data to raw pixel look-up table */
    for(i = 0; i < 0x200; i += 1)
    {
        int r = (i >> 3) & 7;
        int g = (i >> 6) & 7;
        int b = (i >> 0) & 7;
        pixel_lut[i] = (r << 13 | g << 8 | b << 2) & 0xE71C;
    }

    render_line = render_line_generic;
    return (1);
}


void render_reset(void)
{
    render_line = render_line_generic;
}


void render_shutdown(void)
{
}


int make_sprite_list(void)
{
    uint16 *sat = &objramw[0];
    int xpos, ypos, name, attr;
    int cgx, xflip, cgy, yflip;
    int width, height;
    int i;
    uint32 flip;

    used_sprite_index = 0;
    memset(&used_sprite_list, 0, sizeof(used_sprite_list));

    memset(&sprite_list, 0, sizeof(sprite_list));

    for(i = 0; i < 0x40; i += 1)
    {
        ypos = sat[(i << 2) | (0)];
        xpos = sat[(i << 2) | (1)];
        name = sat[(i << 2) | (2)];
        attr = sat[(i << 2) | (3)];

        ypos &= 0x3FF;
        xpos &= 0x3FF;

        if(xpos && ypos)
        {
            ypos -= 64;
            if(ypos >= 0x100) continue;
            cgy = (attr >> 12) & 3;
            cgy |= (cgy >> 1);
            height = (cgy + 1) << 4;
            if((ypos + height) < 0) continue;

            xpos -= 32;
            if(xpos >= 0x200) continue;
            cgx = (attr >> 8) & 1;
            width  = (cgx) ? 32 : 16;
            if((xpos + width) < 0) continue;

            xflip = (attr >> 11) & 1;
            yflip = (attr >> 15) & 1;
            flip = ((xflip << 9) /*| (yflip << 10)*/) & 0x600;

            name = (name >> 1) & 0x1FF;
            name &= ~((cgy << 1) | cgx);
            name |= flip;
            if(xflip && cgx) name ^= 1;

            sprite_list[i].top = ypos;
            sprite_list[i].bottom = ypos + height;
            sprite_list[i].xpos = xpos;
            sprite_list[i].name_left = name;
            sprite_list[i].name_right = name ^ 1;
            sprite_list[i].height = (height - 1);
            sprite_list[i].palette = (attr & 0x0F) << 4;

            if(yflip)
                sprite_list[i].flags |= FLAG_YFLIP;

            if(cgx)
                sprite_list[i].flags |= FLAG_CGX;

            if(!(attr & 0x80))
                sprite_list[i].flags |= FLAG_PRIORITY;

            used_sprite_list[used_sprite_index] = (i);
            used_sprite_index += 1;
        }
    }

    return (used_sprite_index);
}


/*--------------------------------------------------------------------------*/
/* Pattern and object cache update routines                                 */
/*--------------------------------------------------------------------------*/

void update_bg_pattern_cache(void)
{
    int i;
    uint8 x, y, c;
    uint16 name, index1, index2;
    uint32 temp;

    if(!bg_list_index) return;

    for(i = 0; i < bg_list_index; i += 1)
    {
        name = bg_name_list[i];
        bg_name_list[i] = 0;

        for(y = 0; y < 8; y += 1)
        {
            if(bg_name_dirty[name] & (1 << y))
            {
                index1 = vramw[(name << 4) | (y)];
                index2 = vramw[(name << 4) | (y) | (8)];

                temp = (bp_lut[index1] >> 2) | bp_lut[index2];

                for(x = 0; x < 8; x += 1)
                {
                    c = (temp >> (x << 2)) & 0x0F;
                    bg_pattern_cache[(name << 6) | (y << 3) | (x)] = (c);
                }
            }
        }
        bg_name_dirty[name] = 0;
    }

    bg_list_index = 0;
}



void update_obj_pattern_cache(void)
{
    int i;
    uint16 name;
    uint16 b0, b1, b2, b3;
    uint8 i0, i1, i2, i3;           
    uint8 x, y, c;

    if(!obj_list_index) return;

    for(i = 0; i < obj_list_index; i += 1)
    {
        name = obj_name_list[i];
        obj_name_list[i] = 0;

        for(y = 0; y < 0x10; y += 1)
        {
            if(obj_name_dirty[name] & (1 << y))
            {
                b0 = vramw[(name << 6) + (y) + (0x00)];
                b1 = vramw[(name << 6) + (y) + (0x10)];
                b2 = vramw[(name << 6) + (y) + (0x20)];
                b3 = vramw[(name << 6) + (y) + (0x30)];

                for(x = 0; x < 0x10; x += 1)
                {
                    i0 = (b0 >> (x ^ 0x0F)) & 1;
                    i1 = (b1 >> (x ^ 0x0F)) & 1;
                    i2 = (b2 >> (x ^ 0x0F)) & 1;
                    i3 = (b3 >> (x ^ 0x0F)) & 1;

                    c = (i3 << 3 | i2 << 2 | i1 << 1 | i0);

                    obj_pattern_cache[(name << 8) | (y << 4) | (x)] = (c);
                    //if (OBJ_CACHE_SIZE > 0x20000)
                        obj_pattern_cache[0x20000 | (name << 8) | (y << 4) | (x ^ 0x0F)] = (c);
                    //if (OBJ_CACHE_SIZE > 0x40000)
                    //    obj_pattern_cache[0x40000 | (name << 8) | ((y ^ 0x0F) << 4) | (x)] = (c);
                    //if (OBJ_CACHE_SIZE > 0x60000)
                    //    obj_pattern_cache[0x60000 | (name << 8) | ((y ^ 0x0F) << 4) | (x ^ 0x0F)] = (c);
                }
            }
        }
        obj_name_dirty[name] = 0;
    }
    obj_list_index = 0;
}


/*--------------------------------------------------------------------------*/
/* Render functions                                                         */
/*--------------------------------------------------------------------------*/

// 16 bits line rendering
static unsigned short linebuf[1024];

#define MIN(a,b) (a<b?a:b)
void render_line_generic(int line)
{
    if((reg[0x05] & 0x80) && (plane_enable & 1))
    {
        update_bg_pattern_cache();
        render_bg(line);
    }
    else
    {
        int i;
        uint16 *ptr = (uint16 *)&linebuf[XOFFSET]; //&bitmap.data[(line * bitmap.pitch) + (XOFFSET * bitmap.granularity)];
        for(i = 0; i < disp_width; i += 1) ptr[i] = pixel[0][0];                     
    }

    if((reg[0x05] & 0x40) && (plane_enable & 2))
    {
        update_obj_pattern_cache();
        render_obj(line);
    }
   
    emu_DrawLine16(&linebuf[XOFFSET], MIN(disp_width,256) , 240, line);

}



void render_bg(int line)
{
    uint16 *nt;
    uint8 *src, palette;
    uint16 *dst;   
    int column, name, attr, x, shift, v_line, nt_scroll;
    int xscroll = (reg[7] & 0x03FF);
    int end = disp_nt_width;

    /* Offset in pattern, in lines */
    v_line = (y_offset & 7);

    /* Offset in name table, in columns */
    nt_scroll = (xscroll >> 3);

    /* Offset in column, in pixels */
    shift = (xscroll & 7);

    /* Draw an extra tile for the last column */
    if(shift) end += 1;

    /* Point to current offset within name table */
    nt = (uint16 *)&vram[(y_offset >> 3) << playfield_shift];

    /* Point to start in line buffer */
    dst = (uint16 *)&linebuf[XOFFSET - shift]; //&bitmap.data[(line * bitmap.pitch) + ((XOFFSET + (0 - shift)) << 1)];

    /* Draw columns */
    for(column = 0; column < end; column += 1)
    {
        /* Get attribute */
        attr = nt[(column + nt_scroll) & playfield_row_mask];

        /* Extract name and palette bits */
        name = (attr & 0x07FF);
        palette = (attr >> 8) & 0xF0;

        /* Point to current pattern line */
        src = &bg_pattern_cache[(name << 6) + (v_line << 3)];

        /* Draw column */
        for(x = 0; x < 8; x += 1)
        {
            dst[(column << 3) | (x)] = pixel[0][(src[x] | palette)];          
        }
    }
}


void render_obj(int line)
{
    t_sprite *p;
    int j, i, x, c;
    int name, name_mask;
    int v_line;
    uint8 *src;
    int nt_line;
    uint16 *dst;  

    for(j = (used_sprite_index - 1); j >= 0; j -= 1)
    {
        i = used_sprite_list[j];
        p = &sprite_list[i];

        if( (line >= p->top) && (line < p->bottom))
        {
            v_line = (line - p->top) & p->height;
            nt_line = v_line;
            if(p->flags & FLAG_YFLIP) nt_line = (p->height - nt_line);
            name_mask = ((nt_line >> 4) & 3) << 1;
            name = (p->name_left | name_mask);
            v_line &= 0x0F;
            //src = &obj_pattern_cache[(name << 8) | ((v_line & 0x0f) << 4)];
            src = &obj_pattern_cache[(name << 8) | ( (p->flags & FLAG_YFLIP?v_line ^ 0x0F:v_line) << 4)];
            dst = (uint16 *)&linebuf[((XOFFSET+p->xpos) & 0x1ff)]; //&bitmap.data[(line * bitmap.pitch) + (((XOFFSET+p->xpos) & 0x1ff) * (bitmap.granularity))]; 
            for(x = 0; x < 0x10; x += 1)
            {
                c = src[x];
                //if ((unsigned int)&dst[x] < (unsigned int)&linebuf[400])
               if(c) dst[x] = pixel[1][((c) | p->palette)];
            }

            if(p->flags & FLAG_CGX)
            {
                name = (p->name_right | name_mask);
                //src = &obj_pattern_cache[(name << 8) | ((v_line & 0x0f) << 4)];
                src = &obj_pattern_cache[(name << 8) | ( (p->flags & FLAG_YFLIP?v_line^ 0x0F:v_line) << 4)];
                dst += 0x10;

                for(x = 0; x < 0x10; x += 1)
                {
                    c = src[x];
                    //if ((unsigned int)&dst[x] < (unsigned int)&linebuf[400])
                   if(c) dst[x] = pixel[1][((c) | p->palette)];        
                }
            }             
          
        }
       
    }
}
