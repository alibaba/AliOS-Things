/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

typedef uint16_t ret_code_t;

 ret_code_t ili9341_init(void);
 void ili9341_pixel_draw(uint16_t x, uint16_t y, uint16_t color);
 void ili9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
 void ili9341_line_draw(uint16_t x, uint16_t y, uint16_t length, uint8_t dir, uint16_t color);
 void ili9341_area_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *frame, uint32_t areaSize);
 void ili9341_frame_draw(uint16_t *frame);
