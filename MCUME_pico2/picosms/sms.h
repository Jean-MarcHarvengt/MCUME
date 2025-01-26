
#ifndef _SMS_H_
#define _SMS_H_

#define TYPE_OVERSEAS   (0)
#define TYPE_DOMESTIC   (1)

/* SMS context */
typedef struct
{
    byte dummy[0x2000];
    byte ram[0x2000];
    byte sram[0x8000];
    byte fcr[4];
    byte paused;
    byte save;
    byte country;
    byte port_3F;
    byte port_F2;
    byte use_fm;
    byte irq;
    byte psg_mask;
}t_sms;

/* Global data */
extern t_sms sms;

/* Function prototypes */
void sms_frame(int skip_render);
void sms_init(void);
void sms_reset(void);
int  sms_irq_callback(int param);
void sms_mapper_w(int address, int data);
void cpu_reset(void);

#endif /* _SMS_H_ */
