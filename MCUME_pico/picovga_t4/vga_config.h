#include "platform_config.h"
#include "iopins.h"

// ****************************************************************************
//                                 
//                            VGA configuration
//
// ****************************************************************************

// === Configuration
#define LAYERS		1 //4	// total layers 1..4 (1 base layer + 3 overlapped layers)
#define SEGMAX		8	// max. number of video segment per video strip (size of 1 sSegm = 28 bytes)
#define STRIPMAX	8	// max. number of video strips (size of 1 sStrip = sSegm size*SEGMAX+4 = 228 bytes)
				    // size of sScreen = sStrip size*STRIPMAX+4 = 1828 bytes

#define MAXX		320 //640	// max. resolution in X direction (must be power of 4)
#define MAXY		240 //480	// max. resolution in Y direction

#define MAXLINE		700	// max. number of scanlines (including sync and dark lines)

// === Scanline render buffers (800 pixels: default size of buffers = 2*4*(800+8+800+24)+800 = 13856 bytes
//    Requirements by format, base layer 0, 1 wrap X segment:
//	GF_GRAPH8 ... control buffer 16 bytes
//	GF_TILE8 ... control buffer "width"+8 bytes
//	GF_TILE16 ... control buffer "width/2"+8 bytes
//	GF_TILE32 ... control buffer "width/4"+8 bytes
//	GF_TILE64 ... control buffer "width/8"+8 bytes
//	GF_PROGRESS ... control buffer 24 bytes
//	other formats: data buffer "width" bytes, control buffer 16 bytes
#define DBUF0_MAX	(MAXX+8)	// max. size of data buffer of layer 0
#define CBUF0_MAX	((MAXX+24)/4)	// max. size of control buffer of layer 0

//    Requirements by format, overlapped layer 1..3:
//	LAYERMODE_SPRITE* ... data buffer "width"+4 bytes, control buffer 24 bytes
//	LAYERMODE_FASTSPRITE* ... data buffer "width"+4 bytes, control buffer up to "width*2"+16 bytes
//	other formats ... data buffer 4 bytes, control buffer 24 bytes
#define DBUF1_MAX	(MAXX+8)	// max. size of data buffer of layer 1
#define CBUF1_MAX	((MAXX+24)/4)	// max. size of control buffer of layer 1

#define DBUF2_MAX	(MAXX+8)	// max. size of data buffer of layer 2
#define CBUF2_MAX	((MAXX+24)/4)	// max. size of control buffer of layer 2

#define DBUF3_MAX	(MAXX+8)	// max. size of data buffer of layer 3
#define CBUF3_MAX	((MAXX+24)/4)	// max. size of control buffer of layer 3

#if LAYERS==1
#define	DBUF_MAX	DBUF0_MAX	// max. size of data buffer
#define	CBUF_MAX	CBUF0_MAX	// max. size of control buffer
#elif LAYERS==2
#define	DBUF_MAX	(DBUF0_MAX+DBUF1_MAX)	// max. size of data buffer
#define	CBUF_MAX	(CBUF0_MAX+CBUF1_MAX)	// max. size of control buffer
#elif LAYERS==3
#define	DBUF_MAX	(DBUF0_MAX+DBUF1_MAX+DBUF2_MAX)	// max. size of data buffer
#define	CBUF_MAX	(CBUF0_MAX+CBUF1_MAX+CBUF2_MAX)	// max. size of control buffer
#elif LAYERS==4
#define	DBUF_MAX	(DBUF0_MAX+DBUF1_MAX+DBUF2_MAX+DBUF3_MAX) // max. size of data buffer
#define	CBUF_MAX	(CBUF0_MAX+CBUF1_MAX+CBUF2_MAX+CBUF3_MAX) // max. size of control buffer
#else
#error Unsupported number of layers!
#endif

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
#define VGA_GPIO_NUM	9	// number of VGA GPIOs, including HSYNC and VSYNC
#define VGA_GPIO_OUTNUM	8	// number of VGA color GPIOs, without HSYNC and VSYNC
#define VGA_GPIO_LAST	(VGA_GPIO_FIRST+VGA_GPIO_NUM-1)	// last VGA GPIO
#define VGA_GPIO_SYNC	VGA_SYNCBASE	// VGA SYNC GPIO

// VGA PIO and state machines
#define VGA_PIO		pio0	// VGA PIO
#define VGA_SM0		0	// VGA state machine of base layer 0
#define VGA_SM1		1	// VGA state machine of overlapped layer 1
#define VGA_SM2		2	// VGA state machine of overlapped layer 2
#define VGA_SM3		3	// VGA state machine of overlapped layer 3
#define VGA_SM(layer)	(VGA_SM0+(layer)) // VGA state machine of the layer

#if LAYERS==1
#define VGA_SMALL	B0	// mask of all state machines
#elif LAYERS==2
#define VGA_SMALL	(B0+B1) // mask of all state machines
#elif LAYERS==3
#define VGA_SMALL	(B0+B1+B2) // mask of all state machines
#elif LAYERS==4
#define VGA_SMALL	(B0+B1+B2+B3) // mask of all state machines
#else
#error Unsupported number of layers!
#endif

// VGA DMA
#define VGA_DMA		2		// VGA DMA base channel
#define VGA_DMA_CB0	(VGA_DMA+0)	// VGA DMA channel - control block of base layer
#define VGA_DMA_PIO0	(VGA_DMA+1)	// VGA DMA channel - copy data of base layer to PIO (raises IRQ0 on quiet)
#define VGA_DMA_CB1	(VGA_DMA+2)	// VGA DMA channel - control block of overlapped layer 1
#define VGA_DMA_PIO1	(VGA_DMA+3)	// VGA DMA channel - copy data of overlapped layer 1 to PIO
#define VGA_DMA_CB2	(VGA_DMA+4)	// VGA DMA channel - control block of overlapped layer 1
#define VGA_DMA_PIO2	(VGA_DMA+5)	// VGA DMA channel - copy data of overlapped layer 2 to PIO
#define VGA_DMA_CB3	(VGA_DMA+6)	// VGA DMA channel - control block of overlapped layer 1
#define VGA_DMA_PIO3	(VGA_DMA+7)	// VGA DMA channel - copy data of overlapped layer 3 to PIO

#define VGA_DMA_CB(layer) (VGA_DMA_CB0+(layer)*2) // VGA DMA control channel of the layer
#define VGA_DMA_PIO(layer) (VGA_DMA_PIO0+(layer)*2) // VGA DMA data channel of the layer

#define VGA_DMA_NUM	(LAYERS*2)	// number of used DMA channels
#define VGA_DMA_FIRST	VGA_DMA		// first used DMA
#define VGA_DMA_LAST	(VGA_DMA_FIRST+VGA_DMA_NUM-1) // last used DMA
