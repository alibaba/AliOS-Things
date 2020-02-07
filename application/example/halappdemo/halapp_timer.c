/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/gpio.h"
#include "aos/hal/timer.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_TIMER_ENABLED > 0)
static gpio_dev_t gpio_out;

static void hal_timer_int_fun(gpio_dev_t *gpio)
{
    if(NULL == gpio) {
        return;
    }

    hal_gpio_output_toggle(gpio);
}

/* toggle gpio_out periodically in timer handler */
void hal_timer_app_run(void)
{
    int32_t ret;
    timer_dev_t timer;

    printf("hal_timer_app_run in\r\n");

    memset(&gpio_out, 0, sizeof(gpio_dev_t));

    gpio_out.port = HALAPP_LED_TEST;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_out.priv = NULL;
    hal_gpio_init(&gpio_out);

    /* timer interrupts every 0.5s */
    timer.port = HALAPP_TIMER_TEST;
    timer.config.period = 500*1000;
    timer.config.reload_mode = TIMER_RELOAD_AUTO;
    timer.config.cb = hal_timer_int_fun;
    timer.config.arg = &gpio_out;

    ret = hal_timer_init(&timer);
    if(ret){
        return;
    }

    hal_timer_start(&timer);

    /* sleep 10s */
    aos_msleep(10000);

    hal_timer_stop(&timer);

    hal_timer_finalize(&timer);

    printf("hal_timer_app_run end\r\n");
}

#endif
