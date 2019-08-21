#ifndef __VDC_H
#define __VDC_H

#define HALFHEIGHT 1

#define BORDERW 8
#define BMPW (160+BORDERW) // 320
#ifdef HALFHEIGHT
#define BMPH 120
#else
#define BMPH 240
#endif


extern Byte coltab[];
extern long clip_low;
extern long clip_high;

extern unsigned char * getGBuf(void);

void init_display(void);
void draw_display(void);
void draw_region(void);
void finish_display();
void close_display(void);
void clear_collision(void);

#endif


