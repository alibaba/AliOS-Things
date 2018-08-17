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
 *  @file       CaptureMSP432.h
 *  @brief      Capture driver interface for MSP432 devices
 *
 *  Refer to @ref Capture.h for a complete description of APIs & example
 *  of use.
 *
 * # Operation #
 * This driver leverages the Timer_A peripheral of the MSP432 device. Each
 * instance of a capture driver will occupy exactly one Timer_A peripheral on
 * the device. This is done to reduce contingencies and conflicts that may
 * arise from using more than one capture/compare registers on a Timer_A
 * peripheral.
 *
 * The specific capture/compare register and pin used by the
 * capture module is determined by the capturePort definition. If a mappable
 * pin is contained in the capturePort parameter of the CaptureMSP432_HWAttrs
 * structure, the driver will automatically configure the Port Mapping
 * Controller (PMAP) module with the correct/corresponding pin assignments.
 *
 * # Accuracy #
 * Each Timer_A peripheral has 16-bits of resolution. An appropriate
 * clockSource and clockDivider must be specified in the CaptureMSP432_HWAttrs
 * for each specific capture application. A higher clock rate may provide a
 * better capture resolution at the cost of a smaller capturable period. The
 * maximum capture period for a given clockSource and clockDivider is
 * calculated as:
 *
 * MAX_CAPTURE_PERIOD = 2 * ((clockDivider * MATCH_VALUE) / CYCLES_PER_US) - 1
 *
 * Given that:
 * MATCH_VALUE = 0xFFFF (65535)
 * CYCLES_PER_US = clockSource / 1000000
 *
 *  ### MPS432 Capture Driver Configuration #
 *
 *  In order to use the Capture APIs, the application is required
 *  to define 4 configuration items in the application Board.c file:
 *
 *  1.  An array of CaptureMSP432_Object elements, which will be used by
 *  by the driver to maintain instance state.
 *  Below is an example CaptureMSP432_Object array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *    #include <ti/drivers/Capture.h>
 *    #include <ti/drivers/capture/CaptureMSP432.h>
 *
 *    CaptureMSP432_Object captureMSP432Objects[2];
 *  @endcode
 *
 *  2.  An array of CaptureMSP432_HWAttrs elements that defines which
 *  pin will be used by the corresponding capture instance
 *  (see @ref capturePortIdentifiersMSP432).
 *  Below is an example CaptureMSP432_HWAttrs array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *  const CaptureMSP432_HWAttrs captureMSP432HWAttrs[2] = {
 *      {
 *          .timerBaseAddress = TIMER_A1_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .clockDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64,
 *          .capturePort = CaptureMSP432_P2_3_TA1,
 *          .intPriority = ~0
 *      },
 *      {
 *          .timerBaseAddress = TIMER_A2_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
 *          .clockDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,
 *          .capturePort = CaptureMSP432_P6_6_TA2,
 *          .intPriority = ~0
 *      },
 *  };
 *  @endcode
 *
 *  3.  An array of @ref Capture_Config elements, one for each Capture instance.
 *  Each element of this array identifies the device-specific API function
 *  table, the device specific capture object instance, and the device specific
 *  Hardware Attributes to be used for each capture channel.
 *  Below is an example @ref Capture_Config array appropriate for the MSP432
 *  Launchpad board:
 *  @code
 *    const Capture_Config Capture_config[2] = {
 *      {
 *          .fxnTablePtr = &CaptureMSP432_captureFxnTable,
 *          .object = &captureMSP432Objects[0],
 *          .hwAttrs = &captureMSP432HWAttrs[0]
 *      },
 *      {
 *          .fxnTablePtr = &CaptureMSP432_captureFxnTable,
 *          .object = &captureMSP432Objects[1],
 *          .hwAttrs = &captureMSP432HWAttrs[1]
 *      }
 *    };
 *  @endcode
 *
 *  4.  A global variable, Capture_count, that informs the driver how many
 *  capture instances are defined:
 *  @code
 *    const uint_least8_t Capture_count = 2;
 *  @endcode
 *
 * # Resource Allocation #
 * The TimerMSP432_allocateTimerResource API will allocate a timer for exclusive
 * use. Any attempt to re-allocate this resource by the TimerMSP432, PWMMSP432,
 * or CaptureMSP432 drivers will result in a false value being returned from
 * the allocation API. To free a timer resource, the
 * TimerMSP432_freeTimerResource is used. The application is not responsible
 * for calling these allocation APIs directly.
 *
 * ### Power Management #
 * The TI-RTOS power management framework will try to put the device into the
 * most power efficient mode whenever possible. Please see the technical
 * reference manual for further details on each power mode.
 *
 *  The following statements are valid:
 *    - After Capture_open(): Clocks are enabled to the timer resource and the
 *      configured capture pin. Dynamic performance level changes are allowed.
 *    - After Capture_start(): DEEPSLEEP is disabled for MSP432P401x devices.
 *      Performance level changes are not allowed.
 *    - After Capture_stop(): Conditions are equal as for after Capture_open.
 *    - After Capture_close(): The underlying GPTimer is turned off, and the
 *      clocks to the timer and pin are disabled.
 ******************************************************************************
 */
#ifndef ti_drivers_capture_CaptureMSP432__include
#define ti_drivers_capture_CaptureMSP432__include

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/timer/TimerMSP432.h>
#include <ti/drivers/Capture.h>

#include <ti/devices/DeviceFamily.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>

/*! \cond */
/*
 *  Capture port definitions for MSP432 Timer_A pins.
 *
 *  The interrupt number is stored in bits 31 - 24
 *  The capture compare register is stored in bits 23 - 16
 *  The port mapping is stored in bits 15 - 8
 *  The port number is stored in bits 7 - 4
 *  The pin number is stored in bits 3 - 0
 *
 *    31 - 24          23 - 16             15 - 8       7 - 4     3 - 0
 *  ----------------------------------------------------------------------
 *  | Int Num | Capture Compare Register | Port Map | Port Num | Pin Num |
 *  ----------------------------------------------------------------------
 *
 * For ports 2, 3, and 7; the PMAP is used to map TA0/TA1 capture and
 * compare to any pin. Other ports have limited pin assignments.
 */
#define CAPTUREMSP432_INT_OFS     24
#define CAPTUREMSP432_CCR_OFS     16
#define CAPTUREMSP432_PMAP_OFS    8
/*! \endcond */

/*!
 *  \defgroup capturePortIdentifiersMSP432 CaptureMSP432_HWAttrs 'capturePort'
 *            field options.
 *  @{
 */
/*!
 *  @name PIN 2.0, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_0_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | 0x20 | \
       (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
       (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.0, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_0_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x20 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.1, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_1_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x21 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.1, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_1_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x21 | (INT_TA2_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))

/*!
 *  @name PIN 2.2, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_2_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x22 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.2, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_2_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x22 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.3, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_3_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x23 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.3, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_3_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x23 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.4, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_4_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x24 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.4, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_4_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x24 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.5, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_5_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x25 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.5, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_5_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x25 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.6, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_6_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x26 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.6, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_6_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x26 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.7, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_7_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x27 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 2.7, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P2_7_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x27 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.0, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_0_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x30 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.0, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_0_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x30 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.1, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_1_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x31 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.1, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_1_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x31 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.2, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_2_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x32 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.2, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_2_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x32 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.3, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_3_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x33 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.3, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_3_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x33 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.4, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_4_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x34 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.4, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_4_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x34 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.5, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_5_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x35 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.5, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_5_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x35 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.6, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_6_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x36 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.6, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_6_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x36 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.7, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_7_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x37 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 3.7, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P3_7_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x37 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 5.6, TimerA2.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P5_6_TA2 (0x56 | (INT_TA2_N << CAPTUREMSP432_INT_OFS) |  \
        (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 5.7, TimerA2.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P5_7_TA2 (0x57 | (INT_TA2_N << CAPTUREMSP432_INT_OFS) |  \
        (TIMER_A_CAPTURECOMPARE_REGISTER_2 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 6.6, TimerA2.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P6_6_TA2 (0x66 | (INT_TA2_N << CAPTUREMSP432_INT_OFS) |  \
        (TIMER_A_CAPTURECOMPARE_REGISTER_3 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 6.7, TimerA2.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P6_7_TA2 (0x67 | (INT_TA2_N << CAPTUREMSP432_INT_OFS) |  \
        (TIMER_A_CAPTURECOMPARE_REGISTER_4 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.0, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_0_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x70 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.0, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_0_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x70 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.1, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_1_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x71 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.1, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_1_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x71 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.3,7TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_2_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x72 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.3,7TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_2_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x72 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.3, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_3_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x73 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.3, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_3_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x73 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.4, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_4_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x74 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.4, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_4_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x74 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.5, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_5_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x75 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.5, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_5_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x75 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.6, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_6_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x76 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.6, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_6_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x76 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.7, TimerA0.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_7_TA0 ((PMAP_TA0CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x77 | (INT_TA0_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 7.7, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P7_7_TA1 ((PMAP_TA1CCR1A << CAPTUREMSP432_PMAP_OFS) | \
            0x77 | (INT_TA1_N << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 8.0, TimerA1.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P8_0_TA1 (0x80 | (INT_TA1_0 << CAPTUREMSP432_INT_OFS) |  \
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 8.1, TimerA2.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P8_1_TA2 (0x81 | (INT_TA2_0 << CAPTUREMSP432_INT_OFS) |  \
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 8.3,7TimerA3.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P8_2_TA3 (0x82 | (INT_TA3_N << CAPTUREMSP432_INT_OFS) |  \
            (TIMER_A_CAPTURECOMPARE_REGISTER_2 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 9.3,7TimerA3.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P9_2_TA3 (0x92 | (INT_TA3_N << CAPTUREMSP432_INT_OFS) |  \
            (TIMER_A_CAPTURECOMPARE_REGISTER_3 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 9.3, TimerA3.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P9_3_TA3 (0x93 | (INT_TA3_N << CAPTUREMSP432_INT_OFS) |  \
            (TIMER_A_CAPTURECOMPARE_REGISTER_4 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 10.4, TimerA3.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P10_4_TA3 (0xA4 | (INT_TA3_0 << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_0 << CAPTUREMSP432_CCR_OFS))
/*! @} */

/*!
 *  @name PIN 10.5, TimerA3.
 *  @{  \hideinitializer
 */
#define CaptureMSP432_P10_5_TA3 (0xA5 | (INT_TA3_1 << CAPTUREMSP432_INT_OFS) | \
            (TIMER_A_CAPTURECOMPARE_REGISTER_1 << CAPTUREMSP432_CCR_OFS))
/*! @} */
/*! @} */

extern const Capture_FxnTable CaptureMSP432_captureFxnTable;

/*!
 *  @brief CaptureMSP432 Hardware Attributes
 *
 *  Capture hardware attributes that tell the CaptureMSP432 driver specific
 *  hardware configurations and interrupt/priority settings.
 *
 *  A sample structure is shown below:
 *  @code
 *  const CaptureMSP432_HWAttrs captureMSP432HWAttrs[] =
 *  {
 *      {
 *          .timerBaseAddress = TIMER_A1_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
 *          .clockDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64,
 *          .capturePort = CaptureMSP432_P2_3_TA1,
 *          .intPriority = ~0
 *      },
 *      {
 *          .timerBaseAddress = TIMER_A2_BASE,
 *          .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
 *          .clockDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,
 *          .capturePort = CaptureMSP432_P6_6_TA2,
 *          .intPriority = ~0
 *      },
 *  };
 *  @endcode
 */
typedef struct CaptureMSP432_HWAttrs_ {
    /*! Specifies the base address of the Timer_A peripheral. */
    uint32_t timerBaseAddress;

    /*! Specifies the clock source for the Timer_A peripheral. */
    uint32_t clockSource;

    /*! Specifies the divider applied to the clockSource. */
    uint32_t clockDivider;

    /*! Predefined capture port and pin. The Timer_A peripheral used by
     *  this define should match that of timerBaseAddress. */
    uint32_t capturePort;

    /*! The interrupt priority. */
    uint32_t intPriority;
} CaptureMSP432_HWAttrs;

/*!
 *  @brief CaptureMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct CaptureMSP432_Object_ {
    HwiP_Handle           hwiHandle;
    Capture_CallBackFxn   callBack;
    Capture_PeriodUnits   periodUnits;
    uint32_t              previousCount;
    uint32_t              ccrRegister;
    bool                  isRunning;
} CaptureMSP432_Object;

#endif /* ti_drivers_capture_CaptureMSP432__include */
