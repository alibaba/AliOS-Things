/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/cli.h"
#include "led.h"
#include "key.h"
#include "hal_gpio.h"
#include "hal_iomux_haas1000.h"

extern uint32_t led_test_flag;
extern uint32_t auto_test_flag;

static void key_short_pressed()
{
    printf("key short press\n");
    led_test_flag = 0;
}

static void key_long_pressed()
{
    auto_test_flag = 1;
}

void key_test(void)
{
    key_cfg_t cfg;

    cfg.short_press_handler = key_short_pressed;
    cfg.long_press_handler  = key_long_pressed;
    cfg.long_press_min_ms   = 3000;
    cfg.short_press_max_ms  = 1000;
    key_init(&cfg);
}