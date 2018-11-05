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


/*
*
* @brief
* Defines MCU DMA handles
*
* @details
* MCU functions do always have at least an integer parameter of type DMA_T.
* This integer is casted by MCU to a pointer to MCU_DMA_handle_T.
* The data pointed to by USART_TypeDef* is managed in BSP.
*/

#ifndef MCU_EFM32_BCDS_MCU_DMA_HANDLE_H_
#define MCU_EFM32_BCDS_MCU_DMA_HANDLE_H_

#include "BCDS_HAL.h"
#include "em_dma.h"


struct MCU_DMA_Controller_S
{
	DMA_TypeDef * DmaInstance;
	void * DmaDescriptors;
};

struct MCU_DMA_Channel_S
{
	uint32_t ChannelId;                    /**< an ID that represents the channel        */
	struct DMA_Controller_S * Controller;  /**< pointer to the associated DMA controller */
	DMA_CfgChannel_TypeDef    Config;      /**< channel configuration */
	DMA_CB_TypeDef            CallBack;    /**< interrupt callback function structure*/
	void * Link1;                          /**< general component link1 */
	void * Link2;                          /**< general component link2 */
};

#endif /* MCU_EFM32_BCDS_MCU_DMA_HANDLE_H_ */
