/*
 * $Id: types.h,v 1.1 1995/11/26 21:52:43 alex Exp alex $
 *
 * This file is part of Commodore 64 emulator.
 * See README for copyright notice
 *
 * This file declares types used all over.
 *
 *
 * Written by
 *   Jarkko Sonninen (sonninen@lut.fi)
 *   Jouko Valta (jopi@stekt.oulu.fi)
 *
 *
 * $Log: types.h,v $
 * Revision 1.1  1995/11/26 21:52:43  alex
 * Initial revision
 *
 * Revision 1.4  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * Typedef for signed char.
 * Room for new options for Printer charsets, Basic patch, REU size.
 *
 * Revision 1.3  1994/12/12  16:59:44  jopi
 * 16-bit portability
 *
 * Revision 1.2  1994/08/10  18:34:13  jopi
 * More changeability
 *
 * Revision 1.1  1994/06/16  17:19:26  jopi
 * Initial revision
 *
 */


#ifndef X64_TYPES_H
#define X64_TYPES_H


typedef signed char	SIGNED_CHAR;
/*typedef char		SIGNED_CHAR;*/

typedef unsigned char 	byte;
typedef unsigned char 	BYTE;
typedef unsigned short 	ADDRESS;
typedef unsigned long	CLOCK;

typedef int		ADDR_T;
/*typedef long		ADDR_T;*/

#endif  /* X64_TYPES_H */
