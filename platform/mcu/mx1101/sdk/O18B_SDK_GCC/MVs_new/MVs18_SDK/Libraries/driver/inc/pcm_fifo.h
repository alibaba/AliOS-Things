/**
 **************************************************************************************
 * @file    pcm_fifo.h
 *
 * @author  Aissen Li
 * @version V1.1.1
 * @date    2012/05/09
 *
 * @brief   PCM FIFO Control Functions API
 **************************************************************************************
 *
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __PCM_FIFO_H__
#define __PCM_FIFO_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
#include "pmem.h"

#define PCM_FIFO_MIN_VIABLE_REMAIN_SIZE 8 // PCM FIFO 最小可用size(PCM FIFO剩余size小于8可认为PCM_FIFO已满)

typedef enum _PCM_FIFO_ERROR_CODE
{
	PCM_FIFO_INVALID_OFFSET_ADDR = -255,
	PCM_FIFO_INVALID_DEPTH,
	PCM_FIFO_OK = 0,
} PCM_FIFO_ERROR_CODE;

/**
 * @brief  Initialize PCM FIFO
 * @param  StartAddr  offset from PMEM start address (uint8_t address), 4 bytes align, can be set as 0 ~ PMEM_OFIFO_MAX_LENGTH
 * @param  Depth      PCM FIFO depth (uint8_t address), 4 bytes align, can be set as 8 ~ PMEM_OFIFO_MAX_LENGTH-StartAddr, default value : 13'h07ff, 8K Bytes
 * @param  Status     PCM FIFO pause enable status, 0: pause disable, 1:pause enable
 * @param  ChannelExchangeFlag exchange channel flag, 0:not exchange, 1:exchange
 * @return initialize error code
 *     @arg PCM_FIFO_OK                  initialize success without no error
 *     @arg PCM_FIFO_INVALID_OFFSET_ADDR invalid offset value
 *     @arg PCM_FIFO_INVALID_DEPTH       invalid depth
 */
int32_t PcmFifoInitialize(uint32_t StartAddr, uint32_t Depth, uint32_t Status, uint32_t ChannelExchangeFlag);

/**
 * @brief  A simplified function, initialize as default value, @see PcmFifoInitialize
 * @param  NONE
 * @return initialize error code, @see PCM_FIFO_ERROR_CODE
 */
#define PcmFifoInitAsDefault()          PcmFifoInitialize(PMEM_OFIFO_START_ADDR, DEF_PCM_FIFO_LENGTH, 0, 0)

/**
 * @brief  A simplified function, @see PcmFifoInitialize
 * @param  StartAddr  offset from PMEM start address (uint8_t address), 4 bytes align, can be set as 0 ~ PMEM_OFIFO_MAX_LENGTH
 * @param  Depth      PCM FIFO depth (uint8_t address), 4 bytes align, can be set as 8 ~ PMEM_OFIFO_MAX_LENGTH-StartAddr, default value : 13'h07ff, 8K Bytes
 * @return initialize error code, @see PCM_FIFO_ERROR_CODE
 */
#define PcmFifoInit(StartAddr, Depth)   PcmFifoInitialize(StartAddr, Depth, 0, 0)

/**
 * @brief  Clear PCM FIFO
 * @param  NONE
 * @return NONE
 */
void PcmFifoClear(void);

/**
 * @brief  Get PCM FIFO empty flag
 * @param  NONE
 * @return PCM FIFO empty flag
 *     @arg 0   not empty
 *     @arg 1   empty
 */
bool PcmFifoIsEmpty(void);

/**
 * @brief  Set PCM FIFO pause status
 * @param  PCM FIFO status
 *     @arg 0   PCM FIFO pause disable
 *     @arg 1   PCM FIFO PAUSE/OFF
 * @return NONE
 */
void PcmFifoSetStatus(uint32_t Status);

#define PcmFifoPause()  PcmFifoSetStatus(1)     /**< Pause PCM OFIFO */
#define PcmFifoPlay()   PcmFifoSetStatus(0)     /**< Play  PCM OFIFO */

/*********************************************************************************
 * FIXME: The follow code may not be used in furture SDK, remove them in that case
 *********************************************************************************/

/**
 * @brief   Set PCM FIFO status
 * @param   NONE
 * @return  PCM FIFO status
 *     @arg 0  PCM FIFO pause disable
 *     @arg 1  PCM FIFO PAUSE/OFF
 */
uint32_t PcmFifoGetStatus(void);

/**
 * @brief  Set PCM FIFO exchange flag
 * @param  flag exchange flag
 *     @arg 0   No Channel Exchange
 *     @arg 1   Do Channel Exchange
 * @return NONE
 */
void PcmFifoSetChannelExchangeFlag(uint32_t Flag);

/**
 * @brief  Get PCM channel exchange flag
 * @param  NONE
 * @return exchange flag
 *     @arg 0   No Channel Exchange
 *     @arg 1   Do Channel Exchange
 */
uint32_t PcmFifoGetChannelExchangeFlag(void);

/**
 * @brief  Get PCM FIFO depth in uint32_t
 * @param  NONE
 * @return PCM FIFO depth in uint32_t
 */
uint32_t PcmFifoGetDepth(void);

/**
 * @brief  Get remain samples in PCM FIFO
 * @param  NONE
 * @return PCM FIFO data size in uint32_t
 */
uint32_t PcmFifoGetRemainSamples(void);

/**
 * @brief  Get the PCM FIFO address next step will read, offset value from start addr
 * @param  NONE
 * @return the PCM FIFO address next step will read
 */
uint32_t PcmFifoGetReadAddr(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__PCM_FIFO_H__
