/**
  ******************************************************************************
  * @file    rtl8721dhp_pmc_ram.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the soc power management circut:
  *           - wakeup timer
  *           - wakeup pin
  *           - sleep option
  *           - sleep mode
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

/**
  * @brief  clear wake event.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_ClearWakeEvent_HP(void)
{
	u32 Rtemp = 0;

	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HS_WAKE_EVENT_STATUS0);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_HS_WAKE_EVENT_STATUS0, Rtemp); 
}

/**
  * @brief  set hs wake up event.
  * @param  Option:
  *   This parameter can be any combination of the following values:
  *		 @arg BIT_HS_WEVT_UART_MSK
  *		 @arg BIT_HS_WEVT_TIMER_MSK 
  * @param  NewStatus: TRUE/FALSE.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SetWakeEvent_HP(u32 Option, u32 NewStatus)
{
	u32 WakeEvent = 0;

	WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_WAKE_EVENT_MSK0);
	
	if (NewStatus == ENABLE) {
		WakeEvent |= Option;
	} else {
		WakeEvent &= ~Option;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_WAKE_EVENT_MSK0, WakeEvent);

	/* Set Event */
	WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_MSK0);
	if (NewStatus == ENABLE) {
		WakeEvent |= BIT_LP_WEVT_HS_MSK;
	} else {
		WakeEvent &= ~BIT_LP_WEVT_HS_MSK;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SLP_WAKE_EVENT_MSK0, WakeEvent); 
}

/**
  * @brief  get aon wake reason.
  * @param  None
  * @retval BIT(0): wakepin, BIT(1): aon_timer, BIT(2): RTC, BIT(3): tsf_timer, BIT(4): key-scan
  */
IMAGE2_RAM_TEXT_SECTION
int SOCPS_AONWakeReason(void)
{
	int reason = 0;
	
	reason = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_OPT_STS);	

	return reason;
}

/**
  * @brief  clear aon wake event.
  * @param  BitMask: aon wakepin index
  *		This parameter can be one or combination of the following values:
  *		 @arg BIT_GPIO_WAKE_STS
  *		 @arg BIT_AON_WAKE_TIM0_STS
  *		 @arg BIT_RTC_WAKE_STS
  *		 @arg BIT_DLPS_TSF_WAKE_STS
  *		 @arg BIT_KEYSCAN_WAKE_STS
  *		 @arg BIT_CAPTOUCH_WAKE_STS
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_AONWakeClear(u32 BitMask)
{
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_OPT_STS, BitMask);
}