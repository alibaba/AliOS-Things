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

#ifndef SOURCE_PRIVATE_PERIPHERALDRIVERSTRUCT_H_
#define SOURCE_PRIVATE_PERIPHERALDRIVERSTRUCT_H_

#define ALIOS 1
#define BCDS_TARGET_OS ALIOS

#if (BCDS_TARGET_OS == FREERTOS)
#include "PeripheralDriver_FreeRTOS.h"
#elif (BCDS_TARGET_OS == ALIOS)
#include "PeripheralDriver_AliOs.h"
#else
#include "PeripheralDriver_NoOS.h"
#endif

#endif /* SOURCE_PRIVATE_PERIPHERALDRIVERSTRUCT_H_ */
