
#ifndef _YM2413_H_
#define _YM2413_H_

/* Total # of YM2413's that can be used at once - change as needed */
#define MAX_YM2413  (4)

/* YM2413 context */
typedef struct
{
    unsigned char reg[0x40];            /* 64 registers */
    unsigned char latch;                /* Register latch */
    unsigned char rhythm;               /* Rhythm instruments loaded flag */
    unsigned char user[0x10];           /* User instrument settings */
    struct
    {
        unsigned short int frequency;   /* Channel frequency */
        unsigned char volume;           /* Channel volume */
        unsigned char instrument;       /* Channel instrument */
    }channel[9];
}t_ym2413;

/* Global data */
extern t_ym2413 ym2413[MAX_YM2413];

/* Function prototypes */
void ym2413_init(int count);
void ym2413_reset(int chip);
void ym2413_write(int chip, int address, int data);
void load_instrument(int chip, int ch, int inst, int vol);
void rhythm_mode_init(int chip);

#endif /* _YM2413_H_ */
