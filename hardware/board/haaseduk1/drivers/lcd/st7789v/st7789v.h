/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

uint8_t st7789_init(void);
void    st7789_pixel_draw(uint16_t x, uint16_t y, uint16_t color);
void    st7789_rect_draw(uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         uint16_t color);
void    st7789_line_draw(uint16_t x,
                         uint16_t y,
                         uint16_t length,
                         uint8_t  dir,
                         uint16_t color);
void    st7789_area_draw(uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         uint8_t *frame,
                         uint32_t areaSize);
void    st7789_frame_draw(uint8_t *frame);
