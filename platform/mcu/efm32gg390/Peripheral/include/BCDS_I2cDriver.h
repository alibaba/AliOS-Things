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
 * @brief BCDS I2C Driver
 *
 * @details This provides peripheral device related API's for I2C communication
 */

#ifndef BCDS_I2CDRIVER_H_
#define BCDS_I2CDRIVER_H_

#include "BCDS_PeripheralDriver.h"
#include "I2cDriverPublic.h"
#include "port_hw/I2cDriverPort.h"

typedef struct I2cDriver_S *I2cDriverPtr_T;

typedef struct I2cDriverMem_S I2cDriver_T;

typedef PeripheralDriver_Handle_T I2cDriver_Handle_T;
typedef void * I2cDriver_InterfaceHandle_T;
typedef void * I2cDriver_ConfigHandle_T;

/**
 * @brief Initializes the I2C Peripheral Driver Interface for further communication
 *
 * @param [in] handle
 * driver handle
 *
 * @return  RETCODE_OK on success, or an error code otherwise
 *
 * @note    Must be called once once per peripheral unless the same is de-initialized
 */
Retcode_T I2cDriver_Initialize(I2cDriver_Handle_T i2c, I2cDriver_InterfaceHandle_T interface, I2cDriver_ConfigHandle_T config);

/**
 * @brief   Deinitializes the I2C Peripheral Driver Interface
 *
 * @param [in] handle
 * driver handle
 *
 * @return  RETCODE_OK on success, or an error code otherwise
 *
 * @note    Once called, then the peripheral will not be available for use unless the same is initialized
 */
Retcode_T I2cDriver_Deinitialize(I2cDriver_Handle_T i2c, uint32_t timeout);

Retcode_T I2cDriver_Open(I2cDriver_Handle_T i2c, I2cDriver_ConfigHandle_T config, uint32_t timeout);
Retcode_T I2cDriver_Close(I2cDriver_Handle_T i2c);

Retcode_T I2cDriver_Write(I2cDriver_Handle_T i2c, uint16_t deviceAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending);
Retcode_T I2cDriver_Read(I2cDriver_Handle_T i2c, uint16_t deviceAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending);

#endif /* BCDS_I2CDRIVER_H_ */
