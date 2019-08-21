/*
 * proto.h - FAST 68000 emulator instruction prototypes
 * Version 1.0
 * Copyright (C) 1994, 1995 Joachim Hönig
 * (hoenig@informatik.uni-erlangen.de)
 *
 * This file is part of FAST, the Fine Atari ST Emulator.
 *
 * FAST is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FAST is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FAST; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * Prototypes of all 68000 opcodes defined in op68k*.c
 */

#include "operin.h"

#undef Oper
#define Oper(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12) \
    extern unsigned long a1(operin);

#define PROTOH

#include "op68kadd.cpp"
#include "op68karith.cpp"
#include "op68ksub.cpp"
#include "op68klogop.cpp"
#include "op68kmisc.cpp"
#include "op68kmove.cpp"
#include "op68kshift.cpp"

extern unsigned long     Op4840(operin);   /* SWAP */
extern unsigned long     Op4848(operin);   /* BKPT */
extern unsigned long     Op4880(operin);   /* EXT.W */
extern unsigned long     Op4890(operin);   /* MOVEM.W to (ax) */
extern unsigned long     Op48a0(operin);   /* MOVEM.W to -(ax) */
extern unsigned long     Op48a8(operin);   /* MOVEM.W to d(ax) */
extern unsigned long     Op48b0(operin);   /* MOVEM.W to d(ax,rx) */
extern unsigned long     Op48b8(operin);   /* MOVEM.W to w l */
extern unsigned long     Op48c0(operin);   /* EXT.L */
extern unsigned long     Op48d0(operin);   /* MOVEM.L to (ax) */
extern unsigned long     Op48e0(operin);   /* MOVEM.L to -(ax) */
extern unsigned long     Op48e8(operin);   /* MOVEM.L to d(ax) */
extern unsigned long     Op48f0(operin);   /* MOVEM.L to d(ax,rx) */
extern unsigned long     Op48f8(operin);   /* MOVEM.L to w l */
extern unsigned long     Op4c90(operin);   /* MOVEM.W from (ax) */
extern unsigned long     Op4c98(operin);   /* MOVEM.W from (ax)+ */
extern unsigned long     Op4ca8(operin);   /* MOVEM.W from d(ax) */
extern unsigned long     Op4cb0(operin);   /* MOVEM.W from d(ax,rx) */
extern unsigned long     Op4cb8(operin);   /* MOVEM.W from ea */
extern unsigned long     Op4cd0(operin);   /* MOVEM.L from (ax) */
extern unsigned long     Op4cd8(operin);   /* MOVEM.L from (ax)+ */
extern unsigned long     Op4ce8(operin);   /* MOVEM.L from d(ax) */
extern unsigned long     Op4cf0(operin);   /* MOVEM.L from d(ax,rx) */
extern unsigned long     Op4cf8(operin);   /* MOVEM.L from ea */
extern unsigned long     Op4e40(operin);   /* TRAP */
extern unsigned long     Op4e50(operin);   /* LINK */
extern unsigned long     Op4e58(operin);   /* UNLK */
extern unsigned long     Op4e60(operin);   /* MOVE ax,usp */
extern unsigned long     Op4e68(operin);   /* MOVE usp,ax */
extern unsigned long     Op4e70(operin);   /* RESET, NOP, STOP, RTE,
                                 * RTD, RTS, TRAPV, RTR */
extern unsigned long     Op4e78(operin);   /* MOVEC */

extern unsigned long     Opc140(operin);   /* EXG */
extern unsigned long     Opc148(operin);   /* EXG */
extern unsigned long     Opc188(operin);   /* EXG */
extern unsigned long     Opc340(operin);   /* EXG */
extern unsigned long     Opc348(operin);   /* EXG */
extern unsigned long     Opc388(operin);   /* EXG */
extern unsigned long     Opc540(operin);   /* EXG */
extern unsigned long     Opc548(operin);   /* EXG */
extern unsigned long     Opc588(operin);   /* EXG */
extern unsigned long     Opc740(operin);   /* EXG */
extern unsigned long     Opc748(operin);   /* EXG */
extern unsigned long     Opc788(operin);   /* EXG */
extern unsigned long     Opc940(operin);   /* EXG */
extern unsigned long     Opc948(operin);   /* EXG */
extern unsigned long     Opc988(operin);   /* EXG */
extern unsigned long     Opcb40(operin);   /* EXG */
extern unsigned long     Opcb48(operin);   /* EXG */
extern unsigned long     Opcb88(operin);   /* EXG */
extern unsigned long     Opcd40(operin);   /* EXG */
extern unsigned long     Opcd48(operin);   /* EXG */
extern unsigned long     Opcd88(operin);   /* EXG */
extern unsigned long     Opcf40(operin);   /* EXG */
extern unsigned long     Opcf48(operin);   /* EXG */
extern unsigned long     Opcf88(operin);   /* EXG */
