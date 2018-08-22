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
 *  @file       GPIOMSP432.h
 *
 *  @brief      GPIO driver implementation for MSP432 devices
 *
 *  The GPIO header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/GPIO.h>
 *  #include <ti/drivers/gpio/GPIOMSP432.h>
 *  @endcode
 *
 *  Refer to @ref GPIO.h for a complete description of the GPIO
 *  driver APIs provided and examples of their use.
 *
 *  ### MSP432 GPIO Driver Configuration #
 *
 *  In order to use the GPIO APIs, the application is required
 *  to provide 3 structures in the Board.c file:
 *
 *  1.  An array of @ref GPIO_PinConfig elements that defines the
 *  initial configuration of each pin used by the application. A
 *  pin is referenced in the application by its corresponding index in this
 *  array. The pin type (that is, INPUT/OUTPUT), its initial state (that is
 *  OUTPUT_HIGH or LOW), interrupt behavior (RISING/FALLING edge, etc.)
 *  (see @ref GPIO_PinConfigSettings), and
 *  device specific pin identification (see @ref GPIOMSP432_PinConfigIds)
 *  are configured in each element of this array.
 *  Below is an MSP432 device specific example of the GPIO_PinConfig array:
 *  @code
 *  //
 *  // Array of Pin configurations
 *  // NOTE: The order of the pin configurations must coincide with what was
 *  //       defined in MSP_EXP432P401R.h
 *  // NOTE: Pins not used for interrupts should be placed at the end of the
 *  //       array.  Callback entries can be omitted from callbacks array to
 *  //       reduce memory usage.
 *  //
 *  GPIO_PinConfig gpioPinConfigs[] = {
 *      // Input pins
 *      // MSP_EXP432P401R_GPIO_S1
 *      GPIOMSP432_P1_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
 *      // MSP_EXP432P401R_GPIO_S2
 *      GPIOMSP432_P1_4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
 *
 *      // Output pins
 *      // MSP_EXP432P401R_GPIO_LED1
 *      GPIOMSP432_P1_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW,
 *      // MSP_EXP432P401R_GPIO_LED_RED
 *      GPIOMSP432_P2_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
 *  };
 *  @endcode
 *
 *  2.  An array of @ref GPIO_CallbackFxn elements that is used to store
 *  callback function pointers for GPIO pins configured with interrupts.
 *  The indexes for these array elements correspond to the pins defined
 *  in the @ref GPIO_PinConfig array. These function pointers can be defined
 *  statically by referencing the callback function name in the array
 *  element, or dynamically, by setting the array element to NULL and using
 *  GPIO_setCallback() at runtime to plug the callback entry.
 *  Pins not used for interrupts can be omitted from the callback array to
 *  reduce memory usage (if they are placed at the end of the @ref
 *  GPIO_PinConfig array). The callback function syntax should match the
 *  following:
 *  @code
 *  void (*GPIO_CallbackFxn)(unsigned int index);
 *  @endcode
 *  The index parameter is the same index that was passed to
 *  GPIO_setCallback(). This allows the same callback function to be used
 *  for multiple GPIO interrupts, by using the index to identify the GPIO
 *  that caused the interrupt.
 *  Below is an MSP432 device specific example of the @ref GPIO_CallbackFxn
 *  array:
 *  @code
 *  //
 *  // Array of callback function pointers
 *  // NOTE: The order of the pin configurations must coincide with what was
 *  //       defined in MSP_EXP432P401R.h
 *  // NOTE: Pins not used for interrupts can be omitted from callbacks array
 *  //       to reduce memory usage (if placed at end of gpioPinConfigs
 *  //       array).
 *  //
 *  GPIO_CallbackFxn gpioCallbackFunctions[] = {
 *      // MSP_EXP432P401R_GPIO_S1
 *      NULL,
 *      // MSP_EXP432P401R_GPIO_S2
 *      NULL
 *  };
 *  @endcode
 *
 *  3.  The device specific GPIOMSP432_Config structure that tells the GPIO
 *  driver where the two aforementioned arrays are and the number of elements
 *  in each. The interrupt priority of all pins configured to generate
 *  interrupts is also specified here. Values for the interrupt priority are
 *  device-specific. You should be well-acquainted with the interrupt
 *  controller used in your device before setting this parameter to a
 *  non-default value. The sentinel value of (~0) (the default value) is
 *  used to indicate that the lowest possible priority should be used.
 *  Below is an example of an initialized GPIOMSP432_Config
 *  structure:
 *  @code
 *  const GPIOMSP432_Config GPIOMSP432_config = {
 *      .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
 *      .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
 *      .numberOfPinConfigs = sizeof(gpioPinConfigs)/sizeof(GPIO_PinConfig),
 *      .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
 *      .intPriority = (~0)
 *  };
 *  @endcode
 *
 *  #### Drive Strength Limitation ####
 *  Only pins 2.0 thru 2.3 support high output drive strength. All other pins
 *  only support low drive strength. For pins that do not support high
 *  output drive strength, the GPIO_CFG_OUT_STR_XXX setting is ignored.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_GPIOMSP432__include
#define ti_drivers_GPIOMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <ti/drivers/GPIO.h>

/*!
 *  @brief  GPIO device specific driver configuration structure
 *
 *  The device specific GPIOMSP432_Config structure that tells the GPIO
 *  driver where the two aforementioned arrays are and the number of elements
 *  in each. The interrupt priority of all pins configured to generate
 *  interrupts is also specified here. Values for the interrupt priority are
 *  device-specific. You should be well-acquainted with the interrupt
 *  controller used in your device before setting this parameter to a
 *  non-default value. The sentinel value of (~0) (the default value) is
 *  used to indicate that the lowest possible priority should be used.
 *
 *  Below is an example of an initialized GPIOMSP432_Config
 *  structure:
 *  @code
 *  const GPIOMSP432_Config GPIOMSP432_config = {
 *      .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
 *      .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
 *      .numberOfPinConfigs = sizeof(gpioPinConfigs)/sizeof(GPIO_PinConfig),
 *      .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
 *      .intPriority = (~0)
 *  };
 *  @endcode
 */

/*
 * Device specific interpretation of the GPIO_PinConfig content
 */
typedef struct PinConfig {
    uint8_t pin;
    uint8_t port;
    uint16_t config;
} PinConfig;


typedef struct GPIOMSP432_Config {
    /*! Pointer to the board's GPIO_PinConfig array */
    GPIO_PinConfig  *pinConfigs;

    /*! Pointer to the board's GPIO_CallbackFxn array */
    GPIO_CallbackFxn  *callbacks;

    /*! number of GPIO_PinConfigs defined */
    uint32_t numberOfPinConfigs;

    /*! number of GPIO_Callbacks defined */
    uint32_t numberOfCallbacks;

    /*!
     *  Interrupt priority used for call back interrupts
     *
     *  intPriority is the interrupt priority, as defined by the
     *  underlying OS.  It is passed unmodified to the underlying OS's
     *  interrupt handler creation code, so you need to refer to the OS
     *  documentation for usage.  If the driver uses the ti.dpl
     *  interface instead of making OS calls directly, then the HwiP port
     *  handles the interrupt priority in an OS specific way.  In the case
     *  of the SYS/BIOS port, intPriority is passed unmodified to Hwi_create().
     *
     *  A setting of ~0 will configure the lowest possible priority.
     */
    uint32_t intPriority;
} GPIOMSP432_Config;

/*!
 *  \defgroup GPIOMSP432_PinConfigIds GPIO pin identification macros used to configure GPIO pins
 *  @{
 */

/**
 *  @name Device specific GPIO port/pin identifiers to be used within the board's GPIO_PinConfig table.
 *  @{
*/
#define GPIOMSP432_EMPTY_PIN 0x0000 /*!< @hideinitializer */

/**
 *  @name Only ports 1-6 support interrupts and can be configured with callbacks
 *  @{
 */
#define GPIOMSP432_P1_0    0x101    /*!< @hideinitializer */
#define GPIOMSP432_P1_1    0x102    /*!< @hideinitializer */
#define GPIOMSP432_P1_2    0x104    /*!< @hideinitializer */
#define GPIOMSP432_P1_3    0x108    /*!< @hideinitializer */
#define GPIOMSP432_P1_4    0x110    /*!< @hideinitializer */
#define GPIOMSP432_P1_5    0x120    /*!< @hideinitializer */
#define GPIOMSP432_P1_6    0x140    /*!< @hideinitializer */
#define GPIOMSP432_P1_7    0x180    /*!< @hideinitializer */

#define GPIOMSP432_P2_0    0x201    /*!< @hideinitializer */
#define GPIOMSP432_P2_1    0x202    /*!< @hideinitializer */
#define GPIOMSP432_P2_2    0x204    /*!< @hideinitializer */
#define GPIOMSP432_P2_3    0x208    /*!< @hideinitializer */
#define GPIOMSP432_P2_4    0x210    /*!< @hideinitializer */
#define GPIOMSP432_P2_5    0x220    /*!< @hideinitializer */
#define GPIOMSP432_P2_6    0x240    /*!< @hideinitializer */
#define GPIOMSP432_P2_7    0x280    /*!< @hideinitializer */

#define GPIOMSP432_P3_0    0x301    /*!< @hideinitializer */
#define GPIOMSP432_P3_1    0x302    /*!< @hideinitializer */
#define GPIOMSP432_P3_2    0x304    /*!< @hideinitializer */
#define GPIOMSP432_P3_3    0x308    /*!< @hideinitializer */
#define GPIOMSP432_P3_4    0x310    /*!< @hideinitializer */
#define GPIOMSP432_P3_5    0x320    /*!< @hideinitializer */
#define GPIOMSP432_P3_6    0x340    /*!< @hideinitializer */
#define GPIOMSP432_P3_7    0x380    /*!< @hideinitializer */

#define GPIOMSP432_P4_0    0x401    /*!< @hideinitializer */
#define GPIOMSP432_P4_1    0x402    /*!< @hideinitializer */
#define GPIOMSP432_P4_2    0x404    /*!< @hideinitializer */
#define GPIOMSP432_P4_3    0x408    /*!< @hideinitializer */
#define GPIOMSP432_P4_4    0x410    /*!< @hideinitializer */
#define GPIOMSP432_P4_5    0x420    /*!< @hideinitializer */
#define GPIOMSP432_P4_6    0x440    /*!< @hideinitializer */
#define GPIOMSP432_P4_7    0x480    /*!< @hideinitializer */

#define GPIOMSP432_P5_0    0x501    /*!< @hideinitializer */
#define GPIOMSP432_P5_1    0x502    /*!< @hideinitializer */
#define GPIOMSP432_P5_2    0x504    /*!< @hideinitializer */
#define GPIOMSP432_P5_3    0x508    /*!< @hideinitializer */
#define GPIOMSP432_P5_4    0x510    /*!< @hideinitializer */
#define GPIOMSP432_P5_5    0x520    /*!< @hideinitializer */
#define GPIOMSP432_P5_6    0x540    /*!< @hideinitializer */
#define GPIOMSP432_P5_7    0x580    /*!< @hideinitializer */

#define GPIOMSP432_P6_0    0x601    /*!< @hideinitializer */
#define GPIOMSP432_P6_1    0x602    /*!< @hideinitializer */
#define GPIOMSP432_P6_2    0x604    /*!< @hideinitializer */
#define GPIOMSP432_P6_3    0x608    /*!< @hideinitializer */
#define GPIOMSP432_P6_4    0x610    /*!< @hideinitializer */
#define GPIOMSP432_P6_5    0x620    /*!< @hideinitializer */
#define GPIOMSP432_P6_6    0x640    /*!< @hideinitializer */
#define GPIOMSP432_P6_7    0x680    /*!< @hideinitializer */

/** @} */

/**
 *  @name Ports 7-10 and 'J' do NOT support interrupts.
 *  @{
 */
#define GPIOMSP432_P7_0    0x701    /*!< @hideinitializer */
#define GPIOMSP432_P7_1    0x702    /*!< @hideinitializer */
#define GPIOMSP432_P7_2    0x704    /*!< @hideinitializer */
#define GPIOMSP432_P7_3    0x708    /*!< @hideinitializer */
#define GPIOMSP432_P7_4    0x710    /*!< @hideinitializer */
#define GPIOMSP432_P7_5    0x720    /*!< @hideinitializer */
#define GPIOMSP432_P7_6    0x740    /*!< @hideinitializer */
#define GPIOMSP432_P7_7    0x780    /*!< @hideinitializer */

#define GPIOMSP432_P8_0    0x801    /*!< @hideinitializer */
#define GPIOMSP432_P8_1    0x802    /*!< @hideinitializer */
#define GPIOMSP432_P8_2    0x804    /*!< @hideinitializer */
#define GPIOMSP432_P8_3    0x808    /*!< @hideinitializer */
#define GPIOMSP432_P8_4    0x810    /*!< @hideinitializer */
#define GPIOMSP432_P8_5    0x820    /*!< @hideinitializer */
#define GPIOMSP432_P8_6    0x840    /*!< @hideinitializer */
#define GPIOMSP432_P8_7    0x880    /*!< @hideinitializer */

#define GPIOMSP432_P9_0    0x901    /*!< @hideinitializer */
#define GPIOMSP432_P9_1    0x902    /*!< @hideinitializer */
#define GPIOMSP432_P9_2    0x904    /*!< @hideinitializer */
#define GPIOMSP432_P9_3    0x908    /*!< @hideinitializer */
#define GPIOMSP432_P9_4    0x910    /*!< @hideinitializer */
#define GPIOMSP432_P9_5    0x920    /*!< @hideinitializer */
#define GPIOMSP432_P9_6    0x940    /*!< @hideinitializer */
#define GPIOMSP432_P9_7    0x980    /*!< @hideinitializer */

#define GPIOMSP432_P10_0   0xa01    /*!< @hideinitializer */
#define GPIOMSP432_P10_1   0xa02    /*!< @hideinitializer */
#define GPIOMSP432_P10_2   0xa04    /*!< @hideinitializer */
#define GPIOMSP432_P10_3   0xa08    /*!< @hideinitializer */
#define GPIOMSP432_P10_4   0xa10    /*!< @hideinitializer */
#define GPIOMSP432_P10_5   0xa20    /*!< @hideinitializer */
#define GPIOMSP432_P10_6   0xa40    /*!< @hideinitializer */
#define GPIOMSP432_P10_7   0xa80    /*!< @hideinitializer */

#define GPIOMSP432_PJ_0    0xb01    /*!< @hideinitializer */
#define GPIOMSP432_PJ_1    0xb02    /*!< @hideinitializer */
#define GPIOMSP432_PJ_2    0xb04    /*!< @hideinitializer */
#define GPIOMSP432_PJ_3    0xb08    /*!< @hideinitializer */
#define GPIOMSP432_PJ_4    0xb10    /*!< @hideinitializer */
#define GPIOMSP432_PJ_5    0xb20    /*!< @hideinitializer */
#define GPIOMSP432_PJ_6    0xb40    /*!< @hideinitializer */
#define GPIOMSP432_PJ_7    0xb80    /*!< @hideinitializer */

/** @} */
/** @} */
/** @} end of GPIOMSP432_PinConfigIds group */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_GPIOMSP432__include */
