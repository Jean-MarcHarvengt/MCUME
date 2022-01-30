 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Serial Line Emulation
  *
  * Copyright 1996, 1997 Stefan Reinauer <stepan@matrix.kommune.schokola.de>
  * Copyright 1997 Christian Schmitt <schmittc@uni-freiburg.de>
  */

extern void serial_init(void);
extern void serial_exit(void);

extern uae_u16 SERDATR(void);
extern void  SERDATS(void);
extern void  SERPER(uae_u16 w);
extern void  SERDAT(uae_u16 w);

extern uae_u16 serdat;

