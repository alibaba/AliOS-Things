/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <aos_lcd.h>
#include <yunit/yunit.h>
#include "aos/kernel.h"

#define YELLOW    0xFFE0
#define BLUE      0x001F

/* module name used by log print*/
#define MODULE_NAME "lcd_test"

/* module parameters */
#define area_image_w 10
#define area_image_h 10
uint16_t *area_image = NULL;

/* test lcd init */
void lcd_test_init(void)
{
    int ret;
    ret = aos_hal_lcd_init(1);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test lcd onoff */
void lcd_test_onoff(void)
{
    int ret;
    ret = aos_hal_lcd_display_on();
    YUNIT_ASSERT_EQUAL(ret, 0);
    ret = aos_hal_lcd_display_off();
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test lcd image */
static void color_prepare(uint16_t color) {
    area_image = (uint16_t*)aos_malloc(area_image_w*area_image_h*2);
    for (int i = 0; i <(area_image_w*area_image_h); i++) {
    area_image[i] = color;
  }
}

void lcd_test_image(void)
{
    int ret;
    color_prepare(YELLOW);
    ret=aos_hal_lcd_image(0,0,area_image_w,area_image_h,area_image);
    YUNIT_ASSERT_EQUAL(ret, 0);
    aos_msleep(500);
    ret = aos_hal_lcd_clear(BLUE);
    YUNIT_ASSERT_EQUAL(ret, 0);
    aos_msleep(500);
    aos_free(area_image);
}

/* test lcd finalize */
void lcd_test_finalize(void)
{
    int ret;
    ret = aos_hal_lcd_display_off();
    YUNIT_ASSERT_EQUAL(ret, 0);
}