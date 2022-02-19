
#ifndef _VDP_H_
#define _VDP_H_

/* Global variables */
extern uint8 sat[0x400];
//extern uint8 vram[0x10000];
extern uint8 cram[0x80];               
extern uint8 vsram[0x80];              
extern uint8 reg[0x20];
extern uint16 status;
extern uint16 ntab;                    
extern uint16 ntbb;                    
extern uint16 ntwb;                    
extern uint16 satb;                    
extern uint16 hscb;                    
extern uint8 is_color_dirty;           
extern uint8 color_dirty[0x40];        
extern uint8 border;                   
extern uint8 is_border_dirty;          
//extern uint8 bg_name_dirty[0x800];     
//extern uint16 bg_name_list[0x800];     
extern uint16 bg_list_index;           
//extern uint8 bg_pattern_cache[0x80000];
extern uint8 playfield_shift;          
extern uint8 playfield_col_mask;       
extern uint16 playfield_row_mask;      
extern uint32 y_mask;                  
extern int vint_pending;
extern int hint_pending;
extern int scanline;
extern int counter;
extern int im2_flag;
extern int frame_end;
extern int v_counter, v_update;

extern void (*color_update)(int index, uint16 data);

/* Function prototypes */
void vdp_init(void);
void vdp_reset(void);
void vdp_shutdown(void);
void vdp_ctrl_w(uint16 data);
uint16 vdp_ctrl_r(void);
void vdp_data_w(uint16 data);
uint16 vdp_data_r(void);
void vdp_reg_w(uint8 r, uint8 d);
uint16 vdp_hvc_r(void);
void dma_copy(void);
void dma_vbus(void);
void vdp_test_w(uint16 value);

#endif /* _VDP_H_ */
