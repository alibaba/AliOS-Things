/**
  ******************************************************************************
  * @file    rtl8721dlp_keyscan.c
  * @author
  * @version V1.0.0
  * @date    2017-10-16
  * @brief   This file contains all the functions prototypes for the keyscan firmware
  *             library, including the following functionalities of keyscan controller:
  *           - Initialization
  *           - get key press/release status and index
  *           - Keyscan sleep mode control
  *           - Interrupts and flags management
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
#include "rtl8721d_keyscan.h"

/**
  * @brief  Fills each KeyScan_InitStruct member with its default value.
  * @param  KeyScan_InitStruct: pointer to an KeyScan_InitTypeDef structure which will be initialized.
  * @retval None
  */
void KeyScan_StructInit(KeyScan_InitTypeDef* KeyScan_InitStruct)
{
	/* Load HAL initial data structure default value */
	KeyScan_InitStruct->KS_ClkDiv = 49;
	KeyScan_InitStruct->KS_ColSel = 0x03;
	KeyScan_InitStruct->KS_RowSel = 0x03;

	KeyScan_InitStruct->KS_WorkMode = KS_REGULAR_SCAN_MODE;

	KeyScan_InitStruct->KS_DebounceCnt = 0x3e7;
	KeyScan_InitStruct->KS_IntervalCnt = 0x7cf;
	KeyScan_InitStruct->KS_ReleaseCnt = 0x3e7;

	KeyScan_InitStruct->KS_ThreholdLevel = 8;
	KeyScan_InitStruct->KS_LimitLevel = 2;
	KeyScan_InitStruct->KS_OverCtrl = KS_FIFO_OVER_CTRL_DIS_NEW;
}

/**
  * @brief  Initializes the KeyScan peripheral according to the specified 
  *			parameters in the KeyScan_InitStruct.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_InitStruct: pointer to a KeyScan_InitTypeDef structure that contains
  * 		the configuration information for the specified KeyScan peripheral.
  * @retval None
  */
void KeyScan_Init(KEYSCAN_TypeDef *KeyScan, KeyScan_InitTypeDef* KeyScan_InitStruct)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));
	assert_param(KeyScan_InitStruct->KS_ClkDiv <= 0xfff);
	assert_param((KeyScan_InitStruct->KS_ColSel >= 0x1) && (KeyScan_InitStruct->KS_ColSel <= 0xff));
	assert_param((KeyScan_InitStruct->KS_RowSel >= 0x1) && (KeyScan_InitStruct->KS_RowSel <= 0xff));
	assert_param(KeyScan_InitStruct->KS_DebounceCnt <= 0xfff);
	assert_param(KeyScan_InitStruct->KS_IntervalCnt <= 0xfff);
	assert_param(KeyScan_InitStruct->KS_ReleaseCnt <= 0xfff);
	assert_param(IS_KS_WORK_MODE(KeyScan_InitStruct->KS_WorkMode));
	assert_param(IS_KS_FIFO_OVER_CTRL(KeyScan_InitStruct->KS_OverCtrl));
	assert_param(IS_KS_FIFO_LIMIT_LEVEL(KeyScan_InitStruct->KS_LimitLevel));
	assert_param(IS_KS_FIFO_TH_LEVEL(KeyScan_InitStruct->KS_ThreholdLevel));

	/* Disable the IC first */
	KeyScan->KS_CTRL &= (~BIT_KS_RUN_ENABLE);

	/* Mask all keyscan interrupt */
	KeyScan->KS_IMR &= (~BIT_KS_ALL_INT_MSK);

	/* clock divider config */
	KeyScan->KS_CLK_DIV &= (~BIT_KS_CLK_DIV);
	KeyScan->KS_CLK_DIV |= KeyScan_InitStruct->KS_ClkDiv;

	/* Config scan mode*/
	KeyScan->KS_CTRL &= (~BIT_KS_WORK_MODE);
	KeyScan->KS_CTRL |= KeyScan_InitStruct->KS_WorkMode;
	
	KeyScan->KS_FIFO_CFG = (KeyScan_InitStruct->KS_OverCtrl) | (KeyScan_InitStruct->KS_ThreholdLevel << 16) |\
								(KeyScan_InitStruct->KS_LimitLevel << 24);

	/* time count config */
	KeyScan->KS_TIM_CFG0 &= (~BIT_KS_DEB_TIMER);
	KeyScan->KS_TIM_CFG0 |= KeyScan_InitStruct->KS_DebounceCnt;
	KeyScan->KS_TIM_CFG1 = (KeyScan_InitStruct->KS_IntervalCnt << 16) | KeyScan_InitStruct->KS_ReleaseCnt;

	/* Set col map, config which col to work */
	KeyScan->KS_COL_CFG = KeyScan_InitStruct->KS_ColSel;

	/* Set row map, config which row to work */
	KeyScan->KS_ROW_CFG = KeyScan_InitStruct->KS_RowSel;

	/* clear all interrupt status and status flag */
	KeyScan->KS_ICR |= BIT_KS_ALL_INT_CLR;

	return;
}

/**
  * @brief  Enables or disables the specified KeyScan interrupts mask.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_IT: specifies the KeyScan interrupt sources to be enabled or masked.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_KS_SCAN_EVENT_INT_MSK: Mask Scan event interrupt status
  *            @arg BIT_KS_FIFO_LIMIT_INT_MSK: Mask FIFO limit interrupt status
  *            @arg BIT_KS_FIFO_OVERFLOW_INT_MSK: Mask FIFO overflow interrupt
  *            @arg BIT_KS_FIFO_FULL_INT_MSK: Mask FIFO full interrupt
  *            @arg BIT_KS_SCAN_FINISH_INT_MSK: Mask scan finish interrupt
  *            @arg BIT_KS_FIFO_NOTEMPTY_INT_MSK: Mask FIFO nonempty interrupt
  *            @arg BIT_KS_ALL_RELEASE_INT_MSK: Mask All Release interrupt
  * @param  NewState: new state of the specified KeyScan interrupts mask.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_INTConfig(KEYSCAN_TypeDef *KeyScan, uint32_t KeyScan_IT, u8 newState)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	if (newState == ENABLE) {
		/* Enable KeyScan interrupts */
		KeyScan->KS_IMR |= KeyScan_IT;
	} else {
		/* Disable KeyScan interrupts */
		KeyScan->KS_IMR &= (~KeyScan_IT);
	}
}

/**
  * @brief  Clears the specified KeyScan interrupt pending bit.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_IT: specifies the KeyScan interrupt to be cleared.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_KS_FIFO_LIMIT_INT_STATUS: FIFO limit interrupt status
  *            @arg BIT_KS_FIFO_OVERFLOW_INT_STATUS: FIFO overflow interrupt status
  *            @arg BIT_KS_SCAN_FINISH_INT_STATUS: Scan finish interrupt status
  *            @arg BIT_KS_ALL_RELEASE_INT_STATUS: All Release interrupt status
  * @note   BIT_KS_SCAN_EVENT_INT_STATUS is automatically cleared by hardware when the data is read. 
  *             BIT_KS_FIFO_FULL_INT_STATUS is automatically cleared by hardware when the buffer level 
  *        goes below the BIT_KS_FIFO_THREHOLD_LEVEL threshold. 
  *             BIT_KS_FIFO_NOTEMPTY_INT_STATUS is automatically cleared by hardware when the FIFO is empty.
  * @retval None
  */
void KeyScan_ClearINT(KEYSCAN_TypeDef *KeyScan, u32 KeyScan_IT)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	KeyScan->KS_ICR |= KeyScan_IT;
}

/**
  * @brief  Get KeyScan Raw Interrupt Status.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval raw interrupt status
  */
u32 KeyScan_GetRawINT(KEYSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	return KeyScan->KS_ISR_RAW;
}

/**
  * @brief  Get KeyScan interrupt status.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval interrupt status
  */
u32 KeyScan_GetINT(KEYSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	return KeyScan->KS_ISR;
}

/**
  * @brief  Get data number of keyscan FIFO.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval data number of keyscan FIFO
  */
u8 KeyScan_GetDataNum(KEYSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	return (KeyScan->KS_DATA_NUM & BIT_KS_FIFO_DATA_LEVEL);
}

/**
  * @brief  Clears the FIFO data.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval None
  */
void KeyScan_ClearFIFOData(KEYSCAN_TypeDef *KeyScan)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	TempVal = KeyScan->KS_FIFO_CFG;
	KeyScan->KS_FIFO_CFG = (TempVal | BIT_KS_FIFO_CLR);
}

/**
  * @brief  Get status of keyscan FIFO.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_Flag: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg BIT_KS_FIFO_FULL
  *     @arg BIT_KS_FIFO_EMPTY
  * @retval the new statue of the specified flag.
  *   This parameter can be: _TRUE or _FALSE.
  */
BOOL KeyScan_GetFIFOState(KEYSCAN_TypeDef *KeyScan, u32 KeyScan_Flag)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	if ((KeyScan->KS_DATA_NUM & KeyScan_Flag) != 0) {
		return _TRUE;
	} else {
		return _FALSE;
	}
}

/**
  * @brief  Read data from keyscan FIFO.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  *outBuf: buffer to save data read from KeyScan FIFO.
  * @param  count: number of data to be read.
  * @retval None
  */
void KeyScan_Read(KEYSCAN_TypeDef *KeyScan, u32 *outBuf, u8 count)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	for (TempVal = 0; TempVal < count; TempVal++) {
		*outBuf++ = KeyScan->KS_DATA_ENTRY;
	}

	return;
}

/**
  * @brief  Enables or disables the specified KeyScan peripheral.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  NewState: new state of the KeyScan peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_Cmd(KEYSCAN_TypeDef *KeyScan, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	if (NewState != DISABLE) {
		/* Enable the KeyScan peripheral */
		KeyScan->KS_CTRL |= BIT_KS_RUN_ENABLE;
	} else {
		/* Disable the KeyScan peripheral */
		KeyScan->KS_CTRL &= (~BIT_KS_RUN_ENABLE);
	}
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
