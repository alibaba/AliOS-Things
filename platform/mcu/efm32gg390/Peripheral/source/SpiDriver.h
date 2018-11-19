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
#ifndef SOURCE_SPIDRIVER_H_
#define SOURCE_SPIDRIVER_H_

/* Include all headers which are needed by this file. */

/* Put the type and macro definitions here */

/* Put the function declarations here */

Retcode_T SpiDriver_initialize(SpiDriver_Handle_T spi, SpiDriver_InterfaceHandle_T interface, SpiDriver_ConfigHandle_T config);
Retcode_T SpiDriver_deinitialize(SpiDriver_Handle_T spi);
Retcode_T SpiDriver_open(SpiDriver_Handle_T spi, SpiDriver_ConfigHandle_T config);
Retcode_T SpiDriver_close(SpiDriver_Handle_T spi);
Retcode_T SpiDriver_write(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout);
Retcode_T SpiDriver_read(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout);
Retcode_T SpiDriver_transfer(SpiDriver_Handle_T spi, void * dataIn, void * dataOut, uint32_t length, uint32_t timeout);

Retcode_T SpiDriver_setInterface(SpiDriver_Handle_T spi, SpiDriver_InterfaceHandle_T interface);

#endif /* SOURCE_SPIDRIVER_H_ */

