 /*
  * UAE - The Un*x Amiga Emulator
  *
  *  Serial Line Emulation
  *
  * (c) 1996, 1997 Stefan Reinauer <stepan@matrix.kommune.schokola.de>
  * (c) 1997 Christian Schmitt <schmittc@uni-freiburg.de>
  *
  *  Thanks to Chris Hames <bitey@werple.net.au> for his hints.
  *
  */

#include "shared.h"

#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "readcpu.h"
#include "newcpu.h"
#include "cia.h"

#undef POSIX_SERIAL
/* Some more or less good way to determine whether we can safely compile in 
 * the serial stuff. I'm certain it breaks compilation on some systems. */
#if defined HAVE_SYS_TERMIOS_H && defined HAVE_POSIX_OPT_H && defined HAVE_SYS_IOCTL_H && defined HAVE_TCGETATTR
#define POSIX_SERIAL
#endif

#ifdef POSIX_SERIAL
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#if !defined B300 || !defined B230400
#undef POSIX_SERIAL
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

#define SERIALDEBUG 1 /* 0, 1, 2 3 */
#define MODEMTEST   1 /* 0 or 1 */

void serial_init(void);
void serial_exit(void);

uae_u16 SERDATR(void);
void  SERDATS(void);
void  SERPER(uae_u16 w);
void  SERDAT(uae_u16 w);

int waitqueue=0,carrier=0,dsr=0;

#ifndef __DOS__
//int sd = -1;
#else
//FILE *sd = NULL;
#endif

#ifdef POSIX_SERIAL
    struct termios tios;
#endif

uae_u16 serper=0,serdat;

void SERPER(uae_u16 w)
{
    int baud=0,pspeed;

    if (!currprefs.use_serial)
	return;
#if defined POSIX_SERIAL
    if (serper == w)  /* don't set baudrate if it's already ok */
        return;
    serper=w;

    if (w&0x8000)
        emu_printf ("SERPER: 9bit transmission not implemented\n");

    switch (w & 0x7fff)
    {
     /* These values should be calculated by the current 
      * color clock value (NTSC/PAL). But this solution is
      * easy and it works.
      */

     case 0x2e9b:
     case 0x2e14: baud=300; pspeed=B300; break;
     case 0x170a:
     case 0x0b85: baud=1200; pspeed=B1200; break;
     case 0x05c2:
     case 0x05b9: baud=2400; pspeed=B2400; break;
     case 0x02e9:
     case 0x02e1: baud=4800; pspeed=B4800; break;
     case 0x0174:
     case 0x0170: baud=9600; pspeed=B9600; break;
     case 0x00b9:
     case 0x00b8: baud=19200; pspeed=B19200; break;
     case 0x005c:
     case 0x005d: baud=38400; pspeed=B38400; break;
     case 0x003d: baud=57600; pspeed=B57600; break;
     case 0x001e: baud=115200; pspeed=B115200; break;
     case 0x000f: baud=230400; pspeed=B230400; break;
     default:
      //fprintf(stdout,"SERPER: unsupported baudrate (0x%04x) %d\n",w&0x7fff,
      //        (unsigned int)(3579546.471/(double)((w&0x7fff)+1)));  return;
    }
/*
    if(tcgetattr(sd,&tios)<0) {
      fprintf(stderr,"SERPER: TCGETATTR failed\n");
      return;
    }

    if(cfsetispeed(&tios,pspeed)<0) {
      fprintf(stderr,"SERPER: CFSETISPEED (%d bps) failed\n",baud);
      return;
    }
    if(cfsetospeed(&tios,pspeed)<0) {
      fprintf(stderr,"SERPER: CFSETOSPEED (%d bps) failed\n",baud);
      return;
    }

    if(tcsetattr(sd,TCSADRAIN,&tios)<0) {
      emu_printf("SERPER: TCSETATTR failed\n");
      return;
    }
*/    
#endif

#if SERIALDEBUG > 0
    //fprintf(stdout,"SERPER: set baudrate to %d bit/sec\n",baud);
#endif
}

/* Not (fully) implemented yet:
 *  -  Something's wrong with the Interrupts. 
 *     (NComm works, TERM does not. TERM switches to a
 *     blind mode after a connect and wait's for the end
 *     of an asynchronous read before switching blind
 *     mode off again. It never gets there on UAE :-< )
 *
 *  -  RTS/CTS handshake, this is not really neccessary,
 *     because you can use RTS/CTS "outside" without
 *     passing it through to the emulated Amiga
 *
 *  -  ADCON-Register ($9e write, $10 read) Bit 11 (UARTBRK)
 *     (see "Amiga Intern", pg 246)
 */

void SERDAT(uae_u16 w)
{
    unsigned char z;

    if (!currprefs.use_serial)
	return;

    z=(unsigned char)(w&0xff);
/*
#ifndef __DOS__
    if (sd!=0) {
	     write (sd,&z,1);
    }
#else
    if (sd!=NULL) {
	   fputc (z, sd);
	   fflush (sd);
    }
#endif
*/

#if SERIALDEBUG > 2
    //fprintf(stderr,"SERDAT: wrote 0x%04x\n",w);
#endif

    serdat|=0x2000; /* Set TBE in the SERDATR ... */
    intreq|=1;      /* ... and in INTREQ register */
    return;
}

uae_u16 SERDATR(void)
{
    if (!currprefs.use_serial)
	return 0;
#if SERIALDEBUG > 2
    //fprintf(stdout,"SERDATR: read 0x%04x\n",serdat);
#endif
    waitqueue = 0;
    return serdat;
}

void SERDATS(void)
{
    unsigned char z;
    int status;

    if ((!currprefs.use_serial)||((serdat&0x4000)!=0))
        return;

    if (waitqueue==1) {
      intreq|=0x0800;
      return;
    }
#ifdef unused    
    if ((read (sd, &z,1))==1) {
      waitqueue=1;
      serdat=0x4100; /* RBF and STP set! */
      serdat|=((unsigned int)z)&0xff;
      intreq|=0x0800; /* Set RBF flag (Receive Buffer full) */

#if SERIALDEBUG > 1
      //fprintf (stdout,"SERDATS: received 0x%02x --> serdat==0x%04x\n",
      //         (unsigned int)z, (unsigned int)serdat);
#endif
#ifdef POSIX_SERIAL
      ioctl(sd, TIOCMGET, &status);
      if (status & TIOCM_CAR) {
        if (!carrier) {
          CIA_shakehands_set(0x20); /* Push up Carrier Detect line */
          emu_printf ("SERDATS: carrier detect.\n");
          carrier=1;
        }
      } else {
        if (carrier) {
          CIA_shakehands_clear(0x20); /* Carrier lost */
          emu_printf ("SERDATS: carrier lost.\n");
          carrier=0;
        }
      }

      if (status & TIOCM_DSR) {
        if (!dsr) {
          CIA_shakehands_set(0x08); /* DSR on */
          dsr=1;
        }
      } else {
        if (dsr) {
          CIA_shakehands_clear(0x08); /* DSR off */
          dsr=0;
        }
      }
#endif
    }
#endif    
}

void serial_init(void)
{
    if (!currprefs.use_serial)
	return;

#ifndef __DOS__
    //if ((sd=open(sername,O_RDWR|O_NONBLOCK))<0)
    //  fprintf (stderr,"Error: Could not open Device %s\n",sername);
#else
    //if ((sd=open(sername,O_RDWR|O_NONBLOCK|O_BINARY))<0)
    //  fprintf (stdout,"Error: Could not open Device %s\n",sername);
#endif

#ifdef POSIX_SERIAL
    if(tcgetattr(sd,&tios)<0) {		/* Initialize Serial tty */
      emu_printf("Serial: TCGETATTR failed\n");
      return;
    }

    cfmakeraw(&tios);

#ifndef MODEMTEST
    tios.c_cflag &= ~CRTSCTS; /* Disable RTS/CTS */
#else
    tios.c_cflag |= CRTSCTS; /* Enabled for testing modems */
#endif

    if(tcsetattr(sd,TCSADRAIN,&tios)<0) {
      emu_printf("Serial: TCSETATTR failed\n");
      return;
    }
#endif
    serdat=0x2000;
    return;
}

void serial_exit(void)
{
#ifndef __DOS__
    //if (sd != 0)
    //  close(sd);
#else
    //if (sd != NULL)
    //  fclose(sd);
#endif
    return;
}

