/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <yunit/yunit.h>
#include "aos/kernel.h"

/* module name used by log print*/
#define MODULE_NAME "backlight_test"

/* test backlight init */
void backlight_test_init(void)
{
    int ret;
    ret=aos_lcd_backlight_on();
    for (uint8_t i = 0; i < 5; i++) {
    ret = aos_lcd_backlight(i*20);
    aos_lcd_backlight_valid();
    aos_msleep(2000);
    YUNIT_ASSERT_EQUAL(ret, 0);
    }
}

/* test backlight onoff */
void backlight_test_onoff(void)
{
    int ret;
    aos_lcd_backlight_off();
    YUNIT_ASSERT_EQUAL(ret, 0);
    aos_msleep(2000);
    aos_lcd_backlight_on();
    YUNIT_ASSERT_EQUAL(ret, 0);
    aos_msleep(2000);
    aos_lcd_backlight_toggle();
    YUNIT_ASSERT_EQUAL(ret, 0);
    aos_msleep(2000);
}

/* test backlight silent_reset */
void backlight_test_silent_reset(void)
{
    int ret;
    ret = aos_lcd_backlight(5);
    YUNIT_ASSERT_EQUAL(ret, 0);
    aos_lcd_backlight_valid_for_silent_reset();
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test backlight get_status */
void backlight_test_get_status(void)
{
    int ret;
    uint8_t onoff, level;
    ret = aos_lcd_backlight_status(&onoff, &level);
    YUNIT_ASSERT_EQUAL(ret, 0);
    printf("[%s] backlight status onoff=%d level=%d \n", MODULE_NAME, onoff, level);
}
