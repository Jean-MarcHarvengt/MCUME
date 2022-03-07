/****************************************************************************
* Purpose : Keyboard input driver.
* Module  : Keyboard
* Author  : J-M Harvengt
* History : 10/08/97 Creation
****************************************************************************/
#ifndef _KEYBOARD
#define _KEYBOARD

/****************************************************************************
* Macros / typedefs
****************************************************************************/
#define SOURCE_REM   0x01
#define SOURCE_PAD   0x02
#define SOURCE_KEYB  0x03

/* keys id */
#define KEY_UNKNOWN 0x00
#define KEY_LEFT    0x01
#define KEY_RIGHT   0x02
#define KEY_UP      0x03
#define KEY_DOWN    0x04
#define KEY_SPACE   0x05

#define KEY_FONE    0x06
#define KEY_FTWO    0x07
#define KEY_FTHREE  0x08
#define KEY_FFOUR   0x09
#define KEY_FFIVE   0x0a
#define KEY_FSIX    0x0b
#define KEY_FSEVEN  0x0c
#define KEY_FEIGHT  0x0d
#define KEY_FNINE   0x0e
#define KEY_FZERO   0x0f

#define KEY_ONE     0x10
#define KEY_TWO     0x11
#define KEY_THREE   0x12
#define KEY_FOUR    0x13
#define KEY_FIVE    0x14
#define KEY_SIX     0x15
#define KEY_SEVEN   0x16
#define KEY_EIGHT   0x17
#define KEY_NINE    0x18
#define KEY_ZERO    0x19

#define KEY_OK        KEY_SPACE
#define KEY_BUT       KEY_SPACE
#define KEY_BACK      KEY_FONE
#define KEY_MENU      KEY_FTWO
#define KEY_TELEWEB   KEY_FTHREE
#define KEY_TELETEXT  KEY_FFOUR
#define KEY_COIN      KEY_FTHREE
#define KEY_ONEUP     KEY_FFOUR

/* Bit mask for pad keys */
#define JKEY_PLEFT	0x0001
#define JKEY_PRIGHT	0x0002
#define JKEY_PUP	0x0004
#define JKEY_PDOWN	0x0008
#define JKEY_PSPACE	0x0010
#define JKEY_PONE	0x0020
#define JKEY_PTWO	0x0040
#define JKEY_PTHREE	0x0080
#define JKEY_PFOUR	0x0100
#define JKEY_PFIVE	0x0200
#define JKEY_PSIX	0x0400
#define JKEY_PSEVEN	0x0800
#define JKEY_PEIGHT	0x1000
#define JKEY_PNINE	0x2000
#define JKEY_PZERO	0x4000

#define JKEY_LEFT	0x00010000
#define JKEY_RIGHT	0x00020000
#define JKEY_UP		0x00040000
#define JKEY_DOWN	0x00080000
#define JKEY_SPACE	0x00100000
#define JKEY_ONE	0x00200000
#define JKEY_TWO	0x00400000
#define JKEY_THREE	0x00800000
#define JKEY_FOUR	0x01000000
#define JKEY_FIVE	0x02000000
#define JKEY_SIX	0x04000000
#define JKEY_SEVEN	0x08000000
#define JKEY_EIGHT	0x10000000
#define JKEY_NINE	0x20000000
#define JKEY_ZERO	0x40000000

#define JKEY_OK			JKEY_SPACE
#define JKEY_BUT		JKEY_SPACE
#define JKEY_BACK		JKEY_ONE
#define JKEY_MENU		JKEY_TWO
#define JKEY_TELEWEB	JKEY_THREE
#define JKEY_TELETEXT	JKEY_FOUR
#define JKEY_COIN		JKEY_THREE
#define JKEY_ONEUP	    JKEY_FOUR
#define JKEY_PPLUS	    JKEY_EIGHT
#define JKEY_PMINUS	    JKEY_NINE

#define JKEY_POK		JKEY_PSPACE
#define JKEY_PBUT		JKEY_PSPACE
#define JKEY_PBACK		JKEY_PONE
#define JKEY_PMENU		JKEY_PTWO
#define JKEY_PTELEWEB	JKEY_PTHREE
#define JKEY_PTELETEXT	JKEY_PFOUR
#define JKEY_PEPG		JKEY_PFIVE
#define JKEY_PCOIN		JKEY_PTHREE
#define JKEY_PONEUP	    JKEY_PFOUR
#define JKEY_PPPLUS	    JKEY_PEIGHT
#define JKEY_PPMINUS	JKEY_PNINE

/* Bit mask for mouse button */
#define LMOUSEDOWN      0x40
#define LMOUSEUP        0x80

/****************************************************************************
* Exported procedures
****************************************************************************/
extern void key_Init(void);
extern void key_TurnOn(void);
extern void key_TurnOff(void);

extern void key_OnKeyDown(int aSource, int aKey);
extern void key_OnKeyUp(int aSource, int aKey);
extern void key_SubscribeKeyUp(void * callback);
extern void key_SubscribeKeyDown(void * callback);
extern void key_OnKeyTimer(void);
extern int  key_GetKeyPad(void);

extern void key_OnMouseMove(int x, int y, int b);
extern void key_GetMouseMove(int *x, int *y, int *b);
#endif
