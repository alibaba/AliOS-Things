/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "aos_hal_lcd.h"
#include "udisplay.h"

int32_t aos_hal_lcd_init(void)
{
    return udisplay_init();
}

int32_t aos_hal_lcd_uninit(void)
{
    return udisplay_uninit();
}

int32_t aos_hal_lcd_show(int x, int y, int w, int h, uint8_t *buf, bool rotate)
{
    return udisplay_show_rect(buf, x, y, w, h, rotate);
}

int32_t aos_hal_lcd_fill(int x, int y, int w, int h, uint32_t color)
{
    return udisplay_fill_rect(x, y, w, h, color);
}
