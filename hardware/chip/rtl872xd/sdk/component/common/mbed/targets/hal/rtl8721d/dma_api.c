/** mbed Microcontroller Library
  ******************************************************************************
  * @file    dma_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed GDMA API for memcopy
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "dma_api.h"
#include "cmsis.h"
/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_GDMA
 *  @brief      MBED_GDMA driver modules.
 *  @{
 */

/** @defgroup MBED_GDMA_Exported_Functions MBED_GDMA Exported Functions
  * @{
  */
static void dma_memcpy_int(void *pData)
{
	gdma_t *dma_obj = (gdma_t *)pData;

	/* Clean Auto Reload Bit */
	GDMA_ChCleanAutoReload(dma_obj->index, dma_obj->ch_num, CLEAN_RELOAD_DST);

	/* Clear Pending ISR */
	GDMA_ClearINT(dma_obj->index, dma_obj->ch_num);

	GDMA_Cmd(dma_obj->index, dma_obj->ch_num, DISABLE);

	if (dma_obj->user_cb != NULL) {
		dma_obj->user_cb((VOID*)dma_obj->user_cb_data);
	}
}

/**
 *  @brief Initial the GDMA
 *  @param dma_obj: the GDMA object
 *  @param handler: the callback function for a DMA transfer complete.
 *  @param id: the argument of the callback function.
 *  @retval None
 *         
 */
void dma_memcpy_init(gdma_t *dma_obj, dma_irq_handler handler, uint32_t id)
{
	u8 ch_num;

	dma_obj->index = 0;
	ch_num = GDMA_ChnlAlloc(dma_obj->index, (IRQ_FUN) dma_memcpy_int, (u32)dma_obj, 10);
	if (0xFF == ch_num) {        
		DBG_8195A("%s: Cannot allocate a GDMA Channel\n", __FUNCTION__);
		return;
	}

	dma_obj->user_cb = (IRQ_FUN)handler;
	dma_obj->user_cb_data = id;
	dma_obj->ch_num = ch_num;
}

/**
 *  @brief De-Initial the GDMA
 *  @param dma_obj: the GDMA object
 *  @retval None
 *         
 */
void dma_memcpy_deinit(gdma_t *dma_obj)
{
	GDMA_ChnlFree(dma_obj->index, dma_obj->ch_num);
}

/**
 *  @brief To do a memory copy by DMA
 *  @param dma_obj: the GDMA object
 *  @param dst: destination memory address
 *  @param src: source memory address
 *  @param len: copy data length
 *  @retval None       
 */
void dma_memcpy(gdma_t *dma_obj, void *dst, void* src, uint32_t len)
{
	GDMA_InitTypeDef GDMA_InitStruct;

	_memset((void *)&GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct.GDMA_ChNum = dma_obj->ch_num;
	GDMA_InitStruct.GDMA_Index = dma_obj->index;
	GDMA_InitStruct.GDMA_IsrType = (TransferType|ErrType);

	DBG_PRINTF(MODULE_GDMA, LEVEL_INFO, "%s: ==> Src=0x%x Dst=0x%x Len=%d\r\n", __FUNCTION__, src, dst, len);
	
	if ((((u32)src & 0x03)==0) &&	(((u32)dst & 0x03)==0) && ((len & 0x03)== 0)) {
		/* 4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct.GDMA_SrcMsize   = MsizeEight;
		GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct.GDMA_DstMsize = MsizeEight;
		GDMA_InitStruct.GDMA_DstDataWidth = TrWidthFourBytes;
		GDMA_InitStruct.GDMA_BlockSize = len >> 2;
	} else {
		GDMA_InitStruct.GDMA_SrcMsize   = MsizeEight;
		GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct.GDMA_DstMsize = MsizeEight;
		GDMA_InitStruct.GDMA_DstDataWidth = TrWidthOneByte;
		GDMA_InitStruct.GDMA_BlockSize = len;
	}

	GDMA_InitStruct.GDMA_SrcAddr = (u32)src;
	GDMA_InitStruct.GDMA_DstAddr = (u32)dst;                

	GDMA_Init(dma_obj->index, dma_obj->ch_num, &GDMA_InitStruct);
	GDMA_Cmd(dma_obj->index, dma_obj->ch_num, ENABLE);
}
/** 
  * @}
  */

/** 
  * @}
  */


/** 
  * @}
  */
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
