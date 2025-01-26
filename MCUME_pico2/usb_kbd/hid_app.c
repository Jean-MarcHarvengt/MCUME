/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "bsp/board.h"
#include "tusb.h"
#include "kbd.h"
#include "platform_config.h"

/* ==================  Keycode translation table. ==================== */

/*
 * We only need one translation table, as this simple application only
 * supports 102-key US layouts. We need separate translations for
 * non-shifted and shifted keys because shift is not just a modifier:
 * shift turns a '5' to a '$', for example. We don't need a separate
 * column for 'ctrl', because we can handle that simply by logical
 * AND operations on the keycode. Alt/Meta does not affect the keycode
 * at all -- it's just a flag. 
 */ 

// TODO: fill in the rest of the non-ASCII key codes
// There's a nice list here:
// https://gist.github.com/ekaitz-zarraga/2b25b94b711684ba4e969e5a5723969b
static int conv_table_uk[128][2] = 
{
    {0     , 0      }, /* 0x00 */ \
    {0     , 0      }, /* 0x01 */ \
    {0     , 0      }, /* 0x02 */ \
    {0     , 0      }, /* 0x03 */ \
    {'a'   , 'A'    }, /* 0x04 */ \
    {'b'   , 'B'    }, /* 0x05 */ \
    {'c'   , 'C'    }, /* 0x06 */ \
    {'d'   , 'D'    }, /* 0x07 */ \
    {'e'   , 'E'    }, /* 0x08 */ \
    {'f'   , 'F'    }, /* 0x09 */ \
    {'g'   , 'G'    }, /* 0x0a */ \
    {'h'   , 'H'    }, /* 0x0b */ \
    {'i'   , 'I'    }, /* 0x0c */ \
    {'j'   , 'J'    }, /* 0x0d */ \
    {'k'   , 'K'    }, /* 0x0e */ \
    {'l'   , 'L'    }, /* 0x0f */ \
    {'m'   , 'M'    }, /* 0x10 */ \
    {'n'   , 'N'    }, /* 0x11 */ \
    {'o'   , 'O'    }, /* 0x12 */ \
    {'p'   , 'P'    }, /* 0x13 */ \
    {'q'   , 'Q'    }, /* 0x14 */ \
    {'r'   , 'R'    }, /* 0x15 */ \
    {'s'   , 'S'    }, /* 0x16 */ \
    {'t'   , 'T'    }, /* 0x17 */ \
    {'u'   , 'U'    }, /* 0x18 */ \
    {'v'   , 'V'    }, /* 0x19 */ \
    {'w'   , 'W'    }, /* 0x1a */ \
    {'x'   , 'X'    }, /* 0x1b */ \
    {'y'   , 'Y'    }, /* 0x1c */ \
    {'z'   , 'Z'    }, /* 0x1d */ \
    {'1'   , '!'    }, /* 0x1e */ \
    {'2'   , '@'    }, /* 0x1f */ \
    {'3'   , '#'    }, /* 0x20 */ \
    {'4'   , '$'    }, /* 0x21 */ \
    {'5'   , '%'    }, /* 0x22 */ \
    {'6'   , '^'    }, /* 0x23 */ \
    {'7'   , '&'    }, /* 0x24 */ \
    {'8'   , '*'    }, /* 0x25 */ \
    {'9'   , '('    }, /* 0x26 */ \
    {'0'   , ')'    }, /* 0x27 */ \
    {KBD_KEY_ENTER  , KBD_KEY_ENTER   }, /* 0x28 */ \
    {KBD_KEY_ESC,  KBD_KEY_ESC }, /* 0x29 */ \
    {KBD_KEY_BS  , KBD_KEY_BS }, /* 0x2a */ \
    {'\t'  , '\t'   }, /* 0x2b */ \
    {' '   , ' '    }, /* 0x2c */ \
    {'-'   , '_'    }, /* 0x2d */ \
    {'='   , '+'    }, /* 0x2e */ \
    {'['   , '{'    }, /* 0x2f */ \
    {']'   , '}'    }, /* 0x30 */ \
    {'\\'  , '|'    }, /* 0x31 */ \
    {'#'   , '~'    }, /* 0x32 */ \
    {';'   , ':'    }, /* 0x33 */ \
    {'\''  , '\"'   }, /* 0x34 */ \
    {'`'   , '~'    }, /* 0x35 */ \
    {','   , '<'    }, /* 0x36 */ \
    {'.'   , '>'    }, /* 0x37 */ \
    {'/'   , '?'    }, /* 0x38 */ \
    {KBD_KEY_CAPS , KBD_KEY_CAPS      }, /* 0x39 */ \
    {KBD_KEY_F1 , KBD_KEY_F1}, /* 0x3a */ \
    {KBD_KEY_F2 , KBD_KEY_F2}, /* 0x3b */ \
    {KBD_KEY_F3 , KBD_KEY_F3}, /* 0x3c */ \
    {KBD_KEY_F4 , KBD_KEY_F4}, /* 0x3d */ \
    {KBD_KEY_F5 , KBD_KEY_F5}, /* 0x3e */ \
    {KBD_KEY_F6 , KBD_KEY_F6}, /* 0x3f */ \
    {KBD_KEY_F7 , KBD_KEY_F7}, /* 0x40 */ \
    {KBD_KEY_F8 , KBD_KEY_F8}, /* 0x41 */ \
    {0     , 0      }, /* 0x42 */ \
    {0     , 0      }, /* 0x43 */ \
    {0     , 0      }, /* 0x44 */ \
    {0     , 0      }, /* 0x45 */ \
    {0     , 0      }, /* 0x46 */ \
    {0     , 0      }, /* 0x47 */ \
    {0     , 0      }, /* 0x48 */ \
    {0     , 0      }, /* 0x49 */ \
    {KBD_KEY_HOME , KBD_KEY_HOME}, /* 0x4a */ \
    {KBD_KEY_PGUP , KBD_KEY_PGUP}, /* 0x4b */ \
    {0     , 0      }, /* 0x4c */ \
    {KBD_KEY_END, KBD_KEY_END}, /* 0x4d */ \
    {KBD_KEY_PGDN , KBD_KEY_PGDN }, /* 0x4e */ \
    {KBD_KEY_RIGHT, KBD_KEY_RIGHT }, /* 0x4f */ \
    {KBD_KEY_LEFT, KBD_KEY_LEFT }, /* 0x50 */ \
    {KBD_KEY_DOWN, KBD_KEY_DOWN }, /* 0x51 */ \
    {KBD_KEY_UP, KBD_KEY_UP }, /* 0x52 */ \
    { 0   , 0       }, /* 0x53 */ \
                                  \
    { 0   , 0       }, /* 0x54 */ \
    { 0   , 0       }, /* 0x55 */ \
    { 0   , 0       }, /* 0x56 */ \
    { 0   , 0       }, /* 0x57 */ \
    { 0   , 0       }, /* 0x58 */ \
    { 0   , 0       }, /* 0x59 */ \
    { 0   , 0       }, /* 0x5a */ \
    { 0   , 0       }, /* 0x5b */ \
    { 0   , 0       }, /* 0x5c */ \
    { 0   , 0       }, /* 0x5d */ \
    { 0   , 0       }, /* 0x5e */ \
    { 0   , 0       }, /* 0x5f */ \
    { 0   , 0       }, /* 0x60 */ \
    { 0   , 0       }, /* 0x61 */ \
    { 0   , 0       }, /* 0x62 */ \
    { 0   , 0       }, /* 0x63 */ \
    { 0   , 0       }, /* 0x64 */ \
    { 0   , 0       }, /* 0x65 */ \
    { 0   , 0       }, /* 0x66 */ \
    { 0   , 0       }, /* 0x67 */ \
};



static int conv_table_be[128][2] = 
{
    {0     , 0      }, /* 0x00 */ \
    {0     , 0      }, /* 0x01 */ \
    {0     , 0      }, /* 0x02 */ \
    {0     , 0      }, /* 0x03 */ \
    {'q'   , 'Q'    }, /* 0x04 */ \
    {'b'   , 'B'    }, /* 0x05 */ \
    {'c'   , 'C'    }, /* 0x06 */ \
    {'d'   , 'D'    }, /* 0x07 */ \
    {'e'   , 'E'    }, /* 0x08 */ \
    {'f'   , 'F'    }, /* 0x09 */ \
    {'g'   , 'G'    }, /* 0x0a */ \
    {'h'   , 'H'    }, /* 0x0b */ \
    {'i'   , 'I'    }, /* 0x0c */ \
    {'j'   , 'J'    }, /* 0x0d */ \
    {'k'   , 'K'    }, /* 0x0e */ \
    {'l'   , 'L'    }, /* 0x0f */ \
    {','   , '?'    }, /* 0x10 */ \
    {'n'   , 'N'    }, /* 0x11 */ \
    {'o'   , 'O'    }, /* 0x12 */ \
    {'p'   , 'P'    }, /* 0x13 */ \
    {'a'   , 'A'    }, /* 0x14 */ \
    {'r'   , 'R'    }, /* 0x15 */ \
    {'s'   , 'S'    }, /* 0x16 */ \
    {'t'   , 'T'    }, /* 0x17 */ \
    {'u'   , 'U'    }, /* 0x18 */ \
    {'v'   , 'V'    }, /* 0x19 */ \
    {'z'   , 'Z'    }, /* 0x1a */ \
    {'x'   , 'X'    }, /* 0x1b */ \
    {'y'   , 'Y'    }, /* 0x1c */ \
    {'w'   , 'W'    }, /* 0x1d */ \
    {'1'   , '&'    }, /* 0x1e */ \
    {'2'   , '@'    }, /* 0x1f */ \
    {'3'   , '\"'   }, /* 0x20 */ \
    {'4'   , '\''   }, /* 0x21 */ \
    {'5'   , '('    }, /* 0x22 */ \
    {'6'   , '#'      }, /* 0x23 */ \
    {'7'   , '*'      }, /* 0x24 */ \
    {'8'   , '!'    }, /* 0x25 */ \
    {'9'   , '{'    }, /* 0x26 */ \
    {'0'   , '}'    }, /* 0x27 */ \
    {KBD_KEY_ENTER  , KBD_KEY_ENTER   }, /* 0x28 */ \
    {KBD_KEY_ESC,  KBD_KEY_ESC }, /* 0x29 ESC*/ \
    {KBD_KEY_BS  , KBD_KEY_BS }, /* 0x2a */ \
    {'\t'  , '\t'   }, /* 0x2b */ \
    {' '   , ' '    }, /* 0x2c */ \
    {')'   , 0      }, /* 0x2d */ \
    {'-'   , '_'    }, /* 0x2e */ \
    {'^'   , '['    }, /* 0x2f */ \
    {'$'   , ']'    }, /* 0x30 */ \
    {0     , 0      }, /* 0x31 */ \
    {0     , 0      }, /* 0x32 */ \
    {'m'   , 'M'    }, /* 0x33 */ \
    {0     , '%'    }, /* 0x34 */ \
    {0     , 0      }, /*{'`'   , '~'    },*/ /* 0x35 */ \
    {';'   , '.'    }, /* 0x36 */ \
    {':'   , '/'    }, /* 0x37 */ \
    {'='   , '+'    }, /* 0x38 */ \
    {KBD_KEY_CAPS , KBD_KEY_CAPS      }, /* 0x39 */ \
    {KBD_KEY_F1 , KBD_KEY_F1}, /* 0x3a */ \
    {KBD_KEY_F2 , KBD_KEY_F2}, /* 0x3b */ \
    {KBD_KEY_F3 , KBD_KEY_F3}, /* 0x3c */ \
    {KBD_KEY_F4 , KBD_KEY_F4}, /* 0x3d */ \
    {KBD_KEY_F5 , KBD_KEY_F5}, /* 0x3e */ \
    {KBD_KEY_F6 , KBD_KEY_F6}, /* 0x3f */ \
    {KBD_KEY_F7 , KBD_KEY_F7}, /* 0x40 */ \
    {KBD_KEY_F8 , KBD_KEY_F8}, /* 0x41 */ \
    {0     , 0      }, /* 0x42 */ \
    {0     , 0      }, /* 0x43 */ \
    {0     , 0      }, /* 0x44 */ \
    {0     , 0      }, /* 0x45 */ \
    {0     , 0      }, /* 0x46 */ \
    {0     , 0      }, /* 0x47 */ \
    {0     , 0      }, /* 0x48 */ \
    {0     , 0      }, /* 0x49 */ \
    {KBD_KEY_HOME , KBD_KEY_HOME}, /* 0x4a */ \
    {KBD_KEY_PGUP , KBD_KEY_PGUP}, /* 0x4b */ \
    {0     , 0      }, /* 0x4c */ \
    {KBD_KEY_END, KBD_KEY_END}, /* 0x4d */ \
    {KBD_KEY_PGDN , KBD_KEY_PGDN }, /* 0x4e */ \
    {KBD_KEY_RIGHT, KBD_KEY_RIGHT }, /* 0x4f */ \
    {KBD_KEY_LEFT, KBD_KEY_LEFT }, /* 0x50 */ \
    {KBD_KEY_DOWN, KBD_KEY_DOWN }, /* 0x51 */ \
    {KBD_KEY_UP, KBD_KEY_UP }, /* 0x52 */ \
    { 0   , 0       }, /* 0x53 */ \
    { 0   , 0       }, /* 0x54 */ \
    { 0   , 0       }, /* 0x55 */ \
    { 0   , 0       }, /* 0x56 */ \
    { 0   , 0       }, /* 0x57 */ \
    { 0   , 0       }, /* 0x58 */ \
    { 0   , 0       }, /* 0x59 */ \
    { 0   , 0       }, /* 0x5a */ \
    { 0   , 0       }, /* 0x5b */ \
    { 0   , 0       }, /* 0x5c */ \
    { 0   , 0       }, /* 0x5d */ \
    { 0   , 0       }, /* 0x5e */ \
    { 0   , 0       }, /* 0x5f */ \
    { 0   , 0       }, /* 0x60 */ \
    { 0   , 0       }, /* 0x61 */ \
    { 0   , 0       }, /* 0x62 */ \
    { 0   , 0       }, /* 0x63 */ \
    { '<' , '>'     }, /* 0x64 */ \
    { 0   , 0       }, /* 0x65 */ \
    { 0   , 0       }, /* 0x66 */ \
    { 0   , 0       }, /* 0x67 */ \
};

static KLAYOUT klayout = KEYLAYOUT;


/* =================  End keycode translation table. ================== */

/*===========================================================================
 * is_key_held 
 * Check whether the current key scancode is a repetition of the previous
 * one. One USB report can contain multiple keystrokes, if one key is pressed
 * before another is released. As each new key is pressed, a new report
 * is delivered that contains the keystrokes of the set of keys that
 * are down. As a result, the same keystroke can be found in multiple
 * successive reports.  
 * ========================================================================*/
static inline bool is_key_held (hid_keyboard_report_t const *report, uint8_t keycode)
{
  for (uint8_t i=0; i < 6; i++) 
  {
    if (report->keycode[i] == keycode) 
      return true;
  }
  return false;
}

/*===========================================================================
 * process_kbd_report 
 * Process a report from a keyboard device. The report will contain up
 * to 6 keystrokes, each representing a different key that is down.
 * We need to filter duplicate keystrokes that arise when multiple keys
 * are pressed such that one key is pressed before another is released.
 * This is quite common when typing quickly on a decent keyboard.  
 * ========================================================================*/
static void process_kbd_report (hid_keyboard_report_t const *report)
{
  static hid_keyboard_report_t prev_report = { 0, 0, {0} };
  
  bool is_lshift_pressed = report->modifier 
    & KEYBOARD_MODIFIER_LEFTSHIFT;
  bool is_lctrl_pressed = report->modifier 
    & KEYBOARD_MODIFIER_LEFTCTRL;
  bool is_lalt_pressed = report->modifier 
    & KEYBOARD_MODIFIER_LEFTALT;  
  bool is_rshift_pressed = report->modifier 
    & (KEYBOARD_MODIFIER_RIGHTSHIFT);
  bool is_rctrl_pressed = report->modifier 
    & KEYBOARD_MODIFIER_RIGHTCTRL;
  bool is_ralt_pressed = report->modifier 
    & KEYBOARD_MODIFIER_RIGHTALT;  
  for (uint8_t i=0; i < 6; i++) 
  {
    if (report->keycode[i]) 
    {
      bool found = is_key_held (&prev_report, report->keycode[i]);

      if (!found) 
      {
        int ch; 
        int chshift;
        if (klayout == KLAYOUT_UK) {
          ch = conv_table_uk[report->keycode[i]][0];
          chshift = conv_table_uk[report->keycode[i]][(is_lshift_pressed?1:0)];
        }
        else if (klayout == KLAYOUT_BE) {
          ch = conv_table_be[report->keycode[i]][0];
          chshift = conv_table_be[report->keycode[i]][(is_lshift_pressed?1:0)];
        }

        int flags = 0;
        if (is_lshift_pressed) flags |= KBD_FLAG_LSHIFT;
        if (is_lctrl_pressed) flags |= KBD_FLAG_LCONTROL;
        if (is_lalt_pressed) flags |= KBD_FLAG_LALT;
        if (is_rshift_pressed) flags |= KBD_FLAG_RSHIFT;
        if (is_rctrl_pressed) flags |= KBD_FLAG_RCONTROL;
        if (is_ralt_pressed) flags |= KBD_FLAG_RALT;
        kbd_signal_raw_key(report->keycode[i], ch, chshift, flags, KEY_PRESSED);
        //printf("key=%d\n", report->keycode[i]);
        //printf("flags=%d\n", flags);
      }
    }
  }
  for (uint8_t i=0; i < 6; i++) 
  {
    if (prev_report.keycode[i]) 
    {
      bool found = is_key_held (report, prev_report.keycode[i]);
      if (!found) 
      {
        int ch; 
        int chshift;
        if (klayout == KLAYOUT_UK) {
          ch = conv_table_uk[prev_report.keycode[i]][0];
          chshift = conv_table_uk[prev_report.keycode[i]][(is_lshift_pressed?1:0)];
        }
        else if (klayout == KLAYOUT_BE) {
          ch = conv_table_be[prev_report.keycode[i]][0];
          chshift = conv_table_be[prev_report.keycode[i]][(is_lshift_pressed?1:0)];
        }
        int flags = 0;
        if (is_lshift_pressed) flags |= KBD_FLAG_LSHIFT;
        if (is_lctrl_pressed) flags |= KBD_FLAG_LCONTROL;
        if (is_lalt_pressed) flags |= KBD_FLAG_LALT;
        if (is_rshift_pressed) flags |= KBD_FLAG_RSHIFT;
        if (is_rctrl_pressed) flags |= KBD_FLAG_RCONTROL;
        if (is_ralt_pressed) flags |= KBD_FLAG_RALT;        
        kbd_signal_raw_key(prev_report.keycode[i], ch, chshift, flags, KEY_RELEASED);
      } 
    }
  } 
  prev_report = *report;
}
 
//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

void hid_app_task(void)
{
  // nothing to do
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  (void)desc_report;
  (void)desc_len;
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  printf("VID = %04x, PID = %04x\r\n", vid, pid);

  /* Ask for a report only if this is a keyboard device */
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
  if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD) 
  {
    printf("Keyboard found\n");
    tuh_hid_receive_report (dev_addr, instance);
  }

}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}



// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance; (void) len;
  // In principle we don't need to test that this USB report came from
  //   a keyboard, since we are only asking for reports from keyboards.
  // But, for future expansion, we should be systematic
  switch (tuh_hid_interface_protocol (dev_addr, instance)) 
  {
    case HID_ITF_PROTOCOL_KEYBOARD:
      process_kbd_report ((hid_keyboard_report_t const*) report);
      break;
  }

  // Ask the device for the next report -- asking for a report is a
  //   one-off operation, and must be repeated by the application. 
  tuh_hid_receive_report (dev_addr, instance);

  /*
  if ( is_sony_ds4(dev_addr) )
  {
    process_sony_ds4(report, len);
  }

  // continue to request to receive report
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    printf("Error: cannot request to receive report\r\n");
  }
  */
}

void kbd_set_locale(KLAYOUT layout)
{
  klayout = layout;
}
