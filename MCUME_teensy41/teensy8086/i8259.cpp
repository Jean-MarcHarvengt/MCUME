/* i8259.c - emulation code for the Intel 8259 controller.
   Note: This is not a very complete i8259 interrupt controller
   implementation, but for the purposes of a PC, it's all we need. */

#include <stdint.h>
#include <string.h>

struct structpic {
  uint8_t imr; //mask register
  uint8_t irr; //request register
  uint8_t isr; //service register
  uint8_t icwstep; //used during initialization to keep track of which ICW we're at
  uint8_t icw[5];
  uint8_t intoffset; //interrupt vector offset
  uint8_t priority; //which IRQ has highest priority
  uint8_t autoeoi; //automatic EOI mode
  uint8_t readmode; //remember what to return on read register from OCW3
  uint8_t enabled;
} i8259;


void init8259() {
     memset((void *)&i8259, 0, sizeof(i8259));
}

uint8_t in8259(uint16_t portnum) {
        switch (portnum & 1) {
               case 0:
                    if (i8259.readmode==0) return(i8259.irr); else return(i8259.isr);
               case 1: //read mask register
                    return(i8259.imr);
                }
                return(0); //can't get here, but the compiler bitches
}

extern uint32_t makeupticks;
void out8259(uint16_t portnum, uint8_t value) {
     uint8_t i;
     switch (portnum & 1) {
            case 0:
                 if (value & 0x10) { //begin initialization sequence
                    i8259.icwstep = 1;
                    i8259.imr = 0; //clear interrupt mask register
                    i8259.icw[i8259.icwstep++] = value;
                    return;
                 }
                 if ((value & 0x98)==8) { //it's an OCW3
                    if (value & 2) i8259.readmode = value & 2;
                 }
                 if (value & 0x20) { //EOI command
                    for (i=0; i<8; i++)
                        if ((i8259.isr >> i) & 1) {
                           i8259.isr ^= (1 << i);
                           if ((i==0) && (makeupticks>0)) { makeupticks = 0; i8259.irr |= 1; }
                           return;
                        }
                 }
                 break;
            case 1:
                 if ((i8259.icwstep==3) && (i8259.icw[1] & 2)) i8259.icwstep = 4; //single mode, so don't read ICW3
                 if (i8259.icwstep<5) { i8259.icw[i8259.icwstep++] = value; return; }
                 //if we get to this point, this is just a new IMR value
                 i8259.imr = value;
                 break;
     }
}

uint8_t nextintr() {
        uint8_t i, tmpirr;
        tmpirr = i8259.irr & (~i8259.imr); //XOR request register with inverted mask register
        for (i=0; i<8; i++)
            if ((tmpirr >> i) & 1) {
               i8259.irr ^= (1 << i);
               i8259.isr |= (1 << i);
               return(i8259.icw[2] + i);
                        }
                return(0); //can't get here, but the compiler bitches
}

void doirq(uint8_t irqnum) {
     i8259.irr |= (1 << irqnum);
}

