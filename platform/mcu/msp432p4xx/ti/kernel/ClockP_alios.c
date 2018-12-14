/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== ClockP_alios.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include <k_config.h>

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>

/* System tick period in microseconds */
#define TICK_PERIOD_US (RHINO_CONFIG_TICKS_PER_SECOND / 1000)

void ClockP_callbackFxn(uintptr_t arg);

typedef struct ClockP_AliosThingsObj {
    aos_timer_t      timer;
    ClockP_Fxn       fxn;
    uintptr_t        arg;
    uint32_t         timeout;
    uint32_t         period;
} ClockP_AliosObj;

/*
 *  ======== ClockP_create ========
 */
ClockP_Handle ClockP_create(ClockP_Fxn clockFxn, uint32_t timeout,
        ClockP_Params *params)
{
    ClockP_Params       defaultParams;
    ClockP_AliosObj     *pObj;
    int                 repeat = 0;
    int                 ret = 0;
    uint32_t            initialTimeout = timeout;

    if (params == NULL) {
        params = &defaultParams;
        ClockP_Params_init(&defaultParams);
    }

    if ((pObj = aos_malloc(sizeof(ClockP_AliosObj))) == NULL) {
        return (NULL);
    }

    repeat = (params->period == 0) ? 0 : 1;

    /*
     *  FreeRTOS does not allow you to create a timer with a timeout
     *  of 0.  If timeout is 0, assume that ClockP_setTimeout() will be
     *  called to change it, and create the timer with a non-zero timeout.
     */
    if (timeout == 0) {
        initialTimeout = AOS_WAIT_FOREVER;
    }

    ret = aos_timer_new_ext(&(pObj->timer), clockFxn, (void *)params->arg, timeout, repeat, params->startFlag);
    if (ret) {
        aos_free(pObj);
        pObj = NULL;
        return NULL;
    }

    pObj->fxn = clockFxn;
    pObj->arg = params->arg;
    pObj->timeout = timeout;
    pObj->period = params->period;

    return ((ClockP_Handle)pObj);
}

/*
 *  ======== ClockP_delete ========
 */
void ClockP_delete(ClockP_Handle handle)
{
    ClockP_AliosObj *pObj = (ClockP_AliosObj *)handle;

    if (NULL == handle) {
        return;
    }
    
    aos_timer_free(&pObj->timer);

    aos_free(pObj);
}

/*
 *  ======== ClockP_getCpuFreq ========
 */
void ClockP_getCpuFreq(ClockP_FreqHz *freq)
{
    if (NULL == freq) {
        return;
    }
    
    freq->lo = SystemCoreClock;
    freq->hi = 0;
}

/*
 *  ======== ClockP_getSystemTickPeriod ========
 */
uint32_t ClockP_getSystemTickPeriod()
{
    uint32_t tickPeriodUs;

    tickPeriodUs = 1000000 / RHINO_CONFIG_TICKS_PER_SECOND;

    return (tickPeriodUs);
}

/*
 *  ======== ClockP_getSystemTicks ========
 *  TODO determine if we ever call this from an ISR
 */
uint32_t ClockP_getSystemTicks()
{
    return ((uint32_t)krhino_sys_tick_get());
}

/*
 *  ======== ClockP_getTimeout ========
 */
uint32_t ClockP_getTimeout(ClockP_Handle handle)
{
    ClockP_AliosObj *pObj = (ClockP_AliosObj *)handle;

    if (NULL == handle) {
        return -1;
    }

    
    return pObj->timeout;
}

/*
 *  ======== ClockP_isActive ========
 */
bool ClockP_isActive(ClockP_Handle handle)
{
    if (NULL == handle) {
        return false;
    }
    
    return (true);
}

/*
 *  ======== ClockP_Params_init ========
 */
void ClockP_Params_init(ClockP_Params *params)
{
    params->arg = (uintptr_t)0;
    params->startFlag = false;
    params->period = 0;
}

/*
 *  ======== ClockP_setTimeout ========
 */
void ClockP_setTimeout(ClockP_Handle handle, uint32_t timeout)
{
    ClockP_AliosObj *pObj = (ClockP_AliosObj *)handle;

    /*
     *  We are not allowed to change the timeout once the ClockP objecct has
     *  been started.  The next call to ClockP_start() will set the alios
     *  timer's timeout to the new value.
     */
    pObj->timeout = timeout;
}

/*
 *  ======== ClockP_start ========
 */
void ClockP_start(ClockP_Handle handle)
{
    ClockP_AliosObj *pObj = (ClockP_AliosObj *)handle;

    if (pObj->timeout == 0) {
        return;
    }

    aos_timer_start(&(pObj->timer));
}

/*
 *  ======== ClockP_stop ========
 */
void ClockP_stop(ClockP_Handle handle)
{
    ClockP_AliosObj *pObj = (ClockP_AliosObj *)handle;

    if (NULL == handle) {
        return;
    }
    
    aos_timer_stop(&(pObj->timer));
    
}

/*
 *  ======== ClockP_sleep ========
 */
void ClockP_sleep(uint32_t sec)
{
    aos_msleep(1000 * sec);
}

/*
 *  ======== ClockP_usleep ========
 */
void ClockP_usleep(uint32_t usec)
{
    /*not support for now*/
    usec = usec;
}

/*
 *  ======== setClockObjPeriod ========
 */
static bool setClockObjTimeout(ClockP_Handle handle)
{
    ClockP_AliosObj *pObj = (ClockP_AliosObj *)handle;
    int              ret = 0;

    if (NULL == handle) {
        return false;
    }

    ret = aos_timer_change(&(pObj->timer), pObj->timeout);
    if (ret) {
        return (false);
    }

    return (true);
}
