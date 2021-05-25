/**
  ******************************************************************************
  * @file    rtl8721dhp_crc.c
  * @author
  * @version V1.0.0
  * @date    2017-10-10
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - CRC configuration
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

/**
  * @brief  Fills each CRC_InitStruct member with its default value.
  * @param  CRC_InitStruct: pointer to an CRC_InitTypeDef structure which will be
  *         initialized.
  * @retval None
  */
void CRC_StructInit(CRC_InitTypeDef* CRC_InitStruct)
{
	CRC_InitStruct->CRC_Type = CRC_TYPE_32;
	CRC_InitStruct->CRC_Polynom = 0x4C11DB7;
	CRC_InitStruct->CRC_InitVal = 0xFFFFFFFF;
	CRC_InitStruct->CRC_Xor = 0xFFFFFFFF;
	CRC_InitStruct->CRC_Iswap = 0;
	CRC_InitStruct->CRC_Oswap = 0;
	CRC_InitStruct->CRC_DmaMode = CRC_DMA_MODE;
}

/**
  * @brief  Initializes the CRC registers according to the specified parameters
  *         in CRC_InitStruct.
  * @param  CRC_InitStruct: pointer to an CRC_InitTypeDef structure that contains
  *         the configuration information for the CRC engine
  * @retval CRC init state: SUCCESS or FAIL.
  */
int CRC_Init(CRC_InitTypeDef* CRC_InitStruct)
{
	/* Check the parameters */
	assert_param(IS_CRC_TYPE(CRC_InitStruct->CRC_Type));
	assert_param(CRC_InitStruct->CRC_Iswap < 8);
	assert_param(CRC_InitStruct->CRC_Oswap < 2);
	assert_param(IS_CRC_MODE(CRC_InitStruct->CRC_DmaMode));
	assert_param((CRC_InitStruct->CRC_Polynom & 0x1) == 0x1);
	
	// check if busy
	if ( CRC_S_MODULE->CRC_STAT & CRC_BUSY) {
		DiagPrintf("CRC engine is busy\r\n");
		return FAIL;
	}
	
	// configure crc registers: polynom, initial value, xor output
	CRC_S_MODULE->CRC_POLY = CRC_InitStruct->CRC_Polynom;
	CRC_S_MODULE->CRC_IV = CRC_InitStruct->CRC_InitVal;
	CRC_S_MODULE->CRC_OXOR = CRC_InitStruct->CRC_Xor;
	// set crc input and output swap, data type, dma_mode
	CRC_S_MODULE->CRC_OP = ((CRC_InitStruct->CRC_Iswap & CRC_ISWAP) |
							(CRC_InitStruct->CRC_Oswap << 3) |
							( CRC_InitStruct->CRC_Type << 4));

	return SUCCESS;
}

/**
  * @brief  CRC.
  * @param  CRC_InitStruct: pointer to an CRC_InitTypeDef structure that contains
  *         the configuration information for the CRC engine
  * @param  message: Input data.
  * @param  msglen: Message length. 
  * @param  pCrc:  Output result 
  * @note Use polling mode
  * @retval CRC execution state: SUCCESS or FAIL.
  */
int CRC_Calculate(CRC_InitTypeDef* CRC_InitStruct, IN const u8* message, IN const u32 msglen, OUT u32* pCrc)
{
	u32 j;

	// check if busy
	if ( CRC_S_MODULE->CRC_STAT & CRC_BUSY ) {
		DiagPrintf("CRC engine is busy\r\n");
		return FAIL;
	}

	DCache_CleanInvalidate(((u32)message & CACHE_LINE_ADDR_MSK), (msglen + CACHE_LINE_SIZE));

	// set data length
	CRC_S_MODULE->CRC_OP &= ~ (CRC_LENGTH | CRC_LAST) ;
	if ( msglen != 0 ) { // not last
		CRC_S_MODULE->CRC_OP |= msglen << 16;
	} else {
		CRC_S_MODULE->CRC_OP |= CRC_LAST;
	}

	if(CRC_InitStruct->CRC_DmaMode) {// DMA mode
		// set data buffer
		CRC_S_MODULE->CRC_DATA = (u32)(message);

		// set status and reset crc engine
		CRC_S_MODULE->CRC_STAT &= ~ CRC_STAT_MASK;
		CRC_S_MODULE->CRC_STAT |= (CRC_OK | CRC_INTR_MASK | CRC_LITTLE_ENDIAN);
		CRC_S_MODULE->CRC_RST = CRC_RESET;
		CRC_S_MODULE->CRC_OP |= CRC_DMA;
	} else {
		u32 data;
		int msglen_4bytes;
		int i;
		
		// set status and reset crc engine
		CRC_S_MODULE->CRC_STAT &= ~ CRC_STAT_MASK;
		CRC_S_MODULE->CRC_STAT |= (CRC_OK | CRC_INTR_MASK | CRC_LITTLE_ENDIAN);
		CRC_S_MODULE->CRC_RST = CRC_RESET;
		CRC_S_MODULE->CRC_OP &= ~ CRC_DMA;

		//32 bit
		CRC_S_MODULE->CRC_OP &= ~ CRC_BE;
		CRC_S_MODULE->CRC_OP |= 0 << 8;

		msglen_4bytes = (msglen/4)*4;

		for (i=0; i<msglen_4bytes; i+=4) {
			data = (u32)(message[i]);
			data |= (u32)(message[i+1])<<8;
			data |= (u32)(message[i+2])<<16;
			data |= (u32)(message[i+3])<<24;
			CRC_S_MODULE->CRC_DATA = data;
		}

		//8bit
		CRC_S_MODULE->CRC_OP &= ~ CRC_BE;
		CRC_S_MODULE->CRC_OP |= 1 << 8;
		for (; (u32)(i)<msglen; i++) {
			data = message[i];
			CRC_S_MODULE->CRC_DATA = data;
		}
	}

	j= 0;
	while (j < CRC_TIMEOUT) {
		if ( (CRC_S_MODULE->CRC_STAT & CRC_OK) >> 1 ) break;
		j++;
	}
	if ( j == CRC_TIMEOUT ) {
		DiagPrintf("CRC wait done failed \r\n");
		return FAIL;
	} else {
		*pCrc = CRC_S_MODULE->CRC_RESULT;
		// crc ok clear
		CRC_S_MODULE->CRC_STAT |= CRC_OK;
	}

	return SUCCESS;
}
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
