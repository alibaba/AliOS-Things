/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#define ST7789_HEIGHT    240
#define ST7789_WIDTH     320

typedef struct _st7789_dev_t {
    int spi_port;
    int spi_freq;
    int gpio_dc_id;
    int gpio_reset_id;
    int gpio_bgl_id;

    // int screen_width;
    // int screen_height;
    // below user dont need to care
    uint8_t *fb_addr;
    int      spi_fd;
    int      gpio_fd;
} st7789_dev_t;


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
void    st7789_local_refresh(uint16_t x1,
                         uint16_t y1,
                         uint16_t x2,
                         uint16_t y2,
                         uint8_t *frame);
