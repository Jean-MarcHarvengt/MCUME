#ifndef VMACHINE_H
#define VMACHINE_H

#include "types.h"

#define LINECNT 21
#define MAXLINES 500
#define MAXSNAP 50

#define VBLCLK 5493
#define EVBLCLK_NTSC 5964
#define EVBLCLK_PAL 7259

#define FPS_NTSC 60
#define FPS_PAL 50

extern Byte dbstick1, dbstick2;
extern int last_line;

extern int evblclk;

extern int master_clk;		/* Master clock */
extern int int_clk;		/* counter for length of /INT pulses for JNI */
extern int h_clk;   /* horizontal clock */
extern int mstate;

extern Byte rom1[];
extern Byte rom2[];
extern Byte rom3[];
extern Byte rom4[];

extern Byte intRAM[];
extern Byte extRAM[];
extern Byte extROM[];
extern Byte VDCwrite[256];
extern Byte ColorVector[MAXLINES];
extern Byte AudioVector[MAXLINES];
extern Byte *rom;
extern Byte *orom;

extern int frame;
extern int key2[128];
extern int key2vcnt;
extern unsigned long clk_counter;
extern int ccolflag;

extern int enahirq;
extern int pendirq;
extern int useforen;
extern long regionoff;
extern int sproff;
extern int tweakedaudio;

Byte read_P2(void);
int snapline(int pos, Byte reg, int t);
void ext_write(Byte dat, ADDRESS adr);
Byte ext_read(ADDRESS adr);
void handle_vbl(void);
void handle_evbl(void);
Byte in_bus(void);
void write_p1(Byte d);
Byte read_t1(void);
void init_system(void);
void run(void);


extern struct resource {
	int bank;
	int debug;
	int stick[2];
	int limit;
	int sound_en;
	int speed;
	int wsize;
	int fullscreen;
	int scanlines;
	int voice;
	int svolume;
	int vvolume;	
	int exrom;
	int filter;
	int euro;
	int openb;
	unsigned long crc;
	char *window_title;
	char *scshot;
} app_data;


#endif  /* VMACHINE_H */

