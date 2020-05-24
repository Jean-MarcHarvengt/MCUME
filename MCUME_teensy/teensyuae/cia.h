 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * CIA chip support
  *
  * (c) 1995 Bernd Schmidt
  */

void CIA_reset(void);
void CIA_vsync_handler(void);
void CIA_hsync_handler(void);
void CIA_handler(void);

void dumpcia(void);
