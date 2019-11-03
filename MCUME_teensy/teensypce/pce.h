#ifndef _PCE_H_
#define _PCE_H_

/* Global data */
extern uint8 joy_sel;
extern uint8 joy_clr;
extern uint8 joy_cnt;

/* Function prototypes */
int pce_init(void);
void pce_reset(void);
void pce_shutdown(void);
void cpu_writeport16(int port, int data);
void cpu_writemem21(int address, int data);
int pce_irq_callback(int irqline);
void io_page_w(int address, int data);
int io_page_r(int address);
void input_w(uint8 data);
uint8 input_r(void);
void bank_reset(void);
void bank_set(int bank, int value);

int load_rom(char *filename, int split, int flip);

#endif /* _PCE_H_ */
