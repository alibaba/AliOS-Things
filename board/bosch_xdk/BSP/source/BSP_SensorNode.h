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
 *  @brief Handling routines provided for sensor node related interrupts
 *
 */

#ifndef SensorNode_H
#define SensorNode_H

#include "BCDS_BSP_SensorNode.h"

#if BCDS_FEATURE_BSP_SENSOR_NODE

/**
 * @brief I2C interrupt service routine used to capture events at the I2C
 * resource and to forward them to the MCU driver for processing.
 */
void SensorNode_I2CISR(void);
/**
 * @brief Interrupt service routine used to capture interrupts at the DMA
 * receive channel and to forward them to the MCU driver for processing.
 */
void SensorNode_DMARxISR(void);

/**
 * @brief Interrupt service routine used to capture interrupts at the DMA
 * transmit channel and to forward them to the MCU driver for processing.
 */
void SensorNode_DMATxISR(void);

#endif /* BCDS_FEATURE_BSP_SENSOR_NODE */

#endif /* SensorNode_H */

