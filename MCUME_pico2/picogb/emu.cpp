#include <string.h>

#include "emuapi.h"
#include "iopins.h" 


extern "C" {
#include "minigb_apu/minigb_apu.h"
#include "palettes/gbcolors.h"
#include "palettes/hedley.h"
#include "palettes/peanut_gb.h"
}

#include "flash_t.h"

static uint8_t ram[32768];
static gb_s gb;
struct minigb_apu_ctx apu_ctx = {};


static palette_t palette16; // Colour palette
#define RGB565_TO_RGB888(rgb565) ((((rgb565) & 0xF800) << 8) | (((rgb565) & 0x07E0) << 5) | (((rgb565) & 0x001F) << 3))

void gb_error(struct gb_s* gb, const enum gb_error_e gb_err, const uint16_t addr) {
    const char* gb_err_str[4] = {
        "UNKNOWN",
        "INVALID OPCODE",
        "INVALID READ",
        "INVALID WRITE"
    };
    printf("Error %d occurred: %s at %04X\n.\n", gb_err, gb_err_str[gb_err], addr);
}

/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t __not_in_flash_func(gb_rom_read)(struct gb_s* gb, const uint_fast32_t addr) {
    return flash_start[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t __not_in_flash_func(gb_cart_ram_read)(struct gb_s* gb, const uint_fast32_t addr) {
    return ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
void __not_in_flash_func(gb_cart_ram_write)(struct gb_s* gb, const uint_fast32_t addr, const uint8_t val) {
    ram[addr] = val;
}


static uint8_t tmppixels[LCD_WIDTH];

/**
 * Draws scanline into framebuffer.
 */
void __always_inline lcd_draw_line(struct gb_s* gb, const unsigned char  *pixels, const uint_fast8_t y) {
    if (gb->cgb.cgbMode) {
        emu_DrawLinePal16((unsigned char *)&pixels[0], 160, 128, y);
    }
    else 
    {       
        for (unsigned int x = 0; x < LCD_WIDTH; x++)
            tmppixels[x] = pixels[x] & 3; //(pixels[x] & LCD_PALETTE_ALL) >> 4;
        emu_DrawLinePal16((unsigned char *)&tmppixels[0], 160, 128, y); 
    }
}

void gbc_Init(void)
{
  emu_printf("gbc_Init");   
  emu_printf("gbc_Init done");
}


static int k = 0;
static int ihk = 0;
static int iusbhk; // USB keyboard key

void emu_KeyboardOnDown(int keymodifer, int key) {
  if (key <= 0x7f) iusbhk = key;
  //else if (key == KBD_KEY_UP) iusbhk = 0xD7;  
  //else if (key == KBD_KEY_LEFT) iusbhk = 0xD8;  
  //else if (key == KBD_KEY_RIGHT) iusbhk = 0xD9;  
  //else if (key == KBD_KEY_DOWN) iusbhk = 0xDA;  
  //else if (key == KBD_KEY_BS) iusbhk = 0x7F;  
  else
    iusbhk = 0;
}

void emu_KeyboardOnUp(int keymodifer, int key) {
  iusbhk = 0;
}


void gbc_Input(int click) {
  ihk = emu_ReadI2CKeyboard();
  k = emu_ReadKeys();    
}


void gbc_Start(char * filename)
{
  emu_printf("gbc_Start");

  int size = flash_load(filename);

  /* Initialise GB context. */
  gb_init_error_e ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read,
                                &gb_cart_ram_write, &gb_error, nullptr);


  gb.direct.interlace = false;
  gb.direct.frame_skip = true;

  auto_assign_palette(palette16, gb_colour_hash(&gb), gb_get_rom_name(&gb, filename));
  //manual_assign_palette(palette16, 0);

  for (int i = 0; i < 3; i++)
      for (int j = 0; j < 4; j++) {
          uint32_t val = RGB565_TO_RGB888(palette16[i][j]);
          emu_SetPaletteEntry(val>>16, (val>>8) & 0xff, val & 0xff, i * 4 + j);
      }

  gb_init_lcd(&gb, &lcd_draw_line); 
#ifdef HAS_SND 
  emu_sndInit();
  minigb_apu_audio_init(&apu_ctx);
//  audio_init();
#endif


  emu_printf("gbc_Start done");
}

void gbc_Step(void) {
  gb_run_frame(&gb);

  int hk = ihk;
  if (iusbhk) hk = iusbhk;

  switch(hk) {
    case '1':
      k = MASK_KEY_USER1;
      break;
    case '2':
      k = MASK_KEY_USER2;
      break;
    default:
      break;
  };

  gb.direct.joypad_bits.up = !(( k & MASK_JOY1_UP) || ( k & MASK_JOY2_UP));
  gb.direct.joypad_bits.down = !(( k & MASK_JOY1_DOWN) || ( k & MASK_JOY2_DOWN));
  gb.direct.joypad_bits.right = !(( k & MASK_JOY1_LEFT) || ( k & MASK_JOY2_LEFT));
  gb.direct.joypad_bits.left = !(( k & MASK_JOY1_RIGHT) || ( k & MASK_JOY2_RIGHT));
  gb.direct.joypad_bits.a = !(k & MASK_KEY_USER3);
  gb.direct.joypad_bits.b = !(k & MASK_JOY2_BTN);
  gb.direct.joypad_bits.select = !(k & MASK_KEY_USER1);
  gb.direct.joypad_bits.start = !(k & MASK_KEY_USER2);



  emu_DrawVsync();   
}


#ifdef HAS_SND
extern "C" uint8_t audio_read(uint16_t addr) {
  return minigb_apu_audio_read(&apu_ctx, addr);
} 

extern "C" void audio_write(uint16_t addr, uint8_t value) {
  minigb_apu_audio_write(&apu_ctx, addr, value);
} 
static audio_sample_t snd[AUDIO_SAMPLES_TOTAL];

//static int16_t snd[512];
#endif

void SND_Process(void *stream, int len) {
#ifdef HAS_SND
  audio_sample * snd_buf =  (audio_sample *)stream;
  //audio_callback(NULL,&snd[0],len);
  minigb_apu_audio_callback(&apu_ctx, snd); 
  for (int i = 0; i< len*2; i+=2 )
    *snd_buf++ = (((snd[i]>>8)+(snd[i+1]>>8))/8+128);
#endif
} 
