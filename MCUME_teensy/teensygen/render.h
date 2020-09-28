
#ifndef _RENDER_H_
#define _RENDER_H_


/* Look-up pixel table information */
#define LUT_MAX         (5)
#define LUT_SIZE        (0x10000)

/* Clip structure */
typedef struct
{
    uint8 left;
    uint8 right;
    uint8 enable;
}clip_t;

/* Function prototypes */
int render_init(void);
void render_reset(void);
void render_shutdown(void);
void render_line(int line);
void render_obj(int line, uint8 *buf, uint8 *table);
void render_obj_im2(int line, uint8 *buf, uint8 *table);
void render_ntw(int line, uint8 *buf);
void render_ntw_im2(int line, uint8 *buf);
void render_ntx(int which, int line, uint8 *buf);
void render_ntx_im2(int which, int line, uint8 *buf);
void render_ntx_vs(int which, int line, uint8 *buf);
void update_bg_pattern_cache(void);
void get_hscroll(int line, uint16 *scrolla, uint16 *scrollb);
void window_clip(int line);
void remap_16(uint8 *src, uint16 *dst, uint16 *table, int length);
void merge(uint8 *srca, uint8 *srcb, uint8 *dst, uint8 *table, int width);
void color_update_16(int index, uint16 data);
void make_name_lut(void);
void parse_satb(int line);

#endif /* _RENDER_H_ */
