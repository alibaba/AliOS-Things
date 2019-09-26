/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "timer.h"

using namespace AliOS;

/**
 * This function will create a timer
 * @param[in]  name      name of the timer
 * @param[in]  cb        callbak of the timer
 * @param[in]  round     ticks of the normal timer triger
 * @param[in]  arg       the argument of the callback
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Timer::create(const name_t *name,
              timer_cb_t cb,
              uint32_t millisec,
              void *arg)
{
    tick_t ticks = 0;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == Timer_WAIT_FOREVER){
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(millisec);
    }

    return krhino_timer_create(&_timer_def,
                              name,
                              cb,
                              1,
                              ticks,
                              arg,
                              0);
}

/**
 * This function will delete a timer
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Timer::destory(void)
{
    return krhino_timer_del(&_timer_def);
}

/**
 * This function will start a timer
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Timer::start(void)
{
    return krhino_timer_start(&_timer_def);
}

/**
 * This function will stop a timer
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Timer::stop(void)
{
    return krhino_timer_stop(&_timer_def);
}

/**
 * This function will get a Semaphore struct pointer
 * @param[in]  none
 * @return  Semaphore struct pointer
 */
ktimer_t *Timer::self(void)
{
    return &_timer_def;
}
