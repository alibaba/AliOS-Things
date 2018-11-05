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
 *  @brief Handling routines provided for Bluetooth module related interrupts
 *
 */

#ifndef BT_EM9301_H
#define BT_EM9301_H

#include "BCDS_BSP_BT_EM9301.h"

#if BCDS_FEATURE_BSP_BT_EM9301

/**
 * @brief UART transmit interrupt service routine used to capture interrupts at the UART
 * resource and to forward them to the MCU driver for processing.
 */
void BT_EM9301_UART_TxISR(void);

/**
 * @brief UART receive interrupt service routine used to capture interrupts at the UART
 * resource and to forward them to the MCU driver for processing.
 */
void BT_EM9301_UART_RxISR(void);
/**
 * @brief UART interrupt service routine used to capture interrupts at the DMA
 * receive channel and to forward them to the MCU driver for processing.
 */
void BT_EM9301_UART_DMARxISR(void);

/**
 * @brief UART interrupt service routine used to capture interrupts at the DMA
 * transmit channel and to forward them to the MCU driver for processing.
 */
void BT_EM9301_UART_DMATxISR(void);

#endif /* BCDS_FEATURE_BSP_BT_EM9301 */

#endif /* BT_EM9301_H */

/**@}*/

