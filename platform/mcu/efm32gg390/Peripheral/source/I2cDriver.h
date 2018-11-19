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
 * @brief Add a brief description here.
 *
 * @details Put here the documentation of this header file. Explain the interface exposed
 * by this header, e.g. what is the purpose of use, how to use it, etc.
 */
#ifndef SOURCE_I2CDRIVER_H_
#define SOURCE_I2CDRIVER_H_

/* Include all headers which are needed by this file. */

/* Put the type and macro definitions here */

/* Put the function declarations here */

Retcode_T I2cDriver_initialize(I2cDriver_Handle_T i2c, I2cDriver_InterfaceHandle_T interface, I2cDriver_ConfigHandle_T config);
Retcode_T I2cDriver_deinitialize(I2cDriver_Handle_T i2c);

Retcode_T I2cDriver_open(I2cDriver_Handle_T i2c, I2cDriver_ConfigHandle_T config);
Retcode_T I2cDriver_close(I2cDriver_Handle_T i2c);
Retcode_T I2cDriver_write(I2cDriver_Handle_T i2c, uint16_t deviceAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending);
Retcode_T I2cDriver_read(I2cDriver_Handle_T i2c, uint16_t deviceAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending);

#endif /* SOURCE_I2CDRIVER_H_ */

