/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "osi_api.h"
#include "time.h"
#include "amp_hal_timer.h"
#include "amp_defines.h"

#define MOD_STR "AMP_TIMER"

int32_t amp_hal_timer_init(timer_dev_t *tim)
{
    osiTimer_t *rtc_timer = osiTimerCreate(osiThreadCurrent(), tim->config.cb, NULL);
    tim->port= (int)rtc_timer;
    return 0;
}

int32_t amp_hal_timer_start(timer_dev_t *tim)
{
    osiTimer_t *rtc_timer;
    if (!tim->port)
    {
        amp_error(MOD_STR, "timer not created!\n");
        return -1;
    }
    rtc_timer = (osiTimer_t *)(tim->port);

    if (tim->config.reload_mode)
    {
        osiTimerStartPeriodic(rtc_timer, tim->config.period);
        amp_error(MOD_STR, "fibo_rtc_start_timer : Periodic");
    }
    else
    {
        osiTimerStart(rtc_timer, tim->config.period);
        amp_error(MOD_STR, "fibo_rtc_start_timer");
    }
    amp_error(MOD_STR, "fibo_rtc_start_timer rtc_timer: %p", rtc_timer);
    return 0;
}

void amp_hal_timer_stop(timer_dev_t *tim)
{
    osiTimer_t *rtc_timer;
    if (!tim->port)
    {
        amp_error(MOD_STR, "timer not created!\n");
    }
    else
    {
        rtc_timer = (osiTimer_t *)(tim->port);   
        osiTimerStop(rtc_timer);
    }
}

int32_t amp_hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    return 0;
}

int32_t amp_hal_timer_finalize(timer_dev_t *tim)
{
    osiTimer_t *rtc_timer;
    if (!tim->port)
    {
        amp_error(MOD_STR, "timer not created!\n");
    }
    else
    {
        rtc_timer = (osiTimer_t *)(tim->port);
        osiTimerDelete(rtc_timer);
        amp_error(MOD_STR, "fibo_rtc_delete_timer rtc_timer: %p", rtc_timer);
    }

    return 0;
}