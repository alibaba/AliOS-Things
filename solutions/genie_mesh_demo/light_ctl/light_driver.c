/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <aos/gpioc.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "light_driver.h"
// PA_28
#define RED_LED (28)
// PB_4
#define GREEN_LED (4)
// PB_7
#define BLUE_LED (7)

void light_driver_init(void)
{
    aos_gpioc_ref_t gpioc;
    uint32_t mode;
    aos_status_t r;

    r = aos_gpioc_get(&gpioc, 0);
    if (r) {
        printf("aos_gpioc_get failed %d\r\n", (int)r);
        return;
    }

    mode = AOS_GPIO_DIR_OUTPUT;
    mode |= AOS_GPIO_OUTPUT_CFG_PP;
    mode |= AOS_GPIO_OUTPUT_INIT_LOW;
    r = aos_gpioc_set_mode(&gpioc, RED_LED, mode);
    if (r) {
        printf("aos_gpioc_set_mode failed %d\r\n", (int)r);
        aos_gpioc_put(&gpioc);
        return;
    }

    r = aos_gpioc_get(&gpioc, 1);
    if (r) {
        printf("aos_gpioc_get failed %d\r\n", (int)r);
        return;
    }

    mode = AOS_GPIO_DIR_OUTPUT;
    mode |= AOS_GPIO_OUTPUT_CFG_PP;
    mode |= AOS_GPIO_OUTPUT_INIT_LOW;
    r = aos_gpioc_set_mode(&gpioc, GREEN_LED, mode);
    if (r) {
        printf("aos_gpioc_set_mode failed %d\r\n", (int)r);
        aos_gpioc_put(&gpioc);
        return;
    }

    r = aos_gpioc_set_mode(&gpioc, BLUE_LED, mode);
    if (r) {
        printf("aos_gpioc_set_mode failed %d\r\n", (int)r);
        aos_gpioc_put(&gpioc);
        return;
    }
}

void light_set(uint8_t onoff)
{
    aos_gpioc_ref_t gpioc;
    aos_status_t r;

    r = aos_gpioc_get(&gpioc, 0);
    if (r) {
        printf("aos_gpioc_get failed %d\r\n", (int)r);
        return;
    }

    r = aos_gpioc_set_value(&gpioc, RED_LED, !onoff);

    r = aos_gpioc_get(&gpioc, 1);
    if (r) {
        printf("aos_gpioc_get failed %d\r\n", (int)r);
        return;
    }

    r = aos_gpioc_set_value(&gpioc, GREEN_LED, !onoff);
    r = aos_gpioc_set_value(&gpioc, BLUE_LED, !onoff);
    printf("aos_gpioc_set_value %d returned %d\r\n", onoff, (int)r);
}

void light_blink(void)
{
    int out_val = 0;
    for (int i = 6; i > 0; i--) {
        light_set(out_val);
        out_val = !out_val;
        aos_msleep(500);
    }
}
