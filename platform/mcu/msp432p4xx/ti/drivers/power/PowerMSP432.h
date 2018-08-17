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
/** ============================================================================
 *  @file       PowerMSP432.h
 *
 *  @brief      Power manager interface for the MSP432
 *
 *  The Power header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/Power.h>
 *  #include <ti/drivers/power/PowerMSP432.h>
 *  @endcode
 *
 *  Refer to @ref Power.h for a complete description of APIs.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_power_PowerMSP432__include
#define ti_drivers_power_PowerMSP432__include

#include <stdint.h>

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/utils/List.h>
#include <ti/drivers/Power.h>

/* driverlib header files */
#include <ti/devices/msp432p4xx/driverlib/pcm.h>
#include <ti/devices/msp432p4xx/driverlib/cs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* latency times in microseconds (derived from device datasheet) */

/*! The latency to reserve for resume from SLEEP (usec) */
#define PowerMSP432_RESUMETIMESLEEP           32

/*! The total latency to reserve for entry and exit of SLEEP (usec) */
#define PowerMSP432_TOTALTIMESLEEP            40

/*! The latency to reserve for resume from DEEPSLEEP_0 (usec) */
#define PowerMSP432_RESUMETIMEDEEPSLEEP_0     16

/*! The total latency to reserve for entry and exit of DEEPSLEEP_0 (usec) */
#define PowerMSP432_TOTALTIMEDEEPSLEEP_0      40

/*! The latency to reserve for resume from DEEPSLEEP_1 (usec) */
#define PowerMSP432_RESUMETIMEDEEPSLEEP_1     16

/*! The total latency to reserve for entry and exit of DEEPSLEEP_1 (usec) */
#define PowerMSP432_TOTALTIMEDEEPSLEEP_1      40

/* Power resources (for declaring dependencies) */
#define PowerMSP432_PERIPH_ADC14              0
/*!< Resource ID: ADC14 */

#define PowerMSP432_PERIPH_DMA                1
/*!< Resource ID: DMA */

#define PowerMSP432_PERIPH_EUSCI_A0           2
/*!< Resource ID: EUSCI_A0 */

#define PowerMSP432_PERIPH_EUSCI_A1           3
/*!< Resource ID: EUSCI_A1 */

#define PowerMSP432_PERIPH_EUSCI_A2           4
/*!< Resource ID: EUSCI_A2 */

#define PowerMSP432_PERIPH_EUSCI_A3           5
/*!< Resource ID: EUSCI_A3 */

#define PowerMSP432_PERIPH_EUSCI_B0           6
/*!< Resource ID: EUSCI_B0 */

#define PowerMSP432_PERIPH_EUSCI_B1           7
/*!< Resource ID: EUSCI_B1 */

#define PowerMSP432_PERIPH_EUSCI_B2           8
/*!< Resource ID: EUSCI_B2 */

#define PowerMSP432_PERIPH_EUSCI_B3           9
/*!< Resource ID: EUSCI_B3 */

#define PowerMSP432_PERIPH_TIMER_A0           10
/*!< Resource ID: Timer_A0 */

#define PowerMSP432_PERIPH_TIMER_A1           11
/*!< Resource ID: Timer_A1 */

#define PowerMSP432_PERIPH_TIMER_A2           12
/*!< Resource ID: Timer_A2 */

#define PowerMSP432_PERIPH_TIMER_A3           13
/*!< Resource ID: Timer_A3 */

#define PowerMSP432_PERIPH_TIMER_T32          14
/*!< Resource ID: Timer32 */


/* Power constraints */
#define PowerMSP432_DISALLOW_SLEEP          0
/*!< Constraint: Disallow a transition to the SLEEP state */

#define PowerMSP432_DISALLOW_DEEPSLEEP_0    1
/*!< Constraint: Disallow a transition to the DEEPSLEEP_0 state */

#define PowerMSP432_DISALLOW_DEEPSLEEP_1    2
/*!< Constraint: Disallow a transition to the DEEPSLEEP_1 state */

#define PowerMSP432_DISALLOW_SHUTDOWN_0     3
/*!< Constraint: Disallow a transition to the SHUTDOWN_0 state */

#define PowerMSP432_DISALLOW_SHUTDOWN_1     4
/*!< Constraint: Disallow a transition to the SHUTDOWN_1 state */

#define PowerMSP432_DISALLOW_PERFLEVEL_0    5
/*!< Constraint: Disallow a transition to PERFLEVEL_0 */

#define PowerMSP432_DISALLOW_PERFLEVEL_1    6
/*!< Constraint: Disallow a transition to PERFLEVEL_1 */

#define PowerMSP432_DISALLOW_PERFLEVEL_2    7
/*!< Constraint: Disallow a transition to PERFLEVEL_2 */

#define PowerMSP432_DISALLOW_PERFLEVEL_3    8
/*!< Constraint: Disallow a transition to PERFLEVEL_3 */

#define PowerMSP432_DISALLOW_PERFLEVEL_4    9
/*!< Constraint: Disallow a transition to PERFLEVEL_4 */

#define PowerMSP432_DISALLOW_PERFLEVEL_5    10
/*!< Constraint: Disallow a transition to PERFLEVEL_5 */

#define PowerMSP432_DISALLOW_PERFLEVEL_6    11
/*!< Constraint: Disallow a transition to PERFLEVEL_6 */

#define PowerMSP432_DISALLOW_PERFLEVEL_7    12
/*!< Constraint: Disallow a transition to PERFLEVEL_7 */

#define PowerMSP432_DISALLOW_PERF_CHANGES   13
/*!< Constraint: Disallow any performance level changes */

/* \cond */
#define PowerMSP432_NUMCONSTRAINTS          14
/* \endcond */

/*
 * Power events
 *
 * Each event ID must be a power of two and sequential without any gaps
 */
#define PowerMSP432_ENTERING_SLEEP          0x1
/*!< Power event: The device is entering the SLEEP state */

#define PowerMSP432_ENTERING_DEEPSLEEP      0x2
/*!< Power event: The device is entering a DEEPSLEEP state */

#define PowerMSP432_ENTERING_SHUTDOWN       0x4
/*!< Power event: The device is entering a SHUTDOWN state */

#define PowerMSP432_AWAKE_SLEEP             0x8
/*!< Power event: The device is waking from the SLEEP state */

#define PowerMSP432_AWAKE_DEEPSLEEP         0x10
/*!< Power event: The device is waking from a DEEPSLEEP state */

#define PowerMSP432_START_CHANGE_PERF_LEVEL 0x20
/*!< Power event: A performance level change has been initiated */

#define PowerMSP432_DONE_CHANGE_PERF_LEVEL  0x40
/*!< Power event: A performance level change has completed */

/* \cond */
#define PowerMSP432_NUMEVENTS               7
/* \endcond */

/* Power sleep states */
#define PowerMSP432_SLEEP                   0x1   /*!< Device state of LPM0 */
#define PowerMSP432_DEEPSLEEP_0             0x2   /*!< Device state of LPM3 */
#define PowerMSP432_DEEPSLEEP_1             0x4   /*!< Device state of LPM4 */

/* Power shutdown states */
#define PowerMSP432_SHUTDOWN_0              0x0   /*!< Device state of LPM3.5 */
#define PowerMSP432_SHUTDOWN_1              0x1   /*!< Device state of LPM4.5 */

/*!
 *  @brief Structure defining a performance level.
 *
 *  The MCU performance level can be set with Power_setPerformanceLevel().
 *  A performance level is defined by this PowerMSP432_PerfLevel structure.
 *
 *  The Power driver currently supports four pre-defined performance levels:
 *  @code
 *    Level    MCLK (MHz)    HSMCLK (MHz)    SMCLK (MHz)    ACLK (Hz)
 *    -----    ----------    ------------    -----------    ---------
 *      0         12              3              3           32768
 *      1         24              6              6           32768
 *      2         48             24             12           32768
 *      3         48             48             24           32768
 *  @endcode
 *
 *  Up to four optional 'custom' performance levels can be defined by the user.
 *  Performance levels are designated by an index, starting with
 *  zero.  Any custom performance levels will be indexed following the last
 *  pre-defined level.  For example, if there are 4 pre-defined levels, they
 *  will have indices from '0' to '3'. If there are custom levels defined, they
 *  will be indexed starting with '4'.
 *
 *  Eight performance level constraints are supported, one for each of the
 *  four pre-defined levels:
 *  @code
 *        PowerMSP432_DISALLOW_PERFLEVEL_0
 *        PowerMSP432_DISALLOW_PERFLEVEL_1
 *        PowerMSP432_DISALLOW_PERFLEVEL_2
 *        PowerMSP432_DISALLOW_PERFLEVEL_3
 *  @endcode
 *  plus four for the optional custom performance levels:
 *  @code
 *        PowerMSP432_DISALLOW_PERFLEVEL_4
 *        PowerMSP432_DISALLOW_PERFLEVEL_5
 *        PowerMSP432_DISALLOW_PERFLEVEL_6
 *        PowerMSP432_DISALLOW_PERFLEVEL_7
 *  @endcode
 *
 *  The parameters that are used to define a performance level are shown below.
 *
 *  @warning There is very limited runtime error checking of the
 *  different parameters, and no checking that the ratios and parameter
 *  combinations do not result in an invalid device configuration (which could
 *  overclock or damage the device). It is therefore critical that the user
 *  ensures the values specified for a custom performance level are correct.
 *
 *  To define custom performance levels the user must add an array of
 *  PowerMSP432_PerfLevel structures to their board file, and then reference
 *  this array in the PowerMSP432_config structure. An example is shown
 *  below.
 *
 *  First, if not already included in the board file, add the includes of cs.h
 *  and pcm.h
 *
 *  @code
 *  #include <ti/devices/msp432p4xx/driverlib/cs.h>
 *  #include <ti/devices/msp432p4xx/driverlib/pcm.h>
 *  @endcode
 *
 *  Next, add an array with two new performance levels:
 *
 *  @code
 *  PowerMSP432_PerfLevel myPerfLevels[] = {
 *      { .activeState = PCM_AM_DCDC_VCORE0,
 *        .VCORE = 0,
 *        .DCORESEL = CS_DCO_FREQUENCY_12,
 *        .SELM = CS_HFXTCLK_SELECT,
 *        .DIVM = CS_CLOCK_DIVIDER_4,
 *        .SELS = CS_HFXTCLK_SELECT,
 *        .DIVHS = CS_CLOCK_DIVIDER_16,
 *        .DIVS = CS_CLOCK_DIVIDER_16,
 *        .SELB = CS_LFXTCLK_SELECT,
 *        .SELA = CS_LFXTCLK_SELECT,
 *        .DIVA = CS_CLOCK_DIVIDER_1,
 *        .flashWaitStates = 0,
 *        .enableFlashBuffer = false,
 *        .MCLK = 12000000,
 *        .HSMCLK = 3000000,
 *        .SMCLK = 3000000,
 *        .BCLK = 32768,
 *        .ACLK = 32768
 *        },
 *        { .activeState = PCM_AM_DCDC_VCORE0,
 *        .VCORE = 0,
 *        .DCORESEL = CS_DCO_FREQUENCY_24,
 *        .SELM = CS_DCOCLK_SELECT,
 *        .DIVM = CS_CLOCK_DIVIDER_1,
 *        .SELS = CS_DCOCLK_SELECT,
 *        .DIVHS = CS_CLOCK_DIVIDER_4,
 *        .DIVS = CS_CLOCK_DIVIDER_4,
 *        .SELB = CS_REFOCLK_SELECT,
 *        .SELA = CS_REFOCLK_SELECT,
 *        .DIVA = CS_CLOCK_DIVIDER_1,
 *        .flashWaitStates = 1,
 *        .enableFlashBuffer = true,
 *        .MCLK = 24000000,
 *        .HSMCLK = 6000000,
 *        .SMCLK = 6000000,
 *        .BCLK = 32768,
 *        .ACLK = 32768
 *      },
 *  };
 *  @endcode
 *
 *  Then, update the PowerMSP432_config structure to
 *  1) reference the new custom levels, and 2) as appropriate, enable extended
 *  performance scaling support, and define the relevant extended configuration
 *  parameters.  In the first custom perf level shown above the HFXT and LFXT
 *  crystals are used as clock sources, so for this example the extended
 *  support parameters need to be specfied.
 *
 *  The code below shows additions to the PowerMSP432_config structure for this
 *  example.  The first two additions to PowerMSP432_config reference the array
 *  of custom perf levels, and indicate the number of custom levels.  The third
 *  addition enables extended support for crystals.  The remaining
 *  parameters define crystal settings, plus an application-provided ISR
 *  function for handling any Clock System fault interrupts.
 *
 *  @code
 *  const PowerMSP432_ConfigV1 PowerMSP432_config = {
 *      ...
 *      .customPerfLevels = myPerfLevels,
 *      .numCustom = sizeof(myPerfLevels) / sizeof(PowerMSP432_PerfLevel),
 *      .useExtendedPerf = true,
 *      .HFXTFREQ = CS_48MHZ,
 *      .configurePinHFXT = true,
 *      .bypassHFXT = false,
 *      .configurePinLFXT = true,
 *      .bypassLFXT = false,
 *      .LFXTDRIVE = CS_LFXT_DRIVE3,
 *      .enableInterruptsCS = true,
 *      .priorityInterruptsCS = ~0,
 *      .isrCS = &isrCS
 *  };
 *  @endcode
 */
typedef struct PowerMSP432_PerfLevel {
    /*!
     *  @brief The active state for the device.
     *
     *  The states are identified by the enumerations in pcm.h. The supported
     *  active states are: PCM_AM_LDO_VCORE0, PM_AM_LDO_VCORE1,
     *  PCM_AM_DCDC_VCORE0, PCM_AM_DCDC_VCORE1.  Usage of DCDC states requires
     *  that the DCDC is available for the device and board configuration.
     */
    unsigned int activeState;
    /*!
     *  @brief The core voltage level.
     *
     *  The supported levels are '0' indicating VCORE0, and '1' indicating
     *  VCORE1.
     */
    unsigned int VCORE;
    /*!
     *  @brief The clock source for this performance level.
     *
     *  This configuration parameter is not currently used.
     */
    unsigned int clockSource;
    /*!
     *  @brief The DCO frequency range selection.
     *
     *  The nominal DCO frequency is specified via an enumerated value defined
     *  in cs.h, for example: CS_DCO_FREQUENCY_12, CS_DCO_FREQUENCY_24, etc.
     */
    unsigned int DCORESEL;
    /*!
     *  @brief The MCLK source.
     *
     *  The source is specified via an enumerated value from cs.h, for
     *  example: CS_DCOCLK_SELECT, CS_HFXTCLK_SELECT, etc.
     */
    unsigned int SELM;
    /*!
     *  @brief The MCLK source divider.
     *
     *  The divide value is specified via an enumerated value from cs.h, for
     *  example: CS_CLOCKDIVIDER_1, CS_CLOCKDIVIDER_2, etc.
     */
    unsigned int DIVM;
    /*!
     *  @brief The HSMCLK and SMCLK source.
     *
     *  The source is specified via an enumerated value from cs.h, for
     *  example: CS_DCOCLK_SELECT, CS_HFXTCLK_SELECT, etc.
     */
    unsigned int SELS;
    /*!
     *  @brief The HSMCLK source divider.
     *
     *  The divide value is specified via an enumerated value from cs.h, for
     *  example: CS_CLOCKDIVIDER_1, CS_CLOCKDIVIDER_2, etc.
     */
    unsigned int DIVHS;
    /*!
     *  @brief The SMCLK source divider.
     *
     *  The divide value is specified via an enumerated value from cs.h, for
     *  example: CS_CLOCKDIVIDER_1, CS_CLOCKDIVIDER_2, etc.
     */
    unsigned int DIVS;
    /*!
     *  @brief The BCLK source.
     *
     *  The source is specified via an enumerated value from cs.h, for
     *  example: CS_REFOCLK_SELECT, CS_DCOCLK_SELECT, etc.
     */
    unsigned int SELB;
    /*!
     *  @brief The ACLK source.
     *
     *  The source is specified via an enumerated value from cs.h, for
     *  example: CS_REFOCLK_SELECT, CS_DCOCLK_SELECT, etc.
     */
    unsigned int SELA;
    /*!
     *  @brief The ACLK source divider.
     *
     *  The divide value is specified via an enumerated value from cs.h, for
     *  example: CS_CLOCKDIVIDER_1, CS_CLOCKDIVIDER_2, etc.
     */
    unsigned int DIVA;
    /*!
     *  @brief The number of Flash wait-states to be used for this performance
     *  level.
     *
     *  The number of wait-states is specified as a positive integer value.
     */
    unsigned int flashWaitStates;
    /*!
     *  @brief Boolean specifying if Flash read buffering should be enabled for
     *  this performance level.
     *
     *  If 'true', buffering will be enabled; if 'false', buffering will be
     *  disabled.
     */
    bool enableFlashBuffer;
    /*!
     *  @brief The expected MCLK frequency for this performance level, in Hz.
     *
     *  Note that this is an informational value; it is the expected MCLK
     *  frequency given the DCO frequency and source dividers specified above.
     */
    unsigned int MCLK;
    /*!
     *  @brief The expected HSMCLK frequency for this performance level, in Hz.
     *
     *  Note that this is an informational value; it is the expected HSMCLK
     *  frequency given the DCO frequency and source dividers specified above.
     */
    unsigned int HSMCLK;
    /*!
     *  @brief The expected SMCLK frequency for this performance level, in Hz.
     *
     *  Note that this is an informational value; it is the expected SMCLK
     *  frequency given the DCO frequency and source dividers specified above.
     */
    unsigned int SMCLK;
    /*!
     *  @brief The BCLK frequency for this performance level.  Currently only
     *  32768 Hz is supported.
     */
    unsigned int BCLK;
    /*!
     *  @brief The ACLK frequency for this performance level.  Currently only
     *  32768 Hz is supported.
     */
    unsigned int ACLK;
} PowerMSP432_PerfLevel;

/*! @brief  Structure holding device frequencies (in Hz) */
typedef struct PowerMSP432_Freqs {
    unsigned int MCLK;                 /*!< MCLK frequency (in Hz) */
    unsigned int HSMCLK;               /*!< HSMCLK frequency (in Hz) */
    unsigned int SMCLK;                /*!< SMCLK frequency (in Hz) */
    unsigned int BCLK;                 /*!< BCLK frequency (in Hz) */
    unsigned int ACLK;                 /*!< ACLK frequency (in Hz) */
} PowerMSP432_Freqs;

/*! @brief  Power global configuration (MSP432-specific) */
typedef struct PowerMSP432_ConfigV1 {
    /*!
     *  @brief The Power Policy's initialization function
     *
     *  If the policy does not have an initialization function, 'NULL'
     *  should be specified.
     */
    Power_PolicyInitFxn policyInitFxn;
    /*!
     *  @brief The Power Policy function
     *
     *  When enabled, this function is invoked in the idle loop, to
     *  opportunistically select and activate sleep states.
     *
     *  Two reference policies are provided:
     *
     *    PowerMSP432_sleepPolicy() - a simple policy that will put the device
     *    in either the PowerMSP432_SLEEP state (unless there is a constraint
     *    prohibiting this), or CPU wait for interrupt (WFI)
     *
     *    PowerMSP432_deepSleepPolicy() - a more agressive policy that checks
     *    constraints and optionally chooses PowerMSP432_DEEPSLEEP_1,
     *    PowerMSP432_DEEPSLEEP_0, PowerMSP432_SLEEP, or WFI, in that order
     *    of preference.
     *
     *  Custom policies can be written, and specified via this function pointer.
     *
     *  In addition to this static selection, the Power Policy can be
     *  dynamically changed at runtime, via the Power_setPolicy() API.
     */
    Power_PolicyFxn policyFxn;
    /*!
     *  @brief The initial performance level to be established during Power
     *  Manager initialization.
     *
     *  The performance level is identified by a numeric index into the table
     *  of available performance levels.
     *
     *  This performance level will be activated during initialization only
     *  when performance scaling is enabled (below, via specification of
     *  enablePerf).
     */
    unsigned int initialPerfLevel;
    /*!
     *  @brief Boolean specifying if the Power Policy function is enabled
     *
     *  If 'true', the policy function will be invoked once for each pass
     *  of the idle loop.
     *
     *  If 'false', the policy will not be invoked.
     *
     *  In addition to this static setting, the Power Policy can be dynamically
     *  enabled and disabled at runtime, via the Power_enablePolicy() and
     *  Power_disablePolicy() functions, respectively.
     */
    bool enablePolicy;
    /*!
     *  @brief Boolean specifying if performance scaling is enabled
     *
     *  If 'true', the performance scaling feature will be enabled for the
     *  application.  And, the initial performance level (specified above,
     *  via initialPerfLevel) will be established during Power Manager
     *  initialization.
     *
     *  If 'false', performance scaling is disabled for the application.
     */
    bool enablePerf;
    /*!
     *  @brief Boolean specifying if pull resistors should be automatically
     *  applied to input pins during PowerMSP432_DEEPSLEEP_0 and
     *  PowerMSP432_DEEPSLEEP_1
     *
     *  Leaving an input pin floating during a device deepsleep state will
     *  result in an increase in power consumption.  The Power Manager provides
     *  an option to automatically apply pull resistors to input pins during
     *  deepsleep, so that none are left floating. The type of pull (pull-up or
     *  pull-down) is determined by the current state sensed for each input
     *  pin, as the device is transitioning into deepsleep.  This feature is
     *  referred to as 'automatic pin parking', and is enabled with this
     *  enableParking configuration parameter.
     *
     *  If 'true', before completing a transition into deepsleep, pull resistors
     *  will be applied to all pins configured as input, with General Purpose
     *  I/O (GPIO) function (as configured in the PxSEL1 and PxSEL0 registers).
     *  On wake from deepsleep, the pull resistor settings will be restored to
     *  those in effect prior to deepsleep.
     *
     *  If 'false', pull resistors will not be automatically applied during
     *  deepsleep states.
     *
     *  SPECIAL NOTE: When auto pin parking is enabled special attention must
     *  be paid to the interrupt trigger selection for GPIO pins that are
     *  expected to wake the device from deepsleep.  Specifically, the
     *  interrupt should be triggered on the leading edge of the signal
     *  transition.
     *
     *  As an example, consider a pin that is connected via a debounced
     *  button switch to ground.  An internal pull-up resistor is used
     *  versus an external pull-up resistor.  The GPIO driver is used to
     *  manage this pin, with the following entry in the pin configuration
     *  array:
     *      ...,
     *      GPIOMSP432_P1_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
     *      ...,
     *
     *  Note that the "FALLING" edge is specifically selected so that the
     *  interrupt triggers immediately upon a button press.
     *
     *  If instead the "RISING" edge were selected, in certain situations there
     *  may be a problem with this pin being able to trigger a wakeup from
     *  deepsleep. For example, consider the case where there is an additional
     *  mechanism to wake the device from deepsleep, for example, the watchdog
     *  timer is programmed in interval timer mode, to periodically wake the
     *  device every 250ms. If the button is not pressed, when the device goes
     *  to deepsleep the pin state is sensed as high, and a pull-up resistor is
     *  automatically applied by the Power driver. At some point the button is
     *  pressed, and the pin is pulled to ground.  But since "RISING" edge was
     *  configured for the interrupt, the interrupt request has not yet been
     *  sent to the CPU. Now, while the button is still low, the interval timer
     *  wakes the device, the necessary work is done, the power policy
     *  runs in the idle loop again, and puts the device back to deepsleep.
     *  On this transition to deepsleep, the auto pin parking code runs again,
     *  but this time the button pin level is sensed as low (since the button
     *  is depressed); so this time, a pull-down resistor is applied to the pin.
     *  The device goes to deepsleep, and then eventually the button is
     *  released... but the pin is still held low by the internal pull-down
     *  resistor, so no rising edge occurs, no interrupt is signalled to the
     *  CPU, and the device stays in deesleep.
     *
     *  The above is a hypothetical example, but illustrates the importance of
     *  selecting the appropriate interrupt trigger configuration for GPIO
     *  pins when auto pin parking is enabled.  If a pin is being driven by
     *  an external circuit (versus floating with weak pull resistors as in the
     *  above example), as long as the external circuit can sufficiently drive
     *  the pin to counteract the weak pull resistor, there won't be a similar
     *  issue.
     */
    bool enableParking; /* enables automatic pin parking during LPM3 and LPM4 */
    /*!
     *  @brief  Hook function called when resuming from a SHUTDOWN state
     *
     *  This function is called when the device wakes and resumes from a
     *  previous entry into either the SHUTDOWN_0 or SHUTDOWN_1 states.
     *  If this function is defined (i.e., this pointer is not NULL), it will
     *  be called during Power_init(), right before the Power driver clears
     *  the LOCKLPM5 and LOCKBKUP bits in the PMCTL1 register, to unlock I/Os
     *  and the backup domain.
     */
    void (*resumeShutdownHookFxn)(void);
    /*!
     *  @brief  Pointer to an optional array of custom performance levels
     *
     *  A value of NULL indicates no custom performance levels are defined
     */
    PowerMSP432_PerfLevel * customPerfLevels;
    /*!
     *  @brief  Number of custom performance levels
     */
    uint32_t numCustom;
    /*!
     *  @brief Boolean specifying if extended performance scaling features
     *  are to be supported
     *
     *  If 'true', the following configuration parameters for crystal
     *  oscillator support are enabled.
     *
     *  If 'false', the extended configuration parameters are not suppored.
     *
     *  This Boolean is used to maintain backwards compatibility with previous
     *  releases.  Older board files won't include the extended configuration
     *  parameters, and by default, crystal oscillators won't be supported in
     *  those applications.  New board files can include the extended
     *  configuration parameters, and by including and setting this Boolean
     *  to true, the extended support with crystals will be enabled.
     */
    bool useExtendedPerf;
    /*!
     *  @brief The high frequency crystal (HFXT) frequency.
     *
     *  The source is specified via an enumerated value from cs.h, for
     *  example: CS_48MHZ, CS_24MHZ, etc.
     */
    unsigned int HFXTFREQ;
    /*!
     *  @brief The low frequency crystal (LFXT) drive level.
     *
     *  The source is specified via an enumerated value from cs.h, for
     *  example: CS_LFXT_DRIVE3, CS_LFXT_DRIVE2, etc.
     */
    unsigned int LFXTDRIVE;
    /*!
     *  @brief Boolean specifying if the HFXT pin should be configured
     *  for HFXT function
     *
     *  If 'true', the HFXT pin will be configured for HFXT function.
     *
     *  If 'false', the pin will not be configured.
     */
    bool configurePinHFXT;
    /*!
     *  @brief Boolean specifying if the HFXT pin should be configured
     *  for HFXT bypass
     *
     *  If 'true', the HFXT pin will be configured for HFXT bypass.
     *
     *  If 'false', the pin will not be configured for HFXT bypass.
     */
    bool bypassHFXT;
    /*!
     *  @brief Boolean specifying if the LFXT pin should be configured
     *  for LFXT function
     *
     *  If 'true', the LFXT pin will be configured for LFXT function.
     *
     *  If 'false', the pin will not be configured.
     */
    bool configurePinLFXT;
    /*!
     *  @brief Boolean specifying if the LFXT pin should be configured
     *  for LFXT bypass
     *
     *  If 'true', the LFXT pin will be configured for LFXT bypass.
     *
     *  If 'false', the pin will not be configured for LFXT bypass.
     */
    bool bypassLFXT;
    /*!
     *  @brief Boolean specifying if interrupts from the Clock System (CS)
     *  should be enabled for catching clock and oscillator fault conditions
     *
     *  If 'true', an application-provided interrupt service routine will be
     *  configured for catching CS interrupts.
     *
     *  If 'false', interrupts from the CS will not be enabled.
     *
     *  When a performance level is activated that uses HFXT and/or LFXT, and
     *  this Boolean is true, the corresponding interrupt sources (HFXTIE
     *  and/or LFXTIE) will be enabled at the CS.  If HFXT and/or LFXT are later
     *  disabled, the corresponding interrupt sources will likewise be
     *  disabled.
     */
    bool enableInterruptsCS;
    /*!
     *  @brief The interrupt priority to be configured for CS interrupts
     */
    unsigned int priorityInterruptsCS;
    /*!
     *  @brief  The application-provided interrupt service routine that
     *  should be configured for handling CS interrupts
     */
    void (*isrCS)(void);
} PowerMSP432_ConfigV1;

/*!
 *  @cond NODOC
 *  Internal structure defining Power module state.
 */
typedef struct PowerMSP432_ModuleState {
    List_List notifyList;
    uint32_t constraintMask;
    unsigned int state;
    unsigned int currentPerfLevel;
    bool enablePolicy;
    bool perfInitialized;
    bool initialized;
    uint8_t constraintCounts[PowerMSP432_NUMCONSTRAINTS];
    Power_PolicyFxn policyFxn;
} PowerMSP432_ModuleState;
/*! @endcond */

/*!
 *  @brief  The Power Policy initialization function.
 *
 *  This is an optional policy initialization function that will be called
 *  during application startup, during Power Manager initialization.
 */
void PowerMSP432_initPolicy(void);

/*!
 *  @brief  The SLEEP Power Policy
 *
 *  This is a simple Power Policy that will transition the device to the
 *  PowerMSP432_SLEEP state, unless there is a constraint prohibiting this.
 *  If PowerMSP432_SLEEP cannot be activated, CPU Wait For Interrupt (WFI)
 *  will be chosen instead.
 *
 *  In order for this policy to run, 1) it must be selected as the Power
 *  Policy (either by being specified as the 'policyFxn' in the
 *  PowerMSP432_config structure, or specified at runtime with
 *  Power_setPolicy()), and 2) the Power Policy must be enabled (either via
 *  'enablePolicy' in the PowerMSP432_config structure, or via a call to
 *  Power_enablePolicy() at runtime).
 */
void PowerMSP432_sleepPolicy(void);

/*!
 *  @brief  The DEEPSLEEP Power Policy
 *
 *  This is an agressive Power Policy which checks active constraints and
 *  automatically transitions the device into the deepest sleep state
 *  possible.
 *
 *  The first goal is to enter PowerMSP432_DEEPSLEEP_1; if that is not
 *  allowed, the secondary goal is PowerMSP432_DEEPSLEEP_0; if that is not
 *  allowed, the next goal is PowerMSP432_SLEEP; and finally, if that is not
 *  allowed, CPU Wait For Interrupt (WFI) will be chosen.
 *
 *  In order for this policy to run, 1) it must be selected as the Power
 *  Policy (either by being specified as the 'policyFxn' in the
 *  PowerMSP432_config structure, or specified at runtime with
 *  Power_setPolicy()), and 2) the Power Policy must be enabled (either via
 *  'enablePolicy' in the PowerMSP432_config structure, or via a call to
 *  Power_enablePolicy() at runtime).
 */
void PowerMSP432_deepSleepPolicy(void);

/*!
 *  @cond NODOC
 *  OS-specific frequency update function.  This function is called by
 *  initPerfControl() and Power_setPerformanceLevel(), after activating a
 *  performance level, to inform the OS of the new clock frequencies.
 */
void PowerMSP432_updateFreqs(PowerMSP432_Freqs *freqs);

/*!
 *  @cond NODOC
 *  OS-specific scheduler disable function.  This function is called by
 *  Power_setPerformanceLevel() when thread scheduling needs to be disabled.
 */
void PowerMSP432_schedulerDisable(void);

/*!
 *  @cond NODOC
 *  OS-specific scheduling restore function.  This function is called by
 *  Power_setPerformanceLevel() when thread scheduling activity can be restored.
 */
void PowerMSP432_schedulerRestore(void);

/* function to get the number of performance levels */
uint_fast16_t PowerMSP432_getNumPerfLevels(void);

/* function to get the frequencies for a specific performance level */
int_fast16_t PowerMSP432_getFreqs(uint_fast16_t level,
    PowerMSP432_Freqs * freqs);

/*
 * On MSP432 peripheral clocks are managed automatically by the device; there
 * is no need for software control.  Use #defines for the dependency APIs to
 * eliminate any footprint.
 */
/* \cond */
#define Power_getDependencyCount(resourceId)  Power_EINVALIDINPUT
#define Power_releaseDependency(resourceId)   Power_EINVALIDINPUT
#define Power_setDependency(resourceId)       Power_EINVALIDINPUT
/* \endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_PowerMSP432_PowerMSP432__include */
