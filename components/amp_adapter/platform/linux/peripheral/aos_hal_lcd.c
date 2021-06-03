/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include "aos_hal_lcd.h"

int32_t aos_hal_lcd_init(void)
{
    printf("aos_hal_lcd_init done\n");
    return 0;
}

int32_t aos_hal_lcd_uninit(void)
{
    printf("aos_hal_lcd_uninit done\n");
    return 0;
}

int32_t aos_hal_lcd_show(int x, int y, int w, int h, uint8_t *buf, bool rotate)
{
    printf("aos_hal_lcd_show x: %d, y: %d, w: %d, h: %d, rotate: %d\n", x, y, w, h, rotate);
    for (int i; i < w * h * 2; i++)
        printf("buf: 0x%x", buf[i]);

    return 0;
}

int32_t aos_hal_lcd_fill(int x, int y, int w, int h, uint32_t color)
{
    printf("aos_hal_lcd_fill x: %d, y: %d, w: %d, h: %d, color: 0x%x\n", x, y, w, h, color);
    return 0;
}
