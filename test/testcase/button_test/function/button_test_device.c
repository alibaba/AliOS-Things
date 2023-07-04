/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/kernel.h"

#include <aos_button.h>
#include <yunit/yunit.h>

/* module name used by log print*/
#define MODULE_NAME "button_test"

/* module parameters */
aos_key_cfg_t button_cfg;
static uint8_t button_short_press_flag = 0;
static uint8_t button_long_press_flag = 0;

static void _short_press_handler() { button_short_press_flag = 1; }
static void _long_press_handler() { button_long_press_flag = 1; }

/* test button init */
void button_test_init(void)
{
    int ret;
    button_cfg.short_press_max_ms = 500;
    button_cfg.long_press_min_ms = 1000;
    button_cfg.short_press_handler = _short_press_handler;
    button_cfg.long_press_handler = _long_press_handler;
    ret = aos_key_init(button_cfg);
    YUNIT_ASSERT_EQUAL(ret, 0);
    printf("please short press button");
    aos_msleep(2000);
    YUNIT_ASSERT_EQUAL(button_short_press_flag, 1);
    printf("please long press button for 1s");
    aos_msleep(2000);
    YUNIT_ASSERT_EQUAL(button_long_press_flag, 1);
}