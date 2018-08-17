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

#include <stdint.h>
#include <stdlib.h>

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
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogMSP432.h>

/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/wdt_a.h>

/* Function prototypes */
void WatchdogMSP432_clear(Watchdog_Handle handle);
void WatchdogMSP432_close(Watchdog_Handle handle);
int_fast16_t WatchdogMSP432_control(Watchdog_Handle handle, uint_fast16_t cmd,
        void *arg);
void WatchdogMSP432_init(Watchdog_Handle handle);
Watchdog_Handle WatchdogMSP432_open(Watchdog_Handle handle,
    Watchdog_Params *params);
int_fast16_t WatchdogMSP432_setReload(Watchdog_Handle handle, uint32_t value);
uint32_t WatchdogMSP432_convertMsToTicks(Watchdog_Handle handle,
    uint32_t milliseconds);

/* Watchdog function table for MSP432 implementation */
const Watchdog_FxnTable WatchdogMSP432_fxnTable = {
    WatchdogMSP432_clear,
    WatchdogMSP432_close,
    WatchdogMSP432_control,
    WatchdogMSP432_init,
    WatchdogMSP432_open,
    WatchdogMSP432_setReload,
    WatchdogMSP432_convertMsToTicks
};

/*
 *  ======== WatchdogMSP432_clear ========
 */
void WatchdogMSP432_clear(Watchdog_Handle handle)
{
    MAP_WDT_A_clearTimer();
}

/*
 *  ======== WatchdogMSP432_close ========
 */
void WatchdogMSP432_close(Watchdog_Handle handle)
{
    WatchdogMSP432_Object        *object = handle->object;
    WatchdogMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;

    MAP_WDT_A_holdTimer();

    /* Destroy the SYS/BIOS objects. */
    if (object->hwiHandle) {
        HwiP_delete(object->hwiHandle);
    }

    /* Remove power constraints */
    if (object->resetMode == Watchdog_RESET_ON ||
        hwAttrs->clockSource == WDT_A_CLOCKSOURCE_SMCLK ||
        hwAttrs->clockSource == WDT_A_CLOCKSOURCE_ACLK) {
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
    }
    else {
        Power_releaseConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_1);
    }

    object->isOpen = false;
}

/*
 *  ======== WatchdogMSP432_control ========
 *  @pre    Function assumes that the handle is not NULL
 */
int_fast16_t WatchdogMSP432_control(Watchdog_Handle handle, uint_fast16_t cmd,
        void *arg)
{
    /* No implementation yet */
    return (Watchdog_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== WatchdogMSP432_init ========
 */
void WatchdogMSP432_init(Watchdog_Handle handle)
{
    WatchdogMSP432_Object *object = handle->object;

    object->isOpen = false;
}

/*
 *  ======== WatchdogMSP432_open ========
 */
Watchdog_Handle WatchdogMSP432_open(Watchdog_Handle handle,
    Watchdog_Params *params)
{
    uintptr_t                     key;
    HwiP_Params                   hwiParams;
    WatchdogMSP432_Object        *object = handle->object;
    WatchdogMSP432_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Ensure a callbackFxn is specified if using watchdog in interval mode */
    DebugP_assert((params->resetMode == Watchdog_RESET_ON) ||
        (params->callbackFxn != NULL));

    /* Don't allow preemption */
    key = HwiP_disable();

    /* Check if the Watchdog is open already with the HWAttrs */
    if (object->isOpen) {
        HwiP_restore(key);
        DebugP_log1("Watchdog: Handle %x already in use.", (uintptr_t) handle);
        return (NULL);
    }
    object->isOpen = true;
    HwiP_restore(key);

    /*
     * Add power management support - Disable performance transitions while
     * opening the driver.
     */
    Power_setConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
    if (params->resetMode == Watchdog_RESET_ON ||
        hwAttrs->clockSource == WDT_A_CLOCKSOURCE_SMCLK ||
        hwAttrs->clockSource == WDT_A_CLOCKSOURCE_ACLK) {
        /*
         * Do not support power modes lower than LPM0 if in watchdog mode or
         * in interval mode and using SMCLK or ACLK as clock sources.
         * Additionally, LPM3.5 cannot be reached.
         */
        Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_0);
    }
    else {
        Power_setConstraint(PowerMSP432_DISALLOW_DEEPSLEEP_1);
    }

    /* Construct Hwi object for watchdog */
    if (params->callbackFxn) {
        HwiP_Params_init(&hwiParams);
        hwiParams.arg = (uintptr_t) handle;
        hwiParams.priority = hwAttrs->intPriority;
        object->hwiHandle = HwiP_create(hwAttrs->intNum, params->callbackFxn,
                               &hwiParams);
        if (!object->hwiHandle) {
            DebugP_log0("Watchdog: HwiP_create() failed");
            Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);
            WatchdogMSP432_close(handle);
            return (NULL);
        }
    }

    /* Configure and initialize WDT */
    object->resetMode = params->resetMode;
    if (object->resetMode == Watchdog_RESET_ON) {
        MAP_WDT_A_initWatchdogTimer(hwAttrs->clockSource, hwAttrs->clockDivider);
    }
    else {
        MAP_WDT_A_initIntervalTimer(hwAttrs->clockSource, hwAttrs->clockDivider);
    }
    MAP_WDT_A_startTimer();

    /* Allow performance level changes  */
    Power_releaseConstraint(PowerMSP432_DISALLOW_PERF_CHANGES);

    DebugP_log1("Watchdog: (0x%x) opened and enabled.", (uintptr_t) handle);

    /* Return handle of the Watchdog object */
    return (handle);
}

/*
 *  ======== WatchdogMSP432_setReload ========
 */
int_fast16_t WatchdogMSP432_setReload(Watchdog_Handle handle, uint32_t value)
{
    /* Not supported for MSP432 */
    DebugP_assert(false);
    return (Watchdog_STATUS_UNSUPPORTED);
}

/*
 *  ======== WatchdogMSP432_convertMsToTicks ========
 */
uint32_t WatchdogMSP432_convertMsToTicks(Watchdog_Handle handle,
    uint32_t milliseconds)
{
    /* Not supported for MSP432 */
    return (0);
}
