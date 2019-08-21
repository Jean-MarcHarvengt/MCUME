#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include "config.h"

//extern void do_events (void) ;

extern int emulating;
extern int nScreenRefreshRate;
extern int draw_border, maybe_border;
extern unsigned screen_pitch, screen_width, screen_height;
extern unsigned cyclenext;
extern unsigned vid_adr_cycleyet;
extern unsigned char *vid_cycle;


