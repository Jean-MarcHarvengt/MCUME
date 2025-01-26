#ifndef CPU_H_
#define CPU_H_

#include "atari.h"

#define CPU_N_FLAG 0x80
#define CPU_V_FLAG 0x40
#define CPU_B_FLAG 0x10
#define CPU_D_FLAG 0x08
#define CPU_I_FLAG 0x04
#define CPU_Z_FLAG 0x02
#define CPU_C_FLAG 0x01

void CPU_GetStatus(void);
void CPU_PutStatus(void);
void CPU_Reset(void);
void CPU_StateSave(UBYTE SaveVerbose);
void CPU_StateRead(UBYTE SaveVerbose, UBYTE StateVersion);
void CPU_NMI(void);
void CPU_GO(int limit);
#define CPU_GenerateIRQ() (CPU_IRQ = 1)

extern UWORD CPU_regPC;
extern UBYTE CPU_regA;
extern UBYTE CPU_regP;
extern UBYTE CPU_regS;
extern UBYTE CPU_regY;
extern UBYTE CPU_regX;

#define CPU_SetN CPU_regP |= CPU_N_FLAG
#define CPU_ClrN CPU_regP &= (~CPU_N_FLAG)
#define CPU_SetV CPU_regP |= CPU_V_FLAG
#define CPU_ClrV CPU_regP &= (~CPU_V_FLAG)
#define CPU_SetB CPU_regP |= CPU_B_FLAG
#define CPU_ClrB CPU_regP &= (~CPU_B_FLAG)
#define CPU_SetD CPU_regP |= CPU_D_FLAG
#define CPU_ClrD CPU_regP &= (~CPU_D_FLAG)
#define CPU_SetI CPU_regP |= CPU_I_FLAG
#define CPU_ClrI CPU_regP &= (~CPU_I_FLAG)
#define CPU_SetZ CPU_regP |= CPU_Z_FLAG
#define CPU_ClrZ CPU_regP &= (~CPU_Z_FLAG)
#define CPU_SetC CPU_regP |= CPU_C_FLAG
#define CPU_ClrC CPU_regP &= (~CPU_C_FLAG)

extern UBYTE CPU_IRQ;

extern void (*CPU_rts_handler)(void);

extern UBYTE CPU_cim_encountered;

#define CPU_REMEMBER_PC_STEPS 64
extern UWORD CPU_remember_PC[CPU_REMEMBER_PC_STEPS];
extern UBYTE CPU_remember_op[CPU_REMEMBER_PC_STEPS][3];
extern unsigned int CPU_remember_PC_curpos;
extern int CPU_remember_xpos[CPU_REMEMBER_PC_STEPS];

#define CPU_REMEMBER_JMP_STEPS 16
extern UWORD CPU_remember_JMP[CPU_REMEMBER_JMP_STEPS];
extern unsigned int CPU_remember_jmp_curpos;

#ifdef MONITOR_PROFILE
extern int CPU_instruction_count[256];
#endif

#endif /* CPU_H_ */
