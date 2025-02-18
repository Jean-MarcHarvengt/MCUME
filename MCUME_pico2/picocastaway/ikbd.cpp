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
*  23.05.2002  0.02.00 JH  FAST1.0.1 code import: KR -> ANSI, restructuring
*  09.06.2002  0.02.00 JH  Renamed io.c to st.c again (io.h conflicts with system headers)
*  16.06.2002  0.02.00 JH  New function: IkbdQueryBuffer(). X11 keysym conversion removed.
*/
static char     sccsid[] = "$Id: ikbd.c,v 1.3 2002/06/16 23:16:09 jhoenig Exp $";
//#include "winbase.h"
#include <stdio.h>
#include <string.h>
#include "dcastaway.h"
#include "st.h"
#define IBS 10                  /* Ikbd input buffer size */
#define OBS 20                  /* Ikbd output buffer size */


//extern char Msg[400];
extern FILE* logfile;
extern int exmousex,exmousey;
int             pause = 0;
struct _mouse mouse;
struct _joystick joystick;

extern unsigned char joynum;
unsigned char   inbuff[IBS];
unsigned char   outbuff[OBS];
int             inbuffi = 0;
int             outbuffi = 0;
int				oldjoystate =0;
int				joytick=0;
unsigned char Buttons;
//SYSTEMTIME SystemTime;
unsigned char TempButtons=0x0a;
unsigned char PrevButtons;

unsigned char Misc_ConvertToBCD(unsigned short int Value)
{
	return( ((Value&0xf0)>>4)*10 + (Value&0x0f) );
}

void IkbdRecv(unsigned char inchar)
{
    if (inbuffi == IBS) {
        inbuffi = 0;
    }
    inbuff[inbuffi++] = inchar;
     //Serial.print("d ");
     //Serial.println(inbuff[0]);
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
			mouse.x=mouse.xmax/2;
			mouse.y=mouse.ymax/2;
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
        
		if (outbuffi < (OBS -6)){
			inbuffi = 0;
			/* Test buttons */
			Buttons = 0;
			/* Set buttons to show if up/down */
			if (mouse.stbuttons&1) Buttons |= 0x01; else Buttons |= 0x02;
			if (mouse.stbuttons&2) Buttons |= 0x04; else Buttons |= 0x08;       
			
			/* Mask off it didn't send last time */
			PrevButtons = TempButtons;
			TempButtons = Buttons;
			Buttons &= ~PrevButtons;
			
			IkbdSend (0xf7);
			IkbdSend (Buttons);
			IkbdSend (mouse.stx >> 8);
			IkbdSend (mouse.stx);
			IkbdSend (mouse.sty >> 8);
			IkbdSend (mouse.sty);
		}
        break;
		
    case 0x0e:                  /* load mouse position */
        if (inbuffi == 6) {
            inbuffi = 0;
            mouse.x = exmousex = mouse.stx = (inbuff[2] << 8) | inbuff[3];
            mouse.y = exmousey = mouse.sty = (inbuff[4] << 8) | inbuff[5];
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
        pause = 0;
        break;
    case 0x12:                  /* disable mouse */
        inbuffi = 0;
        //mouse.mode = 0;
        break;
    case 0x13:                  /* pause output */
        inbuffi = 0;
        //pause = 1;
        break;
    case 0x14:                  /* set joystick event reporting */
        inbuffi = 0;
        joystick.mode = 1;
        break;
    case 0x15:                  /* set joystick interrogation mode */
        inbuffi = 0;
        joystick.mode = 2;
        break;
    case 0x16:                  /* joystick interrogation */
        inbuffi = 0;
        IkbdSend (0xfd);
        IkbdSend (0);
        IkbdSend (oldjoystate);
        break;
    case 0x17:                  /* set joystick monitoring */
        if (inbuffi == 2) {
            inbuffi = 0;
            joystick.rate = inbuff[1];
            joystick.mode = 4;
        }
        break;
    case 0x18:                  /* set fire button monitoring */
        inbuffi = 0;
        joystick.mode = 5;
        break;
    case 0x19:                  /* set joystick keycode mode */
        if (inbuffi == 7) {
            inbuffi = 0;
            joystick.mode = 3;
        }
        break;
    case 0x1a:                  /* disable joysticks */
        inbuffi = 0;
		// joystick.mode = 0;
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
    case 0x21:                  /* memory read */
    case 0x22:                  /* controller execute */
        inbuffi = 0;
        break;
    case 0x80:                  /* reset */
        if (inbuffi == 2) {
            IkbdSend (0xf0);
            inbuffi = 0;
            mouse.buttons = mouse.stbuttons = 0;
            switch (vid_shiftmode) {
            case 0:
                mouse.x = exmousex = mouse.stx = 160;
                mouse.y = exmousey = mouse.sty = 100;
                break;
            case 1:
                mouse.x = exmousex = mouse.stx = 320;
                mouse.y = exmousey = mouse.sty = 100;
                break;
            case 2:
                mouse.x = exmousex = mouse.stx = 320;
                mouse.y = exmousey = mouse.sty = 200;
                break;
            }
        }
        break;
    case 0x87:                  /* request button action */
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
			inbuffi = 0;
			switch (joystick.mode) {
			case 1:
				IkbdSend (0x14);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				break;
			case 2:
				IkbdSend (0x15);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				IkbdSend (0);
				break;
			case 3:
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
				if (joystick.mode)
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
    }
}

void IkbdSend (unsigned char outchar)
{
    if (outbuffi < (OBS - 1)) {
        //Serial.print("q ");
        //Serial.println(outchar);
        outbuff[outbuffi++] = outchar;
    }
}

void IkbdWriteBuffer(void)
{
/*
if (mouse.flag && (outbuffi < (OBS / 2))) {
IkbdMouse ();
}
if (joystick.mode==4) IkbdJoystick();
	*/
    if (outbuffi>0)
    {     
      if ((acia1_sr & 0x1) == 0) 
      {
          //Serial.println("KW");
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
}

void IkbdKeyPress (unsigned short keysym) {
    IkbdSend (keysym);
}

void IkbdKeyRelease (unsigned short keysym) {
    IkbdSend (0x80 | keysym);
}

void IkbdMouseMotion (int x, int y)
{
	if (vid_shiftmode==1) {x+=x;}
	if (vid_shiftmode==2) {x+=x; y+=y;}
    mouse.x=x; mouse.y=y;
}

void IkbdMousePress (int code) {
    mouse.buttons |= code;
}

void IkbdMouseRelease (int code) {
    mouse.buttons &= ~code;
}

void joystickevent (int joystate)
{
	//Has joystick moved?
	if (joystate==oldjoystate) return;
	oldjoystate=joystate;
	//send fire button to mouse as well
	if (mouse.mode==1 || mouse.mode==0) {
		if (joystate&0x80) {
			IkbdSend (0xf9);
            IkbdSend (0);
            IkbdSend (0);
		}
		else {
			IkbdSend (0xf8);
            IkbdSend (0);
            IkbdSend (0);
		}
	}
	//In event mode send the joystick event immediately
	if (joystick.mode==1) {
		IkbdSend (0xff);
		IkbdSend (joystate);
	}
}

void IkbdLoop()
{
    int dx, dy;
	if (mouse.mode==1) {
		dx=mouse.x-mouse.stx; if (dx>127) dx=127;  else if (dx<-127) dx=-127;
		dy=mouse.y-mouse.sty; if (dy>127) dy=127; else if (dy<-127) dy=-127;
		if (mouse.stbuttons!=mouse.buttons || dx>mouse.xth || dy>mouse.yth || -dx>mouse.xth || -dy>mouse.yth) {
			if (outbuffi<OBS/2) {
				mouse.stbuttons = mouse.buttons;
				mouse.stx = mouse.stx + dx;
				mouse.sty = mouse.sty + dy;
				if (mouse.stx <= 0) {mouse.stx = 0; dx = -127;}
				if (mouse.sty <= 0) {mouse.sty = 0; dy = -127;}
				switch (vid_shiftmode) {
				case 0:
					if (mouse.stx >= 319) {mouse.stx = 319; dx = 127;}
					if (mouse.sty >= 199) {mouse.sty = 199; dy = 127;}
					break;
				case 1:
					if (mouse.stx >= 639) {mouse.stx = 639; dx = 127;}
					if (mouse.sty >= 199) {mouse.sty = 199; dy = 127;}
					break;
				case 2:
					if (mouse.stx >= 639) {mouse.stx = 639; dx = 127;}
					if (mouse.sty >= 399) {mouse.sty = 399; dy = 127;}
					break;
				}
				IkbdSend (0xf8 | (mouse.stbuttons&3));
				IkbdSend (dx);
				IkbdSend (dy);
			}
		}
	}
	else {
		if (mouse.x>mouse.xmax) mouse.x=mouse.xmax;
		if (mouse.y>mouse.ymax) mouse.y=mouse.ymax;
		mouse.stx=mouse.x;
		mouse.sty=mouse.y;
		mouse.stbuttons=mouse.buttons;
		
	}
	if (joystick.mode==4) {
		//Called at 200hz when joystick is in mode 4
		joytick+=2;
		if (joytick<joystick.rate) return;
		joytick=0;
		IkbdSend (oldjoystate>>7);
		IkbdSend (oldjoystate&15);
	}
}

void IkbdReset (void) 
{
	joystick.mode=1;
	mouse.mode=1;
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
}


void IkbdJoystickChange(int number, uint8 state)
{
    joystickevent(state);
}
