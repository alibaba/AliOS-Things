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
 * @ingroup BCDS_HAL_MCU_IF
 * @defgroup BCDS_HAL_MCU_DMA  DMA Types
 *
 * @{
 *
 * @brief DMA typedefs for DMA handles and channels.
 * @details DMA typedefs for DMA handles and channels.
 *
 * @file
 *
 * ****************************************************************************/

#ifndef MCU_BCDS_MCU_DMA_H_
#define MCU_BCDS_MCU_DMA_H_

#include "BCDS_HAL.h"
/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_DMA

/**
 * @brief Typedef for the DMA handle which is used to identify the DMA controller's HW context to use.
 *        The underlaying DMA Controller structures are defined individually in the MCU specific HAL layer
 */
typedef HWHandle_T DMA_T;
/**
 * @brief Typedef for the DMA Channel handle which is used to identify a controller's DMA channel
 *        The underlaying DMA channel structures are defined individually in the MUC specific HAL layer
 */
typedef HWHandle_T DMA_Channel_T;

#endif /* BCDS_FEATURE_DMA */


#endif /* MCU_BCDS_MCU_DMA_H_ */
/** @} */
