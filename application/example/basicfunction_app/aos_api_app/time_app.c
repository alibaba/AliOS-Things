/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This use case demonstrates the use of system time.
 * note: System time is implemented based on system tick,
 * so its precision is only as high as that of system tick.
 */

/* module name used by ulog */
#define MODULE_NAME "time_app"

void time_app(void)
{
    long long current_time;
    /* Get the time from system startup to the present(ms) */
    current_time = aos_now_ms();
    LOGE(MODULE_NAME, "current time %lld ms", current_time);

    /* sleep 1000ms */
    aos_msleep(1000);

    /* Get the time from system startup to the present(ms) */
    current_time = aos_now_ms();
    LOGE(MODULE_NAME, "current time %lld ms", current_time);

    /* sleep 1000ms */
    aos_msleep(1000);

    /**
     * Get the time from system startup to the present(ns).
     * System time is implemented based on system tick, so its precision is only as high as
     * that of system tick. System tick accuracy is usually limited to ms. If higher precision
     * is required, it needs to be based on the hardware clock.
     */
    current_time = aos_now();
    LOGE(MODULE_NAME, "current time %lld ns", current_time);
}

