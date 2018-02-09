/** mbed Microcontroller Library
  ******************************************************************************
  * @file    dma_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed GDMA API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ****************************************************************************** 
  */
#ifndef MBED_GDMA_API_H
#define MBED_GDMA_API_H
 
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup dma DMA
 *  @ingroup    hal
 *  @brief      dma functions
 *  @{
 */

///@name Ameba Common 
///@{
typedef struct gdma_s gdma_t;	
	
typedef void (*dma_irq_handler)(uint32_t id);
/**
 *  @brief Initial the GDMA
 *  @param dma_obj: the GDMA object
 *  @param handler: the callback function for a DMA transfer complete.
 *  @param id: the argument of the callback function.
 *  @retval None
 *         
 */
void dma_memcpy_init(gdma_t *dma_obj, dma_irq_handler handler, uint32_t id);

/**
 *  @brief De-Initial the GDMA
 *  @param dma_obj: the GDMA object
 *  @retval None
 *         
 */
void dma_memcpy_deinit(gdma_t *dma_obj);

/**
 *  @brief To do a memory copy by DMA
 *  @param dma_obj: the GDMA object
 *  @param dst: destination memory address
 *  @param src: source memory address
 *  @param len: copy data length
 *  @retval None       
 */
void dma_memcpy(gdma_t *dma_obj, void *dst, void* src, uint32_t len);

///@}

#if CONFIG_PLATFORM_8195A
///@name Ameba1 Only 
///@{
/**
 *  @brief   Initial the GDMA
 *  @param dma_obj: the GDMA object
 *  @param handler: the callback function for a DMA transfer complete.
 *  @param id: the argument of the callback function.
 *  @retval None
 *         
 */
void dma_memcpy_aggr_init(gdma_t * dma_obj, dma_irq_handler handler, uint32_t id);

void dma_memcpy_aggr(gdma_t * dma_obj, PHAL_GDMA_BLOCK block_info);
///@}
#endif //CONFIG_PLATFORM_8195A

/**@}*/

#ifdef __cplusplus
}
#endif

#endif  // end of "#define MBED_GDMA_API_H"
