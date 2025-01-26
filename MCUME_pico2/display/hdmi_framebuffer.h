#ifndef HDMIFRAMEBUFFER_H
#define HDMIFRAMEBUFFER_H

#include "stdint.h"
#include <stdio.h>

typedef struct {
    uint32_t *framebuffer;
    size_t framebuffer_len; // in words
    uint32_t *dma_commands;
    size_t dma_commands_len; // in words
    uint16_t width;
    uint16_t height;
    uint16_t pitch; // Number of words between rows. (May be more than a width's worth.)
    uint8_t color_depth;
    uint8_t dma_pixel_channel;
    uint8_t dma_command_channel;
} hdmi_framebuffer_obj_t;

extern void hdmi_framebuffer(hdmi_framebuffer_obj_t *self,
    uint16_t width, uint16_t height,
    uint16_t color_depth);
extern void hdmi_framebuffer_vsync(void);

#endif