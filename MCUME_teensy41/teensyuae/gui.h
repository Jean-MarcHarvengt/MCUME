 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Interface to the Tcl/Tk GUI
  * 
  * Copyright 1996 Bernd Schmidt
  */

extern int gui_init(void);
extern int gui_update(void);
extern void gui_exit(void);
extern void gui_led(int, int);
extern void gui_handle_events(void);
extern void gui_filename(int, const char *);
extern void gui_changesettings(void);

