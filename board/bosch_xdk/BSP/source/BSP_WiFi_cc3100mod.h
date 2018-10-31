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
 *  @brief Handling routines provided for WiFi module related interrupts
 *
 */

#ifndef WiFi_cc3100mod_H
#define WiFi_cc3100mod_H

#include "BCDS_BSP_WiFi_CC3100MOD.h"

#if BCDS_FEATURE_BSP_WIFI_CC3100MOD

/**
 * @brief SPI receive interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void WiFi_CC3100MOD_SPIRxISR(void);
/**
 * @brief SPI transmit interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void WiFi_CC3100MOD_SPITxISR(void);


/**
 * @brief SPI rx dma interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void WiFi_CC3100MOD_SPI_RxDMAComplete(uint32_t channel, bool primary, void * user);

/**
 * @brief SPI tx dma interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void WiFi_CC3100MOD_SPI_TxDMAComplete(uint32_t channel, bool primary, void * user);

#endif /* BCDS_FEATURE_BSP_WIFI_CC3100MOD */

#endif /* WiFi_cc3100mod_H */


