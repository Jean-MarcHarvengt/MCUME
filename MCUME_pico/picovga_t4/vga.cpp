
// ****************************************************************************
//
//                                 VGA output
//
// ****************************************************************************

#include "include.h"

// scanline type
u8 ScanlineType[MAXLINE];

// current videomode
int DispDev;			// current display device
sVmode CurVmode;		// copy of current videomode table
//int LayerMode;		// current layer mode (LAYERMODE_*)
volatile int ScanLine;		// current scan line 1...
volatile u32 Frame;		// frame counter
volatile int BufInx;		// current buffer set (0..1)
volatile Bool VSync;		// current scan line is vsync or dark

// line buffers
ALIGNED u8	LineBuf1[DBUF_MAX]; // scanline 1 image data
ALIGNED u8	LineBuf2[DBUF_MAX]; // scanline 2 image data

int	LineBufSize[LAYERS_MAX] = { DBUF0_MAX, DBUF1_MAX, DBUF2_MAX, DBUF3_MAX }; // size of data buffers

u32	LineBufHsBp[4];		// HSYNC ... back porch-1 ... IRQ command ... image command
u32	LineBufFp;		// front porch+1
u32	LineBufDark[2];		// HSYNC ... dark line
u32	LineBufSync[10];	// vertical synchronization
				//  interlaced (5x half scanlines):
				//	2x half synchronization (HSYNC pulse/2 ... line dark/2)
				//	2x vertical synchronization (invert line dark/2 ... invert HSYNC pulse)
				//	1x half synchronization (HSYNC pulse/2 ... line dark/2)
				// progressive: 1x scanline with vertical synchronization (invert line dark ... invert HSYNC pulse)

ALIGNED u8	LineBuf0[BLACK_MAX]; // line buffer with black color (used to clear rest of scanline)

// control buffers (BufInx = 0 running CtrlBuf1 and preparing CtrlBuf2, BufInx = 1 running CtrlBuf2 and preparing CtrlBuf1)
u32	CtrlBuf1[CBUF_MAX]; // base layer control pairs: u32 count, read address (must be terminated with [0,0])
u32	CtrlBuf2[CBUF_MAX]; // base layer control pairs: u32 count, read address (must be terminated with [0,0])

int	CtrlBufSize[LAYERS_MAX] = { CBUF0_MAX, CBUF1_MAX, CBUF2_MAX, CBUF3_MAX }; // size of control buffers

// next control buffer
u32*	CtrlBufNext[LAYERS_MAX];

// render font pixel mask
u32 RenderTextMask[512];

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
	dma_channel_set_read_addr(VGA_DMA_CB0, CtrlBufNext[0], true);

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
		y0 = line - CurVmode.vfirst1;
		if (CurVmode.dbly) y0 >>= 1;
		VSync = False;	// not vsync
		break;

	case LINE_IMGEVEN1:	// interlaced image even 0, 2, 4,..., 1st subframe
		y0 = line - CurVmode.vfirst1;
		if (CurVmode.dbly) y0 >>= 1;
		y0 <<= 1;
		VSync = False;	// not vsync
		break;

	case LINE_IMGEVEN2:	// interlaced image even 0, 2, 4,..., 2nd subframe
		y0 = line - CurVmode.vfirst2;
		if (CurVmode.dbly) y0 >>= 1;
		y0 <<= 1;
		VSync = False;	// not vsync
		break;

	case LINE_IMGODD1:	// interlaced image odd 1, 3, 5,..., 1st subframe
		y0 = line - CurVmode.vfirst1;
		if (CurVmode.dbly) y0 >>= 1;
		y0 = (y0 << 1) + 1;
		VSync = False;	// not vsync
		break;

	case LINE_IMGODD2:	// interlaced image odd 1, 3, 5,..., 2nd subframe
		y0 = line - CurVmode.vfirst2;
		if (CurVmode.dbly) y0 >>= 1;
		y0 = (y0 << 1) + 1;
		VSync = False;	// not vsync
		break;

	default:
		VSync = True;	// vsync
		break;
	}

	// update DMA control channels of overlapped layers
	// check if scanline is visible
	if (y0 >= 0)
	{
		// loop overlapped layers
		int layer;
		for (layer = 1; layer < LAYERS; layer++)
		{
			// check if this layer is active
			if (CtrlBufNext[layer] == NULL) continue;

			// check if this layer screen is active
			sLayer* s = &LayerScreen[layer];
			if (!s->on || (s->w <= 0) || (y0 < s->y) || (y0 >= s->y + s->h)) continue;

			// wait for idle state
			//   IRQ0 comes a few pixels before end of scanline, when DMA_PIO0 is finished.
			//   We must wait 1 to 2 us to complete layer DMA. Sometimes it can take
			//   longer - for such cases we must restart both DMA and state machine.
			int sm = VGA_SM(layer);
			u32 t1 = time_us_32();
			do {
				u8 a = *(volatile u8*)&VGA_PIO->sm[sm].addr & 0x1f;
				if (a <= CurLayerProg.maxidle+LAYER_OFFSET) break;
			} while ((u32)(time_us_32() - t1) < (u32)10); // wait max. 10 us, low resolution can take long time

			// stop DMA channel
			dma_channel_abort(VGA_DMA_PIO(layer));
			dma_channel_abort(VGA_DMA_CB(layer));
			dma_channel_abort(VGA_DMA_PIO(layer));
			dma_channel_abort(VGA_DMA_CB(layer));

			// restart state machine and clear FIFOs
			pio_sm_set_enabled(VGA_PIO, sm, false);
			pio_sm_clear_fifos(VGA_PIO, sm);
			pio_sm_restart(VGA_PIO, sm);
			pio_sm_exec(VGA_PIO, sm, pio_encode_jmp(CurLayerProg.idle+LAYER_OFFSET));
			pio_sm_set_enabled(VGA_PIO, sm, true);

			// enter new scanline
			pio_sm_exec(VGA_PIO, sm, pio_encode_jmp(CurLayerProg.entry+LAYER_OFFSET));

			// start DMA
			dma_channel_set_read_addr(VGA_DMA_CB(layer), CtrlBufNext[layer], true);
		}
	}

	return bufinx;
}

// render scanline buffers
u32* __not_in_flash_func(VgaBufRender)(u32* cbuf, u32* cbuf0, u8* dbuf, int y0)
{
// ---- render base layer

	// HSYNC + back porch
	*cbuf++ = 4; // send 4x u32
	*cbuf++ = (u32)LineBufHsBp; // HSYNC + back porch

	// render scanline
	//  cbuf ... control buffer
	//  dbuf ... data buffer (pixel data)
	//  line ... current line 0..
	//  pixnum ... total pixels (must be multiple of 4)
	cbuf = Render(cbuf, dbuf, y0, CurVmode.width);

	// front porch
	*cbuf++ = 1; // send 1x u32
	*cbuf++ = (u32)&LineBufFp; // front porch

// ---- render overlapped layers

	int layer;
	for (layer = 1; layer < LAYERS; layer++)
	{
		// shift buffers
		cbuf0 += CtrlBufSize[layer-1];
		dbuf += LineBufSize[layer-1];

		CtrlBufNext[layer] = NULL;

		// check if layer is active
		int mode = LayerModeInx[layer];
		if (mode == LAYERMODE_BASE) continue;

		// check if this layer screen is active
		sLayer* s = &LayerScreen[layer];
		if (!s->on || (s->w <= 0) || (y0 < s->y) || (y0 >= s->y + s->h)) continue;
		int y = y0 - s->y;

		// set next control buffer
		u32* cbuf2 = cbuf0;
		CtrlBufNext[layer] = cbuf2;

		// write init word
		u8* dbuf2 = dbuf;
		*cbuf2++ = 1;
		*cbuf2++ = (u32)dbuf2;
		*(u32*)dbuf2 = BYTESWAP(s->init);
		dbuf2 += 4;

		// render data
		switch(mode)
		{
		case LAYERMODE_SPRITEKEY:
		case LAYERMODE_SPRITEBLACK:
		case LAYERMODE_SPRITEWHITE:
			{
				*cbuf2++ = s->trans;
				*cbuf2++ = (u32)dbuf2;
				MemSet4((u32*)dbuf2, s->keycol, s->w/4);
				RenderSprite(dbuf2, y, s);
			}
			break;

		case LAYERMODE_FASTSPRITEKEY:
		case LAYERMODE_FASTSPRITEBLACK:
		case LAYERMODE_FASTSPRITEWHITE:
			{
				MemSet4((u32*)dbuf2, s->keycol, s->w/4);
				cbuf2 = RenderFastSprite(cbuf2, y, s, dbuf2);
			}
			break;

		case LAYERMODE_PERSPKEY: // layer with key color and image with transformation matrix
		case LAYERMODE_PERSPBLACK: // layer with black key color and image with transformation matrix
		case LAYERMODE_PERSPWHITE: // layer with white key color and image with transformation matrix
			{
				int w = s->w; // destination width
				int x = s->x; // destination coordinate X

				// underflow left edge
				if (x < 0)
				{
					x = ALIGN4(x+4098) - 4096; // round X to 4-pixels
					w += x;	// decrease W
					x = -x; // start offset of X
				}
				else
				{
					// overflow right edge
					if (x + w > CurVmode.width)
					{
						w = CurVmode.width - x; // limit W
					}
					x = 0;
				}

				// align W down
				w = ALIGN4(w);

				if (w <= 0)
				{
					// minimal transparent pixels
					*cbuf2++ = 1;
					*cbuf2++ = (u32)dbuf2;
					*(u32*)dbuf2 = s->keycol;
				}				
				else
				{
					// decode image
					*cbuf2++ = w/4;
					*cbuf2++ = (u32)&dbuf2[x];
					RenderPersp(dbuf2, y, s);
				}
			}
			break;

		case LAYERMODE_PERSP2KEY: // layer with key color and image with transformation matrix
		case LAYERMODE_PERSP2BLACK: // layer with black key color and image with transformation matrix
		case LAYERMODE_PERSP2WHITE: // layer with white key color and image with transformation matrix
			{
				int w = s->w; // destination width
				int x = s->x; // destination coordinate X

				// underflow left edge
				if (x < 0)
				{
					x = ALIGN4(x+4098) - 4096; // round X to 4-pixels
					w += x;	// decrease W
					x = -x; // start offset of X
				}
				else
				{
					// overflow right edge
					if (x + w > CurVmode.width)
					{
						w = CurVmode.width - x; // limit W
					}
					x = 0;
				}

				// align W down
				w = ALIGN4(w);

				if (w <= 0)
				{
					// minimal transparent pixels
					*cbuf2++ = 1;
					*cbuf2++ = (u32)dbuf2;
					*(u32*)dbuf2 = s->keycol;
				}				
				else
				{
					// decode image
					*cbuf2++ = w/4;
					*cbuf2++ = (u32)&dbuf2[x];
					RenderPersp2(dbuf2, y, s);
				}
			}
			break;

		case LAYERMODE_RLE:
			{
				// rows indices
				u16* row = (u16*)s->par;

				// lengt of the row
				int n = row[y+1] - row[y];

				// set transfer count
				*cbuf2++ = n;

				// start new DMA
				*cbuf2++ = (u32)&s->img[row[y]*4];
			}
			break;

		default:
			{
				// set transfer count
				*cbuf2++ = s->trans;

				// start new DMA
				*cbuf2++ = (u32)&s->img[y*s->wb];
			}
			break;
		}

		// end mark of layer
		*cbuf2++ = 0; // end mark
		*cbuf2++ = 0; // end mark
	}

	return cbuf;
}

// VGA DMA handler - called on end of every scanline
extern "C" void __not_in_flash_func(VgaLine)()
{
	// process scanline buffers (will save integer divider state into DividerState)
	int bufinx = VgaBufProcess();

	// prepare buffers to be processed next
	u8* dbuf; // data buffer
	u32* cbuf; // control buffer
	if (bufinx == 0)
	{
		dbuf = LineBuf1;
		cbuf = CtrlBuf1;
	}
	else
	{
		dbuf = LineBuf2;
		cbuf = CtrlBuf2;
	}
	CtrlBufNext[0] = cbuf;
	u32* cbuf0 = cbuf; // control buffer base

	// next rendered scanline
	int line = ScanLine;	// current scanline
	line++; // next line to render
	if (line > CurVmode.vtot) line = 1;
	int y0;

	u8 linetype = ScanlineType[line];
	switch (linetype)
	{
	case LINE_VSYNC:	// long vertical sync
		*cbuf++ = 2; // send 2x u32
		*cbuf++ = (u32)&LineBufSync[0]; // VSYNC
		break;

	case LINE_VVSYNC:	// short vertical + vertical sync
		*cbuf++ = 4;	// send 4x u32
		*cbuf++ = (u32)&LineBufSync[4]; // VSYNC
		break;

	case LINE_VHSYNC:	// short vertical + horizontal sync
		*cbuf++ = 4;	// send 4x u32
		*cbuf++ = (u32)&LineBufSync[6]; // VSYNC + half
		break;

	case LINE_HHSYNC:	// short horizontal + horizontal sync
		*cbuf++ = 4;	// send 4x u32
		*cbuf++ = (u32)&LineBufSync[0]; // half + half
		break;

	case LINE_HVSYNC:	// short horizontal + vertical sync
		*cbuf++ = 4;	// send 4x u32
		*cbuf++ = (u32)&LineBufSync[2]; // half + VSYNC
		break;

	case LINE_DARK:		// dark line
		*cbuf++ = 2; // send 2x u32
		*cbuf++ = (u32)LineBufDark; // dark
		break;

	case LINE_IMG:		// progressive image 0, 1, 2,...
		y0 = line - CurVmode.vfirst1;
		if (CurVmode.dbly) y0 >>= 1;
		cbuf = VgaBufRender(cbuf, cbuf0, dbuf, y0);
		break;

	case LINE_IMGEVEN1:	// interlaced image even 0, 2, 4,..., 1st subframe
		y0 = line - CurVmode.vfirst1;
		if (CurVmode.dbly) y0 >>= 1;
		y0 <<= 1;
		cbuf = VgaBufRender(cbuf, cbuf0, dbuf, y0);
		break;

	case LINE_IMGEVEN2:	// interlaced image even 0, 2, 4,..., 2nd subframe
		y0 = line - CurVmode.vfirst2;
		if (CurVmode.dbly) y0 >>= 1;
		y0 <<= 1;
		cbuf = VgaBufRender(cbuf, cbuf0, dbuf, y0);
		break;

	case LINE_IMGODD1:	// interlaced image odd 1, 3, 5,..., 1st subframe
		y0 = line - CurVmode.vfirst1;
		if (CurVmode.dbly) y0 >>= 1;
		y0 = (y0 << 1) + 1;
		cbuf = VgaBufRender(cbuf, cbuf0, dbuf, y0);
		break;

	case LINE_IMGODD2:	// interlaced image odd 1, 3, 5,..., 2nd subframe
		y0 = line - CurVmode.vfirst2;
		if (CurVmode.dbly) y0 >>= 1;
		y0 = (y0 << 1) + 1;
		cbuf = VgaBufRender(cbuf, cbuf0, dbuf, y0);
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
	int layer;
	for (layer = 0; layer < LAYERS; layer++)
	{
		// layer is not active
		if ((layer > 0) && (LayerModeInx[layer] == LAYERMODE_BASE)) continue;

// ==== prepare DMA control channel

		// prepare DMA default config
		cfg = dma_channel_get_default_config(VGA_DMA_CB(layer));

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
			VGA_DMA_CB(layer),	// channel
			&cfg,			// configuration
			&dma_hw->ch[VGA_DMA_PIO(layer)].al3_transfer_count, // write address
			&CtrlBuf1[0],		// read address - as first, control buffer 1 will be sent out
			2,			// number of transfers in u32
			false			// do not start yet
			);

// ==== prepare DMA data channel

		// prepare DMA default config
		cfg = dma_channel_get_default_config(VGA_DMA_PIO(layer));

		// increment address on read from memory
		channel_config_set_read_increment(&cfg, true);

		// do not increment address on write to PIO
		channel_config_set_write_increment(&cfg, false);

		// each DMA transfered entry is 32-bits
		channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);

		// DMA data request for sending data to PIO
		channel_config_set_dreq(&cfg, pio_get_dreq(VGA_PIO, VGA_SM(layer), true));

		// chain channel to DMA control block
		channel_config_set_chain_to(&cfg, VGA_DMA_CB(layer));

		// raise the IRQ flag when 0 is written to a trigger register (end of chain)
		channel_config_set_irq_quiet(&cfg, true);

		// set byte swapping
		channel_config_set_bswap(&cfg, true);

		// set high priority
		cfg.ctrl |= DMA_CH0_CTRL_TRIG_HIGH_PRIORITY_BITS;

		// DMA configure
		dma_channel_configure(
			VGA_DMA_PIO(layer),	// channel
			&cfg,			// configuration
			&VGA_PIO->txf[VGA_SM(layer)], // write address
			NULL,			// read address
			0,			// number of transfers in u32
			false			// do not start immediately
		);
	}

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
	int i;

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

	// load layer program
	if (LayerProgInx != LAYERPROG_BASE)
	{
		// configure layer program instructions
		memcpy(ins, CurLayerProg.ins, CurLayerProg.length*sizeof(uint16_t)); // copy program into buffer
		for (i = 0; i < CurLayerProg.extranum; i++)
		{
			int extra = (int)cpp - CurLayerProg.extra[i*2+1];
			if (extra < 0) extra = 0;
			ins[CurLayerProg.extra[i*2]] |= extra << 8; // update waits
		}
	
		// load layer program into PIO's instruction memory
		prg.instructions = ins;
		prg.length = CurLayerProg.length;
		prg.origin = LAYER_OFFSET;
		pio_add_program(VGA_PIO, &prg);
	}

	// connect PIO to the pad
    // JMH	
	//for (i = VGA_GPIO_FIRST; i <= VGA_GPIO_LAST; i++) pio_gpio_init(VGA_PIO, i);
	for (i = VGA_GPIO_FIRST; i < VGA_GPIO_LAST; i++) pio_gpio_init(VGA_PIO, i);
	pio_gpio_init(VGA_PIO, VGA_GPIO_SYNC);

	// negative HSYNC output
	if (!CurVmode.psync) gpio_set_outover(VGA_GPIO_SYNC, GPIO_OVERRIDE_INVERT);

	int layer;
	for (layer = 0; layer < LAYERS; layer++)
	{
		// layer is not active
		if ((layer > 0) && (LayerModeInx[layer] == LAYERMODE_BASE)) continue;

		// set pin direction to output
        // JMH
		//pio_sm_set_consecutive_pindirs(VGA_PIO, VGA_SM(layer), VGA_GPIO_FIRST, VGA_GPIO_NUM, true);
		pio_sm_set_consecutive_pindirs(VGA_PIO, VGA_SM(layer), VGA_GPIO_FIRST, VGA_GPIO_OUTNUM, true);
		pio_sm_set_consecutive_pindirs(VGA_PIO, VGA_SM(layer), VGA_GPIO_SYNC, 1, true);

		// get default config
		pio_sm_config cfg = pio_get_default_sm_config();

		// map state machine's OUT and MOV pins	
		sm_config_set_out_pins(&cfg, LayerFirstPin[layer], LayerNumPin[layer]);

		// join FIFO to send only
		sm_config_set_fifo_join(&cfg, PIO_FIFO_JOIN_TX);

		// PIO clock divider
		sm_config_set_clkdiv(&cfg, CurVmode.div);

		// shift left, autopull, pull threshold
		sm_config_set_out_shift(&cfg, false, true, 32);
	
		// base layer 0
		if (layer == 0)
		{
			// set wrap
			sm_config_set_wrap(&cfg, vga_wrap_target+BASE_OFFSET, vga_wrap+BASE_OFFSET);

			// set sideset pins of base layer
			sm_config_set_sideset(&cfg, 1, false, false);
			sm_config_set_sideset_pins(&cfg, VGA_GPIO_SYNC);

			// initialize state machine
			pio_sm_init(VGA_PIO, VGA_SM0, vga_offset_entry+BASE_OFFSET, &cfg);
		}
		else
		{
			// set wrap
			sm_config_set_wrap(&cfg, CurLayerProg.wrap_target+LAYER_OFFSET, CurLayerProg.wrap+LAYER_OFFSET);

			// initialize state machine
			pio_sm_init(VGA_PIO, VGA_SM(layer), CurLayerProg.idle+LAYER_OFFSET, &cfg);
		}
	}
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

	// TV mode
	if (CurVmode.inter)
	{
		// vertical synchronization
		LineBufSync[0] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.hsync/2-3)); // HSYNC
		LineBufSync[1] = BYTESWAP(VGADARK(CurVmode.htot/2-CurVmode.hsync/2-4,0)); // dark line
		LineBufSync[2] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.hsync/2-3)); // HSYNC
		LineBufSync[3] = BYTESWAP(VGADARK((CurVmode.htot+1)/2-CurVmode.hsync/2-4,0)); // dark line

		LineBufSync[4] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.htot/2-CurVmode.hsync-3)); // invert dark line
		LineBufSync[5] = BYTESWAP(VGADARK(CurVmode.hsync-4,0)); // invert HSYNC
		LineBufSync[6] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,(CurVmode.htot+1)/2-CurVmode.hsync-3)); // invert dark line
		LineBufSync[7] = BYTESWAP(VGADARK(CurVmode.hsync-4,0)); // invert HSYNC

		LineBufSync[8] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.hsync/2-3)); // HSYNC
		LineBufSync[9] = BYTESWAP(VGADARK(CurVmode.htot/2-CurVmode.hsync/2-4,0)); // dark line

		// control blocks - initialize to VSYNC
		CtrlBuf1[0] = 4; // send 4x u32
		CtrlBuf1[1] = (u32)&LineBufSync[4]; // VSYNC

		CtrlBuf2[0] = 4; // send 4x u32
		CtrlBuf2[1] = (u32)&LineBufSync[4]; // VSYNC
	}

	// VGA mode
	else
	{
		// vertical synchronization
		//   hsync must be min. 4
		LineBufSync[0] = BYTESWAP(VGACMD(vga_offset_sync+BASE_OFFSET,CurVmode.htot-CurVmode.hsync-3)); // invert dark line
		LineBufSync[1] = BYTESWAP(VGADARK(CurVmode.hsync-4,0)); // invert HSYNC

		// control blocks - initialize to VSYNC
		CtrlBuf1[0] = 2; // send 2x u32
		CtrlBuf1[1] = (u32)&LineBufSync[0]; // VSYNC

		CtrlBuf2[0] = 2; // send 2x u32
		CtrlBuf2[1] = (u32)&LineBufSync[0]; // VSYNC
	}

	CtrlBuf1[2] = 0; // stop mark
	CtrlBuf1[3] = 0; // stop mark

	CtrlBuf2[2] = 0; // stop mark
	CtrlBuf2[3] = 0; // stop mark
}

// terminate VGA service
void VgaTerm()
{
	int i;

	// abort DMA channels
	dma_channel_abort(VGA_DMA_PIO0); // pre-abort, could be chaining right now
	dma_channel_abort(VGA_DMA_CB0);
	for (i = 0; i < LAYERS; i++)
	{
		dma_channel_abort(VGA_DMA_PIO(i));
		dma_channel_abort(VGA_DMA_CB(i));
	}

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
	for (i = 0; i < LAYERS; i++)
	{
		pio_sm_clear_fifos(VGA_PIO, VGA_SM(i));
		CtrlBufNext[i] = NULL;
	}

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
	if (!v->inter)
	{
		// vertical sync (VGA 2)
		for (i = v->vsync1; i > 0; i--) *d++ = LINE_VSYNC;

		// dark (VGA 33)
		for (i = v->vback1; i > 0; i--) *d++ = LINE_DARK;

		// image (VGA 480)
		for (i = v->vact1; i > 0; i--) *d++ = LINE_IMG;

		// dark (VGA 10)
		for (i = v->vfront1; i > 0; i--) *d++ = LINE_DARK;
	}

	// interlaced mode (PAL 625, NTSC 525)
	//  - frames start with whole VSYNC
	else
	{
		// vertical sync (PAL 2, NTSC 3)
		for (i = v->vsync1/2; i > 0; i--) *d++ = LINE_VVSYNC;

		// vertical sync + half sync (PAL 1, NTSC 0)
		if ((v->vsync1 & 1) != 0) *d++ = LINE_VHSYNC;

		// half sync (PAL 2, NTSC 3)
		for (i = v->vpost1/2; i > 0; i--) *d++ = LINE_HHSYNC;

		// dark (PAL 18+23, NTSC 10+2)
		for (i = v->vback1; i > 0; i--) *d++ = LINE_DARK;

		// image 1st sub-frame (PAL 240, NTSC 240)
		if (v->odd)
			for (i = v->vact1; i > 0; i--) *d++ = LINE_IMGODD1; // odd lines 1, 3, 5, ... (PAL)
		else
			for (i = v->vact1; i > 0; i--) *d++ = LINE_IMGEVEN1; // even lines 0, 2, 4, ... (NTSC)

		// dark (PAL 24, NTSC 1)
		for (i = v->vfront1; i > 0; i--) *d++ = LINE_DARK;

		// half sync (PAL 2, NTSC 3)
		for (i = v->vpre1/2; i > 0; i--) *d++ = LINE_HHSYNC;

		// half sync + vertical sync (PAL 1, NTSC 1)
		k = v->vpre1 & 1;
		if (k != 0) *d++ = LINE_HVSYNC;

		// vertical sync (PAL 2, NTSC 2)
		for (i = (v->vsync2 - k)/2; i > 0; i--) *d++ = LINE_VVSYNC;

		// vertical sync + half sync (PAL 0, NTSC 1)
		if (((v->vsync2 - k) & 1) != 0) *d++ = LINE_VHSYNC;

		// half sync (PAL 2, NTSC 2)
		for (i = v->vpost2/2; i > 0; i--) *d++ = LINE_HHSYNC;

		// dark (PAL 18+23, NTSC 11+2)
		for (i = v->vback2; i > 0; i--) *d++ = LINE_DARK;

		// image 2nd sub-frame (PAL 240, NTSC 240)
		if (v->odd)
			for (i = v->vact2; i > 0; i--) *d++ = LINE_IMGEVEN2; // even lines 0, 2, 4, ... (PAL)
		else
			for (i = v->vact2; i > 0; i--) *d++ = LINE_IMGODD2; // odd lines 1, 3, 5, ... (NTSC)

		// dark (PAL 24, NTSC 1)
		for (i = v->vfront2; i > 0; i--) *d++ = LINE_DARK;

		// half sync (PAL 3, NTSC 3)
		for (i = v->vpre2/2; i > 0; i--) *d++ = LINE_HHSYNC;
	}
}

// scanline names
const char* ScanlineName[] = {
	"VSYNC",	// long vertical sync
	"VVSYNC",	// short vertical + vertical sync
	"VHSYNC",	// short vertical + horizontal sync
	"HHSYNC",	// short horizontal + horizontal sync
	"HVSYNC",	// short horizontal + vertical sync
	"DARK",		// dark line
	"IMG",		// progressive image 0, 1, 2,...
	"IMGEVEN1",	// interlaced image even 0, 2, 4,..., 1st subframe
	"IMGEVEN2",	// interlaced image even 0, 2, 4,..., 2nd subframe
	"IMGODD1",	// interlaced image odd 1, 3, 5,..., 1st subframe
	"IMGODD2",	// interlaced image odd 1, 3, 5,..., 2nd subframe
};

// print table if scanline types
void ScanlineTypePrint(const u8* scan, int lines)
{
	// skip scanline 0
	scan++;

	// load scanline 1
	u8 last = *scan++;
	int num = 1;
	int line = 1;

	// process other scanlines
	int i;
	for (i = 2; i <= lines; i++)
	{
		if ((*scan != last) || (i == lines))
		{
			if (num == 1)
				printf("%d (1): %s\n", line, line + num - 1, ScanlineName[last]);
			else
				printf("%d..%d (%d): %s\n", line, line + num - 1, num, ScanlineName[last]);

			last = *scan;
			num = 1;
			line = i;
		}
		else
			num++;
		scan++;
	}
}

// initialize videomode (returns False on bad configuration)
// - All layer modes must use same layer program (LAYERMODE_BASE = overlapped layers are OFF)
void VgaInit(const sVmode* vmode)
{
	int i;

	// stop old state
	VgaTerm();

	// initialize scanline type table
	ScanlineTypeInit(vmode);

	// prepare render font pixel mask
	for (i = 0; i < 256; i++)
	{
		// higher 4 bits
		u32 m = 0;
		if ((i & B7) != 0) m |= 0xff;
		if ((i & B6) != 0) m |= 0xff << 8;
		if ((i & B5) != 0) m |= 0xff << 16;
		if ((i & B4) != 0) m |= 0xff << 24;
		RenderTextMask[2*i] = m;

		// lower 4 bits
		m = 0;
		if ((i & B3) != 0) m |= 0xff;
		if ((i & B2) != 0) m |= 0xff << 8;
		if ((i & B1) != 0) m |= 0xff << 16;
		if ((i & B0) != 0) m |= 0xff << 24;
		RenderTextMask[2*i+1] = m;
	}

	// emergency check of structure definitions
	if (	(SSPRITE_SIZE != sizeof(sSprite)) ||
		(SLAYER_SIZE != sizeof(sLayer)) ||
		(SSEGM_SIZE != sizeof(sSegm)) ||
		(SSTRIP_SIZE != sizeof(sStrip)) ||
		(SSCREEN_SIZE != sizeof(sScreen)))
	{
		while (1) {}
	}

	// clear buffer with black color
	memset(LineBuf0, COL_BLACK, BLACK_MAX);

	// save current videomode
	memcpy(&CurVmode, vmode, sizeof(sVmode));

	// initialize parameters
	ScanLine = 1; // currently processed scanline
//	Frame = 0;
	BufInx = 0; // at first, control buffer 1 will be sent out
	CtrlBufNext[0] = CtrlBuf2;

	// initialize base layer
	LayerModeInx[0] = LAYERMODE_BASE;
	memcpy(&CurLayerMode[0], &LayerMode[LAYERMODE_BASE], sizeof(sLayerMode));
	memset(&LayerScreen[0], 0, sizeof(sLayer));

	// save layer modes
	LayerModeInx[1] = vmode->mode[1];
	LayerModeInx[2] = vmode->mode[2];
	LayerModeInx[3] = vmode->mode[3];

	LayerMask = B0; // mask of active layers
	for (i = 1; i < LAYERS; i++)
	{
		memcpy(&CurLayerMode[i], &LayerMode[LayerModeInx[i]], sizeof(sLayerMode));
		if (LayerModeInx[i] != LAYERMODE_BASE) LayerMask |= (1 << i);
	}

	// get layer program
	LayerProgInx = vmode->prog;
	memcpy(&CurLayerProg, &LayerProg[LayerProgInx], sizeof(sLayerProg));

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
	pio_enable_sm_mask_in_sync(VGA_PIO, LayerMask);
}

const sVmode* volatile VgaVmodeReq = NULL; // request to reinitialize videomode, 1=only stop driver

void (* volatile Core1Fnc)() = NULL; // core 1 remote function

// VGA core
void VgaCore()
{
	const sVmode* v;
	void (*fnc)();
	while (1)
	{
		__dmb();

		// initialize videomode
		v = VgaVmodeReq;
		if (v != NULL)
		{
			if ((u32)v == (u32)1)
				VgaTerm(); // terminate
			else
				VgaInit(v);
			__dmb();
			VgaVmodeReq = NULL;
		}

		// execute remote function
		fnc = Core1Fnc;
		if (fnc != NULL)
		{
			fnc();
			__dmb();
			Core1Fnc = NULL;
		}
	}
}

// request to initialize VGA videomode, NULL=only stop driver (wait to initialization completes)
void VgaInitReq(const sVmode* vmode)
{
	if (vmode == NULL) vmode = (const sVmode*)1;
	__dmb();
	VgaVmodeReq = vmode;
	while (VgaVmodeReq != NULL) { __dmb(); }
}

// execute core 1 remote function
void Core1Exec(void (*fnc)())
{
	__dmb();
	Core1Fnc = fnc;
	__dmb();
}

// check if core 1 is busy (executing remote function)
Bool Core1Busy()
{
	__dmb();
	return Core1Fnc != NULL;
}

// wait if core 1 is busy (executing remote function)
void Core1Wait()
{
	while (Core1Busy()) {}
}

// wait for VSync scanline
void WaitVSync()
{
	// wait for end of VSync
	while (VSync) { __dmb(); }

	// wait for start of VSync
	while (!VSync) { __dmb(); }
}
