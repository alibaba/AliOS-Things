/**
  ******************************************************************************
  * @file    rtl8721d_comparator.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Comparator peripheral:
  *           - Initialization and Configuration
  *           - Analog configuration
  *           - Mode configuration
  *           - Interrupts and flags management
  *
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
  * @brief	 Initializes the parameters in the CMP_InitStruct with default value.
  * @param CMP_InitStruct: pointer to a CMP_InitTypeDef structure that contains
  *         the configuration information for the Comparator peripheral.
  * @retval None
  */
void CMP_StructInit(CMP_InitTypeDef *CMP_InitStruct)
{
	u8 i;

	for(i = 0; i < COMP_CH_NUM; i++) {
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_ChIndex = i;
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_Ref0 = 0;		// Vref0 = 0.1*0=0v
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_Ref1 = 0x1F;		//  Vref1 = 0.1*31=3.1v
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_WakeType = COMP_WK_NONE;
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_WakeSysCtrl = COMP_WITHIN_REF0_AND_REF1;
		CMP_InitStruct->CMP_ChanCtrl[i].CMP_WakeADCCtrl = COMP_WITHIN_REF0_AND_REF1;
	}
}

/**
  * @brief	 Initializes the Comparator according to the specified parameters in CMP_InitStruct.
  * @param  CMP_InitStruct: pointer to a CMP_InitTypeDef structure that contains
  *         the configuration for the Comparator peripheral.
  * @retval None
  */
void CMP_Init(CMP_InitTypeDef* CMP_InitStruct)
{
	u8 i, index;
	u32 tmp, val_ch = 0;
	CMP_TypeDef *comparator = COMPARATOR;
	CMP_CHTypeDef *compChan = CMP_InitStruct->CMP_ChanCtrl;

	CMP_Cmd(DISABLE);

	tmp = comparator->COMP_INTR_CTRL;
	
	for(i = 0; i < COMP_CH_NUM; i++) {
		index = compChan[i].CMP_ChIndex;
		
		val_ch |= (u32)(index << BIT_SHIFT_COMP_CHSW(i));
		
		comparator->COMP_REF_CH[index] = (u32)((compChan[i].CMP_Ref0 << BIT_SHIFT_COMP_REF0) | \
									(compChan[i].CMP_Ref1 << BIT_SHIFT_COMP_REF1));

		if(compChan[i].CMP_WakeType & COMP_WK_ADC) {
			tmp &= ~ BIT_MASK_COMP_WK_ADC_CTRL(index);
			tmp |= (compChan[i].CMP_WakeADCCtrl << BIT_SHIFT_COMP_WK_ADC_CTRL(index)) | \
					BIT_COMP_WK_ADC_EN(index);
		}

		if(compChan[i].CMP_WakeType & COMP_WK_SYS) {
			tmp &= ~ BIT_MASK_COMP_WK_SYS_CTRL(index);
			tmp |= (compChan[i].CMP_WakeSysCtrl << BIT_SHIFT_COMP_WK_SYS_CTRL(index)) | \
					BIT_COMP_WK_SYS_EN(index);
		}
	}

	comparator->COMP_INTR_CTRL = tmp;
	comparator->COMP_CHSW_LIST = val_ch;

	comparator->COMP_ANALOG |= BIT_SD_POSEDGE;
	comparator->COMP_AUTO_SHUT = BIT_COMP_AUTO_SHUT;
}

/**
  * @brief  Enable or Disable the Comparator peripheral.
  * @param  NewState: new state of the Comparator peripheral.
  *   			This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CMP_Cmd(u32 NewState)
{
	CMP_TypeDef *comparator = COMPARATOR;

	if(NewState != DISABLE)
		comparator->COMP_EN_TRIG |= BIT_COMP_ENABLE | BIT_COMP_EN_TRIG;
	else
		comparator->COMP_EN_TRIG &= ~(BIT_COMP_ENABLE | BIT_COMP_EN_TRIG);
}

/**
  * @brief  Indicate the Comparator is busy or not .
  * @param  None
  * @retval   busy status value:
  *           - 1: Busy
  *           - 0: Not Busy.
  */
u32 CMP_Busy(void)
{
	CMP_TypeDef *comparator = COMPARATOR;

	if(comparator->COMP_BUSY_STS & BIT_COMP_BUSY_STS)
		return 1;
	else
		return 0;
}
/**
  * @brief  Get Comparator Wakeup ADC/SYS Status Register .
  * @param  None
  * @retval   Current Comparator Wakeup ADC/SYS Status Register
  */
u32 CMP_GetISR(void)
{
	CMP_TypeDef *comparator = COMPARATOR;

	return comparator->COMP_WK_STS;
}

/**
  * @brief  Clear pending bit in Comparator Wakeup ADC/SYS Status Register.
  * @param  Comparator Wakeup ADC/SYS Status Register
  * @retval   None
  */
void CMP_INTClearPendingBit(u32 Cmp_IT)
{
	CMP_TypeDef *comparator = COMPARATOR;

	comparator->COMP_WK_STS = Cmp_IT;
}

/**
  * @brief  Get the comparison result .
  * @param  channel : the Comparator channel index
  * @retval  The comparison result of specified channel index. The return value can be:
  *		-0: when Vin < Vref0
  *		-2: when Vin > Vref0 & Vin < Vref1
  *		-3: when Vin > Vref1
  */
u32 CMP_GetCompStatus(u8 channel)
{
	CMP_TypeDef *comparator = COMPARATOR;
	u32 value = (comparator->COMP_CH_STS & BIT_COMP_CH_STS(channel)) >> \
		BIT_SHIFT_COMP_CH_STS(channel);
	
	return value;
}

/**
  * @brief  Get the channel list index of the last used channel.
  * @param  None.
  * @retval  The channel list index of the last used channel. Not channel ID.
  */
u32 CMP_GetLastChan(void)
{
	CMP_TypeDef *comparator = COMPARATOR;

	return comparator->COMP_LAST_CH;
}

/**
  * @brief  Reset the channel switch to default state.
  * @param  None
  * @retval  None
  */
void CMP_ResetCSwList(void)
{
	CMP_TypeDef *comparator = COMPARATOR;

	comparator->COMP_RST_LIST = BIT_COMP_RST_LIST;
	comparator->COMP_RST_LIST = 0;
}

/**
  * @brief  Controls the automatic channel swtich enabled or disabled in Comparator Automatic Mode.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the automatic channel switch. 
  *				When setting this bit, an automatic channel switch starts from the first channel in the channel switch list.
  *			@arg DISABLE:  Disable the automatic channel switch. 
  *				If an automatic channel switch is in progess, writing 0 will terminate the automatic channel switch.
  * @retval  None.
  */
void CMP_AutoCSwCmd(u32 NewState)
{
	CMP_TypeDef *comparator = COMPARATOR;

	if(NewState != DISABLE) {
		comparator->COMP_AUTOSW_EN = BIT_COMP_AUTOSW_EN;
		comparator->COMP_EN_TRIG |= BIT_COMP_EN_TRIG;
	} else {
		comparator->COMP_AUTOSW_EN = 0;
		comparator->COMP_EN_TRIG &= ~BIT_COMP_EN_TRIG;
	}
}

/**
  * @brief	 Initialize the trigger timer when in Comparator Timer-Trigger Mode.
  * @param  Tim_Idx: The timer index would be used to make the Comparator module do a comparison.
  * @param  PeriodMs: Indicate the period of trigger timer.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the Comparator timer trigger mode.
  *			@arg DISABLE: Disable the Comparator timer trigger mode.
  * @retval  None.
  * @note  Used in Comparator Timer-Trigger Mode
  */
void CMP_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState)
{
	CMP_TypeDef *comparator = COMPARATOR;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	comparator->COMP_EXT_TRIG_TIMER_SEL = Tim_Idx;

	if(NewState != DISABLE) {
		RTIM_TimeBaseStructInit(&TIM_InitStruct);
		TIM_InitStruct.TIM_Idx = Tim_Idx;
		TIM_InitStruct.TIM_Period = (PeriodMs *32768)/1000/2;//ms to tick
		
		RTIM_TimeBaseInit(TIMx_LP[Tim_Idx], &TIM_InitStruct, TIMx_irq_LP[Tim_Idx], (IRQ_FUN)NULL, (u32)NULL);
		RTIM_Cmd(TIMx_LP[Tim_Idx], ENABLE);

		comparator->COMP_EXT_TRIG_CTRL = BIT_COMP_EXT_WK_TIMER;
	} else {
		RTIM_Cmd(TIMx_LP[Tim_Idx], DISABLE);
		comparator->COMP_EXT_TRIG_CTRL = 0;
	}
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
