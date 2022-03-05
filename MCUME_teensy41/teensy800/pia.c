#include <stdio.h>

#include "atari.h"
#include "cpu.h"
#include "pia.h"

UBYTE PACTL;
UBYTE PBCTL;
UBYTE PORTA;
UBYTE PORTB;


static UBYTE PORTA_mask = 0xff;
static UBYTE PORTB_mask = 0xff;

void PIA_Initialise(void)
{
	PORTA = 0xff;
	PORTB = 0xff;
}

UBYTE PIA_GetByte(UWORD addr)
{
	UBYTE byte;

	addr &= 0x03;
	switch (addr) {
  	case _PACTL:
  		byte = PACTL;
  		break;
  	case _PBCTL:
  		byte = PBCTL;
  		break;
  	case _PORTA:
  		byte = Atari_PORT(0);
  		byte &= PORTA_mask;
  		break;
  	case _PORTB:
  		byte = Atari_PORT(1);
  		byte &= PORTB_mask;
  		break;
	}

	return byte;
}

int PIA_PutByte(UWORD addr, UBYTE byte)
{
	addr &= 0xff03;

	switch (addr) {
  	case _PACTL:
  		PACTL = byte;
  		break;
  	case _PBCTL:
  		PBCTL = byte;
  		break;
  	case _PORTA:
  		if (!(PACTL & 0x04))
  			PORTA_mask = ~byte;
  		break;
  	case _PORTB:
  //		if ((byte == 0) && (machine == AtariXL || machine == AtariXE))
  //			break;				/* special hack for old Atari800 games like is Tapper, for example */
  		if (!(PBCTL & 0x04))
  			PORTB_mask = ~byte;
  		break;
	}

	return FALSE;
}
