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
#include "BCDS_HAL.h"

#ifndef DMA_H
#define DMA_H

#if BCDS_FEATURE_DMA

#include "BCDS_Basics.h"

#include "BSP_BoardType.h"
#include "BSP_BoardSettings.h"
//#include "BSP_BoardShared.h"
#include "BSP_IrqHandler.h"

#include "BCDS_MCU_DMA_Handle.h"


/* Helper Macros for configuring DMA */

#define BSP_DMA_LINK_TX(dmaChannel, deviceHandle)            deviceHandle.Link1 = (void *) &(dmaChannel)
#define BSP_DMA_LINK_RX(dmaChannel, deviceHandle)            deviceHandle.Link2 = (void *) &(dmaChannel)



/**
 * @brief   Initialize the DMA controller in the BSP context
 *
 * The function brings up the DMA Controller.
 *
 * @param[in] dma : the start address of the DMA control block memory
 *
 * @return RETCODE_OK: a valid address has been passed
 *         RETCODE_INVALID_PARAM: otherwise
 * @note The controller itself is not enabled after initialization
 *
 */
Retcode_T BSP_DMA_Initialize(void * dma);

/**
 * @brief   Enables the DMA controller in the BSP context
 *
 * The function enables the DMA controller's clock domain and enables the interrupts.
 *
 * @return RETCODE_OK: the device was successfully enabled.
 *
 */
Retcode_T BSP_DMA_Enable(void);


/**
 * @brief   Disables the DMA controller in the BSP context
 *
 * The function disables the DMA controller's clock domain and disables the interrupts.
 *
 * @return RETCODE_OK: the device was successfully enabled.
 *
 */
Retcode_T BSP_DMA_Disable(void);

#endif /* BCDS_FEATURE_DMA */
#endif /* DMA_H */


