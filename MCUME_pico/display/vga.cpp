// ****************************************************************************
//
//                                 VGA output
//
// file derived from the PicoVGA project
// https://github.com/Panda381/PicoVGA
// by Miroslav Nemecek
//
// ****************************************************************************

#include "include.h"

// base layer commands
#define VGADARK(num,col) (((u32)(vga_offset_dark+BASE_OFFSET)<<27) | ((u32)(num)<<8) | (u32)(col)) // assemble control word of "dark" command
#define VGACMD(jmp,num) (((u32)(jmp)<<27) | (u32)(num)) // assemble control word

// scanline type
u8 ScanlineType[MAXLINE];

// current videomode
sVmode CurVmode;		// copy of current videomode table
volatile int ScanLine;		// current scan line 1...
volatile u32 Frame;		// frame counter
volatile int BufInx;		// current buffer set (0..1)
volatile Bool VSync;		// current scan line is vsync or dark

static u8* framebuffer;
static int fbwidth;


u32	LineBufHsBp[4];     // HSYNC ... back porch-1 ... IRQ command ... image command
u32	LineBufFp;          // front porch+1
u32	LineBufDark[2];     // HSYNC ... dark line
u32	LineBufSync[10];    // vertical synchronization

// control buffers (BufInx = 0 running CtrlBuf1 and preparing CtrlBuf2, BufInx = 1 running CtrlBuf2 and preparing CtrlBuf1)
u32	CtrlBuf1[CBUF_MAX]; // base layer control pairs: u32 count, read address (must be terminated with [0,0])
u32	CtrlBuf2[CBUF_MAX]; // base layer control pairs: u32 count, read address (must be terminated with [0,0])

// next control buffer
u32*	CtrlBufNext;

// saved integer divider state
hw_divider_state_t DividerState;

// process scanline buffers (will save integer divider state into DividerState)
int __not_in_flash_func(VgaBufProcess)()
{
	// Clear the interrupt request for DMA control channel
	dma_hw->ints0 = (1u << VGA_DMA_PIO0);

	// switch current buffer index
	//   BufInx = 0 running CtrlBuf1 and preparing CtrlBuf2, BufInx = 1 running CtrlBuf2 and preparing CtrlBuf1
	//   bufinx = 0 was running CtrlBuf1, will run CtrlBuf2, will process CtrlBuf1
	int bufinx = BufInx;
	BufInx = bufinx ^ 1;

	// update DMA control channels of base layer, and run it
	dma_channel_set_read_addr(VGA_DMA_CB0, CtrlBufNext, true);

	// save integer divider state
	hw_divider_save_state(&DividerState);

	// increment scanline
	int line = ScanLine;	// current scanline
	line++; 		// new current scanline
	if (line > CurVmode.vtot) // last scanline?
	{
		Frame++;	// increment frame counter
		line = 1; 	// restart scanline
	}
	ScanLine = line;	// store new scanline

	int y0 = -1;
	u8 linetype = ScanlineType[line];
	switch (linetype)
	{
	case LINE_IMG:		// progressive image 0, 1, 2,...
		y0 = line - CurVmode.vfirst;
		if (CurVmode.dbly) y0 >>= 1;
		VSync = False;	// not vsync
		break;
	default:
		VSync = True;	// vsync
		break;
	}

	return bufinx;
}



// VGA DMA handler - called on end of every scanline
extern "C" void __not_in_flash_func(VgaLine)()
{
	// process scanline buffers (will save integer divider state into DividerState)
	int bufinx = VgaBufProcess();

	// prepare buffers to be processed next
	u32* cbuf; // control buffer
	if (bufinx == 0)
	{
		cbuf = CtrlBuf1;
	}
	else
	{
		cbuf = CtrlBuf2;
	}
	CtrlBufNext = cbuf;

	// next rendered scanline
	int line = ScanLine;	// current scanline
	line++; // next line to render
	if (line > CurVmode.vtot) line = 1;
	int y0;

	u8 linetype = ScanlineType[line];
#ifdef VGA_VSYNC
	if (linetype == LINE_VSYNC)
	{
		gpio_put(VGA_VSYNC, 0);
	}
	else	
	{
		gpio_put(VGA_VSYNC, 1);
	}
#endif
	switch (linetype)
	{
	case LINE_VSYNC:	// long vertical sync
		*cbuf++ = 2; // send 2x u32
		*cbuf++ = (u32)&LineBufSync[0]; // VSYNC
		break;

	case LINE_DARK:		// dark line
		*cbuf++ = 2; // send 2x u32
		*cbuf++ = (u32)LineBufDark; // dark
		break;

	case LINE_IMG:		// progressive image 0, 1, 2,...
		y0 = line - CurVmode.vfirst;
		if (CurVmode.dbly) y0 >>= 1;

		// HSYNC + back porch
		*cbuf++ = 4; // send 4x u32
		*cbuf++ = (u32)LineBufHsBp; // HSYNC + back porch

		// image data
		*cbuf++ = fbwidth/4;
		*cbuf++ = (u32)&framebuffer[y0*fbwidth];
		
		// front porch
		*cbuf++ = 1; // send 1x u32
		*cbuf++ = (u32)&LineBufFp; // front porch
		break;
	}

	*cbuf++ = 0; // end mark
	*cbuf++ = 0; // end mark

	// restore integer divider state
	hw_divider_restore_state(&DividerState);
}

// initialize VGA DMA
//   control blocks aliases:
//                  +0x0        +0x4          +0x8          +0xC (Trigger)
// 0x00 (alias 0):  READ_ADDR   WRITE_ADDR    TRANS_COUNT   CTRL_TRIG
// 0x10 (alias 1):  CTRL        READ_ADDR     WRITE_ADDR    TRANS_COUNT_TRIG
// 0x20 (alias 2):  CTRL        TRANS_COUNT   READ_ADDR     WRITE_ADDR_TRIG
// 0x30 (alias 3):  CTRL        WRITE_ADDR    TRANS_COUNT   READ_ADDR_TRIG ... !

void VgaDmaInit()
{
	dma_channel_config cfg;

	// ==== prepare DMA control channel

	// prepare DMA default config
	cfg = dma_channel_get_default_config(VGA_DMA_CB0);

	// increment address on read from memory
	channel_config_set_read_increment(&cfg, true);

	// increment address on write to DMA port
	channel_config_set_write_increment(&cfg, true);

	// each DMA transfered entry is 32-bits
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);

	// write ring - wrap to 8-byte boundary (TRANS_COUNT and READ_ADDR_TRIG of data DMA)
	channel_config_set_ring(&cfg, true, 3);

	// DMA configure
	dma_channel_configure(
		VGA_DMA_CB0,	// channel
		&cfg,			// configuration
		&dma_hw->ch[VGA_DMA_PIO0].al3_transfer_count, // write address
		&CtrlBuf1[0],		// read address - as first, control buffer 1 will be sent out
		2,			// number of transfers in u32
		false			// do not start yet
		);

	// ==== prepare DMA data channel

	// prepare DMA default config
	cfg = dma_channel_get_default_config(VGA_DMA_PIO0);

	// increment address on read from memory
	channel_config_set_read_increment(&cfg, true);

	// do not increment address on write to PIO
	channel_config_set_write_increment(&cfg, false);

	// each DMA transfered entry is 32-bits
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);

	// DMA data request for sending data to PIO
	channel_config_set_dreq(&cfg, pio_get_dreq(VGA_PIO, VGA_SM0, true));

	// chain channel to DMA control block
	channel_config_set_chain_to(&cfg, VGA_DMA_CB0);

	// raise the IRQ flag when 0 is written to a trigger register (end of chain)
	channel_config_set_irq_quiet(&cfg, true);

	// set byte swapping
	channel_config_set_bswap(&cfg, true);

	// set high priority
	cfg.ctrl |= DMA_CH0_CTRL_TRIG_HIGH_PRIORITY_BITS;

	// DMA configure
	dma_channel_configure(
		VGA_DMA_PIO0,	// channel
		&cfg,			// configuration
		&VGA_PIO->txf[VGA_SM0], // write address
		NULL,			// read address
		0,			// number of transfers in u32
		false			// do not start immediately
	);

// ==== initialize IRQ0, raised from base layer 0

	// enable DMA channel IRQ0
	dma_channel_set_irq0_enabled(VGA_DMA_PIO0, true);

	// set DMA IRQ handler
	irq_set_exclusive_handler(DMA_IRQ_0, VgaLine);

	// set highest IRQ priority
	irq_set_priority(DMA_IRQ_0, 0);
}

// initialize VGA PIO
void VgaPioInit()
{
	// clear PIO instruction memory 
	pio_clear_instruction_memory(VGA_PIO);

	// configure main program instructions
	uint16_t ins[32]; // temporary buffer of program instructions
	memcpy(ins, &vga_program_instructions, vga_program.length*sizeof(uint16_t)); // copy program into buffer
	u16 cpp = (u16)CurVmode.cpp; // number of clocks per pixel
	ins[vga_offset_extra1] |= (cpp-2) << 8; // update waits
	ins[vga_offset_extra2] |= (cpp-2) << 8; // update waits
	
	// load main program into PIO's instruction memory
	struct pio_program prg;
	prg.instructions = ins;
	prg.length = vga_program.length;
	prg.origin = BASE_OFFSET;
	pio_add_program(VGA_PIO, &prg);

	// connect PIO to the pad
	for (int i = VGA_GPIO_FIRST; i < VGA_GPIO_LAST; i++) pio_gpio_init(VGA_PIO, i);
	pio_gpio_init(VGA_PIO, VGA_GPIO_SYNC);
#if VGA_VSYNC
	gpio_init(VGA_VSYNC);
#endif

	// negative HSYNC output
	if (!CurVmode.psync) gpio_set_outover(VGA_GPIO_SYNC, GPIO_OVERRIDE_INVERT);

	// set pin direction to output
	pio_sm_set_consecutive_pindirs(VGA_PIO, VGA_SM0, VGA_GPIO_FIRST, VGA_GPIO_OUTNUM, true);
	pio_sm_set_consecutive_pindirs(VGA_PIO, VGA_SM0, VGA_GPIO_SYNC, 1, true);
#if VGA_VSYNC
	gpio_set_dir(VGA_VSYNC, GPIO_OUT);
#endif	
	// get default config
	pio_sm_config cfg = pio_get_default_sm_config();

	// map state machine's OUT and MOV pins	
	sm_config_set_out_pins(&cfg, VGA_GPIO_FIRST, VGA_GPIO_OUTNUM);

	// join FIFO to send only
	sm_config_set_fifo_join(&cfg, PIO_FIFO_JOIN_TX);

	// PIO clock divider
	sm_config_set_clkdiv(&cfg, CurVmode.div);

	// shift left, autopull, pull threshold
	sm_config_set_out_shift(&cfg, false, true, 32);

	// base layer 0
	// set wrap
	sm_config_set_wrap(&cfg, vga_wrap_target+BASE_OFFSET, vga_wrap+BASE_OFFSET);

	// set sideset pins of base layer
	sm_config_set_sideset(&cfg, 1, false, false);
	sm_config_set_sideset_pins(&cfg, VGA_GPIO_SYNC);

	// initialize state machine
	pio_sm_init(VGA_PIO, VGA_SM0, vga_offset_entry+BASE_OFFSET, &cfg);
}

// initialize scanline buffers
void VgaBufInit()
{
	// init HSYNC..back porch buffer
	//  hsync must be min. 3
	//  hback must be min. 13
	LineBufHsBp[0] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.hsync-3)); // HSYNC
	LineBufHsBp[1] = BYTESWAP(VGADARK(CurVmode.hback-4-1-9,0)); // back porch - 1 - 9
	LineBufHsBp[2] = BYTESWAP(VGACMD(vga_offset_irqset+BASE_OFFSET,0)); // IRQ command (takes 9 clock cycles)
	LineBufHsBp[3] = BYTESWAP(VGACMD(vga_offset_output+BASE_OFFSET, CurVmode.width - 2)); // missing 2 clock cycles after last pixel

	// init front porch buffer
	//  hfront must be min. 4
	LineBufFp = BYTESWAP(VGADARK(CurVmode.hfront-4,0)); // front porch

	// init dark line
	LineBufDark[0] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.hsync-3)); // HSYNC
	LineBufDark[1] = BYTESWAP(VGADARK(CurVmode.htot-CurVmode.hsync-4,0)); // dark line

	// VGA mode
	// vertical synchronization

#ifdef VGA_VSYNC
	// if VSYNC line
	LineBufSync[0] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.hsync-3)); // HSYNC
	LineBufSync[1] = BYTESWAP(VGADARK(CurVmode.htot-CurVmode.hsync-4,0)); // dark line
#else
	//   hsync must be min. 4
	LineBufSync[0] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.htot-CurVmode.hsync-3)); // invert dark line
	LineBufSync[1] = BYTESWAP(VGADARK(CurVmode.hsync-4,0)); // invert HSYNC
#endif	

	// control blocks - initialize to VSYNC
	CtrlBuf1[0] = 2; // send 2x u32
	CtrlBuf1[1] = (u32)&LineBufSync[0]; // VSYNC

	CtrlBuf2[0] = 2; // send 2x u32
	CtrlBuf2[1] = (u32)&LineBufSync[0]; // VSYNC

	CtrlBuf1[2] = 0; // stop mark
	CtrlBuf1[3] = 0; // stop mark

	CtrlBuf2[2] = 0; // stop mark
	CtrlBuf2[3] = 0; // stop mark
}

// terminate VGA service
void VgaTerm()
{
	// abort DMA channels
	dma_channel_abort(VGA_DMA_PIO0); // pre-abort, could be chaining right now
	dma_channel_abort(VGA_DMA_CB0);

	// disable IRQ0 from DMA0
	irq_set_enabled(DMA_IRQ_0, false);
	dma_channel_set_irq0_enabled(VGA_DMA_PIO0, false);

	// Clear the interrupt request for DMA control channel
	dma_hw->ints0 = (1u << VGA_DMA_PIO0);

	// stop all state machines
	pio_set_sm_mask_enabled(VGA_PIO, VGA_SMALL, false);

	// restart state machine
	pio_restart_sm_mask(VGA_PIO, VGA_SMALL);

	// clear FIFOs
	pio_sm_clear_fifos(VGA_PIO, VGA_SM0);
	CtrlBufNext = NULL;

	// clear PIO instruction memory 
	pio_clear_instruction_memory(VGA_PIO);
}

// initialize scanline type table
void ScanlineTypeInit(const sVmode* v)
{
	u8* d = ScanlineType;
	int i, k;

	// line 0 is not used
	*d++ = LINE_DARK;

	// progressive mode (VGA 525)
	// vertical sync (VGA 2)
	for (i = v->vsync; i > 0; i--) *d++ = LINE_VSYNC;

	// dark (VGA 33)
	for (i = v->vback; i > 0; i--) *d++ = LINE_DARK;

	// image (VGA 480)
	for (i = v->vact; i > 0; i--) *d++ = LINE_IMG;

	// dark (VGA 10)
	for (i = v->vfront; i > 0; i--) *d++ = LINE_DARK;
}

// initialize videomode (returns False on bad configuration)
void VgaInit(const sVmode* vmode, u8* buf, int width, int height, int stride)
{
	int i;

	framebuffer = buf;
	fbwidth = width;

	// stop old state
	VgaTerm();

	// initialize scanline type table
	ScanlineTypeInit(vmode);

	// save current videomode
	memcpy(&CurVmode, vmode, sizeof(sVmode));

	// initialize parameters
	ScanLine = 1; // currently processed scanline
	BufInx = 0; // at first, control buffer 1 will be sent out
	CtrlBufNext = CtrlBuf2;

	// initialize VGA PIO
	VgaPioInit();

	// initialize scanline buffers
	VgaBufInit();

	// initialize DMA
	VgaDmaInit();

	// enable DMA IRQ
	irq_set_enabled(DMA_IRQ_0, true);

	// start DMA with base layer 0
	dma_channel_start(VGA_DMA_CB0);

	// run state machines
	pio_enable_sm_mask_in_sync(VGA_PIO, B0);
}


// wait for VSync scanline
void WaitVSync()
{
	// wait for end of VSync
	while (VSync) { __dmb(); }

	// wait for start of VSync
	while (!VSync) { __dmb(); }
}
