/**
  ******************************************************************************
  * @file    rtl8721dhp_sdio.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the
  *          functionalities of the SDIO device peripheral:
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "ameba_soc.h"

/**
  * @brief  Fills each SDIO_InitStruct member with its default value.
  * @param  SDIO_InitStruct: pointer to a SDIO_InitTypeDef structure which will be 
  *         initialized.
  * @retval None
  */

void SDIO_StructInit(SDIO_InitTypeDef* SDIO_InitStruct)
{
	/* TXBD */
	SDIO_InitStruct->TXBD_BAR = (u32)NULL; /* init txbd_bar */
	SDIO_InitStruct->TXBD_RING_SIZE = 20; /* init TXBD ring size */
	SDIO_InitStruct->TX_BUFFER_SIZE = 0xFF; /* init txbuf_size */
	
	/* RXBD */
	SDIO_InitStruct->RXBD_BAR = (u32)NULL; /* init rxbd_bar reg */
	SDIO_InitStruct->RXBD_RING_SIZE = 32; /* init rxbd ring size */
	SDIO_InitStruct->RXBD_FREE_TH = 5; /* init AHB rx_blk_size */
}

/**
  * @brief  Initializes the SDIO registers according to the specified parameters 
  *         in SDIO_InitStruct.
  * @param  SDIO_InitStruct: pointer to a SDIO_InitTypeDef structure that contains 
  *         the configuration information for the SDIO peripheral.
  * @retval none
  */

void SDIO_Init(SDIO_InitTypeDef* SDIOInit_Struct)
{
	u16 Temp = 0;

	/* Reset SDIO DMA */
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_RST_DMA, BIT_CPU_RST_SDIO_DMA);
	
	/* initial TX BD */
	HAL_SDIO_WRITE32(REG_SPDIO_TXBD_ADDR, SDIOInit_Struct->TXBD_BAR);
	HAL_SDIO_WRITE16(REG_SPDIO_TXBD_SIZE, SDIOInit_Struct->TXBD_RING_SIZE);
	
	/* Set TX_BUFF_UNIT_SIZE */
	HAL_SDIO_WRITE8(REG_SPDIO_TX_BUF_UNIT_SZ, SDIOInit_Struct->TX_BUFFER_SIZE);

	/* Set DISPATCH_TXAGG_PKT */
	HAL_SDIO_WRITE32(REG_SPDIO_AHB_DMA_CTRL, HAL_SDIO_READ32(REG_SPDIO_AHB_DMA_CTRL)|BIT31);
	
	/*  Reset HW TX BD pointer */
	HAL_SDIO_WRITE32(REG_SPDIO_TXBD_RPTR, HAL_SDIO_READ32(REG_SPDIO_TXBD_WPTR));

	/* Init RX BD and RX Buffer */
	HAL_SDIO_WRITE32(REG_SPDIO_RXBD_ADDR, SDIOInit_Struct->RXBD_BAR);
	HAL_SDIO_WRITE16(REG_SPDIO_RXBD_SIZE, SDIOInit_Struct->RXBD_RING_SIZE);
	
	/*  Reset HW RX BD pointer */
	HAL_SDIO_WRITE16(REG_SPDIO_RXBD_C2H_WPTR, HAL_SDIO_READ16(REG_SPDIO_RXBD_C2H_RPTR));

	/* Set the threshold of free RX BD count to trigger interrupt */
	HAL_SDIO_WRITE16(REG_SPDIO_RX_BD_FREE_CNT, SDIOInit_Struct->RXBD_FREE_TH);

	/*  Indicate Host this is a iNIC FW */
	Temp = HAL_SDIO_READ16(REG_SPDIO_CCPWM2);
	Temp |= CPWM2_INIC_FW_RDY_BIT;
	Temp ^= CPWM2_TOGGLE_BIT;
	HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, Temp);
}

/**
  * @brief  SDIO interrupt clear
  * @note	clear all interrupt of SDIO
  * @retval None
  */

void SDIO_INTClear(void)
{
	u16 IntStatus;
	
	IntStatus = HAL_SDIO_READ16(REG_SPDIO_CPU_INT_STAS);
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, IntStatus);	// clean the ISR
}

/**
  * @brief  SDIO disable or enable interrupt by modify the interrupt mask
  * @param  IntMask: The bit map to enable the interrupt.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_TXFIFO_H2C_OVF:  
  *            @arg BIT_H2C_BUS_RES_FAIL:  
  *            @arg BIT_H2C_DMA_OK:  
  *            @arg BIT_C2H_DMA_OK:  
  *            @arg BIT_H2C_MSG_INT: 
  *            @arg BIT_RPWM1_INT: 
  *            @arg BIT_RPWM2_INT:
  *            @arg BIT_SDIO_RST_CMD_INT: 
  *            @arg BIT_RXBD_FLAG_ERR_INT: 
  *            @arg BIT_RX_BD_AVAI_INT: 
  *            @arg BIT_HOST_WAKE_CPU_INT: 
  * @param  NewState: ENABLE or DISABLE.
  * @retval None
  */

VOID SDIO_INTConfig(u16 IntMask, u32 NewState)
{
	u16 IntMaskTmp = HAL_SDIO_READ16(REG_SPDIO_CPU_INT_MASK);
	
	if (NewState == ENABLE) {
		IntMaskTmp |= IntMask;
	} else {
		IntMaskTmp &= ~IntMask;
	}

	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_MASK, IntMaskTmp);
}

/**
  * @brief  Get RPWM1 from host.
  * @param  none.
  * @retval RPWM1 val.
  */

u8 SDIO_RPWM1_Get(void)
{
	return HAL_SDIO_READ8(REG_SPDIO_CRPWM);
}

/**
  * @brief  Get RPWM2 from host.
  * @param  none.
  * @retval RPWM2 val.
  */

u16 SDIO_RPWM2_Get(void)
{
	return HAL_SDIO_READ16(REG_SPDIO_CRPWM2);
}

/**
  * @brief  Set CPWM1 to host
  * @param  Val: CPWM msg to send.
  * @retval None
  */

void SDIO_CPWM1_Set(u8 Val)
{
	u8  Temp = HAL_SDIO_READ8(REG_SPDIO_CCPWM);
	
	Temp ^= CPWM1_TOGGLE_BIT;
	Temp |= Val;
	
	HAL_SDIO_WRITE8(REG_SPDIO_CCPWM, Temp); 
}

/**
  * @brief  Set CPWM2 to host
  * @param  Val: CPWM msg to send.
  * @param  Newstate: DISABLE/ENABLE.
  * @retval None
  */

void SDIO_CPWM2_Set(u16 Val, u32 Newstate)
{
	u16  Temp = HAL_SDIO_READ16(REG_SPDIO_CCPWM2);
	
	Temp ^= CPWM2_TOGGLE_BIT;

	if (Newstate == ENABLE)
		Temp |= Val;
	else
		Temp &= ~Val;

	HAL_SDIO_WRITE16(REG_SPDIO_CCPWM2, Temp); 
}

/**
  * @brief  Get RXBD read pointer, updated by SDIO IP.
  * @param  none
  * @retval RXBD read pointer
  */

u16 SDIO_RXBD_RPTR_Get(void)
{
	return HAL_SDIO_READ16(REG_SPDIO_RXBD_C2H_RPTR);
}

/**
  * @brief  Set RXBD write pointer, updated by cpu CM4 driver.
  * @param  Val: RXBD write pointer
  * @retval none
  */

void SDIO_RXBD_WPTR_Set(u16 Val)
{
	HAL_SDIO_WRITE16(REG_SPDIO_RXBD_C2H_WPTR, Val);
}

/**
  * @brief  Get TXBD write pointer, updated by SDIO IP.
  * @param  none
  * @retval TXBD write pointer
  */

u16 SDIO_TXBD_WPTR_Get(void)
{
	return HAL_SDIO_READ16(REG_SPDIO_TXBD_WPTR);
}

/**
  * @brief  Set TXBD read pointer, updated by CM4 driver.
  * @param  none
  * @retval TXBD read pointer
  */

void SDIO_TXBD_RPTR_Set(u16 Val)
{
	HAL_SDIO_WRITE16(REG_SPDIO_TXBD_RPTR, Val);
}

/**
  * @brief  Reset SDIO DMA.
  * @param  none
  * @retval TXBD read pointer
  */

void SDIO_DMA_Reset(void)
{
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_RST_DMA, BIT_CPU_RST_SDIO_DMA);
}
	
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
