/*
 * Castaway
 *  (C) 1994 - 2002 Martin Doering, Joachim Hoenig
 *
 * ikbd.c - ST keyboard processor emulator (german keymap)
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 * revision history
 *  23.05.2002  JH  FAST1.0.1 code import: KR -> ANSI, restructuring
 *  09.06.2002  JH  Renamed io.c to st.c again (io.h conflicts with system headers)
 *  16.06.2002  JH  New function: IkbdQueryBuffer(). X11 keysym conversion removed.
 *  29.10.2002  JH  Added joystick interrogation mode and Joystick functions.
 */
static char     sccsid[] = "$Id: ikbd.c,v 1.5 2003/03/07 20:24:15 mdoering Exp $";
#include <stdio.h>
#ifndef DREAMCAST
#include <string.h>
#else
#include <string.h>
#endif
#include "config.h"
#include "st.h"
#include "mem.h"
#include "m68k_intrf.h"

#define max(A,B) ((A>B)?A:B)


void IkbdMouse(void);

struct _mouse mouse;
struct _joystick joystick;
unsigned char   inbuff[IBS];
unsigned char   outbuff[OBS];
int             inbuffi = 0;
int             outbuffi = 0;

static unsigned char prevbut=0x2|0x8;

int ikbd_pulling=0;
int ikbd_direct=0;
static unsigned ikbd_writed=6;
static int ikbd_pause = 0;
static int mouse_xabs=0;
static int mouse_yabs=0;
static int mouse_dx=0;
static int mouse_dy=0;

void            IkbdRecv(unsigned char inchar)
{
#ifdef DEBUG_IKBD
    printf ("IkbdRecv: 0x%X (inbuffi=%i)\n", inchar, inbuffi); fflush(stdout);
#endif
    ikbd_pulling=0;
    if (inbuffi == IBS) {
#ifdef DEBUG_IKBD
        printf ("IkbdRecv: Input buffer overrun!\n"); fflush(stdout);
#endif
        inbuffi = 0;
    }
    inbuff[inbuffi++] = inchar;
    switch (inbuff[0]) {
    case 0x07:                  /* set mouse button action */
        if (inbuffi == 2) {
            inbuffi = 0;
            mouse.button_action = inbuff[1];
        }
        break;
    case 0x08:                  /* set relative mouse position reporting */
        inbuffi = 0;
        mouse.mode = 1;
        break;
    case 0x09:                  /* set absolute mouse positioning */
        if (inbuffi == 5) {
            mouse.mode = 2;
            mouse.xmax = (inbuff[1] << 8) | inbuff[2];
            mouse.ymax = (inbuff[3] << 8) | inbuff[4];
            inbuffi = 0;
        }
        break;
    case 0x0a:                  /* set mouse keycode mode */
        if (inbuffi == 3) {
            mouse.mode = 3;
            mouse.xkcm = inbuff[1];
            mouse.ykcm = inbuff[2];
            inbuffi = 0;
        }
        break;
    case 0x0b:                  /* set mouse threshold */
        if (inbuffi == 3) {
            mouse.xth = inbuff[1];
            mouse.yth = inbuff[2];
            inbuffi = 0;
        }
        break;
    case 0x0c:                  /* set mouse scale */
        if (inbuffi == 3) {
            mouse.xscale = inbuff[1];
            mouse.yscale = inbuff[2];
            inbuffi = 0;
        }
        break;
    case 0x0d:                  /* interrogate mouse position */
	inbuffi=0;
	if ((outbuffi+6) > (OBS - 1))
		break;

	if (mouse_xabs>mouse.xmax) mouse_xabs=mouse.xmax;
	else if (mouse_xabs<0) mouse_xabs=0;
	if (mouse_yabs>mouse.ymax) mouse_yabs=mouse.ymax;
	else if (mouse_yabs<0) mouse_yabs=0;

	if (ikbd_writed>=6)
	{
		ikbd_writed=0;
		unsigned char but=0,tmprev;
		if(mouse.buttons&1)
			but|=1;
		else
			but|=2;
		if (mouse.buttons&2)
			but|=4;
		else
			but|=8;

		tmprev=prevbut;
		prevbut = but;
		but &= ~tmprev;

        	IkbdSend (0xf7);
		IkbdSend (but);
		IkbdSend ((mouse_xabs >> 8)&0xff);
		IkbdSend (mouse_xabs&0xff);
		IkbdSend ((mouse_yabs >> 8)&0xff);
		IkbdSend ((mouse_yabs)&0xff);
		mouse.flag = 1;
	}
        break;
    case 0x0e:                  /* load mouse position */
        if (inbuffi == 6) {
            inbuffi = 0;
            mouse.x = (inbuff[2] << 8) | inbuff[3];
            mouse.y = (inbuff[4] << 8) | inbuff[5];
        }
        break;
    case 0x0f:                  /* set Y=0 at bottom */
        inbuffi = 0;
        mouse.yinv = 1;
        break;
    case 0x10:                  /* set Y=0 at top */
        inbuffi = 0;
        mouse.yinv = 0;
        break;
    case 0x11:                  /* resume */
        inbuffi = 0;
/*
        ikbd_pause = 0;
*/
        break;
#if 0
    case 0x00:			/* ??? */
        inbuffi = 0;
	ikbd_direct = 0;
	break;
#endif
    case 0x12:                  /* disable mouse */
        inbuffi = 0;
#if 0
	if (!ikbd_direct)
        	mouse.mode = 0;
	else
		mouse.mode = 2;
#else
	mouse.mode = 0;
	ikbd_direct = 1;
#endif
	break;
    case 0x13:                  /* ikbd_pause output */
        inbuffi = 0;
/*
        ikbd_pause = 1;
*/
        break;
    case 0x14:                  /* set joystick event reporting */
        inbuffi = 0;
	ikbd_direct = 1;
        joystick.mode = inbuff[0];
        break;
    case 0x15:                  /* set joystick interrogation mode */
        inbuffi = 0;
	ikbd_direct = 0;
        joystick.mode = inbuff[0];
        break;
    case 0x16:                  /* joystick interrogation */
        inbuffi = 0;
        IkbdSend(0xfd);
        IkbdSend(joystick.state1);
        IkbdSend(joystick.state0);
    	ikbd_pulling=1;
        break;
    case 0x17:                  /* set joystick monitoring */
        if (inbuffi == 2) {
            inbuffi = 0;
            joystick.rate = inbuff[1];
            joystick.mode = inbuff[0];
        }
        break;
    case 0x18:                  /* set fire button monitoring */
        inbuffi = 0;
        joystick.mode = inbuff[0];
        break;
    case 0x19:                  /* set joystick keycode mode */
        if (inbuffi == 7) {
            inbuffi = 0;
            joystick.mode = inbuff[0];
        }
        break;
    case 0x1a:                  /* disable joysticks */
	if (!mouse.mode)
		mouse.mode=2;
        inbuffi = 0;
        joystick.mode = inbuff[0];
	ikbd_direct = 1;
        break;
    case 0x1b:                  /* time-of-day clock set */
        if (inbuffi == 7) {
            inbuffi = 0;
        }
        break;
    case 0x1c:                  /* interrogate time-of-day clock */
        inbuffi = 0;
        IkbdSend (0xfc);
        IkbdSend (0x0);
        IkbdSend (0x0);
        IkbdSend (0x0);
        IkbdSend (0x0);
        IkbdSend (0x0);
        IkbdSend (0x0);
        break;
    case 0x20:                  /* memory load */
        if (inbuffi == 4) {
            inbuffi = 0;
        }
	break;
    case 0x21:                  /* memory read */
        if (inbuffi == 3) {
            inbuffi = 0;
        }
	break;
    case 0x22:                  /* controller execute */
        if (inbuffi == 3) {
            inbuffi = 0;
        }
        break;
    case 0x80:                  /* reset */
        if (inbuffi == 2)
	{
            IkbdSend (0xf0);
            inbuffi = 0;
	    ikbd_direct = 0;
            mouse.buttons = 0;
            mouse.stbuttons = 0;
            switch (vid_shiftmode) {
            case 0:
                mouse.stx = 160;
                mouse.sty = 100;
                break;
            case 1:
                mouse.stx = 320;
                mouse.sty = 100;
                break;
            case 2:
                mouse.stx = 320;
                mouse.sty = 200;
                break;
            }
        }
        break;
    case 0x87:                  /* request button action */
        inbuffi = 0;
        IkbdSend (0x07);
        IkbdSend (mouse.button_action);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        break;
    case 0x88:                  /* request mouse mode */
    case 0x89:
    case 0x8a:
        switch (mouse.mode) {
        case 1:
            IkbdSend (0x08);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            break;
        case 2:
            IkbdSend (0x09);
            IkbdSend (mouse.xmax >> 8);
            IkbdSend (mouse.xmax);
            IkbdSend (mouse.ymax >> 8);
            IkbdSend (mouse.ymax);
            IkbdSend (0);
            IkbdSend (0);
            break;
        case 3:
            IkbdSend (0x09);
            IkbdSend (mouse.xkcm);
            IkbdSend (mouse.ykcm);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            break;
        }
        inbuffi = 0;
        mouse.mode = 1;
        break;
    case 0x8b:                  /* request mouse threshold */
        IkbdSend (0x0b);
        IkbdSend (mouse.xth);
        IkbdSend (mouse.yth);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        inbuffi = 0;
        break;
    case 0x8c:                  /* request mouse scale */
        IkbdSend (0x0b);
        IkbdSend (mouse.xscale);
        IkbdSend (mouse.yscale);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        inbuffi = 0;
        break;
    case 0x8f:                  /* request mouse vertical coordinates */
    case 0x90:
        if (mouse.yinv)
            IkbdSend (0x0f);
        else
            IkbdSend (0x10);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        inbuffi = 0;
        break;
    case 0x92:                  /* disable mouse */
        if (mouse.mode)
            IkbdSend (0);
        else
            IkbdSend (0x12);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        IkbdSend (0);
        inbuffi = 0;
        break;
    case 0x94:                  /* request joystick mode */
    case 0x95:
    case 0x99:                  /* set joystick keycode mode */
        switch (joystick.mode) {
        case 0x14:
        case 0x15:
            IkbdSend (joystick.mode);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            IkbdSend (0);
            break;
        case 0x19:
            IkbdSend (0x19);
            IkbdSend (joystick.rx);
            IkbdSend (joystick.ry);
            IkbdSend (joystick.tx);
            IkbdSend (joystick.ty);
            IkbdSend (joystick.vx);
            IkbdSend (joystick.vy);
            break;
        }
        inbuffi = 0;
        break;
    case 0x9a:                  /* request joystick availability */
        if (joystick.mode != 0x1a)
            IkbdSend (0);
        else
            IkbdSend (0x1a);
        IkbdSend (joystick.rx);
        IkbdSend (joystick.ry);
        IkbdSend (joystick.tx);
        IkbdSend (joystick.ty);
        IkbdSend (joystick.vx);
        IkbdSend (joystick.vy);
        inbuffi = 0;
        break;
    default:
	inbuffi = 0;
    }
}

void            IkbdSend (unsigned char outchar)
{
#ifdef DEBUG_IKBD
	printf("IkbdSend(0x%X)\n",outchar); fflush(stdout);
#endif
    if (outbuffi < (OBS - 1))
        outbuff[outbuffi++] = outchar;
}



/*
 * This function tests if there is still
 * Ikbd Output waiting to be written
 */
int             IkbdQueryBuffer(void)
{
    return ((mouse.flag || outbuffi > 0) && !ikbd_pause);
}

void            IkbdWriteBuffer(void)
{
    if (mouse.flag && (outbuffi < (OBS / 2))) {
        IkbdMouse ();
    }

    if (outbuffi > 0 && !ikbd_pause && (acia1_sr & 0x1) == 0) {
	ikbd_writed++;
        acia1_sr |= 1;
        if (acia1_cr & 0x80) {  /* receiver interrupt enabled? */
            acia1_sr |= 0x80;
            mfp_gpip &= (~0x10);
        }
        acia1_dr = outbuff[0];
        memcpy (&outbuff[0], &outbuff[1], (OBS - 1));
        outbuffi--;
    }

}

void            IkbdKeyPress (unsigned short keysym)
{
#ifdef DEBUG_IKBD
	printf("IkbdKeyPress(%X)\n",keysym); fflush(stdout);
#endif
    IkbdSend (keysym);
}

void            IkbdKeyRelease (unsigned short keysym)
{
#ifdef DEBUG_IKBD
	printf("IkbdKeyRelease(%X)\n",keysym); fflush(stdout);
#endif
    IkbdSend (0x80 | keysym);
}

void IkbdMouse(void)
{
    int dx,dy;
    mouse.flag = 0;
    switch (mouse.mode) {

    case 1:
    	dx = mouse.x - mouse.stx;
    	dy = mouse.y - mouse.sty;
        if (dx > 127) {
            dx = 127;
            mouse.flag = 1;
        }
        if (dx < -127) {
            dx = -127;
            mouse.flag = 1;
        }
        if (dy > 127) {
            dy = 127;
            mouse.flag = 1;
        }
        if (dy < -127) {
            dy = -127;
            mouse.flag = 1;
        }
        if (mouse.stbuttons != mouse.buttons
                || max (dx, -dx) > mouse.xth || max (dy, -dy) > mouse.yth) {
            mouse.stbuttons = mouse.buttons;
            mouse.stx = mouse.stx + dx;
            mouse.sty = mouse.sty + dy;
            if (mouse.stx <= 0) {
                mouse.stx = 0;
                dx = -127;
            }
            if (mouse.sty <= 0) {
                mouse.sty = 0;
                dy = -127;
            }
            switch (vid_shiftmode) {
            case 0:
                if (mouse.stx >= 319) {
                    mouse.stx = 319;
                    dx = 127;
                }
                if (mouse.sty >= 199) {
                    mouse.sty = 199;
                    dy = 127;
                }
                break;
            case 1:
                if (mouse.stx >= 639) {
                    mouse.stx = 639;
                    dx = 127;
                }
                if (mouse.sty >= 199) {
                    mouse.sty = 199;
                    dy = 127;
                }
                break;
            case 2:
                if (mouse.stx >= 639) {
                    mouse.stx = 639;
                    dx = 127;
                }
                if (mouse.sty >= 399) {
                    mouse.sty = 399;
                    dy = 127;
                }
                break;
            }
            IkbdSend (0xf8 | mouse.buttons);
            IkbdSend (dx);
            IkbdSend (dy);
        }
#ifdef DEBUG_IKBD
        printf ("Mouse1: x %d, y %d, dx %d, dy %d, but %d\n", mouse.stx, mouse.sty, dx, dy, mouse.buttons); fflush(stdout);
#endif
        break;

    case 2:
//if (mouse.x>mouse.xmax) mouse.x=mouse.xmax;
//if (mouse.y>mouse.ymax) mouse.y=mouse.ymax;
#ifdef DEBUG_IKBD
	puts("MOUSE MODE NOT SUPPORTED YET"); fflush(stdout);
#endif
//        break;

//case 0:
    case 3:
        if ((mouse.stbuttons&0x3) != (mouse.buttons&0x3))
	{
	    if ((mouse.stbuttons&0x1) != (mouse.buttons&0x1))
	    {
		    if (mouse.stbuttons&0x1)
		    {
			    IkbdSend(0xF5);
			    IkbdSend(0xF5);
		    }
		    else
		    {
			    IkbdSend(0x75);
			    IkbdSend(0x75);
		    }
	    }
	    else
	    if ((mouse.stbuttons&0x2) != (mouse.buttons&02))
	    {
		    if (mouse.stbuttons&0x2)
		    {
			    IkbdSend(0xF4);
			    IkbdSend(0xF4);
		    }
		    else
		    {
			    IkbdSend(0x74);
			    IkbdSend(0x74);
		    }
	    }
            mouse.stbuttons = mouse.buttons;
	}
	{
		int i;
    		dx = mouse_dx;
    		dy = mouse_dy;
		for(i=0;i<10;i++)
		{
			if (dx<0)
			{
				IkbdSend(75);
				IkbdSend(75|0x80);
				dx++;
			}
			else if (dx>0)
			{
				IkbdSend(77);
				IkbdSend(77|0x80);
				dx--;
			}
			if (dy<0)
			{
				IkbdSend(72);
				IkbdSend(72|0x80);
				dy++;
			}
			else if (dy>0)
			{
				IkbdSend(80);
				IkbdSend(80|0x80);
				dy--;
			}
		}
	}
        mouse_dx = mouse_dy = 0;
#ifdef DEBUG_IKBD
        printf ("Mouse0: x %d, y %d, dx %d, dy %d, but %d\n", mouse.stx, mouse.sty, dx, dy, mouse.buttons); fflush(stdout);
#endif
	break;
#ifdef DEBUG_IKBD
    default:
	puts("MOUSE MODE ERROR"); fflush(stdout);
#endif
   }
}

void            IkbdMouseMotion (int x, int y, int dx, int dy)
{
    static int xant=0, yant=0;

    mouse_xabs=x;
    mouse_yabs=y;
    mouse_dx=dx;
    mouse_dy=dy;
    switch (vid_shiftmode) {
    case 1:
	 mouse.x += (x-xant);
	 if (mouse.x < 0)
		mouse.x = 0;
	 else
		if (mouse.x > 640)
			mouse.x = 639;
	 mouse.y += (y-yant);
	 if (mouse.y < 0)
		mouse.y = 0;
	 else
		if (mouse.y>200)
			mouse.y=199;
	 break;
    case 0:
	 mouse.x += (x-xant);
	 if (mouse.x < 0)
		 mouse.x = 0;
	 else
		if (mouse.x > 320)
			mouse.x = 319;
	 mouse.y += (y-yant); 
	 if (mouse.y < 0)
		 mouse.y = 0;
	 else
		if (mouse.y>200)
			mouse.y=199;
	 break;
    case 2:
	 mouse.x += (x-xant);
	 if (mouse.x < 0)
		mouse.x = 0;
	 else
		if (mouse.x > 640)
			mouse.x = 639;
	 mouse.y += (y-yant);
	 if (mouse.y < 0)
		 mouse.y = 0;
	 else
		if (mouse.y>400)
			mouse.y=399;
        break;
    }
    mouse.flag = 1;
    xant=x;
    yant=y;
}

void            IkbdMousePress (int code)
{
#ifdef DEBUG_IKBD
	printf("IkbdMousePress(%X)\n",code); fflush(stdout);
#endif
    mouse.buttons |= code;
    IkbdMouse();
}

void            IkbdMouseRelease (int code)
{
#ifdef DEBUG_IKBD
	printf("IkbdMouseRelease(%X)\n",code); fflush(stdout);
#endif
    mouse.buttons &= ~code;
    IkbdMouse();
}

void            IkbdJoystickChange(int number, uint8 state)
{
    static uint8 joyold0=0;
    static uint8 joyold1=0;
#ifdef DEBUG_IKBD
	printf("IkbdJoystickChange(%i,0x%X)\n",number,state); fflush(stdout);
#endif
    if (number == 0 && joystick.state0 != state) {
        joystick.state0 = state;
        /* TODO: only if joystick event reporting selected */
	if (!ikbd_pulling)
	{
		if (!ikbd_direct)
		{
			if ((state&0x80)!=joyold0)
			{
				IkbdSend(0xf8|(state>>7));
				IkbdSend(state&0x7f);
				IkbdSend(0);
				joyold0=state&0x80;
			}
			else
			{
        			IkbdSend(0xff);
        			IkbdSend(state&0x7f);
			}
		}
		else
		{
        		IkbdSend(0xff);
        		IkbdSend(state);
		}
	}
    }
    if (number == 1 && joystick.state1 != state) {
        joystick.state1 = state;
        /* TODO: only if joystick event reporting selected */
	if (!ikbd_pulling)
	{
		if (!ikbd_direct)
		{
			if ((state&0x80)!=joyold1)
			{
				IkbdSend(0xfa|(state>>7));
				IkbdSend(state&0x7f);
				IkbdSend(0);
				joyold1=state&0x80;
			}
			else
			{
        			IkbdSend(0xfe);
        			IkbdSend(state&0x7f);
			}
		}
		else
		{
        		IkbdSend(0xfe);
        		IkbdSend(state);
		}
	}
    }
}


void  Ikbd_Reset(void)
{
	//SDL_Event event;
	//SDL_Delay(333);
	//while(SDL_PollEvent(&event))
	//	SDL_Delay(20);
	IkbdSend(0x80);
	IkbdSend(0xff);
	IkbdSend(0x80);
	acia1_cr=150;
	acia1_sr=0;
	acia1_dr=240;
	acia2_cr=149;
	acia2_sr=0;
	acia2_dr=0;
	inbuffi = 0;
	outbuffi = 0;
	ikbd_pause = 0;
	ikbd_pulling=0;
	ikbd_direct=0;
        mouse.mode = 1;
	prevbut=0x2|0x8;
	ikbd_writed=6;
}

void  IkbdReset(void)
{
    Ikbd_Reset();
}

void  IkbdLoop(void)
{
}

void  IkbdMouseMotion(int x, int y)
{
    IkbdMouseMotion(x,y,1,1);
}

