/**
  ******************************************************************************
  * @file    rtl8721d_wdg.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Watch Dog (WDG) peripheral:
  *           - Initialization
  *           - WDG refresh
  *           - Interrupts and flags management
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
  * @brief  Get WDG dividor parameters based on Period
  * @param  Period: WDG timeout (ms).
  * @param  pCountProcess: count id of WDG.
  *         This parameter can be set: 0 ~ 11, represent count value: 0x001~0xFFF
  * @note	The relationship between count id and count: Count = ((0x00000001 << (CountId+1))-1)
  * @param  pDivFacProcess: clock divisor of WDG timeout count.
  * @note	Minimum dividing factor is 1
  * @note	The formula to calculate dividing factor: DivFactor = (u16)(( TimeoutMs * 100)/(Count * 3))
  * @retval None
  */
void WDG_Scalar(u32 Period, u32 *pCountProcess, u32 *pDivFacProcess)
{
	u8 CountId;
	u16 DivFactor;
	u32 CountTemp; 
	u32 PeriodProcess = 100*Period;
	u32 MinPeriodTemp = 0xFFFFFFFF;
	u32 PeriodTemp = 0;

	for (CountId = 0; CountId < 12; CountId++) {
		CountTemp = ((0x00000001 << (CountId+1))-1);
		DivFactor = (u16)((PeriodProcess)/(CountTemp*3));

		if (DivFactor > 0) {
			PeriodTemp = 3*(DivFactor+1)*CountTemp;
			if (PeriodProcess < PeriodTemp) {
				if (MinPeriodTemp > PeriodTemp) {
					MinPeriodTemp = PeriodTemp;
					*pCountProcess = CountId;
					*pDivFacProcess = DivFactor;
				}
			}
		}
	}
}

/**
  * @brief  Initializes the  WDG registers according to the specified parameters
  *         get from WDG_Scalar.
  * @param  WDG_InitStruct: pointer to a WDG_InitTypeDef structure that contains
  *         the configuration information for the WDG peripheral.
  * @retval None
  */
void WDG_Init(WDG_InitTypeDef *WDG_InitStruct)
{
	WDG_TypeDef* WDG = ((WDG_TypeDef *) WDG_REG_BASE);
	u32 wdg_reg = 0;
	u32 temp = 0;

	wdg_reg = WDG_InitStruct->DivFacProcess & 0xFFFF; /* WdgScalar */
	wdg_reg &= ~(0x00FF0000); /* WdgEnByte */
	wdg_reg |= (WDG_InitStruct->CountProcess & 0xF) << 25;
	wdg_reg |= WDG_BIT_CLEAR;
	wdg_reg |= WDG_BIT_RST_MODE; /* RESET_MODE */
	wdg_reg |= WDG_BIT_ISR_CLEAR; /*Clear ISR*/

	WDG->VENDOR = wdg_reg;

	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_KM0_CTRL);
	if (WDG_InitStruct->RstAllPERI == 0)
		temp &= ~BIT_LSYS_WDT_RESET_HS_ALL;
	else
		temp |= BIT_LSYS_WDT_RESET_HS_ALL;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_KM0_CTRL, temp);
}

/**
  * @brief  Clear WDG interrupt when WDG init use WDG_IrqInit .
  * @param  None
  * @note   This function only used in interrupt mode
  * @retval None
  */
void WDG_IrqClear(void)
{
	WDG_TypeDef* WDG = ((WDG_TypeDef *) WDG_REG_BASE);
	u32 temp = WDG->VENDOR;

	/* Clear ISR */
	temp |= WDG_BIT_ISR_CLEAR;
	WDG->VENDOR = temp;
}

/**
  * @brief  init WDG as interrupt mode (close reset mode).
  * @param  handler: WDG interrupt handler
  * @param  Id: WDG interrupt handler parameter
  * @retval None
  */
void WDG_IrqInit(void *handler, u32 Id)
{
	WDG_TypeDef* WDG = ((WDG_TypeDef *) WDG_REG_BASE);
	u32 temp = WDG->VENDOR;

	InterruptRegister((IRQ_FUN)handler, WDG_IRQ, Id, 0);
	InterruptEn(WDG_IRQ, 0);

	temp |= WDG_BIT_ISR_CLEAR;
	temp &= ~WDG_BIT_RST_MODE; /* INT_MODE */
	WDG->VENDOR = temp;
}

/**
  * @brief  Disable/Enable WDG
  * @param  NewState: new state of the WDG.
  *          This parameter can be: ENABLE or DISABLE
  * @note   To enable WDG timer, set 0x1 to WDG register Bit[16]
  * @retval None
  */
void WDG_Cmd(u32 NewState)
{
	WDG_TypeDef* WDG = ((WDG_TypeDef *) WDG_REG_BASE);
	u32 temp = WDG->VENDOR;

	/* WdgEnBit */
	if (NewState == ENABLE)
		temp |= WDG_BIT_ENABLE;
	else
		temp &= ~WDG_BIT_ENABLE; 
		
	WDG->VENDOR = temp;

	WDG_IrqClear();
}

/**
  * @brief  Clear WDG timer
  * @param  None
  * @note   If call this function to refresh WDG before timeout period,
  *			then MCU reset or WDG interrupt won't generate
  * @retval None
  */
void WDG_Refresh(void)
{
	WDG_TypeDef* WDG = ((WDG_TypeDef *) WDG_REG_BASE);
	u32 temp = WDG->VENDOR;
	
	temp |= WDG_BIT_CLEAR;
	WDG->VENDOR = temp;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
