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
 * @file
 * @brief BCDS UART Driver
 *
 * @details This provides peripheral driver related API's for UART communication
 */

#ifndef BCDS_UARTDRIVER_H_
#define BCDS_UARTDRIVER_H_

#include "BCDS_PeripheralDriver.h"

typedef struct UartDriver_S
{
    PeripheralDriver_T PeripheralDriver;
} UartDriver_T, *UartDriver_Handle_T;

/**
 * @brief Initializes the UART Peripheral Driver Interface for further communication
 *
 * @param [in] handle
 * driver handle
 *
 * @return  RETCODE_OK on success, or an error code otherwise
 *
 * @note    Must be called once once per peripheral unless the same is de-initialized
 */
Retcode_T UartDriver_Initialize(PeripheralDriver_Handle_T handle);

/**
 * @brief   Deinitializes the UART Peripheral Driver Interface
 *
 * @param [in] handle
 * driver handle
 *
 * @return  RETCODE_OK on success, or an error code otherwise
 *
 * @note    Once called, then the peripheral will not be available for use unless the same is initialized
 */
Retcode_T UartDriver_Deinitialize(PeripheralDriver_Handle_T handle);

#endif /* BCDS_UARTDRIVER_H_ */
