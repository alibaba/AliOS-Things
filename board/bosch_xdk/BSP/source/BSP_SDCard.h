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
 *  @brief Handling routines provided for SD Card related interrupts
 *
 */

#ifndef SDCard_H
#define SDCard_H

#include "BCDS_BSP_SD_Card.h"

#if BCDS_FEATURE_BSP_SD_CARD

/**
 * @brief SPI receive interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void SDCard_SPIRxISR(void);
/**
 * @brief SPI transmit interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void SDCard_SPITxISR(void);

/**
 * @brief SPI rx dma interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void SDCard_SPI_RxDMAComplete(uint32_t channel, bool primary, void * user);

/**
 * @brief SPI tx dma interrupt service routine used to capture interrupts at the SPI
 * resource and to forward them to the MCU driver for processing.
 */
void SDCard_SPI_TxDMAComplete(uint32_t channel, bool primary, void * user);


#endif /* BCDS_FEATURE_BSP_SD_CARD */

#endif /* SDCard_H */


