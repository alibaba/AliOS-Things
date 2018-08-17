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
 * By default disable both asserts and log for this module.
 * This must be done before DebugP.h is included.
 */
#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif
#ifndef DebugP_LOG_ENABLED
#define DebugP_LOG_ENABLED 0
#endif

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>
#include <ti/drivers/timer/TimerMSP432.h>
#include <ti/drivers/pwm/PWMTimerMSP432.h>

#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/gpio.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>
#include <ti/devices/msp432p4xx/driverlib/pmap.h>

#define PinConfigValue(config)            (((config) >> 10) & 0x1F)
#define PinConfigModuleFunction(config)   (((config) >> 8) & 0x3)
#define PinConfigPort(config)             (((config) >> 4) & 0xF)
#define PinConfigPin(config)              (1 << ((config) & 0x7))
#define PinConfigTimerId(config)          (((config) >> 16) & 0xF)
#define PinConfigCompareRegister(config)  (((config) >> 20) & 0xF)

void PWMTimerMSP432_close(PWM_Handle handle);
int_fast16_t PWMTimerMSP432_control(PWM_Handle handle, uint_fast16_t cmd,
    void *arg);
void PWMTimerMSP432_init(PWM_Handle handle);
PWM_Handle PWMTimerMSP432_open(PWM_Handle handle, PWM_Params *params);
int_fast16_t PWMTimerMSP432_setDuty(PWM_Handle handle, uint32_t dutyValue);
int_fast16_t PWMTimerMSP432_setPeriod(PWM_Handle handle, uint32_t periodValue);
void PWMTimerMSP432_start(PWM_Handle handle);
void PWMTimerMSP432_stop(PWM_Handle handle);

/* Static functions */
static inline bool allocatePWMResource(PWM_Handle handle, PWM_Period_Units units,
    uint32_t period, uint32_t periodCounts, uint32_t dutyCounts,
    uint8_t prescalar);
static inline uint8_t calculatePrescalar(uint32_t period);
static int32_t corroborateDuty(PWM_Handle handle, uint32_t period, uint32_t duty);
static int32_t corroboratePeriod(PWM_Handle handle, uint32_t period,
    uint8_t prescalar);
static inline void freePWMResource(PWM_Handle handle);
static uint32_t getDutyCounts(PWM_Duty_Units dutyUnits, uint32_t dutyValue,
    uint32_t periodCounts, uint32_t clockFreq);
static uint32_t getPeriodCounts(PWM_Period_Units periodUnits,
    uint32_t periodValue, uint32_t clockFreq);
static void releaseConstraints(PWM_Handle handle);
static void setConstraints(PWM_Handle handle, PWM_Period_Units units,
    uint32_t period);
static void initHw(PWM_Handle handle);
static void mapPin(uint8_t port, uint8_t pin, uint8_t value);
static int_fast16_t perfChangeNotifyFxn(uint_fast16_t eventType,
    uintptr_t eventArg, uintptr_t clientArg);

/* PWM function table for PWMTimerMSP432 implementation */
const PWM_FxnTable PWMTimerMSP432_fxnTable = {
    PWMTimerMSP432_close,
    PWMTimerMSP432_control,
    PWMTimerMSP432_init,
    PWMTimerMSP432_open,
    PWMTimerMSP432_setDuty,
    PWMTimerMSP432_setPeriod,
    PWMTimerMSP432_start,
    PWMTimerMSP432_stop
};

static const uint32_t pwmTimerBaseAddr[] = {
    TIMER_A0_BASE,
    TIMER_A1_BASE,
    TIMER_A2_BASE,
    TIMER_A3_BASE
};

/*
 * Internal value to notify an error has occurred while calculating a duty
 * or period.
 */
static const uint32_t PWM_INVALID_VALUE = (~0);

/*
 * Timer_A peripheral registers have 16 bit resolution.  The max register value
 * which be set is 65535.
 */
static const uint16_t PWM_MAX_MATCH_REG_VALUE = (~0);

/*
 * Timer_A peripheral has a max prescalar of 64.
 */
static const uint16_t PWM_MAX_PRESCALAR = (64);

/* Internal Timer status structures */
static PWMTimerMSP432_Status pwmTimerStatus[PWMTimerMSP432_NUM_TIMERS] = {0};

/*
 *  ======== allocatePWMResource ========
 *  This function ensures allocation of a Timer_A peripheral and corresponding
 *  Capture Compare Register. A Timer_A peripheral can support up to 6
 *  individual PWM outputs which all share a clock source, prescalar and period.
 *  This function calls setConstraints() to set initial power performance
 *  constraints. See also freePWMResource().
 */
static bool inline allocatePWMResource(PWM_Handle handle, PWM_Period_Units units,
    uint32_t period, uint32_t periodCounts, uint32_t dutyCounts,
    uint8_t prescalar)
{
    PWMTimerMSP432_Object const *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    uintptr_t key;

    key = HwiP_disable();

    /* Check if this Timer_A peripheral is already in use by this driver */
    if (object->timerStatus->openMask == 0x00) {

        /* Attempt to allocate Timer_A peripheral */
        if(!TimerMSP432_allocateTimerResource(object->baseAddress)) {
            HwiP_restore(key);

            return (false);
        }

        object->timerStatus->clockSource = hwAttrs->clockSource;
        object->timerStatus->periodCounts = periodCounts;
        object->timerStatus->period = period;
        object->timerStatus->periodUnits = units;
        object->timerStatus->prescalar = prescalar;

        /* Set performance level constraints */
        setConstraints(handle, units, period);

        /* Register function to reconfigure peripheral on perf level changes */
        Power_registerNotify(&(object->timerStatus->perfChangeNotify),
            PowerMSP432_DONE_CHANGE_PERF_LEVEL, perfChangeNotifyFxn,
            (uintptr_t) handle);

    }
    /* Check if this Timer_A capture compare register (CCR) is in use */
    else if (object->timerStatus->openMask & (1 << object->compareOutputNum)) {
        HwiP_restore(key);

        return (false);
    }
    /* Period, periodUnits and clock source must be the same */
    else if ((object->timerStatus->period != period) ||
        (object->timerStatus->periodUnits != units)||
        (object->timerStatus->clockSource != hwAttrs->clockSource)) {
        HwiP_restore(key);

        return (false);
    }

    object->timerStatus->openMask |= (1 << object->compareOutputNum);
    object->timerStatus->duties[object->compareOutputNum] = dutyCounts;

    HwiP_restore(key);

    return (true);
}

/*
 *  ======== calculatePrescalar ========
 *  Calculates timer prescalar for a given period.
 *
 *  @param  period      in timer ticks
 *  @return prescalar   required to generate period
 */
static inline uint8_t calculatePrescalar(uint32_t period)
{
    /* Initialize to a prescalar of 1 */
    uint8_t prescalar = 1;

    while (period > PWM_MAX_MATCH_REG_VALUE) {
        prescalar <<= 1;
        period /=2;
    }

    return (prescalar);
}

/*
 *  ======== corroborateDuty ========
 */
static int32_t corroborateDuty(PWM_Handle handle, uint32_t period,
    uint32_t duty)
{
    if (duty == PWM_INVALID_VALUE) {
        DebugP_log1("PWM:(%p) duty units could not be determined.",
            (uintptr_t) handle);

        return (PWM_STATUS_ERROR);
    }

    if (duty > period) {
        DebugP_log1("PWM:(%p) duty is out of range.", (uintptr_t) handle);

        return (PWM_STATUS_INVALID_DUTY);
    }

    return (PWM_STATUS_SUCCESS);
}

/*
 *  ======== corroboratePeriod ========
 */
static int32_t corroboratePeriod(PWM_Handle handle, uint32_t period,
    uint8_t prescalar)
{
    if (period == PWM_INVALID_VALUE) {
        DebugP_log1("PWM:(%p) period units could not be determined.",
            (uintptr_t) handle);

        return (PWM_STATUS_ERROR);
    }

    if ((period == 0) || (prescalar > PWM_MAX_PRESCALAR)) {
        DebugP_log1("PWM:(%p) period is out of range.", (uintptr_t) handle);

        return (PWM_STATUS_INVALID_PERIOD);
    }

    return (PWM_STATUS_SUCCESS);
}

/*
 *  ======== freePWMResource ========
 *  This function will undo allocation performed by
 *  allocatePWMResource() appropriately.
 */
static inline void freePWMResource(PWM_Handle handle)
{
    PWMTimerMSP432_Object const *object = handle->object;
    uintptr_t key;

    key = HwiP_disable();

    object->timerStatus->duties[object->compareOutputNum] = 0;
    object->timerStatus->openMask &= ~(1 << object->compareOutputNum);

    /* If all PWM instances are closed on this Timer_A peripheral */
    if (object->timerStatus->openMask == 0x00) {
        object->timerStatus->period = 0;
        object->timerStatus->periodCounts = 0;
        object->timerStatus->prescalar = 0;

        releaseConstraints(handle);
        Power_unregisterNotify(&object->timerStatus->perfChangeNotify);
        MAP_Timer_A_stopTimer(object->baseAddress);
        TimerMSP432_freeTimerResource(object->baseAddress);
    }

    HwiP_restore(key);
}

/*
 *  ======== getDutyCounts ========
 */
static uint32_t getDutyCounts(PWM_Duty_Units dutyUnits, uint32_t dutyValue,
    uint32_t periodCounts, uint32_t clockFreq)
{
    uint32_t duty = 0;

    switch (dutyUnits) {
        case PWM_DUTY_COUNTS:
            duty = dutyValue;
            break;

        case PWM_DUTY_FRACTION:
            duty = (((uint64_t) dutyValue) * ((uint64_t) periodCounts)) /
                PWM_DUTY_FRACTION_MAX;
            break;

        case PWM_DUTY_US:
            duty = (dutyValue != 0) ? (dutyValue * (clockFreq/1000000)) - 1 : 0;
            break;

        default:
            /* Unsupported duty units return an invalid duty */
            duty = PWM_INVALID_VALUE;
    }

    return (duty);
}

/*
 *  ======== getPeriodCounts ========
 */
static uint32_t getPeriodCounts(PWM_Period_Units periodUnits,
    uint32_t periodValue, uint32_t clockFreq)
{
    uint32_t period = 0;

    switch (periodUnits) {
        case PWM_PERIOD_COUNTS:
            period = periodValue;
            break;

        case PWM_PERIOD_HZ:
            if (periodValue && periodValue <= clockFreq) {
                period = (clockFreq / periodValue) - 1;
            }
            break;

        case PWM_PERIOD_US:
            period = (periodValue * (clockFreq/1000000)) - 1;
            break;

        default:
            /* Unsupported period units return an invalid period */
            period = PWM_INVALID_VALUE;
    }

    return (period);
}

/*
 *  ======== initHw ========
 */
static void initHw(PWM_Handle handle)
{
    PWMTimerMSP432_Object    const *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    Timer_A_PWMConfig               pwmConfig;
    uint32_t                        period, duty;
    uintptr_t                       key;

    key = HwiP_disable();

    duty = object->timerStatus->duties[object->compareOutputNum];
    period = object->timerStatus->periodCounts;

    /*
     * This condition ensures that the output will remain active if the duty
     * is equal to the period.
     */
    duty = duty / object->timerStatus->prescalar;
    period = period / object->timerStatus->prescalar;
    if (period == duty) {
        duty++;
    }

    pwmConfig.clockSource = hwAttrs->clockSource;
    pwmConfig.clockSourceDivider = object->timerStatus->prescalar;;
    pwmConfig.timerPeriod = period;
    pwmConfig.compareRegister = PinConfigCompareRegister(hwAttrs->pwmPin);
    pwmConfig.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    pwmConfig.dutyCycle = duty;
    MAP_Timer_A_generatePWM(object->baseAddress, &pwmConfig);

    HwiP_restore(key);
}

/*
 *  ======== perfChangeNotifyFxn ========
 *  Called by Power module before and after performance level is changed.
 */
static int_fast16_t perfChangeNotifyFxn(uint_fast16_t eventType,
    uintptr_t eventArg, uintptr_t clientArg)
{
    PWM_Handle handle = (PWM_Handle) clientArg;
    PWMTimerMSP432_Object const *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    PowerMSP432_Freqs powerFreqs;
    uint32_t          clockFreq;
    uintptr_t         key;

    /* Get new performance level clock frequencies */
    PowerMSP432_getFreqs((uint32_t) eventArg, &powerFreqs);

    clockFreq = (hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_SMCLK) ?
            powerFreqs.SMCLK : powerFreqs.ACLK;

    key = HwiP_disable();

    object->timerStatus->periodCounts =
        getPeriodCounts(object->timerStatus->periodUnits,
        object->timerStatus->period, clockFreq);

    object->timerStatus->prescalar =
            calculatePrescalar(object->timerStatus->periodCounts);

    HwiP_restore(key);

    initHw(handle);

    return (Power_NOTIFYDONE);
}

/*
 *  ======== releaseConstraints ========
 *  This function will release all performance level constraints set in
 *  setConstraints().
 */
static void releaseConstraints(PWM_Handle handle)
{
    PWMTimerMSP432_Object const *object = handle->object;
    uint32_t i;

    for (i = 0; object->timerStatus->perfConstraintMask; i++) {

        if (object->timerStatus->perfConstraintMask & 0x01) {

            Power_releaseConstraint(PowerMSP432_DISALLOW_PERFLEVEL_0 + i);
        }

        object->timerStatus->perfConstraintMask >>= 1;
    }
}

/*
 *  ======== setConstraints ========
 *  This function will determine and set appropriate performance levels
 *  constraints given the period and period units. Performance levels
 *  set are tracked in perfConstraintMask.
 */
static void setConstraints(PWM_Handle handle, PWM_Period_Units units,
    uint32_t period)
{
    PWMTimerMSP432_Object const *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    PowerMSP432_Freqs powerFreqs;
    uint32_t perfLevel, numPerfLevels, clockFreq;

    numPerfLevels = PowerMSP432_getNumPerfLevels();

    for (perfLevel = 0; perfLevel < numPerfLevels; perfLevel++) {

        PowerMSP432_getFreqs(perfLevel, &powerFreqs);

        clockFreq = (hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_SMCLK) ?
        powerFreqs.SMCLK : powerFreqs.ACLK;

        period = getPeriodCounts(units, period, clockFreq);

        if (corroboratePeriod(handle, period, calculatePrescalar(period)) !=
            PWM_STATUS_SUCCESS) {

            /* Set constraint and keep track of it in perfConstraintMask */
            object->timerStatus->perfConstraintMask |= (1 << perfLevel);
            Power_setConstraint(PowerMSP432_DISALLOW_PERFLEVEL_0 + perfLevel);
        }
    }
}

/*
 *  ======== PWMTimerMSP432_close ========
 *  @pre    Function assumes that the handle is not NULL
 */
void PWMTimerMSP432_close(PWM_Handle handle)
{
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;

    PWMTimerMSP432_stop(handle);
    freePWMResource(handle);

    /* If the pin was mapped in open, restore to PMAP_NONE */
    if (PinConfigValue(hwAttrs->pwmPin) != 0) {

        mapPin(PinConfigPort(hwAttrs->pwmPin), (hwAttrs->pwmPin) & 0x7,
            PMAP_NONE);
    }

    DebugP_log1("PWM:(%p) closed", (uintptr_t) handle);
}

/*
 *  ======== PWMTimerMSP432_control ========
 *  @pre    Function assumes that the handle is not NULL
 */
int_fast16_t PWMTimerMSP432_control(PWM_Handle handle, uint_fast16_t cmd, void *arg)
{
    /* No implementation yet */
    return (PWM_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== PWMTimerMSP432_init ========
 *  @pre    Function assumes that the handle is not NULL
 */
void PWMTimerMSP432_init(PWM_Handle handle)
{
}

/*
 *  ======== PWMTimerMSP432_open ========
 *  @pre    Function assumes that the handle is not NULL
 */
PWM_Handle PWMTimerMSP432_open(PWM_Handle handle, PWM_Params *params)
{
    PWMTimerMSP432_Object          *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    PowerMSP432_Freqs               powerFreqs;
    uint32_t                        periodCounts, dutyCounts;
    uint32_t                        clockFreq;
    uint16_t                        port, pin, value, pinmask;
    uint8_t                         structIndex, prescalar;

    if (PinConfigCompareRegister(hwAttrs->pwmPin) ==
        TIMER_A_CAPTURECOMPARE_REGISTER_0) {

        DebugP_log1("PWM:(%p) Cannot use COMPARE_REGISTER_0 to generate PWM.",
            (uintptr_t) handle);

        return (NULL);
    }

    if (hwAttrs->clockSource != TIMER_A_CLOCKSOURCE_ACLK &&
        hwAttrs->clockSource != TIMER_A_CLOCKSOURCE_SMCLK) {

        DebugP_log1("PWM:(%p) Unsupported PWM clock source.",
            (uintptr_t) handle);

        return (NULL);
    }

    if ((hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_ACLK) &&
        ((params->periodUnits == PWM_PERIOD_US) ||
            (params->dutyUnits == PWM_DUTY_US))) {

        DebugP_log1("PWM:(%p) Microseconds units unsupported with ACLK source.",
            (uintptr_t) handle);

        return (NULL);
    }

    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);

    clockFreq = (hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_SMCLK) ?
        powerFreqs.SMCLK : powerFreqs.ACLK;

    periodCounts = getPeriodCounts(params->periodUnits, params->periodValue,
        clockFreq);
    dutyCounts = getDutyCounts(params->dutyUnits, params->dutyValue, periodCounts,
        clockFreq);
    prescalar = calculatePrescalar(periodCounts);

    if (corroboratePeriod(handle, periodCounts, prescalar) != PWM_STATUS_SUCCESS) {
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        return (NULL);
    }

    if (corroborateDuty(handle, periodCounts, dutyCounts) != PWM_STATUS_SUCCESS) {
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        return (NULL);
    }

    /* Assign corresponding status structure to the PWM instance */
    structIndex = PinConfigTimerId(hwAttrs->pwmPin);
    object->timerStatus = &(pwmTimerStatus[structIndex]);
    object->baseAddress = pwmTimerBaseAddr[structIndex];
    object->compareOutputNum = (PinConfigCompareRegister(hwAttrs->pwmPin) / 2) - 2;

    if (allocatePWMResource(handle, params->periodUnits, params->periodValue,
        periodCounts, dutyCounts, prescalar) == false) {
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        return (NULL);
    }

    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    /* Store PWM configuration */
    object->dutyUnits = params->dutyUnits;
    object->idleLevel = params->idleLevel;

    /* Map the pin, only if its a mappable pin. */
    value = PinConfigValue(hwAttrs->pwmPin);
    port = PinConfigPort(hwAttrs->pwmPin);
    pin = (hwAttrs->pwmPin) & 0x7;
    pinmask = 1 << pin;

    if (value != 0) {
        mapPin(port, pin, value);
    }

    /* Set the idleLevel */
    MAP_GPIO_setAsOutputPin(port, pinmask);
    MAP_GPIO_setDriveStrengthHigh(port, pinmask);

    if (object->idleLevel) {
        MAP_GPIO_setOutputHighOnPin(port, pinmask);
    }
    else {
        MAP_GPIO_setOutputLowOnPin(port, pinmask);
    }

    /* Initialize the peripheral & set the period & duty */
    initHw(handle);

    DebugP_log3("PWM:(%p) opened; period set to: %d; duty set to: %d",
        (uintptr_t) handle, params->periodValue, params->dutyValue);

    return (handle);
}

/*
 *  ======== PWMTimerMSP432_setDuty ========
 *  @pre    Function assumes that handle is not NULL
 */
int_fast16_t PWMTimerMSP432_setDuty(PWM_Handle handle, uint32_t dutyValue)
{
    PWMTimerMSP432_Object          *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    PowerMSP432_Freqs               powerFreqs;
    uintptr_t                       key;
    int32_t                         result;
    uint32_t                        period, duty, clockFreq;

    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);
    clockFreq = (hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_SMCLK) ?
        powerFreqs.SMCLK : powerFreqs.ACLK;

    key = HwiP_disable();

    period = object->timerStatus->periodCounts;
    duty = getDutyCounts(object->dutyUnits, dutyValue, period, clockFreq);
    result = corroborateDuty(handle, period, duty);

    if (result != PWM_STATUS_SUCCESS) {
        HwiP_restore(key);

        return (result);
    }

    /*
     * Set & store the new duty.  IMPORTANT: this must be saved before the
     * duty is divided by the prescalar & the duty = period corner case.
     */
    object->timerStatus->duties[object->compareOutputNum] = duty;

    duty /= object->timerStatus->prescalar;
    period /= object->timerStatus->prescalar;

    /*
     * This condition ensures that the output will remain active if the duty
     * is equal to the period.
     */
    if (duty == period) {
        duty++;
    }

    MAP_Timer_A_setCompareValue(object->baseAddress,
        PinConfigCompareRegister(hwAttrs->pwmPin), duty);

    HwiP_restore(key);

    DebugP_log2("PWM:(%p) duty set to: %d", (uintptr_t) handle, duty);

    return (PWM_STATUS_SUCCESS);
}

/*
 *  ======== PWMTimerMSP432_setPeriod ========
 *  @pre    Function assumes that handle is not NULL
 */
int_fast16_t PWMTimerMSP432_setPeriod(PWM_Handle handle, uint32_t periodValue)
{
    uintptr_t                       key;
    int8_t                          i;
    uint8_t                         prescalar;
    int32_t                         result;
    uint32_t                        periodCounts;
    uint32_t                        clockFreq;
    Timer_A_PWMConfig               pwmConfig;
    PowerMSP432_Freqs               powerFreqs;
    PWMTimerMSP432_Object          *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;

    PowerMSP432_getFreqs(Power_getPerformanceLevel(), &powerFreqs);
    clockFreq = (hwAttrs->clockSource == TIMER_A_CLOCKSOURCE_SMCLK) ?
        powerFreqs.SMCLK : powerFreqs.ACLK;

    key = HwiP_disable();

    periodCounts = getPeriodCounts(object->timerStatus->periodUnits, periodValue,
        clockFreq);
    prescalar = calculatePrescalar(periodCounts);
    result = corroboratePeriod(handle, periodCounts, prescalar);

    if (result != PWM_STATUS_SUCCESS) {
        HwiP_restore(key);

        return (result);
    }

    /* If more than one PWM is active on this->timerPeripheral */
    if (object->timerStatus->openMask != (1 << object->compareOutputNum)) {

        /* Ensure the new period can be generated with the current prescalar */
        if (prescalar != object->timerStatus->prescalar) {
            HwiP_restore(key);

            DebugP_log1("PWM:(%p) period is out of range.", (uintptr_t) handle);

            return (PWM_STATUS_INVALID_PERIOD);
        }

        /*
         * Due to Timer_A peripherals generating multiple PWM outputs on a single
         * timer, we need to ensure the new period is greater than all the duties
         * currently set on the timer peripheral.
         */
        for (i = 0; i < PWMTimerMSP432_NUM_PWM_OUTPUTS; i++) {

            if (object->timerStatus->duties[i] &&
                    (periodCounts <= object->timerStatus->duties[i])) {

                HwiP_restore(key);
                DebugP_log1("PWM:(%p) period is out of range.", (uintptr_t) handle);

                return (PWM_STATUS_INVALID_PERIOD);
            }
        }
    }
    else {
        if (periodCounts <= object->timerStatus->duties[object->compareOutputNum]) {

            HwiP_restore(key);
            DebugP_log1("PWM:(%p) period is out of range.", (uintptr_t) handle);

            return (PWM_STATUS_INVALID_PERIOD);
        }
    }

    /* Set new power performance constraints based on new period */
    releaseConstraints(handle);
    setConstraints(handle, object->timerStatus->periodUnits, periodValue);

    /* Update static object */
    object->timerStatus->prescalar = prescalar;
    object->timerStatus->periodCounts = periodCounts;
    object->timerStatus->period = periodValue;

    pwmConfig.clockSource = hwAttrs->clockSource;
    pwmConfig.clockSourceDivider = prescalar;
    pwmConfig.timerPeriod = (periodCounts / prescalar);
    pwmConfig.compareRegister = PinConfigCompareRegister(hwAttrs->pwmPin);
    pwmConfig.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    pwmConfig.dutyCycle = ((object->timerStatus->duties[object->compareOutputNum]) / prescalar);

    MAP_Timer_A_generatePWM(object->baseAddress, &pwmConfig);

    HwiP_restore(key);

    DebugP_log2("PWM:(%p) period set to: %d", (uintptr_t) handle, periodValue);

    return (PWM_STATUS_SUCCESS);
}

/*
 *  ======== PWMTimerMSP432_start ========
 *  @pre    Function assumes that handle is not NULL
 */
void PWMTimerMSP432_start(PWM_Handle handle)
{
    uintptr_t                       key;
    PWMTimerMSP432_Object          *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    uint16_t                        pinmask;
    uint16_t                        port;
    uint16_t                        moduleFunction;

    key = HwiP_disable();

    /* If this PWM CCR output is not active */
    if ((object->timerStatus->activeOutputsMask &
        (1 << object->compareOutputNum)) == 0) {

        object->timerStatus->activeOutputsMask |=
            (1 << object->compareOutputNum);

        /* If this is the only active PWM on this Timer_A peripheral */
        if (object->timerStatus->activeOutputsMask ==
            (1 << object->compareOutputNum)) {

            MAP_Timer_A_startCounter(object->baseAddress, TIMER_A_UP_MODE);
        }

        port = PinConfigPort(hwAttrs->pwmPin);
        moduleFunction = (PinConfigValue(hwAttrs->pwmPin) == 0) ?
            PinConfigModuleFunction(hwAttrs->pwmPin) :
            GPIO_PRIMARY_MODULE_FUNCTION;
        pinmask  = PinConfigPin(hwAttrs->pwmPin);

        MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port, pinmask,
            moduleFunction);

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        DebugP_log1("PWM:(%p) started.", (uintptr_t) handle);
    }

    HwiP_restore(key);

}

/*
 *  ======== PWMTimerMSP432_stop ========
 *  @pre    Function assumes that handle is not NULL
 */
void PWMTimerMSP432_stop(PWM_Handle handle)
{
    uintptr_t                       key;
    PWMTimerMSP432_Object          *object = handle->object;
    PWMTimerMSP432_HWAttrsV2 const *hwAttrs = handle->hwAttrs;
    uint16_t                        pinmask;
    uint16_t                        port;

    pinmask = PinConfigPin(hwAttrs->pwmPin);
    port = PinConfigPort(hwAttrs->pwmPin);

    key = HwiP_disable();

    if ((object->timerStatus->activeOutputsMask &
            (1 << object->compareOutputNum))) {

        object->timerStatus->activeOutputsMask &=
                ~(1 << object->compareOutputNum);

        /* If all PWMs stopped */
        if (object->timerStatus->activeOutputsMask == 0x00) {
            MAP_Timer_A_stopTimer(object->baseAddress);
        }

        /* Set idleLevel */
        MAP_GPIO_setAsOutputPin(port, pinmask);
        MAP_GPIO_setDriveStrengthHigh(port, pinmask);

        if (object->idleLevel) {
            MAP_GPIO_setOutputHighOnPin(port, pinmask);
        }
        else {
            MAP_GPIO_setOutputLowOnPin(port, pinmask);
        }

#if DeviceFamily_ID == DeviceFamily_ID_MSP432P401x
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
#endif
        Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

        DebugP_log1("PWM:(%p) stopped.", (uintptr_t) handle);
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
