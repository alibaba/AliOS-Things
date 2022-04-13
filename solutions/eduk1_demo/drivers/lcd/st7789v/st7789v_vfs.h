/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "hal_iomux_haas1000.h"
#include "st7789v_hdconfig.h"
#include <stdio.h>

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

#define SPI_MAX_BLOCK 60000

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
