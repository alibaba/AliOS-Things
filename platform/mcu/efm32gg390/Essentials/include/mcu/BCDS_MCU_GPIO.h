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
 * @ingroup BCDS_HAL_MCU_IF
 * @defgroup BCDS_HAL_MCU_GPIO  GPIO
 *
 * @{
 * @brief Unified GPIO API
 * @details Interface for accessing GPIOs, i.e. initialize and deinitialize, read, write, toggle.
 *
 * @file
 *
 * ****************************************************************************/

#ifndef BCDS_MCU_GPIO_H_
#define BCDS_MCU_GPIO_H_

#include "BCDS_HAL.h"
/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_GPIO

/**
 * @brief Typedef for the GPIO handle which is used to identify the GPIO HW context to use.
 */
typedef HWHandle_T GPIO_T;


enum MCU_GPIO_PinState_E
{
    MCU_GPIO_PIN_STATE_LOW,
    MCU_GPIO_PIN_STATE_HIGH,
    MCU_GPIO_PIN_STATE_TRISTATE,
    MCU_GPIO_PIN_STATE_OUT_OF_RANGE
};
typedef enum MCU_GPIO_PinState_E MCU_GPIO_PinState_T;
/**
 * @brief Initializes the GPIO context
 * @param[in] gpio : the GPIO pin context to be initialized
 * @retval RETCODE_OK if initialization has been successful
 */
Retcode_T MCU_GPIO_Initialize(GPIO_T gpio);

/**
 * @brief De-Initializes the GPIO context
  */
Retcode_T MCU_GPIO_DeInitialize(GPIO_T gpio);

/**
 * @brief modify the output level of a GPIO pin from the GPIO perspective
 * @param[in] gpio     : the GPIO pin context (optional can be null, when not used)
 * @param[in] gpioLevel: the GPIO pin output level to be set
 * @return : when successful this function will return RETCODE_OK, a custom error otherwise
 */
Retcode_T MCU_GPIO_WritePin(GPIO_T gpio, MCU_GPIO_PinState_T gpioLevel );

/**
 * @brief Read the output level of a GPIO pin from the GPIO perspective
 * @param[in] gpio     : the GPIO context (optional can be null, when not used)
 * @param[out] gpioLevel: the GPIO's pin current input level being read
 * @return : when successful this function will return RETCODE_OK, a custom error otherwise
 */
Retcode_T MCU_GPIO_ReadPin(GPIO_T gpio, MCU_GPIO_PinState_T *gpioLevel );

/**
 * @brief Toggle the output level of a GPIO pin
 * @param[in] gpio     : the GPIO context (optional can be null, when not used)
 * return : when successful this function will return RETCODE_OK, a custom error otherwise
 */
Retcode_T MCU_GPIO_TogglePin(GPIO_T gpio);


#endif /* BCDS_FEATURE_GPIO */
#endif /* BCDS_MCU_GPIO_H_ */
/**  @} */

