/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <k_api.h>
#include "port.h"
#include "../internal/mb.h"
#include "../internal/mbport.h"

uint16_t t35_timerout;
uint16_t turn_around_timerout;
uint16_t response_timerout;

static ktimer_t mb_timer;

static void timer_cb(void *timer, void *arg);

bool mb_timer_init(uint16_t usTim1Timerout50us)
{
    kstat_t    ret;

    turn_around_timerout = krhino_ms_to_ticks(1000);
    response_timerout    = krhino_ms_to_ticks(200);
    
    t35_timerout         = (usTim1Timerout50us * 50 * RHINO_CONFIG_TICKS_PER_SECOND) / (1000 * 1000);

    if(t35_timerout < 3) {
        t35_timerout = 3;
    }

    ret = krhino_timer_create(&mb_timer, "modbus", timer_cb, t35_timerout, 0, NULL, 0);
    if(ret == RHINO_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

void mb_timer_close(  )
{
    /* Does not use any hardware resources. */
}

void mb_t35_timer_enable( )
{
    krhino_timer_stop(&mb_timer);
    krhino_timer_change(&mb_timer, t35_timerout, 0);
    krhino_timer_start(&mb_timer);
}

void mb_turnaround_timer_enable( )
{
    krhino_timer_stop(&mb_timer);
    krhino_timer_change(&mb_timer, turn_around_timerout, 0);
    krhino_timer_start(&mb_timer);
}

void mb_response_timer_enable( )
{
    krhino_timer_stop(&mb_timer);
    krhino_timer_change(&mb_timer, response_timerout, 0);
    krhino_timer_start(&mb_timer);
}

void mb_timers_disable(  )
{
    krhino_timer_stop(&mb_timer);
}

void mb_timer_wait_t35()
{
    krhino_task_sleep(t35_timerout);
}

static void timer_cb(void *timer, void *arg)
{
    mb_timer_expired_func();
}

