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
 *
 *  @file
 *
 *  @brief Handling routines provided for MAX31865 related interrupts
 *
 */
#ifndef BSP_MAX31865_H_
#define BSP_MAX31865_H_

#include "BCDS_BSP_Max31865.h"

#if BCDS_FEATURE_BSP_MAX31865

#define MAX31865_SPI_BAUDRATE                                                   UINT32_C(1000000)
#define MAX31865_CS_PIN                                                         BSP_EXTENSIONPORT_GPIO_PD8

#endif /* BCDS_FEATURE_BSP_MAX31865 */

#endif /* BSP_MAX31865_H_ */
