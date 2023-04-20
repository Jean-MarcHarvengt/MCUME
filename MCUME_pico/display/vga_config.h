#include "platform_config.h"
#include "iopins.h"

// ****************************************************************************
//                                 
//                            VGA configuration
//
// file derived from the PicoVGA project
// https://github.com/Panda381/PicoVGA
// by Miroslav Nemecek
//
// ****************************************************************************

// === Configuration
#define MAXX		320 //640	// max. resolution in X direction (must be power of 4)
#define MAXY		240 //480	// max. resolution in Y direction

#define MAXLINE		525 //700	// max. number of scanlines (including sync and dark lines)

// === Scanline render buffers (800 pixels: default size of buffers = 2*4*(800+8+800+24)+800 = 13856 bytes
//    Requirements by format, base layer 0, 1 wrap X segment:
//	GF_GRAPH8 ... control buffer 4*4=16 bytes
#define	CBUF_MAX	8 //((MAXX+24)/4)	// max. size of control buffer of layer 0


// === VGA port pins
//	GP0 ... VGA B0 blue
//	GP1 ... VGA B1
//	GP2 ... VGA G0 green
//	GP3 ... VGA G1
//	GP4 ... VGA G2
//	GP5 ... VGA R0 red
//	GP6 ... VGA R1
//	GP7 ... VGA R2
//	GP8 ... VGA SYNC synchronization (inverted: negative SYNC=LOW=0x80, BLACK=HIGH=0x00)
#define VGA_GPIO_FIRST	VGA_COLORBASE	// first VGA GPIO
#ifdef VGA222
#define VGA_GPIO_OUTNUM	6   // number of VGA color GPIOs, without HSYNC and VSYNC
#define VGA_GPIO_NUM	7	// number of VGA GPIOs, including HSYNC and VSYNC
#else
#define VGA_GPIO_OUTNUM	8	// number of VGA color GPIOs, without HSYNC and VSYNC
#define VGA_GPIO_NUM	9	// number of VGA GPIOs, including HSYNC and VSYNC
#endif
#define VGA_GPIO_LAST	(VGA_GPIO_FIRST+VGA_GPIO_NUM-1)	// last VGA GPIO
#define VGA_GPIO_SYNC	VGA_SYNCBASE	// VGA SYNC GPIO

// === VGA PIO program
#define BASE_OFFSET	17	// offset of base layer program

// VGA PIO and state machines
#define VGA_PIO		pio0	// VGA PIO
#define VGA_SM0		0	// VGA state machine of base layer 0

// LAYERS==1
#define VGA_SMALL	B0	// mask of all state machines


// VGA DMA
#define VGA_DMA		2		// VGA DMA base channel
#define VGA_DMA_CB0	(VGA_DMA+0)	// VGA DMA channel - control block of base layer
#define VGA_DMA_PIO0	(VGA_DMA+1)	// VGA DMA channel - copy data of base layer to PIO (raises IRQ0 on quiet)

#define VGA_DMA_NUM	(LAYERS*2)	// number of used DMA channels
#define VGA_DMA_FIRST	VGA_DMA		// first used DMA
#define VGA_DMA_LAST	(VGA_DMA_FIRST+VGA_DMA_NUM-1) // last used DMA
