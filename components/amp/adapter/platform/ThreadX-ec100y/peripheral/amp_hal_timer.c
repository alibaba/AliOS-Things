/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_timer.h"
#include "ql_timer.h"
#include "amp_defines.h"
#include "amp_system.h"


#define MOD_STR "Timer"

typedef struct {
    timer_dev_t *dev;
    int timerid;
    unsigned int flag;
} ql_timer_data_t;

static ql_timer_data_t *g_timers[2];

static void amp_hal_timer_cb(unsigned int param)
{
    ql_timer_data_t *data = g_timers[param];
    timer_dev_t *dev;

    if (!data)
        return;

    dev = data->dev;
    if (!dev)
        return;

    if (dev->config.cb)
        dev->config.cb(dev->config.arg);
}

int32_t amp_hal_timer_init(timer_dev_t *tim)
{
    ql_timer_data_t *data;

    if (tim->port < 0 || tim->port > 1) {
        amp_error(MOD_STR, "timer port %d not support", tim->port);
        return -1;
    }

    if (g_timers[tim->port]) {
        amp_error(MOD_STR, "timer port init already");
        return -1;
    }

    data = HAL_Malloc(sizeof(ql_timer_data_t));
    if (!data) {
        amp_error(MOD_STR, "alloc timer data fail");
        return -1;
    }
    data->dev = tim;

    g_timers[tim->port] = data;
    return 0;
}

int32_t amp_hal_timer_start(timer_dev_t *tim)
{
    ql_timer_data_t *data;
    unsigned int param = tim->port;
    timer_config_t *config = &tim->config;

    if (tim->port < 0 || tim->port > 1) {
        amp_error(MOD_STR, "timer port %d not support", tim->port);
        return -1;
    }

    data = g_timers[tim->port];
    if (!data) {
        amp_error(MOD_STR, "timer port %d not init", tim->port);
        return -1;
    }

    data->flag = config->reload_mode == TIMER_RELOAD_AUTO ? QL_TIMER_PERIOD : QL_TIMER_AUTO_DELETE;
    data->timerid = ql_start_timer(data->flag, config->period, amp_hal_timer_cb, param);
    if (data->timerid < 1 || data->timerid > 32) {
        amp_error(MOD_STR, "start timer fail, port %d, flag %u, period %u, id %d",
            tim->port, data->flag, config->period, data->timerid);
        return -1;
    }
    //amp_info(MOD_STR, "start timer success, port %d, period %d, id %d",
    //    tim->port, config->period, data->timerid);
    return 0;
}

void amp_hal_timer_stop(timer_dev_t *tim)
{
    ql_timer_data_t *data;

    if (tim->port < 0 || tim->port > 1) {
        amp_error(MOD_STR, "timer port %d not support", tim->port);
        return -1;
    }

    data = g_timers[tim->port];
    if (!data) {
        amp_error(MOD_STR, "timer port %d not init", tim->port);
        return -1;
    }

    ql_delete_timer(data->timerid);
}

int32_t amp_hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    return 0;
}

int32_t amp_hal_timer_finalize(timer_dev_t *tim)
{
    ql_timer_data_t *data;

    if (tim->port < 0 || tim->port > 1) {
        amp_error(MOD_STR, "timer port %d not support", tim->port);
        return -1;
    }

    if (!g_timers[tim->port]) {
        amp_error(MOD_STR, "timer port %d not init", tim->port);
        return -1;
    }

    HAL_Free(g_timers[tim->port]);
    g_timers[tim->port] = NULL;
    return 0;
}
