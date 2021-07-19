/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "aos_hal_lcd.h"
#ifndef AOS_BOARD_HAAS700
#include "udisplay.h"
#endif

int32_t aos_hal_lcd_init(void)
{
#ifndef AOS_BOARD_HAAS700
    return udisplay_init();
#else
    return -1;
#endif
}

int32_t aos_hal_lcd_uninit(void)
{
#ifndef AOS_BOARD_HAAS700
    return udisplay_uninit();
#else
    return -1;
#endif
}

int32_t aos_hal_lcd_show(int x, int y, int w, int h, uint8_t *buf, bool rotate)
{
#ifndef AOS_BOARD_HAAS700
    return udisplay_show_rect(buf, x, y, w, h, rotate);
#else
    return -1;
#endif
}

int32_t aos_hal_lcd_fill(int x, int y, int w, int h, uint32_t color)
{
#ifndef AOS_BOARD_HAAS700
    return udisplay_fill_rect(x, y, w, h, color);
#else
    return -1;
#endif
}
