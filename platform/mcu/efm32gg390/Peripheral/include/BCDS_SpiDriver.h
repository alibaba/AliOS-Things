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
 * @brief BCDS SPI Driver
 *
 * @details This provides peripheral device related API's for SPI communication
 */

#ifndef BCDS_SPIDRIVER_H_
#define BCDS_SPIDRIVER_H_

#include "BCDS_PeripheralDriver.h"
#include "SpiDriverPublic.h"
#include "port_hw/SpiDriverPort.h"

typedef struct SpiDriver_S *SpiDriverPtr_T;

typedef struct SpiDriverMem_S SpiDriver_T;

typedef void * SpiDriver_InterfaceHandle_T;
typedef PeripheralDriver_Handle_T SpiDriver_Handle_T;
typedef void * SpiDriver_ConfigHandle_T;

/**
 * @brief Initializes the SPI Peripheral Driver Interface for further communication
 *
 * @param [in] handle
 * driver handle
 *
 * @return  RETCODE_OK on success, or an error code otherwise
 *
 * @note    Must be called once once per peripheral unless the same is de-initialized
 */
Retcode_T SpiDriver_Initialize(SpiDriver_Handle_T spi, SpiDriver_InterfaceHandle_T interface, SpiDriver_ConfigHandle_T config);

/**
 * @brief   Deinitializes the SPI Peripheral Driver Interface
 *
 * @param [in] handle
 * driver handle
 *
 * @return  RETCODE_OK on success, or an error code otherwise
 *
 * @note    Once called, then the peripheral will not be available for use unless the same is initialized
 */
Retcode_T SpiDriver_Deinitialize(SpiDriver_Handle_T spi, uint32_t timeout);

Retcode_T SpiDriver_Open(SpiDriver_Handle_T spi, SpiDriver_ConfigHandle_T config, uint32_t timeout);

Retcode_T SpiDriver_Close(SpiDriver_Handle_T spi);

Retcode_T SpiDriver_Read(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout);

Retcode_T SpiDriver_Write(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout);

Retcode_T SpiDriver_Transfer(SpiDriver_Handle_T spi, void * dataOut, void * dataIn, uint32_t length, uint32_t timeout);

#endif /* BCDS_SPIDRIVER_H_ */
