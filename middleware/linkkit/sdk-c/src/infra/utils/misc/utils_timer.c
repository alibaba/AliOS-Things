/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include "iot_import.h"
#include "utils_timer.h"


void iotx_time_start(iotx_time_t *timer)
{
    if (!timer) {
        return;
    }

    timer->time = HAL_UptimeMs();
}

uint64_t utils_time_spend(iotx_time_t *start)
{
    uint64_t now, res;

    if (!start) {
        return 0;
    }

    now = HAL_UptimeMs();
    res = now - start->time;
    return res;
}

uint64_t iotx_time_left(iotx_time_t *end)
{
    uint64_t now, res;

    if (!end) {
        return 0;
    }

    if (utils_time_is_expired(end)) {
        return 0;
    }

    now = HAL_UptimeMs();
    res = end->time - now;
    return res;
}

int utils_time_is_expired(iotx_time_t *timer)
{
    uint64_t cur_time;

    if (!timer) {
        return 1;
    }

    cur_time = HAL_UptimeMs();
    /*
     *  WARNING: Do NOT change the following code until you know exactly what it do!
     *
     *  check whether it reach destination time or not.
     */
    if ((cur_time - timer->time) < (UINT64_MAX / 2)) {
        return 1;
    } else {
        return 0;
    }
}

void iotx_time_init(iotx_time_t *timer)
{
    if (!timer) {
        return;
    }

    timer->time = 0;
}

void utils_time_countdown_ms(iotx_time_t *timer, uint64_t millisecond)
{
    if (!timer) {
        return;
    }

    timer->time = HAL_UptimeMs() + millisecond;
}

uint64_t utils_time_get_ms(void)
{
    return HAL_UptimeMs();
}

