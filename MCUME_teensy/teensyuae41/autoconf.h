 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Autoconfig device support
  *
  * (c) 1996 Ed Hanway
  */

typedef ULONG (*TrapFunction)(void);

extern ULONG addr(int);
extern void dw(UWORD);
extern void dl(ULONG);
extern ULONG ds(char *);
extern void calltrap(ULONG);
extern void org(ULONG);
extern ULONG here(void);
extern int deftrap(TrapFunction);
extern void align(int);
extern ULONG CallLib(CPTR base, WORD offset);

#define RTS (0x4e75)

extern ULONG explibname;
