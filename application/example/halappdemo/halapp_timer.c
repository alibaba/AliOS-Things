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

/* gpio app macro default value
   need to ajust for each board
*/
#ifndef ON_BOARD_LED02
#define ON_BOARD_LED02      23 //PB0
#endif

#ifndef PORT_TIMER5
#define PORT_TIMER5         5   //logic num, define corresponding physical port for each board
#endif

static void hal_gpio_int_fun(gpio_dev_t * gpio)
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
    timer_dev_t time;
    gpio_dev_t * pgpio_out = malloc(sizeof(gpio_dev_t)); /*GPIO8*/
    if(NULL == pgpio_out) {
        return;
    }

    printf("hal_timer_app_run in\r\n");

    memset(pgpio_out,0, sizeof(gpio_dev_t));

    pgpio_out->port = ON_BOARD_LED02;
    pgpio_out->config = OUTPUT_PUSH_PULL;
    pgpio_out->priv = NULL;
    hal_gpio_init(pgpio_out);

    time.port = PORT_TIMER5;
    time.config.period = 1000*1000; //us
    time.config.reload_mode = TIMER_RELOAD_AUTO;
    time.config.cb = hal_gpio_int_fun;
    time.config.arg = pgpio_out;

    ret = hal_timer_init(&time);
    if(ret){
        return;
    }

    hal_timer_start(&time);

    aos_msleep(5000);
    hal_timer_stop(&time);
}

#endif
