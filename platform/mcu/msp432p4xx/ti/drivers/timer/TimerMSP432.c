/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/power/PowerMSP432.h>
#include <ti/drivers/timer/TimerMSP432.h>

#include <ti/devices/DeviceFamily.h>
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>
#include <ti/devices/msp432p4xx/driverlib/timer32.h>

void TimerMSP432_close(Timer_Handle handle);
int_fast16_t TimerMSP432_control(Timer_Handle handle,
    uint_fast16_t cmd, void *arg);
uint32_t TimerMSP432_getCount(Timer_Handle handle);
void TimerMSP432_init(Timer_Handle handle);
Timer_Handle TimerMSP432_open(Timer_Handle handle, Timer_Params *params);
int32_t TimerMSP432_start(Timer_Handle handle);
void TimerMSP432_stop(Timer_Handle handle);

/* Internal static Functions */
static bool initTimerHardware(Timer_Handle handle, PowerMSP432_Freqs powerFreqs);
static inline uint32_t getT32DividerValue(uint32_t div);
static inline uint32_t getTimerResourceMask(uint32_t baseAddress);
static bool isTimer32Bit(uint32_t baseAddress);
static int_fast16_t perfChangeNotifyFxn(uint_fast16_t eventType,
    uintptr_t eventArg, uintptr_t clientArg);
static bool setT32TickCount(Timer_Handle handle, uint32_t clockFreq);
static bool setTATickCount(Timer_Handle handle, uint32_t clockFreq);
static void TimerMSP432_hwiIntFunction(uintptr_t arg);

/* Function table of function to handle Timer_A */
const Timer_FxnTable TimerMSP432_Timer_A_fxnTable = {
    .closeFxn    = TimerMSP432_close,
    .openFxn     = TimerMSP432_open,
    .startFxn    = TimerMSP432_start,
    .stopFxn     = TimerMSP432_stop,
    .initFxn     = TimerMSP432_init,
    .getCountFxn = TimerMSP432_getCount,
    .controlFxn  = TimerMSP432_control
};

/* Function table of function to handle Timer32 */
const Timer_FxnTable TimerMSP432_Timer32_fxnTable= {
    .closeFxn    = TimerMSP432_close,
    .openFxn     = TimerMSP432_open,
    .startFxn    = TimerMSP432_start,
    .stopFxn     = TimerMSP432_stop,
    .initFxn     = TimerMSP432_init,
    .getCountFxn = TimerMSP432_getCount,
    .controlFxn  = TimerMSP432_control
};

/*
 * Internal Timer status structure
 *
 * bitMask: Each timer peripheral occupies 1 bit in the bitMask. There are 2
 * Timer32 peripherals available. The 0 bit represents the TIMER32_0_BASE and
 * the 1 bit represents the TIMER32_1_BASE.
 *
 *     31 - 8       5         4         3        2       1 - 0
 *  -------------------------------------------------------------
 *  | Reserved | Timer_A3 | Timer_A2 | Timer_A1 |Timer_A0 | Timer32 |
 *  -------------------------------------------------------------
 */
static struct {
    uint32_t bitMask;
} timerState;

/* Timer32 Clock Dividers */
static const uint32_t timer32Dividers[] = {
    TIMER32_PRESCALER_256,
    TIMER32_PRESCALER_16,
    TIMER32_PRESCALER_1
};

/* Timer_A Clock Dividers */
static const uint32_t timerADividers[] = {
    TIMER_A_CLOCKSOURCE_DIVIDER_64,
    TIMER_A_CLOCKSOURCE_DIVIDER_56,
    TIMER_A_CLOCKSOURCE_DIVIDER_48,
    TIMER_A_CLOCKSOURCE_DIVIDER_40,
    TIMER_A_CLOCKSOURCE_DIVIDER_32,
    TIMER_A_CLOCKSOURCE_DIVIDER_28,
    TIMER_A_CLOCKSOURCE_DIVIDER_24,
    TIMER_A_CLOCKSOURCE_DIVIDER_20,
    TIMER_A_CLOCKSOURCE_DIVIDER_16,
    TIMER_A_CLOCKSOURCE_DIVIDER_14,
    TIMER_A_CLOCKSOURCE_DIVIDER_12,
    TIMER_A_CLOCKSOURCE_DIVIDER_10,
    TIMER_A_CLOCKSOURCE_DIVIDER_8,
    TIMER_A_CLOCKSOURCE_DIVIDER_7,
    TIMER_A_CLOCKSOURCE_DIVIDER_6,
    TIMER_A_CLOCKSOURCE_DIVIDER_5,
    TIMER_A_CLOCKSOURCE_DIVIDER_4,
    TIMER_A_CLOCKSOURCE_DIVIDER_3,
    TIMER_A_CLOCKSOURCE_DIVIDER_2,
    TIMER_A_CLOCKSOURCE_DIVIDER_1,
};

/* Structure used for Timer_A Configuration */
static Timer_A_UpModeConfig upConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,           /* SMCLK Clock Source */
    TIMER_A_CLOCKSOURCE_DIVIDER_1,       /* Default to a 1 divider */
    0xFFFF,                              /* Default to the max period */
    TIMER_A_TAIE_INTERRUPT_DISABLE,      /* Disable Timer interrupt */
    TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,  /* Enable CCR0 interrupt */
    TIMER_A_DO_CLEAR                     /* Clear value */
};

/*
 *  ======== initTimerHardware ========
 */
static bool initTimerHardware(Timer_Handle handle, PowerMSP432_Freqs powerFreqs)
{
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    TimerMSP432_Object        *object = handle->object;

    if (isTimer32Bit(hwAttrs->timerBaseAddress)) {

        if (object->units == Timer_PERIOD_COUNTS) {
            MAP_Timer32_initModule(hwAttrs->timerBaseAddress,
            TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
            MAP_Timer32_setCount(hwAttrs->timerBaseAddress, object->rawPeriod);

            return (true);
        }
        else {

            return (setT32TickCount(handle, powerFreqs.MCLK));
        }
    }
    else {

        upConfig.clockSource = hwAttrs->clockSource;
        upConfig.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;

        if (object->units == Timer_PERIOD_COUNTS) {

            if (object->rawPeriod <= UINT16_MAX) {

                upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
                upConfig.timerPeriod = object->rawPeriod;
                MAP_Timer_A_configureUpMode(hwAttrs->timerBaseAddress, &upConfig);

                return (true);
            }
        }
        else {
            if (hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_SMCLK) {

                return (setTATickCount(handle, powerFreqs.SMCLK));
            }
            else {

                return (setTATickCount(handle, powerFreqs.ACLK));
            }
        }
    }

    return (false);
}

/*
 *  ======== getT32DividerValue ========
 */
static inline uint32_t getT32DividerValue(uint32_t div)
{
    return (1 << div);
}

/*
 * ========= getTimerResourceMask =========
 * This function returns the bit mask needed to allocate and free a timer
 * resource. The bit value returned corresponds to a bit in the
 * timerState.bitMask structure.
 */
static inline uint32_t getTimerResourceMask(uint32_t baseAddress)
{
    switch (baseAddress) {

        case TIMER32_0_BASE:

            return (0x01);

        case TIMER32_1_BASE:

            return (0x02);

        case TIMER_A0_BASE:

            return (0x04);

        case TIMER_A1_BASE:

            return (0x08);

        case TIMER_A2_BASE:

            return (0x10);

        case TIMER_A3_BASE:

            return (0x20);

        default:

            return (0x00);
    }
}

/*
 *  ======== isTimer32Bit ========
 * This function is used to determine if a timer instance is using a
 * Timer32 (32 bit) or Timer_A (16 bit) peripheral. The baseAddress is assumed
 * to be valid due to the check made by getTimerResourceMask() during the
 * TimerMSP432_allocateTimerResource() function.
 */
static inline bool isTimer32Bit(uint32_t baseAddress)
{
    if (baseAddress < TIMER32_BASE) {
        return (false);
    }

    return (true);
}

/*
 *  ======== perfChangeNotifyFxn ========
 *
 *  Called by Power module before and after performance level is changed.
 */
static int_fast16_t perfChangeNotifyFxn(uint_fast16_t eventType,
    uintptr_t eventArg, uintptr_t clientArg)
{
    PowerMSP432_Freqs powerFreqs;

    /* Get new performance level clock frequencies */
    PowerMSP432_getFreqs((uint32_t) eventArg, &powerFreqs);

    /*
     * Performance level constraints set at open allow only
     * compatible performance level changes, therefore the
     * initTimerHardware() call cannot fail.
     */
    initTimerHardware((Timer_Handle) clientArg, powerFreqs);

    return (Power_NOTIFYDONE);
}

/*
 *  ======== setT32TickCount ========
 */
static bool setT32TickCount(Timer_Handle handle, uint32_t clockFreq)
{
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    TimerMSP432_Object *object = handle->object;
    uint32_t i, curClock, dividers;
    uint32_t bestDiff, curDiff, bestDivider;

    dividers = sizeof(timer32Dividers) / sizeof(uint32_t);
    bestDiff = 0;

    for (i = 0; i < dividers; i++) {

        curClock = (clockFreq / getT32DividerValue(timer32Dividers[i]));

        if (object->units == Timer_PERIOD_US) {
            curDiff = (uint32_t)((object->rawPeriod / 1000000.0f) * curClock);
        }
        else {
            curDiff = curClock / object->rawPeriod;
        }

        if (curDiff > bestDiff) {
            bestDiff = curDiff;
            bestDivider = timer32Dividers[i];
        }
    }

    if (bestDiff) {

        MAP_Timer32_initModule(hwAttrs->timerBaseAddress, bestDivider,
            TIMER32_32BIT, TIMER32_PERIODIC_MODE);
        MAP_Timer32_setCount(hwAttrs->timerBaseAddress, bestDiff);
        object->period = bestDiff;

        return (true);
    }

    return (false);
}

/*
 *  ======== setTATickCount ========
 */
static bool setTATickCount(Timer_Handle handle, uint32_t clockFreq)
{
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    TimerMSP432_Object *object = handle->object;
    uint32_t i, curClock, dividers;
    uint32_t bestDiff, bestDivider, curDiff;

    dividers = sizeof(timerADividers) / sizeof(uint32_t);
    bestDiff = 0;

    for (i = 0; i < dividers; i++) {

        curClock = (clockFreq / timerADividers[i]);

        if (object->units == Timer_PERIOD_US) {
            curDiff = (uint32_t)((object->rawPeriod / 1000000.0f) * (curClock));
        }
        else {
            curDiff = curClock / object->rawPeriod;
        }

        if (curDiff > UINT16_MAX) {
            continue;
        }

        if (curDiff > bestDiff) {
            bestDiff = curDiff;
            bestDivider = timerADividers[i];
        }
    }

    if (bestDiff) {
        object->period = bestDiff;
        upConfig.timerPeriod = bestDiff;
        upConfig.clockSourceDivider = bestDivider;
        MAP_Timer_A_configureUpMode(hwAttrs->timerBaseAddress, &upConfig);

        return (true);
    }

    return (false);
}

/*
 *  ======== TimerMSP432_allocateTimerResource ========
 */
bool TimerMSP432_allocateTimerResource(uint32_t baseAddress)
{
    uint32_t  mask;
    uintptr_t key;
    bool status = false;

    key = HwiP_disable();

    if (!isTimer32Bit(baseAddress)) {
        /* If Timer_A already enabled */
        if ((TIMER_A_CMSIS(baseAddress)->CTL & TIMER_A_CTL_MC_3) !=
                 TIMER_A_STOP_MODE) {

            HwiP_restore(key);

            return (false);
        }
    }

    mask = getTimerResourceMask(baseAddress);

    if (mask) {

        if (!(timerState.bitMask & mask)) {
            timerState.bitMask = timerState.bitMask | mask;
            status = true;
        }
    }

    HwiP_restore(key);

    return (status);
}

/*
 *  ======== TimerMSP432_close ========
 */
void TimerMSP432_close(Timer_Handle handle)
{
    TimerMSP432_Object *object = handle->object;
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t i;

    TimerMSP432_stop(handle);

    if (object->timerSem) {
        SemaphoreP_delete(object->timerSem);
        object->timerSem = NULL;
    }

    if (object->hwiHandle) {
        HwiP_delete(object->hwiHandle);
        object->hwiHandle = NULL;
    }

    for (i = 0; object->perfConstraintMask; i++) {

        if (object->perfConstraintMask & 0x01) {
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERFLEVEL_0 + i);
        }

        object->perfConstraintMask >>= 1;
    }

    Power_unregisterNotify(&object->perfChangeNotify);

    TimerMSP432_freeTimerResource((uint32_t)hwAttrs->timerBaseAddress);
}

/*
 *  ======== TimerMSP432_control ========
 */
int_fast16_t TimerMSP432_control(Timer_Handle handle,
    uint_fast16_t cmd, void *arg)
{
    return (Timer_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== TimerMSP432_freeTimerResource ========
 */
void TimerMSP432_freeTimerResource(uint32_t baseAddress)
{
    uintptr_t key;
    uint32_t  mask;

    mask = getTimerResourceMask(baseAddress);

    key = HwiP_disable();

    timerState.bitMask = (timerState.bitMask & ~mask);

    HwiP_restore(key);
}

/*
 *  ======== TimerMSP432_getCount ========
 */
uint32_t TimerMSP432_getCount(Timer_Handle handle)
{
    TimerMSP432_Object const *object = handle->object;
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t count;

    if (isTimer32Bit(hwAttrs->timerBaseAddress)) {
        /* Virtual up counter */
        count = MAP_Timer32_getValue(hwAttrs->timerBaseAddress);
        count = object->period - count;
    }
    else {
        count = MAP_Timer_A_getCounterValue(hwAttrs->timerBaseAddress);
    }

    return (count);
}

/*
 *  ======== TimerMSP432_init ========
 */
void TimerMSP432_init(Timer_Handle handle)
{
    return;
}

/*
 *  ======== TimerMSP432_hwiIntFunction ========
 */
void TimerMSP432_hwiIntFunction(uintptr_t arg)
{
    Timer_Handle handle = (Timer_Handle) arg;
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    TimerMSP432_Object  const *object  = handle->object;

    if (isTimer32Bit(hwAttrs->timerBaseAddress)) {
        MAP_Timer32_clearInterruptFlag(hwAttrs->timerBaseAddress);
    }
    else {
        MAP_Timer_A_clearCaptureCompareInterrupt(hwAttrs->timerBaseAddress,
            TIMER_A_CAPTURECOMPARE_REGISTER_0);
    }

    if (object->mode != Timer_CONTINUOUS_CALLBACK) {
        TimerMSP432_stop(handle);
    }

   if (object->mode != Timer_ONESHOT_BLOCKING) {
        object->callBack(handle);
    }
}

/*
 *  ======== TimerMSP432_open ========
 */
Timer_Handle TimerMSP432_open(Timer_Handle handle, Timer_Params *params)
{
    TimerMSP432_Object *object = handle->object;
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    PowerMSP432_Freqs powerFreqs;
    SemaphoreP_Params semParams;
    HwiP_Params hwiParams;
    uint8_t numPerfLevels, perfLevel;

    /* Check for valid parameters */
    if (((params->timerMode == Timer_ONESHOT_CALLBACK ||
          params->timerMode == Timer_CONTINUOUS_CALLBACK) &&
          params->timerCallback == NULL) ||
          params->period == 0) {

        return (NULL);
    }

    if (!TimerMSP432_allocateTimerResource(hwAttrs->timerBaseAddress)) {

        return (NULL);
    }

    object->isRunning = false;
    object->period = params->period;
    object->rawPeriod = params->period;
    object->callBack = params->timerCallback;
    object->mode = params->timerMode;
    object->units = params->periodUnits;

    if (object->mode != Timer_FREE_RUNNING) {

        HwiP_Params_init(&hwiParams);
        hwiParams.arg = (uintptr_t) handle;
        hwiParams.priority = hwAttrs->intPriority;
        object->hwiHandle = HwiP_create(hwAttrs->intNum,
            TimerMSP432_hwiIntFunction, &hwiParams);

        if (object->hwiHandle == NULL) {

            TimerMSP432_close(handle);

            return (NULL);
        }
    }

    /* Creating the semaphore if mode is blocking */
    if (object->mode == Timer_ONESHOT_BLOCKING) {

        SemaphoreP_Params_init(&semParams);
        semParams.mode = SemaphoreP_Mode_BINARY;
        object->timerSem = SemaphoreP_create(0, &semParams);

        if (object->timerSem == NULL) {

            TimerMSP432_close(handle);

            return (NULL);
        }
    }

    /* Disable performance transitions. */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);

    /* Initialize timer hardware */
    if (!initTimerHardware(handle, powerFreqs)) {

        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
        TimerMSP432_close(handle);

        return (NULL);
    }

    /*
     * Set constraints for other performance levels.
     * Given the desired period and units, determine which performance
     * levels with which this timer is compatible.
     */
    numPerfLevels = PowerMSP432_getNumPerfLevels();

    for (perfLevel = 0; perfLevel < numPerfLevels; perfLevel++) {

        PowerMSP432_getFreqs(perfLevel, &powerFreqs);

        if (!initTimerHardware(handle, powerFreqs)) {
            /* Set constraint and keep track of it in perfConstraintMask */
            object->perfConstraintMask |= (1 << perfLevel);
            Power_setConstraint(PowerMSP432_DISALLOW_PERFLEVEL_0 + perfLevel);
        }
    }

    /* Register function to reconfigure peripheral on perf level changes */
    Power_registerNotify(&object->perfChangeNotify,
        PowerMSP432_DONE_CHANGE_PERF_LEVEL, perfChangeNotifyFxn,
        (uintptr_t) handle);

    /* Restore original hardware settings */
    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);
    initTimerHardware(handle, powerFreqs);

    return (handle);
}

/*
 *  ======== TimerMSP432_start ========
 */
int32_t TimerMSP432_start(Timer_Handle handle)
{
    TimerMSP432_Object *object = handle->object;
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    const uint32_t baseAddress = hwAttrs->timerBaseAddress;
    uintptr_t key;

    key = HwiP_disable();

    if (object->isRunning) {

        HwiP_restore(key);

        return (Timer_STATUS_ERROR);
    }

    object->isRunning = true;

    if (isTimer32Bit(baseAddress)) {

        if (object->mode != Timer_FREE_RUNNING) {
                MAP_Timer32_enableInterrupt(baseAddress);
        }

        MAP_Timer32_setCount(baseAddress, object->period);
        MAP_Timer32_startTimer(baseAddress, false);

    }
    else {

         MAP_Timer_A_clearTimer(baseAddress);

        if (object->mode != Timer_FREE_RUNNING) {
            MAP_Timer_A_enableCaptureCompareInterrupt(baseAddress,
                TIMER_A_CAPTURECOMPARE_REGISTER_0);
        }

        MAP_Timer_A_startCounter(baseAddress, TIMER_A_UP_MODE);
    }

    HwiP_restore(key);

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    if (object->mode == Timer_ONESHOT_BLOCKING) {

        /* Pend forever, ~0 */
        SemaphoreP_pend(object->timerSem, ~0);
    }

    return (Timer_STATUS_SUCCESS);
}

/*
 *  ======== TimerMSP432_stop ========
 */
void TimerMSP432_stop(Timer_Handle handle)
{
    TimerMSP432_Object *object = handle->object;
    TimerMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    const uint32_t baseAddress = hwAttrs->timerBaseAddress;
    uintptr_t key;
    bool flag = false;

    key = HwiP_disable();

    if (object->isRunning) {

        object->isRunning = false;

        if (isTimer32Bit(baseAddress)) {
            MAP_Timer32_haltTimer(baseAddress);
            MAP_Timer32_disableInterrupt(baseAddress);
        }
        else {
           MAP_Timer_A_stopTimer(baseAddress);
           MAP_Timer_A_disableCaptureCompareInterrupt(baseAddress,
               TIMER_A_CAPTURECOMPARE_REGISTER_0);
        }

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        if (object->mode == Timer_ONESHOT_BLOCKING) {
            flag = true;
        }
    }

    HwiP_restore(key);

    if (flag) {
        SemaphoreP_post(object->timerSem);
    }
}
