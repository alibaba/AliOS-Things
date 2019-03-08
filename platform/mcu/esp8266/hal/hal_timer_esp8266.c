/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <stddef.h>

#include "aos/hal/timer.h"
#include "driver/hw_timer.h"

int32_t hal_timer_init(timer_dev_t *tim)
{
    bool reload = 0;

    if (tim == NULL) {
        return -1;
    }

    hw_timer_init();
    hw_timer_set_func(tim->config.cb, tim->config.arg);

    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        reload = true;
    } else {
        reload = false;
    }

    hw_timer_arm(tim->config.period, reload);

    return 0;
}

void hal_timer_stop(timer_dev_t *tim)
{
    hw_timer_enable(false);

    return 0;
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    hw_timer_enable(true);

    return 0;
}

int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    bool reload = 0;

    if (para.reload_mode == TIMER_RELOAD_AUTO) {
        reload = true;
    } else {
        reload = false;
    }

    hw_timer_enable(false);

    tim->config.period = para.period;
    tim->config.reload_mode = para.reload_mode;
    tim->config.arg = para.arg;

    hw_timer_arm(para.period, reload);

    hw_timer_enable(true);

    return 0;
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    hw_timer_disarm();

    return 0;
}
