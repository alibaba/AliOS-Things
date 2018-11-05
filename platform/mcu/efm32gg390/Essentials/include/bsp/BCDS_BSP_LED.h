/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 *
 *  @file
 *
 *  @defgroup BCDS_HAL_BSP_LED LED
 *  @brief Generic interface to the LED BSP API
 *
 *
 *  @ingroup  BCDS_HAL_BSP_IF
 *  @{
 *
 * The application needs to call BSP_LED_Connect() to establish
 * the link between the timer (internal MCU resource) and the LED peripheral
 * then it enables it via the BSP_LED_Enable() or BSP_LED_EnableAll()
 * then it is possible to drive it using BSP_LED_Switch() which takes as
 * input the brightness of the LED.
 *
 * The application can then disable the LED and disconnect it from the MCU
 * via BSP_LED_Disable() or BSP_LED_DisableAll() and
 * BSP_LED_Disconnect() functions.
 *
 *
 */

#ifndef BCDS_BSP_LED_H
#define BCDS_BSP_LED_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_LED

enum BSP_LED_Retcode_E
{
    RETCODE_LED_DISABLED = RETCODE_FIRST_CUSTOM_CODE,
};

/**
 * @brief Maps the GPIO pins To their desired function.
 * @details This function once called will map the GPIO pins to the timer in
 * order for the LEDs to operate in PWM mode

 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LED_Connect(void);


/**
 * @brief Enables the control over a particular LED.
 *
 * @details This function once called will allow the user to control the
 * specified LED
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LED_Enable(uint32_t id);

/**
 * @brief Enables the control over all LEDs.
 *
 * @details This function once called will allow the user to control all
 * on-board LEDs.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 *
 */
Retcode_T BSP_LED_EnableAll(void);

/**
 * @brief Switches on/off a particular LED.
 * @details This function is called to change the LED State .
 *
 * @param id : identifier of the desired led to switch
 * @param command : The command to execute (e.g. switch ON, switch OFF, etc. )
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
*/
Retcode_T BSP_LED_Switch(uint32_t id, uint32_t command);

/**
 * @brief Switches on/off all LEDs.
 * @details This function is called to change the State of all LEDs .
 *
 * @param command : The command to execute (e.g. switch ON, switch OFF, etc. )
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
*/
Retcode_T BSP_LED_SwitchAll(uint32_t command);


/**
 * @brief Disables the control over an LED.
 *
 * @param id : identifier of the desired LED:
 * This is defined in the BSP_BoardType.h Interface
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LED_Disable(uint32_t id);

/**
 * @brief Disables the control over all LEDs.
 *
 * This is defined in the BSP_BoardType.h Interface
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_LED_DisableAll(void);

/**
 * @brief Disconnects the LED peripherals.
 *
 * @details This function  once called will disconnect the GPIOs of the LEDs
 * and put them into low power consumption mode.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 *
 */
Retcode_T BSP_LED_Disconnect(void);

#endif /* BCDS_FEATURE_BSP_LED */

#endif /* BSP_LED_H */

/**@}*/
