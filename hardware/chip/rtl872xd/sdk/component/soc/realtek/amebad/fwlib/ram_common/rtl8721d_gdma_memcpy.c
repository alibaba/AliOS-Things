/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "ameba_soc.h"

struct gdma_memcopy_s {
	u32 ch_num;
	volatile u32 dma_done;
	GDMA_InitTypeDef GDMA_InitStruct;
};

struct gdma_memcopy_s gdma_memcpy;
//xSemaphoreHandle dma_memcpy_sema = NULL;

IMAGE2_RAM_TEXT_SECTION
static void memcpy_gdma_int(void *pData)
{
	/* To avoid gcc warnings */
	( void ) pData;
	
	//portBASE_TYPE taskWoken = pdFALSE;
	
	/* Clear Pending ISR */
	GDMA_ClearINT(0, gdma_memcpy.ch_num);
	GDMA_Cmd(0, gdma_memcpy.ch_num, DISABLE);	
	
	gdma_memcpy.dma_done = 1;
	//xSemaphoreGiveFromISR(dma_memcpy_sema, &taskWoken);
	//portEND_SWITCHING_ISR(taskWoken);
}

IMAGE2_RAM_TEXT_SECTION
void memcpy_gdma_init(void)
{
	gdma_memcpy.dma_done = 1;
	gdma_memcpy.ch_num = GDMA_ChnlAlloc(0, (IRQ_FUN)memcpy_gdma_int, NULL, 10);

	GDMA_StructInit(&(gdma_memcpy.GDMA_InitStruct));
	gdma_memcpy.GDMA_InitStruct.GDMA_ChNum = gdma_memcpy.ch_num;
	gdma_memcpy.GDMA_InitStruct.GDMA_Index = 0;
	gdma_memcpy.GDMA_InitStruct.GDMA_IsrType = (TransferType|ErrType);
	
	gdma_memcpy.GDMA_InitStruct.GDMA_SrcMsize = MsizeEight;
	gdma_memcpy.GDMA_InitStruct.GDMA_DstMsize = MsizeEight;

	//vSemaphoreCreateBinary(dma_memcpy_sema);
	//xSemaphoreTake(dma_memcpy_sema, portMAX_DELAY);
}

IMAGE2_RAM_TEXT_SECTION
static inline u32 memcpy_use_cpu(void *dest, void *src, u32 size) 
{
	/* To avoid gcc warnings */
	( void ) dest;
	( void ) src;
	
	if (size < 128) {
		return TRUE;
	}

	if (gdma_memcpy.dma_done == 0) {
		return TRUE;
	}

	return FALSE;
}

IMAGE2_RAM_TEXT_SECTION
int memcpy_gdma(void *dest, void *src, u32 size) 
{
	u32 size_4byte = size & ~(0x03);
	u32 left = size & (0x03);

	if (memcpy_use_cpu(dest, src, size) == TRUE) {
		_memcpy(dest, src, size);

		return 0;
	}
	gdma_memcpy.dma_done = 0;

	if (((u32)(src)& 0x03) || ((u32)(dest)& 0x03)) {
		gdma_memcpy.GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthOneByte;
		gdma_memcpy.GDMA_InitStruct.GDMA_DstDataWidth = TrWidthOneByte;
		gdma_memcpy.GDMA_InitStruct.GDMA_BlockSize = size;
	} else {
		/* 4-bytes aligned, move 4 bytes each transfer */
		gdma_memcpy.GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthFourBytes;
		gdma_memcpy.GDMA_InitStruct.GDMA_DstDataWidth = TrWidthFourBytes;
		gdma_memcpy.GDMA_InitStruct.GDMA_BlockSize = size_4byte >> 2;

		if (left != 0) {
			char *dst0 = (char *) dest + size_4byte;
			char *src0 = (char *) src + size_4byte;
			while (left--){
				*dst0++ = *src0++;
			}
		}
	}

	gdma_memcpy.GDMA_InitStruct.GDMA_SrcAddr = (u32)(src);
	gdma_memcpy.GDMA_InitStruct.GDMA_DstAddr = (u32)(dest);
	GDMA_Init(0, gdma_memcpy.ch_num, &(gdma_memcpy.GDMA_InitStruct));
	GDMA_Cmd(0, gdma_memcpy.ch_num, ENABLE);

	//xSemaphoreTake(dma_memcpy_sema, portMAX_DELAY);
	while (gdma_memcpy.dma_done == 0);

	return 0;
}

