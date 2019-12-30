/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/init.h"
#include "aos/kernel.h"
#include "k_api.h"

#include "jse_port.h"

#include "soc_init.h"

#ifdef AOS_BOARD_DEVELOPERKIT

void hal_lcd_init(void* arg)
{
    st7789_init();
}

void hal_lcd_draw_point(uint16_t xpos, uint16_t ypos, uint16_t color)
{
    ST7789H2_WritePixel(xpos, ypos, color);
}

void hal_lcd_draw_line(uint16_t xpos, uint16_t ypos, uint32_t rgb,
                       uint16_t length)
{
    ST7789H2_WriteLine(xpos, ypos, rgb, length);
}

#endif