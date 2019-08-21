/*
 * $Id: mnemonic.h,v 1.1 1995/11/26 21:52:43 alex Exp alex $
 *
 * This file is part of Commodore 64 emulator.
 * See README for copyright notice
 *
 * This file contains #defines for MOS6010 instruction mnemonics.
 *
 * Written by
 *   Vesa-Matti Puro (vmp@lut.fi)
 *   Jouko Valta (jopi@stekt.oulu.fi)
 *
 * $Log: mnemonic.h,v $
 * Revision 1.1  1995/11/26 21:52:43  alex
 * Initial revision
 *
 * Revision 1.6  1994/12/12  16:59:44  jopi
 * *** empty log message ***
 *
 * Revision 1.5  1994/06/16  17:19:26  jopi
 * Code reorganized and cleaned up
 *
 * Revision 1.4  1993/11/10  01:55:34  jopi
 * reu, asm and disk directory fixed
 * REL_ADDR macro and 1541 made more portable
 *
 * Revision 1.3  93/06/21  13:38:45  jopi
 *  X64 version 0.2 PL 0
 *
 * Revision 1.2  1993/06/13  08:21:50  sonninen
 * *** empty log message ***
 *
 *
 */

#ifndef X64_MNEMONIC_H
#define X64_MNEMONIC_H


/* INSTRUCTION MNEMONICS. */

#define ADC	"ADC"
#define AND	"AND"
#define ASL	"ASL"
#define BCC	"BCC"
#define BCS	"BCS"
#define BEQ	"BEQ"
#define BIT	"BIT"
#define BMI	"BMI"
#define BNE	"BNE"
#define BPL	"BPL"
#define BRK	"BRK"
#define BVC	"BVC"
#define BVS	"BVS"
#define CLC	"CLC"
#define CLD	"CLD"
#define CLI	"CLI"
#define CLV	"CLV"
#define CMP	"CMP"
#define CPX	"CPX"
#define CPY	"CPY"
#define DEC	"DEC"
#define DEX	"DEX"
#define DEY	"DEY"
#define EOR	"EOR"
#define INC	"INC"
#define INX	"INX"
#define INY	"INY"
#define JMP	"JMP"
#define JSR	"JSR"
#define LDA	"LDA"
#define LDX	"LDX"
#define LDY	"LDY"
#define LSR	"LSR"
#define NOOP	"NOOP"
#define NOP	"NOP"
#define ORA	"ORA"
#define PHA	"PHA"
#define PHP	"PHP"
#define PLA	"PLA"
#define PLP	"PLP"
#define ROL	"ROL"
#define ROR	"ROR"
#define RTI	"RTI"
#define RTS	"RTS"
#define SBC	"SBC"
#define SEC	"SEC"
#define SED	"SED"
#define SEI	"SEI"
#define STA	"STA"
#define STX	"STX"
#define STY	"STY"
#define TAX	"TAX"
#define TAY	"TAY"
#define TSX	"TSX"
#define TXA	"TXA"
#define TXS	"TXS"
#define TYA	"TYA"

#ifndef NO_UNDOC_CMDS
#define ANC	"ANC"
#define ANE	"ANE"
#define ARR	"ARR"
#define ASR	"ASR"
#define DCP	"DCP"
#define ISB	"ISB"
#define JAM	"JAM"
#define LAS	"LAS"
#define LAX	"LAX"
#define LXA	"LXA"
 /* NOOP undefined NOP */
#define RLA	"RLA"
#define RRA	"RRA"
#define SAX	"SAX"
#define USBC	"USBC"	/* undefined SBC */
#define SBX	"SBX"
#define SHA	"SHA"
#define SHS	"SHS"
#define SHX	"SHX"
#define SHY	"SHY"
#define SLO	"SLO"
#define SRE	"SRE"

#else
#define ANC	NOOP
#define ANE	NOOP
#define ARR	NOOP
#define ASR	NOOP
#define DCP	NOOP
#define ISB	NOOP
#define JAM	NOOP
#define LAS	NOOP
#define LAX	NOOP
#define LXA	NOOP
 /* NOOP undefined NOP */
#define RLA	NOOP
#define RRA	NOOP
#define SAX	NOOP
#define USBC	NOOP
#define SBX	NOOP
#define SHA	NOOP
#define SHS	NOOP
#define SHX	NOOP
#define SHY	NOOP
#define SLO	NOOP
#define SRE	NOOP
#endif

#endif  /* X64_MNEMONIC_H */
