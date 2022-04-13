/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "to_rgb565.h"
#include <stdbool.h>

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

/* Multiplication over 8 bit emulation */
#define mul8(a, b) (((a) * (b) + 128 + (((a) * (b) + 128) >> 8)) >> 8)

/* Conversion from rgb24 to rgb565 */
#define rgb2rgb565(r, g, b) \
    ((mul8(r, 31) << 11) | (mul8(g, 63) << 5) | (mul8(b, 31) << 0))

int rgb888torgb565(unsigned char *rgb888_buf, int rgb888_size,
                     unsigned short *rgb565_buf, int rgb565_size)
{
    int i = 0;
    unsigned char Red = 0;
    unsigned char Green = 0;
    unsigned char Blue = 0;
    int count = 0;

    if (rgb888_buf == NULL || rgb888_size <= 0 || rgb565_buf == NULL || \
        rgb565_size <= 0 || (rgb565_size < (rgb888_size / 3) * 2)) {
        printf("Invail input parameter in %s\n", __func__);
        return -1 ;
    }

    for (i = 0; i < rgb888_size; i += 3) {
        Red = rgb888_buf[i] >> 3;
        Green = rgb888_buf[i + 1] >> 2;
        Blue = rgb888_buf[i + 2] >> 3;
        rgb565_buf[count++] = ((Red << 11) | (Green << 5) | (Blue));
    }
    return count;
}

int rgb565torgb888(unsigned short *rgb565_buf, int rgb565_size, unsigned char *rgb888_buf, int rgb888_size)
{
    int i = 0;
    unsigned char Red = 0;
    unsigned char Green = 0;
    unsigned char Blue = 0;
    int count = 0;

    if (rgb565_buf == NULL || rgb565_size <= 0 || rgb888_buf == NULL || rgb888_size <= 0) {
        printf("Invaild input parameter in %s\n", __func__);
        return -1;
    }

    for (i = 0; i < rgb565_size; i++) {
        // 获取RGB单色，并填充低位
        Red = (rgb565_buf[i] & RGB565_RED) >> 8 ;
        Green = (rgb565_buf[i] & RGB565_GREEN) >> 3 ;
        Blue = (rgb565_buf[i] & RGB565_BLUE) << 3 ;
        // 连接
        rgb888_buf[count++] = Red;
        rgb888_buf[count++] = Green;
        rgb888_buf[count++] = Blue;
    }
    return count;
}

