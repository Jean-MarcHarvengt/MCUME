 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Keyboard buffer. Not really needed for X, but for SVGAlib and possibly
  * Mac and DOS ports.
  * 
  * (c) 1995 Bernd Schmidt
  */

#include "shared.h"

//#include <assert.h>

#include "keybuf.h"
#include "keyboard.h"

/* This is cryptic, but hey, it works! */
int fakestate[5] = { 0, 0, 0, 0, 0 };

void getjoystate(UWORD *st, int *button)
{
    if (fake_joystick) {
	int top = fakestate[0];
	int bot = fakestate[3];
	if (fakestate[1]) top = !top;
	if (fakestate[2]) bot = !bot;
	*st = bot | (fakestate[2] << 1) | (top << 8) | (fakestate[1] << 9);
	*button = fakestate[4];
    } else
	read_joystick(st, button);
}

static int kpb_first, kpb_last;

static int keybuf[256];

int keys_available (void)
{
    return kpb_first != kpb_last;
}

int get_next_key (void)
{
    int key;
    
    //assert (kpb_first != kpb_last);
    
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
	emu_printf("Keyboard buffer overrun. Congratulations.\n");
	return;
    }
    if (fake_joystick) {
	switch (kc >> 1) {
	 case AK_NP8: fakestate[0] = !(kc & 1); return;
	 case AK_NP4: fakestate[1] = !(kc & 1); return;
	 case AK_NP6: fakestate[2] = !(kc & 1); return;
	 case AK_NP2: fakestate[3] = !(kc & 1); return;
	 case AK_NP0: case AK_NP5: fakestate[4] = !(kc & 1); return;
	}
    }
    keybuf[kpb_first] = kc;
    kpb_first = kpb_next;
}

void keybuf_init (void)
{
    kpb_first = kpb_last = 0;
}
