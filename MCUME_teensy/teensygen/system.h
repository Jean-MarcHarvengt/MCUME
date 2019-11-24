
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/* Input devices */
#define MAX_DEVICES         (8)     /* Unsure of maximum */
#define DEVICE_2BUTTON      (0)     /* 2-button gamepad */
#define DEVICE_3BUTTON      (1)     /* 3-button gamepad */
#define DEVICE_6BUTTON      (2)     /* 6-button gamepad */

/* Input bitmasks */
#define INPUT_MODE      (0x00000800)
#define INPUT_Z         (0x00000400)
#define INPUT_Y         (0x00000200)
#define INPUT_X         (0x00000100)
#define INPUT_START     (0x00000080)
#define INPUT_C         (0x00000040)
#define INPUT_B         (0x00000020)
#define INPUT_A         (0x00000010)
#define INPUT_LEFT      (0x00000008)
#define INPUT_RIGHT     (0x00000004)
#define INPUT_DOWN      (0x00000002)
#define INPUT_UP        (0x00000001)

typedef struct
{
//    uint8 *data;        /* Bitmap data */
    int width;          /* Bitmap width (32+512+32) */
    int height;         /* Bitmap height (256) */
//    int depth;          /* Color depth (8 bits) */
    int pitch;          /* Width of bitmap in bytes */
//    int granularity;    /* Size of each pixel in bytes */
//    int remap;          /* 1= Translate pixel data */
    struct {
        int x;          /* X offset of viewport within bitmap */
        int y;          /* Y offset of viewport within bitmap */
        int w;          /* Width of viewport */
        int h;          /* Height of viewport */
        int ow;         /* Previous width of viewport */
        int oh;         /* Previous height of viewport */
        int changed;    /* 1= Viewport width or height have changed */
    }viewport;
}t_bitmap;  

typedef struct
{
    uint8 dev[MAX_DEVICES];     /* Can be any of the DEVICE_* values */
    uint32 pad[MAX_DEVICES];    /* Can be any of the INPUT_* bitmasks */
    uint32 system;              /* Can be any of the SYSTEM_* bitmasks */
}t_input;

typedef struct
{
    int sample_rate;    /* Sample rate (8000-44100) */
    int enabled;        /* 1= sound emulation is enabled */
    int buffer_size;    /* Size of sound buffer (in bytes) */
    int16 *buffer[2];   /* Signed 16-bit stereo sound data */
    struct {
        int curStage;
        int lastStage;
        int16 *buffer[2];   
    } fm;
    struct {
        int curStage;
        int lastStage;
        int16 *buffer;   
    } psg;
}t_snd;

/* Global variables */
extern t_bitmap gbitmap;
extern t_input input;
extern t_snd snd;

/* Function prototypes */
void system_init(void);
int system_frame(int skip);
void system_reset(void);
void system_shutdown(void);
int audio_init(int rate);
void audio_update(void);
void audio_play_sample(int16 *bufl, int16 *bufr, int length);

#endif /* _SYSTEM_H_ */

