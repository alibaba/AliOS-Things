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

/*
 *
 * @brief
 * Defines MCU Gpio handle
 *
 * @details
 * MCU functions do always have at least an integer parameter of type GPIO_T.
 */

#ifndef BCDS_MCU_GPIO_HANDLE_H_
#define BCDS_MCU_GPIO_HANDLE_H_

#include "BCDS_MCU_GPIO.h"

#if BCDS_FEATURE_GPIO

#include "em_gpio.h"

/**
 * @brief predeclaration of MCU_GPIO_Handle_S structure
 */

struct MCU_GPIO_Handle_S
{
    GPIO_Port_TypeDef Port; /*!< GPIO ports identificator */
    uint32_t Pin;           /*!< GPIO pin identificator  */
    GPIO_Mode_TypeDef Mode; /*!< GPIO Pin mode  */
    uint32_t InitialState;  /*!< GPIO Pin Default State */
    void * Link1;           /*!< General purpose link register1  */
};

/*!This data type represent a handle type for the MCU_GPIO_Handle_S */
typedef struct MCU_GPIO_Handle_S MCU_GPIO_Handle_T;
#endif /* BCDS_FEATURE_GPIO */

#endif /* BCDS_MCU_GPIO_HANDLE_H_ */

