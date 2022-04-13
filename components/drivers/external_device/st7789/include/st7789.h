/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <stdio.h>

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

#define ST7789_HEIGHT 240
#define ST7789_WIDTH  320

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_IDMOFF  0x38
#define ST7789_IDMON   0x39

#define ST7789_CASET 0x2A
#define ST7789_RASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_RAMRD 0x2E

#define ST7789_COLMOD 0x3A
#define ST7789_MADCTL 0x36

#define ST7789_PTLAR    0x30
#define ST7789_VSCRDEF  0x33
#define ST7789_VSCRSADD 0x37

#define ST7789_WRDISBV  0x51
#define ST7789_WRCTRLD  0x53
#define ST7789_WRCACE   0x55
#define ST7789_WRCABCMB 0x5e

#define ST7789_POWSAVE    0xbc
#define ST7789_DLPOFFSAVE 0xbd

#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x08

#define ST_CMD_DELAY 0x80

int         st7789_hw_init(st7789_dev_t *st7789_dev);
static void st7789_init_command(st7789_dev_t st7789_dev);
int         st7789_hw_uninit(st7789_dev_t *st7789_dev);
void        st7789_draw_pixel(st7789_dev_t st7789_dev,
                              uint16_t     x,
                              uint16_t     y,
                              uint16_t     color);
void        st7789_draw_area(st7789_dev_t st7789_dev,
                             uint16_t     x,
                             uint16_t     y,
                             uint16_t     width,
                             uint16_t     height,
                             uint8_t *frame);
void        st7789_draw_rect(st7789_dev_t st7789_dev,
                             uint16_t     x,
                             uint16_t     y,
                             uint16_t     width,
                             uint16_t     height,
                             uint16_t     color);
void        st7789_draw_frame(st7789_dev_t st7789_dev, uint8_t *frame);
void        st7789_set_rotation(st7789_dev_t st7789_dev, uint16_t rotation);
void        st7789_set_invert(st7789_dev_t st7789_dev, uint8_t invert);
void        st7789_enter_sleep(st7789_dev_t st7789_dev);
void        st7789_exit_sleep(st7789_dev_t st7789_dev);
