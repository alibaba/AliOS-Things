/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
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

#include <ti/drivers/Capture.h>
#include <ti/drivers/Power.h>

#include <ti/drivers/capture/CaptureMSP432.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/timer/TimerMSP432.h>
#include <ti/drivers/power/PowerMSP432.h>

#include <ti/devices/DeviceFamily.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/pmap.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>

#define getCompareRegister(port) (((port) >> CAPTUREMSP432_CCR_OFS) & 0xF)
#define getIntNum(port)          (((port) >> CAPTUREMSP432_INT_OFS) & 0xFF)
#define getPinPort(port)         (((port) >> 4) & 0xF)
#define getPin(port)             ((port) & 0xF)
#define getPortMap(port)         (((port) >> CAPTUREMSP432_PMAP_OFS) & 0xFF)

void CaptureMSP432_close(Capture_Handle handle);
int_fast16_t CaptureMSP432_control(Capture_Handle handle,
    uint_fast16_t cmd, void *arg);
void CaptureMSP432_init(Capture_Handle handle);
Capture_Handle CaptureMSP432_open(Capture_Handle handle, Capture_Params *params);
int32_t CaptureMSP432_start(Capture_Handle handle);
void CaptureMSP432_stop(Capture_Handle handle);

/* Internal static functions */
static void CaptureMSP432_hwiIntFunction(uintptr_t arg);
static void mapPin(uint8_t port, uint8_t pin, uint8_t value);

/* Function table for CaptureMSP432 implementation */
const Capture_FxnTable CaptureMSP432_captureFxnTable = {
    .closeFxn   = CaptureMSP432_close,
    .openFxn    = CaptureMSP432_open,
    .startFxn   = CaptureMSP432_start,
    .stopFxn    = CaptureMSP432_stop,
    .initFxn    = CaptureMSP432_init,
    .controlFxn = CaptureMSP432_control
};

/* Timer_A Capture Mode Configuration Parameter */
static Timer_A_CaptureModeConfig captureModeConfig =
{
    TIMER_A_CAPTURECOMPARE_REGISTER_1,        /* CC Register 1 */
    TIMER_A_CAPTUREMODE_RISING_EDGE,          /* Rising Edge */
    TIMER_A_CAPTURE_INPUTSELECT_CCIxA,        /* CCIxA Input Select */
    TIMER_A_CAPTURE_ASYNCHRONOUS,             /* ASynchronous Capture */
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, /* Disable interrupt */
    TIMER_A_OUTPUTMODE_OUTBITVALUE            /* Output bit value */
};

/* Timer_A Continuous Mode Configuration Parameter */
static Timer_A_ContinuousModeConfig continuousModeConfig =
{
    TIMER_A_CLOCKSOURCE_SMCLK,           /* SMCLK Clock Source */
    TIMER_A_CLOCKSOURCE_DIVIDER_1,       /* (SMCLK / 1) */
    TIMER_A_TAIE_INTERRUPT_DISABLE,      /* Disable Timer ISR */
    TIMER_A_DO_CLEAR                     /* Clear Counter */
};

/*
 *  ======== Capture_close ========
 */
void CaptureMSP432_close(Capture_Handle handle)
{
    CaptureMSP432_Object *object = handle->object;
    CaptureMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t port, pin;

    CaptureMSP432_stop(handle);

    port = getPinPort(hwAttrs->capturePort);
    pin = getPin(hwAttrs->capturePort);

    /* Undo port mapping */
    if (getPortMap(hwAttrs->capturePort)) {
        mapPin(port, pin, PMAP_NONE);
    }

    MAP_GPIO_setAsOutputPin(getPinPort(hwAttrs->capturePort),
        (1 << pin));
    MAP_GPIO_setOutputLowOnPin(port, pin);

    if (object->hwiHandle) {
        HwiP_delete(object->hwiHandle);

        object->hwiHandle = NULL;
    }

    TimerMSP432_freeTimerResource(hwAttrs->timerBaseAddress);
}

/*
 *  ======== Capture_control ========
 */
int_fast16_t CaptureMSP432_control(Capture_Handle handle, uint_fast16_t cmd,
    void *arg)
{
        return (Capture_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== Capture_hwiIntFunction ========
 */
void CaptureMSP432_hwiIntFunction(uintptr_t arg)
{
    Capture_Handle handle = (Capture_Handle) arg;
    CaptureMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    CaptureMSP432_Object *object = handle->object;
    PowerMSP432_Freqs curFrequencies;
    uint32_t currentCount, interval, clockFreq;

    /* Read the capture count */
    currentCount = MAP_Timer_A_getCaptureCompareCount(hwAttrs->timerBaseAddress,
        object->ccrRegister);

    /* Clear interrupt */
    MAP_Timer_A_clearCaptureCompareInterrupt(hwAttrs->timerBaseAddress,
        object->ccrRegister);

    /* Calculate the interval */
    if (currentCount < object->previousCount) {

        /* Calculate the difference if the timer rolled over */
        interval = currentCount + (0xFFFF - object->previousCount);
    }
    else if (currentCount > object->previousCount) {

        interval = currentCount - object->previousCount;
    }
    else {
        interval = 1;
    }

    /* Store the count for the next interrupt */
    object->previousCount = currentCount;

    /* If the units are set to counts, we just need to return the count */
    if (object->periodUnits != Capture_PERIOD_COUNTS) {

        /* Otherwise we need to do some calculation with the clock source */
        PowerMSP432_getFreqs(Power_getPerformanceLevel(), &curFrequencies);

        if (hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_SMCLK) {

            clockFreq = curFrequencies.SMCLK / hwAttrs->clockDivider;
        }
        else {

            clockFreq = curFrequencies.ACLK / hwAttrs->clockDivider;
        }

        /* Convert the interval to microseconds or hertz */
        if (object->periodUnits == Capture_PERIOD_US) {

            /*
             * Scaling instead of using floating point.
             * 0 <= interval <= 0xFFFF
             * 512 <= clockFreq <= 24000000
             */
            interval = (interval * 10000) / (clockFreq / 100);
        }
        else if (object->periodUnits == Capture_PERIOD_HZ) {

            /* interval != 0 */
            interval = clockFreq / interval;
        }
    }

    /* Invoke the callback */
    object->callBack(handle, (uint32_t) interval);
}

/*
 *  ======== Capture_init ========
 */
void CaptureMSP432_init(Capture_Handle handle)
{
    return;
}

/*
 *  ======== Capture_open ========
 */
Capture_Handle CaptureMSP432_open(Capture_Handle handle, Capture_Params *params)
{
    CaptureMSP432_Object *object = handle->object;
    CaptureMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    HwiP_Params hwiParams;
    uint32_t port, pin, periphValue, mode;
    uintptr_t key;

    if (params == NULL || params->callbackFxn == NULL ||
       (hwAttrs->clockSource != TIMER_A_CLOCKSOURCE_SMCLK &&
        hwAttrs->clockSource != TIMER_A_CLOCKSOURCE_ACLK)) {

        return (NULL);
    }

    /* Attempt to allocate the timer hardware resource */
    if (!TimerMSP432_allocateTimerResource(hwAttrs->timerBaseAddress)) {

        return (NULL);
    }

    HwiP_Params_init(&hwiParams);
    hwiParams.arg = (uintptr_t) handle;
    hwiParams.priority = hwAttrs->intPriority;

    object->hwiHandle = HwiP_create(getIntNum(hwAttrs->capturePort),
        CaptureMSP432_hwiIntFunction, &hwiParams);

    if (object->hwiHandle == NULL) {

        TimerMSP432_freeTimerResource(hwAttrs->timerBaseAddress);

        return (NULL);
    }

    object->isRunning = false;
    object->callBack = params->callbackFxn;
    object->periodUnits = params->periodUnit;
    object->ccrRegister = getCompareRegister(hwAttrs->capturePort);

    if (params->mode == Capture_RISING_EDGE) {
        mode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    }
    else if (params->mode == Capture_FALLING_EDGE) {
        mode = TIMER_A_CAPTUREMODE_FALLING_EDGE;
    }
    else {
        mode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    }

    port = getPinPort(hwAttrs->capturePort);
    pin = getPin(hwAttrs->capturePort);
    periphValue = GPIO_PRIMARY_MODULE_FUNCTION;

    /* If this is a port mapped configuration, configure the port mapping. */
    if (getPortMap(hwAttrs->capturePort)) {
        mapPin(port, pin, getPortMap(hwAttrs->capturePort));
    }
    else if (hwAttrs->capturePort == CaptureMSP432_P8_0_TA1 ||
        hwAttrs->capturePort == CaptureMSP432_P8_1_TA2) {
        periphValue = GPIO_SECONDARY_MODULE_FUNCTION;
    }

    /* Set pin as input */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port, (1 << pin),
        periphValue);

    key = HwiP_disable();

    /* Setting up the timer hardware for continuous mode */
    continuousModeConfig.clockSource = hwAttrs->clockSource;
    continuousModeConfig.clockSourceDivider = hwAttrs->clockDivider;
    MAP_Timer_A_configureContinuousMode(hwAttrs->timerBaseAddress,
        &continuousModeConfig);

    /* Initializing the capture */
    captureModeConfig.captureMode = mode;
    captureModeConfig.captureRegister = object->ccrRegister;
    MAP_Timer_A_initCapture(hwAttrs->timerBaseAddress,
        &captureModeConfig);

    HwiP_restore(key);

    return (handle);
}

/*
 *  ======== Capture_start ========
 */
int32_t CaptureMSP432_start(Capture_Handle handle)
{
    CaptureMSP432_Object *object = handle->object;
    CaptureMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    uintptr_t key;

    key = HwiP_disable();

    if (object->isRunning) {

        HwiP_restore(key);

        return (Capture_STATUS_ERROR);
    }

    object->isRunning = true;
    object->previousCount = 0;

    /*
     * For MSP432P401x devices set the DEEPSLEEP_0 constraint to allow the
     * capture operation to complete.  This constraint is not necessary for
     * later MSP432 devices, because on those devices the timers can continue
     * to run when the CPU enters deep sleep.
     */
#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
    Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif

    /* Set a constraint to prevent performance level changes during capture. */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Force the timer counter to be cleared */
    continuousModeConfig.clockSource = hwAttrs->clockSource;
    continuousModeConfig.clockSourceDivider = hwAttrs->clockDivider;
    MAP_Timer_A_configureContinuousMode(hwAttrs->timerBaseAddress,
        &continuousModeConfig);

    MAP_Timer_A_clearTimer(hwAttrs->timerBaseAddress);

    MAP_Timer_A_clearCaptureCompareInterrupt(hwAttrs->timerBaseAddress,
        object->ccrRegister);
    MAP_Timer_A_enableCaptureCompareInterrupt(hwAttrs->timerBaseAddress,
        object->ccrRegister);
    MAP_Timer_A_startCounter(hwAttrs->timerBaseAddress,
            TIMER_A_CONTINUOUS_MODE);

    HwiP_restore(key);

    return (Capture_STATUS_SUCCESS);
}

/*
 *  ======== Capture_stop ========
 */
void CaptureMSP432_stop(Capture_Handle handle)
{
    CaptureMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;
    CaptureMSP432_Object *object = handle->object;
    uintptr_t key;

    key = HwiP_disable();

    if (object->isRunning) {

        object->isRunning = false;

        MAP_Timer_A_stopTimer(hwAttrs->timerBaseAddress);
        MAP_Timer_A_disableCaptureCompareInterrupt(hwAttrs->timerBaseAddress,
            object->ccrRegister);

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
    }

    HwiP_restore(key);
}

static void mapPin(uint8_t port, uint8_t pin, uint8_t value)
{
    volatile uint8_t pmap;

    /*
     * Port 2 = 0x10 (2 * 8)
     * Port 3 = 0x18 (3 * 8)
     * Port 7 = 0x38 (7 * 8)
     */
    pmap = port * 0x08;

    /* Enable write-access to port mapping registers */
    PMAP->KEYID = PMAP_KEYID_VAL;

    /* Configure port mapping */
    PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | PMAP_ENABLE_RECONFIGURATION;
    HWREG8((uint32_t) (PMAP_BASE + pin + pmap)) = value;

    /* Disable write-access to port mapping registers */
    PMAP->KEYID = 0;
}
