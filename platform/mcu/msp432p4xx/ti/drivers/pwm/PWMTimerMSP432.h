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
/*!****************************************************************************
 * @file       PWMTimerMSP432.h
 *
 * @brief      PWM driver implementation using MSP432 Timer_A peripherals.
 *
 * The PWM header file should be included in an application as follows:
 * @code
 * #include <ti/drivers/PWM.h>
 * #include <ti/drivers/pwm/PWMTimerMSP432.h>
 * @endcode
 *
 * *  Refer to @ref PWM.h for a complete description of APIs & example of use.
 *
 * ## Overview #
 * This driver configures a MSP432 Timer_A peripheral for PWM. If the
 * timer is already in use (by the kernel for instance), PWM instances will
 * not be opened.
 *
 * When used for PWM generation, each Timer_A can produce up to 6 PWM outputs
 * and this driver manages each output as an independent PWM instance. However
 * since a single period, prescalar, and clock source are used for all PWM
 * outputs, there are limitations in place to ensure proper operation:
 *     * The PWM period and prescalar are calculated and set based on the first
 *       instance opened. Opening a second instance will fail if the period
 *       is not the same as what was set by the first instance. The PWM period
 *       unit and clock source are set based on the first
 *       instance opened. Opening a second instance will fail if either
 *       are not the same.
 *     * PWM_setPeriod() will fail if the calculated period is less than
 *       any calculated duty currently open on the Timer_A peripheral.
 *     * PWM_setPeriod() will fail if the calculated prescalar is not
 *       equal to the prescalar currently set on the Timer_A peripheral.
 *
 * The timer is automatically configured in count-up mode using the clock
 * source specified in the hwAttrs structure.  In PWM mode, the timer
 * capture/compare register 0 is used as the period register and cannot be
 * used to generate a PWM output.
 *
 * The period and duty registers are 16 bits wide, thus a prescalar is used to
 * divide the input clock and allow for larger periods.  The maximum period
 * supported is calculated as:
 *     - MAX_PERIOD = (MAX_PRESCALAR * MAX_MATCH_VALUE) / CYCLES_PER_US
 *     - Ex:
 *           - 12 MHz clock: (64 * 65535) / 12 = 349520 microseconds
 *           - 6 MHz clock: (64 * 65535) / 6 = 699040 microseconds
 *           - 3 MHz clock: (64 * 65535) / 3 = 1398080 microseconds
 *
 * After opening, the PWM_setPeriod() API can be used to change the PWM period.
 * Keep in mind the period is shared by all other PWMs on the timer, so all
 * other PWM outputs on the timer will change.
 *
 *  ### MPS432 PWM Driver Configuration #
 *
 *  In order to use the PWM APIs, the application is required
 *  to define 4 configuration items in the application Board.c file:
 *
 *  1.  An array of PWMTimerMSP432_Object elements, which will be used by
 *  by the driver to maintain instance state.
 *  Below is an example PWMTimerMSP432_Object array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *    #include <ti/drivers/PWM.h>
 *    #include <ti/drivers/pwm/PWMTimerMSP432.h>
 *
 *    PWMTimerMSP432_Object pwmTimerMSP432Objects[2];
 *  @endcode
 *
 *  2.  An array of PWMTimerMSP432_HWAttrsV2 elements that defines which
 *  pin will be used by the corresponding PWM instance
 *  (see @ref pwmPinIdentifiersMSP432).
 *  Below is an example PWMTimerMSP432_HWAttrsV2 array appropriate for the
 *  MSP432 Launchpad board:
 *  @code
 *  const PWMTimerMSP432_HWAttrsV2 pwmTimerMSP432HWAttrs[2] = {
 *      {
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .pwmPin = PWMTimerMSP432_P2_1_TA1CCR1A
 *      },
 *      {
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .pwmPin = PWMTimerMSP432_P2_2_TA1CCR2A
 *      }
 *  };
 *  @endcode
 *
 *  3.  An array of @ref PWM_Config elements, one for each PWM instance. Each
 *  element of this array identifies the device-specific API function table,
 *  the device specific PWM object instance, and the device specific Hardware
 *  Attributes to be used for each PWM channel.
 *  Below is an example @ref PWM_Config array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *    const PWM_Config PWM_config[2] = {
 *      {
 *          .fxnTablePtr = &PWMTimerMSP432_fxnTable,
 *          .object = &pwmTimerMSP432Objects[0],
 *          .hwAttrs = &pwmTimerMSP432HWAttrs[0]
 *      },
 *      {
 *          .fxnTablePtr = &PWMTimerMSP432_fxnTable,
 *          .object = &pwmTimerMSP432Objects[1],
 *          .hwAttrs = &pwmTimerMSP432HWAttrs[1]
 *      }
 *    };
 *  @endcode
 *
 *  4.  A global variable, PWM_count, that informs the driver how many PWM
 *  instances are defined:
 *  @code
 *    const uint_least8_t PWM_count = 2;
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
 * levels in which the period cannot be generated. When PWM_setPeriod() is
 * called, the compatible performance levels are re-calculated and set.
 *
 * After a performance level change, the period is recalculated such that the
 * generated period will remain the same. The duty cycle is not recalculated.
 * The application is responsible for calling PWM_setDuty() after a performance
 * level change. The exact period may vary after a performance level transition.
 * This is due to a change in clock frequency and hence period per clock cycle.
 *
 *  @code
 *    PWM_stop(pwmHandle);
 *    // Change performance level code
 *    PWM_setDuty(pwmHandle, duty);
 *    PWM_start(pwmHandle);
 *  @endcode
 *
 *  The following statements are valid:
 *    - After PWM_open(): Clocks are enabled to the timer resource and the
 *      configured pwmPin. Dynamic performance level changes are allowed.
 *    - After PWM_start(): DEEPSLEEP mode is disabled. The device can only go
 *      to Idle power mode since the high-frequency clock is needed for PWM
 *      operation. Performance level changes are not allowed.
 *    - After PWM_stop():  Conditions are equal as for after PWM_open().
 *    - After PWM_close(): The underlying GPTimer is turned off, and the clock
 *      to the timer and pin are disabled.
 *
 ******************************************************************************
 */

#ifndef ti_driver_pwm_PWMTimerMSP432__include
#define ti_driver_pwm_PWMTimerMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/PWM.h>

#include <ti/devices/msp432p4xx/driverlib/pmap.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>

/*! \cond */
/*
 *  PWMTimer port/pin defines for pin configuration.  Ports P2, P3, and P7 are
 *  configurable through the port mapping controller.
 *  Value specifies the pin function and ranges from 0 to 31
 *  pin range: 0 - 7, port range: 0 - 15.
 *  The timer id (0, 1, 2, 3, or 4) is stored in bits 16 - 19
 *  The capture compare register is stored in bits 20 - 23.
 *
 *
 *    23 - 20   19 - 16   15 - 10   9  8  7 - 4  3 - 0
 *  ---------------------------------------------------
 *  |   CCR  | Timer id  |  VALUE | X | X | PORT | PIN |
 *  ---------------------------------------------------
 *
 *  value = pinConfig >> 10
 *  port = (pinConfig >> 4) & 0xf
 *  pin = pinConfig & 0x7
 *  ccr = (pinConfig >> 16) & 0xf
 *
 *  pmap = port * 0x8;  // 2 -> 0x10, 3 -> 0x18, 7 -> 0x38
 *  portMapReconfigure = PMAP_ENABLE_RECONFIGURATION;
 *
 *  Code from pmap.c:
 *   //Get write-access to port mapping registers:
 *   PMAP->KEYID = PMAP_KEYID_VAL;
 *
 *   //Enable/Disable reconfiguration during runtime
 *   PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | portMapReconfigure;
 *   HWREG8(PMAP_BASE + pin + pmap) = value;
 *
 *  For non-configurable ports (bits 20 - 12 will be 0).
 *  Bits 8 and 9 hold the module function (PRIMARY, SECONDARY, or
 *  TERTIALRY).
 *
 *  23 - 20   19 - 16    15 - 10     9          8       7 - 4  3 - 0
 *  -----------------------------------------------------------------
 *  | CCR |  Timer id  |    0    | PnSEL1.x | PnSEL0.x | PORT | PIN |
 *  -----------------------------------------------------------------
 *
 *  moduleFunction = (pinConfig >> 8) & 0x3
 *  port = (pinConfig >> 4) & 0xf
 *  pin = 1 << (pinConfig & 0xf)
 *
 *  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port,
 *       pin, moduleFunction);
 *
 */

#define PWMTimerMSP432_CCR1 (TIMER_A_CAPTURECOMPARE_REGISTER_1 << 20)
#define PWMTimerMSP432_CCR2 (TIMER_A_CAPTURECOMPARE_REGISTER_2 << 20)
#define PWMTimerMSP432_CCR3 (TIMER_A_CAPTURECOMPARE_REGISTER_3 << 20)
#define PWMTimerMSP432_CCR4 (TIMER_A_CAPTURECOMPARE_REGISTER_4 << 20)

#define PWMTimerMSP432_TA0  (0 << 16)
#define PWMTimerMSP432_TA1  (1 << 16)
#define PWMTimerMSP432_TA2  (2 << 16)
#define PWMTimerMSP432_TA3  (3 << 16)

#define PWMTimerMSP432_TA0CCR1 (PWMTimerMSP432_TA0 | PWMTimerMSP432_CCR1 | (PMAP_TA0CCR1A << 10))
#define PWMTimerMSP432_TA0CCR2 (PWMTimerMSP432_TA0 | PWMTimerMSP432_CCR2 | (PMAP_TA0CCR2A << 10))
#define PWMTimerMSP432_TA0CCR3 (PWMTimerMSP432_TA0 | PWMTimerMSP432_CCR3 | (PMAP_TA0CCR3A << 10))
#define PWMTimerMSP432_TA0CCR4 (PWMTimerMSP432_TA0 | PWMTimerMSP432_CCR4 | (PMAP_TA0CCR4A << 10))

#define PWMTimerMSP432_TA1CCR1 (PWMTimerMSP432_TA1 | PWMTimerMSP432_CCR1 | (PMAP_TA1CCR1A << 10))
#define PWMTimerMSP432_TA1CCR2 (PWMTimerMSP432_TA1 | PWMTimerMSP432_CCR2 | (PMAP_TA1CCR2A << 10))
#define PWMTimerMSP432_TA1CCR3 (PWMTimerMSP432_TA1 | PWMTimerMSP432_CCR3 | (PMAP_TA1CCR3A << 10))
#define PWMTimerMSP432_TA1CCR4 (PWMTimerMSP432_TA1 | PWMTimerMSP432_CCR4 | (PMAP_TA1CCR4A << 10))

#define PWMTimerMSP432_TA2CCR1 (PWMTimerMSP432_TA2 | PWMTimerMSP432_CCR1)
#define PWMTimerMSP432_TA2CCR2 (PWMTimerMSP432_TA2 | PWMTimerMSP432_CCR2)
#define PWMTimerMSP432_TA2CCR3 (PWMTimerMSP432_TA2 | PWMTimerMSP432_CCR3)
#define PWMTimerMSP432_TA2CCR4 (PWMTimerMSP432_TA2 | PWMTimerMSP432_CCR4)

#define PWMTimerMSP432_TA3CCR1 (PWMTimerMSP432_TA3 | PWMTimerMSP432_CCR1)
#define PWMTimerMSP432_TA3CCR2 (PWMTimerMSP432_TA3 | PWMTimerMSP432_CCR2)
#define PWMTimerMSP432_TA3CCR3 (PWMTimerMSP432_TA3 | PWMTimerMSP432_CCR3)
#define PWMTimerMSP432_TA3CCR4 (PWMTimerMSP432_TA3 | PWMTimerMSP432_CCR4)

/*! \endcond */

/*!
 *  \defgroup pwmPinIdentifiersMSP432 PWMTimerMSP432_HWAttrs 'pwmPin' field options
 *  @{
 */
/*!
 *  @name Port 2, Pin 0 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_0_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x20) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_0_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x20) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_0_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x20) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_0_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x20) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_0_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x20) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_0_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x20) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_0_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x20) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_0_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x20) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 2, Pin 1 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_1_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x21) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_1_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x21) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_1_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x21) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_1_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x21) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_1_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x21) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_1_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x21) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_1_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x21) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_1_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x21) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 2, Pin 2 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_2_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x22) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_2_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x22) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_2_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x22) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_2_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x22) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_2_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x22) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_2_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x22) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_2_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x22) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_2_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x22) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 2, Pin 3 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_3_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x23) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_3_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x23) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_3_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x23) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_3_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x23) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_3_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x23) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_3_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x23) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_3_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x23) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_3_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x23) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 2, Pin 4 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_4_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x24) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_4_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x24) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_4_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x24) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_4_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x24) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_4_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x24) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_4_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x24) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_4_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x24) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_4_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x24) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 2, Pin 5 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_5_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x25) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_5_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x25) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_5_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x25) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_5_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x25) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_5_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x25) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_5_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x25) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_5_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x25) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_5_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x25) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 2, Pin 6 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_6_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x26) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_6_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x26) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_6_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x26) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_6_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x26) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_6_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x26) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_6_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x26) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_6_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x26) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_6_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x26) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 2, Pin 7 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P2_7_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x27) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_7_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x27) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_7_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x27) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_7_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x27) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_7_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x27) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_7_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x27) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_7_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x27) /*!< @hideinitializer */
#define PWMTimerMSP432_P2_7_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x27) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 0 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_0_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x30) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_0_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x30) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_0_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x30) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_0_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x30) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_0_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x30) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_0_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x30) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_0_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x30) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_0_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x30) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 1 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_1_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x31) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_1_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x31) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_1_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x31) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_1_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x31) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_1_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x31) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_1_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x31) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_1_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x31) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_1_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x31) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 2 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_2_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x32) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_2_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x32) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_2_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x32) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_2_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x32) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_2_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x32) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_2_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x32) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_2_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x32) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_2_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x32) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 3 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_3_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x33) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_3_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x33) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_3_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x33) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_3_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x33) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_3_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x33) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_3_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x33) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_3_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x33) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_3_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x33) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 4 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_4_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x34) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_4_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x34) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_4_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x34) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_4_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x34) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_4_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x34) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_4_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x34) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_4_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x34) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_4_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x34) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 5 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_5_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x35) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_5_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x35) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_5_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x35) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_5_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x35) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_5_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x35) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_5_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x35) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_5_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x35) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_5_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x35) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 6 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_6_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x36) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_6_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x36) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_6_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x36) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_6_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x36) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_6_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x36) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_6_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x36) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_6_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x36) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_6_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x36) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 3, Pin 7 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P3_7_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x37) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_7_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x37) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_7_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x37) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_7_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x37) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_7_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x37) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_7_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x37) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_7_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x37) /*!< @hideinitializer */
#define PWMTimerMSP432_P3_7_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x37) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 5 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P5_6_TA2CCR1A   (PWMTimerMSP432_TA2CCR1 | 0x156) /*!< @hideinitializer */
#define PWMTimerMSP432_P5_7_TA2CCR2A   (PWMTimerMSP432_TA2CCR2 | 0x157) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 6 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P6_6_TA2CCR3A   (PWMTimerMSP432_TA2CCR3 | 0x166) /*!< @hideinitializer */
#define PWMTimerMSP432_P6_7_TA2CCR4A   (PWMTimerMSP432_TA2CCR4 | 0x167) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 0 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_0_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x70) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_0_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x70) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_0_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x70) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_0_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x70) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_0_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x70) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_0_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x70) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_0_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x70) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_0_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x70) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 1 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_1_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x71) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_1_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x71) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_1_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x71) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_1_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x71) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_1_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x71) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_1_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x71) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_1_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x71) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_1_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x71) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 2 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_2_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x72) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_2_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x72) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_2_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x72) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_2_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x72) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_2_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x72) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_2_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x72) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_2_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x72) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_2_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x72) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 3 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_3_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x73) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_3_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x73) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_3_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x73) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_3_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x73) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_3_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x73) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_3_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x73) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_3_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x73) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_3_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x73) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 4 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_4_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x74) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_4_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x74) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_4_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x74) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_4_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x74) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_4_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x74) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_4_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x74) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_4_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x74) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_4_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x74) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 5 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_5_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x75) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_5_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x75) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_5_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x75) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_5_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x75) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_5_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x75) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_5_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x75) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_5_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x75) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_5_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x75) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 6 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_6_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x76) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_6_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x76) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_6_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x76) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_6_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x76) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_6_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x76) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_6_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x76) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_6_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x76) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_6_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x76) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 7, Pin 7 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P7_7_TA0CCR1A   (PWMTimerMSP432_TA0CCR1 | 0x77) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_7_TA0CCR2A   (PWMTimerMSP432_TA0CCR2 | 0x77) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_7_TA0CCR3A   (PWMTimerMSP432_TA0CCR3 | 0x77) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_7_TA0CCR4A   (PWMTimerMSP432_TA0CCR4 | 0x77) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_7_TA1CCR1A   (PWMTimerMSP432_TA1CCR1 | 0x77) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_7_TA1CCR2A   (PWMTimerMSP432_TA1CCR2 | 0x77) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_7_TA1CCR3A   (PWMTimerMSP432_TA1CCR3 | 0x77) /*!< @hideinitializer */
#define PWMTimerMSP432_P7_7_TA1CCR4A   (PWMTimerMSP432_TA1CCR4 | 0x77) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 8, 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P8_2_TA3CCR2A   (PWMTimerMSP432_TA3CCR2 | 0x182) /*!< @hideinitializer */
/*! @} */

/*!
 *  @name Port 9 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P9_2_TA3CCR3A   (PWMTimerMSP432_TA3CCR3 | 0x192) /*!< @hideinitializer */
#define PWMTimerMSP432_P9_3_TA3CCR4A   (PWMTimerMSP432_TA3CCR4 | 0x193) /*!< @hideinitializer */

/*!
 *  @name Port 10 'pwmPin' setting variations
 *  @{
 */
#define PWMTimerMSP432_P10_5_TA3CCR1A   (PWMTimerMSP432_TA3CCR1 | 0x1A5) /*!< @hideinitializer */
/*! @} */
/*! @} End of pwmPin group */

/*!
 *  @addtogroup PWM_STATUS
 *  PWMTimerMSP432_STATUS_* macros are command codes only defined in the
 *  PWMTimerMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/pwm/PWMTimerMSP432.h>
 *  @endcode
 *  @{
 */

/* Add PWMTimerMSP432_STATUS_* macros here */

/*! @}*/

/*!
 *  @addtogroup PWM_CMD
 *  PWMTimerMSP432_CMD_* macros are command codes only defined in the
 *  PWMTimerMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/pwm/PWMTimerMSP432.h>
 *  @endcode
 *  @{
 */

/* Add PWMTimerMSP432_CMD_* macros here */

/*! @}*/

/*! \cond */
/* Number of Timer_A peripherals available on a device. */
#define PWMTimerMSP432_NUM_TIMERS       (4)

/* Number of PWM outputs a Timer_A peripheral can generate. */
#define PWMTimerMSP432_NUM_PWM_OUTPUTS  (6)
/*! \endcond */

/* PWM function table pointer */
extern const PWM_FxnTable PWMTimerMSP432_fxnTable;

/*!
 *  @brief  PWMTimerMSP432 Hardware attributes
 *
 *  The 'clockSource' field is used by driverlib APIs and therefore must be
 *  populated by driverlib macro definitions. For msp432_driverlib these
 *  definitions are found in:
 *      - timer_a.h
 *
 *  The 'pwmPin' field identifies which physical pin as well as which timer
 *  compare register to use for a particular PWM channel. The encoded
 *  pin identifier for initializing the 'pwmPin' field must be selected
 *  from the @ref pwmPinIdentifiersMSP432 macros.
 *
 *  A sample structure is shown below:
 *  @code
 *  const PWMTimerMSP432_HWAttrsV2 PWMTimerMSP432HWAttrs[] = {
 *      {
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .pwmPin = PWMTimerMSP432_P2_1_TA1CCR1A
 *      },
 *  };
 *  @endcode
 */
typedef struct PWMTimerMSP432_HWAttrsV2 {
    uint16_t clockSource;          /*!< TIMER A Clock Source
                                        (see timer_a.h for options) */
    uint32_t pwmPin;               /*!< Pin to output PWM signal on
                                        (see @ref pwmPinIdentifiersMSP432) */
} PWMTimerMSP432_HWAttrsV2;

/*!
 *  @brief PWMTimerMSP432_Status
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct PWMTimerMSP432_Status {
    Power_NotifyObj perfChangeNotify;
    PWM_Period_Units periodUnits;
    uint32_t perfConstraintMask;
    uint32_t clockSource;
    uint32_t duties[PWMTimerMSP432_NUM_PWM_OUTPUTS];
    uint32_t periodCounts;
    uint32_t period;
    uint8_t  prescalar;
    uint8_t  activeOutputsMask;
    uint8_t  openMask;
} PWMTimerMSP432_Status;

/*!
 *  @brief  PWMTimerMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct PWMTimerMSP432_Object {
    PWMTimerMSP432_Status *timerStatus;
    uint32_t               baseAddress;     /* PWMTimer base address */
    PWM_Duty_Units         dutyUnits;       /* Current duty cycle unit */
    PWM_IdleLevel          idleLevel;       /* PWM idle level when stopped */
    uint8_t                compareOutputNum; /* Timer's compare output */
                                             /* number */
} PWMTimerMSP432_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_driver_pwm_PWMTimerMSP432__include */
