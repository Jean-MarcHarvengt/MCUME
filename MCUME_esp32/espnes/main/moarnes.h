#define PIXEL_ACCURACY 1
 
//#define resetCPU() CPU_reset() //nes6502_reset()
//#define execCPU(x) CPU_execute(x) //nes6502_execute(x)
//#define nmiCPU() NMI(0) //nes6502_nmi()  
//#define irqCPU() IRQ(0) //nes6502_irq()  

#define resetCPU() reset6502();
#define execCPU(x) exec6502(x) 
#define nmiCPU() nmi6502()  
#define irqCPU() irq6502()  


#include <stdlib.h>
#include "stdint.h"
#include "6502.h"
//#include "APU.h"
#include "PPU.h"
#include "rom.h"
#include "prototypes.h"
