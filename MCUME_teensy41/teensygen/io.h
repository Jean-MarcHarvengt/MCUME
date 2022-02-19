#ifndef _IO_H_
#define _IO_H_

/*

void io_reset(void);
void io_set_version(int export, int pal, int fdd, int n);


*/

typedef struct
{
    void (*data_w)(uint8 data);
    uint8 (*data_r)(void);
} port_t;

typedef struct
{
    uint8 state;
} t_input_3b;

/* Global variables */
extern port_t port[3];
extern uint8 io_reg[0x10];

/* Function prototypes */
extern void io_reset(void);
extern void gen_io_w(int offset, int value);
extern int gen_io_r(int offset);
extern uint8 pad_2b_r(void);
extern uint8 device_3b_r(void);
extern void device_3b_w(uint8 data);

#endif /* _IO_H_ */

