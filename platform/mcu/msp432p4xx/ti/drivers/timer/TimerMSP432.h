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
/*!****************************************************************************
 *  @file       TimerMSP432.h
 *  @brief      Timer driver interface for MSP432 devices
 *
 *  # Operation #
 *  This driver implements two independent hardware peripherals on the
 *  MSP432: Timer_A and Timer32. The Timer_A implementation supports two clock
 *  sources, SMCLK and ACLCK, with multiple clock dividers. The Timer32
 *  implementation supports a single clock source, MCLK, with 3 clock
 *  dividers. For each driver instance, the configuration of the clock dividers
 *  is determined using the clock source, period, and period units. Note that
 *  the SMCLK and MCLK frequencies can change after Timer_open().
 *
 *  ### MPS432 Timer Driver Configuration #
 *
 *  In order to use the Timer APIs, the application is required
 *  to define 4 configuration items in the application Board.c file:
 *
 *  1.  An array of TimerMSP432_Object elements, which will be used by
 *  by the driver to maintain instance state.
 *  Below is an example TimerMSP432_Object array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *    #include <ti/drivers/Timer.h>
 *    #include <ti/drivers/timer/TimerMSP432.h>
 *
 *    TimerMSP432_Object timerMSP432Objects[3];
 *  @endcode
 *
 *  2.  An array of TimerMSP432_HWAttrs elements that defines which
 *
 *  Below is an example TimerMSP432_HWAttrs array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *  const TimerMSP432_HWAttrs timerMSP432HWAttrs[3] =
 *  {
 *      {
 *          .timerBaseAddress = TIMER32_0_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .intNum = INT_T32_INT1,
 *          .intPriority = ~0,
 *      },
 *      {
 *          .timerBaseAddress = TIMER_A1_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .intNum = INT_TA1_0,
 *          .intPriority = ~0,
 *      },
 *      {
 *          .timerBaseAddress = TIMER_A2_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
 *          .intNum = INT_TA2_0,
 *          .intPriority = ~0,
 *      },
 *  };
 *  @endcode
 *
 *  3.  An array of @ref Timer_Config elements, one for each Timer instance.
 *  Each element of this array identifies the device-specific API function
 *  table, the device specific timer object instance, and the device specific
 *  Hardware Attributes to be used for each timer instance.
 *  Below is an example @ref Timer_Config array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *    const Timer_Config Timer_config[3] = {
 *      {
 *          .fxnTablePtr = &TimerMSP432_Timer32_fxnTable,
 *          .object = &timerMSP432Objects[0],
 *          .hwAttrs = &timerMSP432HWAttrs[0]
 *      },
 *      {
 *          .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
 *          .object = &timerMSP432Objects[1],
 *          .hwAttrs = &timerMSP432HWAttrs[1]
 *      }
 *      {
 *          .fxnTablePtr = &TimerMSP432_Timer_A_fxnTable,
 *          .object = &timerMSP432Objects[2],
 *          .hwAttrs = &timerMSP432HWAttrs[2]
 *      }
 *    };
 *  @endcode
 *
 *  4.  A global variable, Timer_count, that informs the driver how many
 *  timer instances are defined:
 *  @code
 *    const uint_least8_t Timer_count = 3;
 *  @endcode
 *
 *  # Resource Allocation #
 *  Allocation of each timer peripheral is managed through a set of resource
 *  allocation APIs. For example, the TimerMSP432_allocateTimerResource API
 *  will allocate a timer for exclusive use. Any attempt to allocate this
 *  resource in the future will result in a false value being returned from the
 *  allocation API. To free a timer resource, the TimerMSP432_freeTimerResource
 *  is used. The application is not responsible for calling these allocation
 *  APIs directly.
 *
 * ### Power Management #
 * The TI-RTOS power management framework will try to put the device into the
 * most power efficient mode whenever possible. Please see the technical
 * reference manual for further details on each power mode.
 *
 * This driver supports dynamic power performance levels. The driver will
 * determine which power performance levels are compatible with the desired
 * period and period units. The driver prevents transitions to performance
 * levels in which the period cannot be generated.
 *
 * After a performance level change, the period is recalculated such that the
 * generated period will remain the same. The exact period may vary after a
 * performance level transition. This is due to a change in clock frequency and
 * hence period per clock cycle.
 *
 *  The following statements are valid:
 *    - After Timer_open(): Clocks are enabled to the timer resource.
 *      Dynamic performance level changes are allowed.
 *    - After Timer_start(): DEEPSLEEP mode is disabled. The device can only go
 *      to Idle power mode. Performance level changes are not allowed.
 *    - After Timer_stop(): Conditions are equal as for after Timer_open.
 *    - After Timer_close(): The underlying GPTimer is turned off, and the
 *      clock to the timer and pin are disabled.
 ******************************************************************************
 */
#ifndef ti_drivers_timer_TimerMSP432__include
#define ti_drivers_timer_TimerMSP432__include

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

extern const Timer_FxnTable TimerMSP432_Timer_A_fxnTable;
extern const Timer_FxnTable TimerMSP432_Timer32_fxnTable;

/*!
 *  @brief TimerMSP432 Hardware Attributes
 *
 *  Timer hardware attributes that tell the TimerMSP432 driver specific hardware
 *  configurations and interrupt priority settings.
 *
 *  The Timer32 only supports MCLK as a clock source. As a result, the
 *  clockSource field has no event for Timer32 instances.
 *
 *  A sample structure is shown below:
 *  @code
 *  const TimerMSP432_HWAttrs timerMSP432HWAttrs[] =
 *  {
 *      {
 *          .timerBaseAddress = TIMER32_0_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .intNum = INT_T32_INT1,
 *          .intPriority = ~0,
 *      },
 *      {
 *          .timerBaseAddress = TIMER_A1_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .intNum = INT_TA1_0,
 *          .intPriority = ~0
 *      },
 *      {
 *          .timerBaseAddress = TIMER_A2_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
 *          .intNum = INT_TA2_0,
 *          .intPriority = ~0
 *      },
 *  };
 *  @endcode
 */
typedef struct TimerMSP432_HWAttr_ {
    /*! The base address of the timer peripheral. */
    uint32_t timerBaseAddress;

    /*! Specifies the clock source for the timer peripheral. */
    uint32_t clockSource;

    /*! The hardware interrupt number for the timer peripheral. */
    uint32_t intNum;

    /*! The interrupt priority. */
    uint32_t intPriority;
} TimerMSP432_HWAttrs;

/*!
 *  @brief TimerMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct TimerMSP432_Object_ {
    HwiP_Handle         hwiHandle;
    SemaphoreP_Handle   timerSem;
    Power_NotifyObj     perfChangeNotify;
    uint32_t            perfConstraintMask;
    Timer_CallBackFxn   callBack;
    Timer_Mode          mode;
    Timer_PeriodUnits   units;
    uint32_t            rawPeriod;
    uint32_t            period;
    bool                isRunning;
} TimerMSP432_Object;

/*!
 *  @brief  Function to allocate a timer peripheral.
 *
 *  This function is intended to be used by any driver which implements a
 *  timer hardware peripheral.
 *
 *  @param  baseAddress The base address of a timer hardware peripheral.
 *
 *  @return A bool returning true if the timer resource was successfully
 *          allocated. If the base address is not valid or if the resource is
 *          not available, false is returned.
 *
 *  @sa     TimerMSP432_freeTimerResource()
 */
extern bool TimerMSP432_allocateTimerResource(uint32_t baseAddress);

/*!
 *  @brief  Function to de-allocate a timer peripheral.
 *
 *  This function is intended to be used by any driver which implements a
 *  timer hardware peripheral. Calling this function will release power
 *  constraints set in TimerMSP432_allocateTimerResource();
 *
 *  @pre    A successful call to TimerMSP432_allocateTimerResource() using the
 *          baseAddress must have been made prior to calling this API.
 *
 *  @param  baseAddress The base address of a timer hardware peripheral.
 *
 *  @sa     TimerMSP432_allocateTimerResource()
 */
extern void TimerMSP432_freeTimerResource(uint32_t baseAddress);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_timer_TimerMSP432__include */
