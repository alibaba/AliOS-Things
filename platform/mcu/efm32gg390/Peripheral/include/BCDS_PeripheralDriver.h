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
 * @brief BCDS Peripheral Driver
 *
 * @details This driver provides peripheral driver related common resources
 */

#ifndef BCDS_PERIPHERALDRIVER_H_
#define BCDS_PERIPHERALDRIVER_H_

#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"

#include "PeripheralDriverPublic.h"

#define PERIPHERALDRIVER_NULLCHECK_RETURN(x) if (x == NULL) return RETCODE(RETCODE_SEVERITY_ERROR,RETCODE_NULL_POINTER);

#define PERIPHERALDRIVER_NULLCHECK_RETCODE(val) (x != NULL ? RETCODE(RETCODE_SEVERITY_ERROR,RETCODE_NULL_POINTER) : RETCODE_OK);

typedef struct PeripheralDriver_S PeripheralDriver_T, *PeripheralDriverPtr_T;

typedef void * PeripheralDriver_Handle_T; /**< a general driver handle data type */

/**
 * @brief Function to initialize the base peripheral implementation
 * @param [in] peripheral : handle to a peripheral driver structure
 * @param [in] interface  : Reference to a HW interface
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T PeripheralDriver_Initialize(PeripheralDriver_Handle_T peripheral, PeripheralDriver_InterfaceHandle_T interface);

/**
 * @brief Open a peripheral driver and claim the interface
 * @param [in] peripheral : handle to a peripheral driver structure
 * @param [in] timeout    : timeout in milliseconds
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T PeripheralDriver_Open(PeripheralDriver_Handle_T peripheral, uint32_t timeout);

/**
 * @brief Close a peripheral driver and release the interface
 * @param [in] peripheral : handle to a peripheral driver structure
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T PeripheralDriver_Close(PeripheralDriver_Handle_T peripheral);

/**
 * @brief Deinitialize a peripheral driver
 * @param [in] peripheral : handle to a peripheral driver structure
 * @param [in] timeout    : timeout in milliseconds
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T PeripheralDriver_Deinitialize(PeripheralDriver_Handle_T peripheral, uint32_t timeout);

/**
 * @brief Wait for an event to synchronize a peripheral driver
 * @param [in] peripheral : handle to a peripheral driver structure
 * @param [in] timeout    : timeout in milliseconds
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T PeripheralDriver_WaitSignal(PeripheralDriver_Handle_T peripheral, uint32_t * event, uint32_t timeout);

/**
 * @brief Deinitialize a peripheral driver
 * @param [in] peripheral : handle to a peripheral driver structure
 * @param [in] timeout    : timeout in milliseconds
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T PeripheralDriver_Signal(PeripheralDriver_Handle_T peripheral, uint32_t event);

Retcode_T PeripheralDriver_ResetSignal(PeripheralDriver_Handle_T peripheral);

#endif /* BCDS_PERIPHERALDRIVER_H_ */
