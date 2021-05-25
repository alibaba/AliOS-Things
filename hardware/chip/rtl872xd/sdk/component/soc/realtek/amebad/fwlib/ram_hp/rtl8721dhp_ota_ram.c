/**
  ******************************************************************************
  * @file    rtl8721dhp_ota_ram.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the OTA functions.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"
#include "strproc.h"

/**
  * @brief  Set OTA image index.
  * @param  OTAIdx: This parameter can be  one of the following values:
  *            @arg OTA_INDEX_1: select OTA1(low flash address) as image2
  *            @arg OTA_INDEX_2: select OTA2(high flash address) as image2   
  * @retval status value:
  *          - 1: set ok
  *          - 0: set fail
  * @note SelectOTA2: will set LSB  odd bits 0, 0xFFFFFFFE/0xFFFFFFF8... means select OTA2
  * @note SelectOTA1: will set LSB  even bits 0,  0xFFFFFFFF/0xFFFFFFFC... means select OTA1
  */
IMAGE2_RAM_TEXT_SECTION
u32 OTA_Change(u32 OTAIdx)
{
	u32 BitIdx = 0;
	u32 ValidIMG2 = HAL_READ32(SPI_FLASH_BASE, FLASH_SYSTEM_DATA_ADDR + 4);

	/* erase ValidIMG2 when all 32bits cleared */
	if (ValidIMG2 == 0x00000000) {
		FLASH_EreaseDwordsXIP(FLASH_SYSTEM_DATA_ADDR + 4, 1);
		ValidIMG2 = 0xFFFFFFFF;
	}

	/* get first bit which is not cleared */
	for (BitIdx = 0; BitIdx <= 31; BitIdx++) {
		if ((ValidIMG2 & BIT(BitIdx)) != 0) {
			break;
		}
	}

	DBG_8195A("BitIdx: %d \n", BitIdx);

	/* even bits 0 currrent is OTA1 */
	if ((BitIdx % 2) == 0) {
		if (OTAIdx == OTA_INDEX_1) {
			DBG_8195A("currrent is OTA1, select OTA1 \n");
			return _TRUE;
		} else {
			DBG_8195A("currrent is OTA1, select OTA2 \n");
			ValidIMG2 &= ~BIT(BitIdx);
			FLASH_TxData12BXIP(FLASH_SYSTEM_DATA_ADDR + 4, 4, (u8*)&ValidIMG2);
		}
	} else { /* odd bits 0 currrent is OTA2 */
		if (OTAIdx == OTA_INDEX_1) {
			DBG_8195A("currrent is OTA2, select OTA1 \n");
			ValidIMG2 &= ~BIT(BitIdx);
			FLASH_TxData12BXIP(FLASH_SYSTEM_DATA_ADDR + 4, 4, (u8*)&ValidIMG2);
		} else {
			DBG_8195A("currrent is OTA2, select OTA2 \n");
			return _TRUE;
		}	
	}
	
	return _TRUE;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
