/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== ClockP_freertos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <timers.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>

static TickType_t ticksToWait = portMAX_DELAY;

void ClockP_callbackFxn(uintptr_t arg);

typedef struct ClockP_FreeRTOSObj {
    TimerHandle_t    timer;
    ClockP_Fxn       fxn;
    uintptr_t        arg;
    uint32_t         timeout;
    uint32_t         period;
} ClockP_FreeRTOSObj;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
typedef struct ClockP_StaticFreeRTOSObj {
    ClockP_FreeRTOSObj clockObj;
    StaticTimer_t      staticTimer;
} ClockP_StaticFreeRTOSObj;
#endif

/*
 *  ======== ClockP_callbackFxn ========
 */
void ClockP_callbackFxn(uintptr_t arg)
{
    TimerHandle_t    handle = (TimerHandle_t)arg;
    ClockP_FreeRTOSObj *obj;

    obj = (ClockP_FreeRTOSObj *)pvTimerGetTimerID(handle);
    (obj->fxn)(obj->arg);
}

/*
 *  ======== ClockP_create ========
 */
ClockP_Handle ClockP_create(ClockP_Fxn clockFxn, uint32_t timeout,
        ClockP_Params *params)
{
    ClockP_Params defaultParams;
    ClockP_FreeRTOSObj *pObj;
    TimerHandle_t    handle = NULL;

    if (params == NULL) {
        params = &defaultParams;
        ClockP_Params_init(&defaultParams);
    }

    if ((pObj = pvPortMalloc(sizeof(ClockP_FreeRTOSObj))) == NULL) {
        return (NULL);
    }

    handle = xTimerCreate(NULL, 1, 0, (void *)pObj,
                          (TimerCallbackFunction_t)ClockP_callbackFxn);

    if (handle == NULL) {
        vPortFree(pObj);
        return (NULL);
    }

    pObj->timer = handle;
    pObj->fxn = clockFxn;
    pObj->arg = params->arg;
    pObj->timeout = timeout;

    if (params->startFlag) {
        /* Just returns if timeout is 0 */
        ClockP_start((ClockP_Handle)pObj);
    }

    return ((ClockP_Handle)pObj);
}

/*
 *  ======== ClockP_delete ========
 */
void ClockP_delete(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    BaseType_t status;

    status = xTimerDelete((TimerHandle_t)pObj->timer, ticksToWait);

    configASSERT(status == pdPASS);

    if (status == pdPASS) {
        vPortFree(pObj);
    }
}

/*
 *  ======== ClockP_getCpuFreq ========
 */
void ClockP_getCpuFreq(ClockP_FreqHz *freq)
{
    unsigned long configCpuFreq;

    /*
     *  configCPU_CLOCK_HZ is #define'd in the target's header file,
     *  eg, in FreeRTOS/Demo/ARM7_AT91FR40008_GCC/FreeRTOSConfig.h.
     *  Sometimes configCPU_CLOCK_HZ is #define'd to a specific value,
     *  or to an extern uint32_t variable, eg:
     *
     *  #define configCPU_CLOCK_HZ     ( SystemFrequency )  // extern uint32_t
     *
     *  #define configCPU_CLOCK_HZ     ( ( unsigned long ) 8000000 )
     */

    configCpuFreq = (unsigned long)configCPU_CLOCK_HZ;
    freq->lo = (uint32_t)configCpuFreq;
    freq->hi = 0;
//    freq->hi = (uint32_t)(configCpuFreq >> 32);
}

/*
 *  ======== ClockP_getSystemTickPeriod ========
 */
uint32_t ClockP_getSystemTickPeriod()
{
    uint32_t tickPeriodUs;

    /*
     *  Tick period in microseconds. configTICK_RATE_HZ is defined in the
     *  application's FreeRTOSConfig.h, which is include by FreeRTOS.h
     */
    tickPeriodUs = 1000000 / configTICK_RATE_HZ;

    return (tickPeriodUs);
}

/*
 *  ======== ClockP_getSystemTicks ========
 *  TODO determine if we ever call this from an ISR
 */
uint32_t ClockP_getSystemTicks()
{
    return ((uint32_t)xTaskGetTickCount());
}

/*
 *  ======== ClockP_getTimeout ========
 */
uint32_t ClockP_getTimeout(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    TickType_t          remainingTime;

    /*
     *  Calculate the time that remains before the timer expires.
     *  TickType_t is an unsigned type, so the subtraction will result in
     *  the correct answer even if the timer will not expire until after
     *  the tick count has overflowed.
     */
    remainingTime = xTimerGetExpiryTime(pObj->timer) - xTaskGetTickCount();

    return ((uint32_t)remainingTime);
}

/*
 *  ======== ClockP_isActive ========
 */
bool ClockP_isActive(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;

    if (xTimerIsTimerActive(pObj->timer) != pdFALSE) {
        return (true);
    }

    return (false);
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
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;

    pObj->timeout = timeout;
}

/*
 *  ======== ClockP_start ========
 */
void ClockP_start(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    BaseType_t status;

    if (pObj->timeout == 0) {
        return;
    }

    if (!HwiP_inISR()) {
        status = xTimerChangePeriod(pObj->timer, (TickType_t)pObj->timeout,
                ticksToWait);

        configASSERT(status == pdPASS);

        if (status == pdPASS) {
            xTimerStart(pObj->timer, ticksToWait);
        }
    }
    else {
        status = xTimerChangePeriodFromISR(pObj->timer,
                (TickType_t)pObj->timeout, NULL);
        if (status == pdPASS) {
            xTimerStartFromISR(pObj->timer, NULL);
        }
    }
}

#if (configSUPPORT_STATIC_ALLOCATION == 1)
/*
 *  ======== ClockP_staticObjectSize ========
 */
size_t ClockP_staticObjectSize(void)
{
    return (sizeof(ClockP_StaticFreeRTOSObj));
}
#endif

/*
 *  ======== ClockP_stop ========
 */
void ClockP_stop(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    BaseType_t status;

    if (!HwiP_inISR()) {
        status = xTimerStop(pObj->timer, ticksToWait);
    }
    else {
        status = xTimerStopFromISR(pObj->timer, NULL);
    }
    configASSERT(status == pdPASS);
}

/*
 *  ======== ClockP_sleep ========
 */
void ClockP_sleep(uint32_t sec)
{
    uint32_t msecs = sec * 1000;
    TickType_t xDelay;

    /* Take the ceiling */
    xDelay = (msecs + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS;

    vTaskDelay(xDelay);
}

/*
 *  ======== ClockP_usleep ========
 */
void ClockP_usleep(uint32_t usec)
{
    uint32_t msecs = (usec + 999) / 1000;
    TickType_t xDelay;

    /* Take the ceiling */
    xDelay = (msecs + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS;

    vTaskDelay(xDelay);
}
