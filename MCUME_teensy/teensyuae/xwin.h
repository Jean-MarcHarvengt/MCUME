 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Interface to the graphics system (X, SVGAlib)
  * 
  * (c) 1995 Bernd Schmidt
  */

typedef unsigned short xcolnr;

extern xcolnr xcolors[4096];

extern int buttonstate[3];
extern int newmousecounters;
extern int lastmx, lastmy;

extern void handle_events(void);
extern void flush_line(int);
extern void flush_block(int, int);
extern void flush_screen(int, int);

extern int debuggable(void);
extern int needmousehack(void);
extern void togglemouse(void);
extern void LED(int);

struct vidbuf_description
{
    char *bufmem;
    int rowbytes;
    int pixbytes;
    int maxblocklines;
};

extern struct vidbuf_description gfxvidinfo;
