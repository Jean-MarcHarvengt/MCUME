#define KBD_FLAG_LSHIFT    0x01
#define KBD_FLAG_LCONTROL  0x02
#define KBD_FLAG_LALT      0x04
#define KBD_FLAG_RSHIFT    0x08
#define KBD_FLAG_RCONTROL  0x10
#define KBD_FLAG_RALT      0x20

#define KBD_KEY_BS         0x14 //8
#define KBD_KEY_ENTER      13
#define KBD_KEY_DOWN       1000
#define KBD_KEY_UP         1001
#define KBD_KEY_PGDN       1002
#define KBD_KEY_PGUP       1003
#define KBD_KEY_RIGHT      1004
#define KBD_KEY_LEFT       1005
#define KBD_KEY_HOME       1006
#define KBD_KEY_END        1007
#define KBD_KEY_ESC        1008
#define KBD_KEY_CAPS       1010

#define KBD_KEY_F1         1011
#define KBD_KEY_F2         1012
#define KBD_KEY_F3         1013
#define KBD_KEY_F4         1014
#define KBD_KEY_F5         1015
#define KBD_KEY_F6         1016
#define KBD_KEY_F7         1017
#define KBD_KEY_F8         1018
#define KBD_KEY_F9         1019
#define KBD_KEY_F10        1020
#define KBD_KEY_F11        1021
#define KBD_KEY_F12        1022

#define KEY_PRESSED  1
#define KEY_RELEASED 0

typedef enum {
	KLAYOUT_UK = 0,
	KLAYOUT_BE,
} KLAYOUT;


#ifdef __cplusplus
extern "C" {
#endif

/* raw_key_down should be called whenever a key is pressed
 * or released. */
extern void kbd_signal_raw_key(int keycode, int code, int codeshifted, int flags, int pressed);
extern void kbd_set_locale(KLAYOUT layout);
#ifdef __cplusplus
}
#endif