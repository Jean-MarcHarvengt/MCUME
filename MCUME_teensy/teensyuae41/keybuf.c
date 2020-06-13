 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Keyboard buffer. Not really needed for X, but for SVGAlib and possibly
  * Mac and DOS ports.
  * 
  * Note: it's possible to have two threads in UAE, one reading keystrokes
  * and the other one writing them. Despite this, no synchronization effort
  * is needed. This code should be perfectly thread safe. At least if you
  * assume that integer store instructions are atomic.
  * 
  * Copyright 1995, 1997 Bernd Schmidt
  */

#include "shared.h"

#include <assert.h>
#include "keybuf.h"
#include "keyboard.h"
#include "joystick.h"

static int fakestate[3][5] = { { 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0 } };

static int *fs_np;
static int *fs_ck;
static int *fs_se;

void getjoystate(int nr, unsigned int *st, int *button)
{
    int *fake = 0;
    
    if (JSEM_ISJOY0 (nr, currprefs.fake_joystick))
	nr = 0;
    else if (JSEM_ISJOY1 (nr, currprefs.fake_joystick))
	nr = 1;
    else if (JSEM_ISMOUSE (nr, currprefs.fake_joystick)) {
	*st = 0;
	*button = 0;
	return;
    } else
	fake = fakestate[nr];

    if (fake) {
	int top = fake[0];
	int bot = fake[3];
	if (fake[1]) top = !top;
	if (fake[2]) bot = !bot;
	*st = bot | (fake[2] << 1) | (top << 8) | (fake[1] << 9);
	*button = fake[4];
    } else
	read_joystick (nr, st, button);
}

/* Not static so the DOS code can mess with them */
int kpb_first, kpb_last;

int keybuf[256];

int keys_available (void)
{
    int val;
    val = kpb_first != kpb_last;
    return val;
}

int get_next_key (void)
{
    int key;
    assert (kpb_first != kpb_last);
    
    key = keybuf[kpb_last];
    if (++kpb_last == 256) 
	kpb_last = 0;
    return key;    
}

void record_key (int kc)
{
    int kpb_next = kpb_first + 1;

    if (kpb_next == 256)
	kpb_next = 0;
    if (kpb_next == kpb_last) {
	write_log ("Keyboard buffer overrun. Congratulations.\n");
	return;
    }
    if (fs_np != 0) {
	switch (kc >> 1) {
	 case AK_NP8: fs_np[0] = !(kc & 1); return;
	 case AK_NP4: fs_np[1] = !(kc & 1); return;
	 case AK_NP6: fs_np[2] = !(kc & 1); return;
	 case AK_NP2: fs_np[3] = !(kc & 1); return;
	 case AK_NP0: case AK_NP5: fs_np[4] = !(kc & 1); return;
	}
    }
    if (fs_ck != 0) {
	switch (kc >> 1) {
	 case AK_UP: fs_ck[0] = !(kc & 1); return;
	 case AK_LF: fs_ck[1] = !(kc & 1); return;
	 case AK_RT: fs_ck[2] = !(kc & 1); return;
	 case AK_DN: fs_ck[3] = !(kc & 1); return;
	 case AK_RCTRL: fs_ck[4] = !(kc & 1); return;
	}
    }
    if (fs_se != 0) {
	switch (kc >> 1) {
	 case AK_T: fs_se[0] = !(kc & 1); return;
	 case AK_F: fs_se[1] = !(kc & 1); return;
	 case AK_H: fs_se[2] = !(kc & 1); return;
	 case AK_B: fs_se[3] = !(kc & 1); return;
	 case AK_LALT: fs_se[4] = !(kc & 1); return;
	}
    }
    if (kc == AK_RCTRL)
	kc = AK_CTRL;
    keybuf[kpb_first] = kc;
    kpb_first = kpb_next;
}

void joystick_setting_changed (void)
{
    fs_np = fs_ck = fs_se = 0;

    if (JSEM_ISNUMPAD (0, currprefs.fake_joystick))
	fs_np = fakestate[0];
    else if (JSEM_ISNUMPAD (1, currprefs.fake_joystick))
	fs_np = fakestate[1];
    
    if (JSEM_ISCURSOR (0, currprefs.fake_joystick))
	fs_ck = fakestate[0];
    else if (JSEM_ISCURSOR (1, currprefs.fake_joystick))
	fs_ck = fakestate[1];

    if (JSEM_ISSOMEWHEREELSE (0, currprefs.fake_joystick))
	fs_se = fakestate[0];
    else if (JSEM_ISSOMEWHEREELSE (1, currprefs.fake_joystick))
	fs_se = fakestate[1];
    
}

void keybuf_init (void)
{
    kpb_first = kpb_last = 0;
    joystick_setting_changed ();
}
