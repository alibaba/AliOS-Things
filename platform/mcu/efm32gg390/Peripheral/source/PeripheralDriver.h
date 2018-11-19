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
#ifndef SOURCE_PRIVATE_PERIPHERALDRIVER_H_
#define SOURCE_PRIVATE_PERIPHERALDRIVER_H_

#include "BCDS_Basics.h"
#include "BCDS_Assert.h"
#include "BCDS_PeripheralDriver.h"

/* Include all headers which are needed by this file. */

/* Put the type and macro definitions here */

/* Put the function declarations here */

void PeripheralDriver_setInterface(PeripheralDriver_Handle_T peripheral, PeripheralDriver_InterfaceHandle_T interface);

PeripheralDriver_InterfaceHandle_T PeripheralDriver_getInterface(PeripheralDriver_Handle_T peripheral);

Retcode_T PeripheralDriver_initialize(PeripheralDriver_Handle_T peripheral);

Retcode_T PeripheralDriver_open(PeripheralDriver_Handle_T peripheral, uint32_t timeout);

Retcode_T PeripheralDriver_close(PeripheralDriver_Handle_T peripheral);

Retcode_T PeripheralDriver_deinitialize(PeripheralDriver_Handle_T peripheral, uint32_t timeout);

Retcode_T PeripheralDriver_waitSignal(PeripheralDriver_Handle_T peripheral, uint32_t *event, uint32_t timeout);

Retcode_T PeripheralDriver_signal(PeripheralDriver_Handle_T peripheral, uint32_t event);

Retcode_T PeripheralDriver_resetSignal(PeripheralDriver_Handle_T peripheral);

#endif /* SOURCE_PRIVATE_PERIPHERALDRIVER_H_ */

