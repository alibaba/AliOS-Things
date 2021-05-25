/** mbed Microcontroller Library
  ******************************************************************************
  * @file    captouch_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for CAPTOUCH.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "objects.h"
#include "captouch_api.h"
#include "PinNames.h"
#include "pinmap.h"

typedef void (*ctc_irq_handler)(u8 ch);

/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_CAPTOUCH
 *  @brief      MBED_CAPTOUCH driver modules.
 *  @{
 */

/** @defgroup MBED_CAPTOUCH_Exported_Functions MBED_CAPTOUCH Exported Functions
  * @{
  */

/**
  * @brief  captouch related function and interrupt handler.
  * @param  obj: captouch object define in application software.
  * @retval none  
  */ 
static void captouch_irq_handler(captouch_t *obj)
{
	u8 ch;
	u32 IntStatus;
	ctc_irq_handler handler_press;
	ctc_irq_handler handler_release;
	
	IntStatus = CapTouch_GetISR(CAPTOUCH_DEV);
	CapTouch_INTClearPendingBit(CAPTOUCH_DEV, IntStatus);
	
	for(ch = 0; ch < CT_CHANNEL_NUM; ch++) {
		if(IntStatus & CT_CHX_PRESS_INT(ch)) {
			DBG_8195A("Key	%x press \n",ch);

			if (obj->irq_handler_press) {
				handler_press = (ctc_irq_handler)obj->irq_handler_press;
				handler_press(ch);
			}
		}

		if(IntStatus & CT_CHX_RELEASE_INT(ch)) {
			DBG_8195A("Key	%x release \n", ch);

			if (obj->irq_handler_release) {
				handler_release = (ctc_irq_handler)obj->irq_handler_release;
				handler_release(ch);
			}
		}
	}

	/* User do not need care the Noise interrupt */
	if(IntStatus & BIT_CT_OVER_P_NOISE_THRESHOLD_INT) {		
		CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);
		CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
	}

}


/**
  * @brief  initializes the captouch device.
  * @param  obj: captouch object define in application software.
  * @retval none  
  */
void captouch_init (captouch_t *obj)
{
	u32 ch = 0;

	CapTouch_InitTypeDef Touch_InitStruct;

	/* Load HAL initial data structure default value */
	Touch_InitStruct.CT_DebounceEn = 1;
	Touch_InitStruct.CT_SampleCnt = 6;
	Touch_InitStruct.CT_ScanInterval = 60;
	Touch_InitStruct.CT_ETCStep = 1;
	Touch_InitStruct.CT_ETCFactor = 4;
	Touch_InitStruct.CT_ETCScanInterval = 3;

	for(ch = 0; ch < CT_CHANNEL_NUM; ch++) {
		if (obj->CT_Channel[ch].CT_CHEnable == ENABLE) {
			Touch_InitStruct.CT_Channel[ch].CT_CHEnable = obj->CT_Channel[ch].CT_CHEnable;
			Touch_InitStruct.CT_Channel[ch].CT_DiffThrehold = obj->CT_Channel[ch].CT_DiffThrehold;
			Touch_InitStruct.CT_Channel[ch].CT_MbiasCurrent = obj->CT_Channel[ch].CT_MbiasCurrent; 
			Touch_InitStruct.CT_Channel[ch].CT_ETCNNoiseThr = obj->CT_Channel[ch].CT_ETCNNoiseThr;
			Touch_InitStruct.CT_Channel[ch].CT_ETCPNoiseThr = obj->CT_Channel[ch].CT_ETCPNoiseThr;

			PAD_PullCtrl((_PB_4 + ch), GPIO_PuPd_NOPULL);
		}
	}

	CapTouch_Init(CAPTOUCH_DEV, &Touch_InitStruct);
	CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
	CapTouch_INTConfig(CAPTOUCH_DEV, BIT_CT_ALL_INT_EN, ENABLE);

	/* Register interrupt Callback function */
	InterruptRegister((IRQ_FUN) captouch_irq_handler, CTOUCH_IRQ, (u32)obj, 5);
	InterruptEn(CTOUCH_IRQ, 5);
}


/**
  * @brief  deinitializes the captouch device.
  * @param  obj: captouch object define in application software.
  * @retval none  
  */
void captouch_deinit (captouch_t *obj)
{
	/* To avoid gcc warnings */
	( void ) obj;

	InterruptDis(CTOUCH_IRQ);
	InterruptUnRegister(CTOUCH_IRQ);
	CapTouch_INTConfig(CAPTOUCH_DEV, BIT_CT_ALL_INT_EN, DISABLE);
	CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);
}

/**
  * @brief  enable specified channel.
  * @param  obj: captouch object define in application software.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval none
  */
void captouch_ch_enable(captouch_t *obj, u8 Channel)
{
	/* To avoid gcc warnings */
	( void ) obj;

	CapTouch_ChCmd(CAPTOUCH_DEV, Channel, ENABLE);
}

/**
  * @brief  disable specified channel.
  * @param  obj: captouch object define in application software.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval none
  */
void captouch_ch_disable(captouch_t *obj, u8 Channel)
{
	/* To avoid gcc warnings */
	( void ) obj;

	CapTouch_ChCmd(CAPTOUCH_DEV, Channel, DISABLE);
}

/**
  * @brief  set captouch scan interval.
  * @param  obj: captouch object define in application software.
  * @param Interval: scan interval in units of ms
  * @retval none
  */
void captouch_set_scan_interval(captouch_t *obj, u32 Interval)
{
	/* To avoid gcc warnings */
	( void ) obj;

	CapTouch_SetScanInterval(CAPTOUCH_DEV, Interval);
}

/**
  * @brief  read baseline data from specified channel.
  * @param  obj: captouch object define in application software.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval baseline data
  */
u32 captouch_get_ch_baseline(captouch_t *obj, u8 Channel)
{
	/* To avoid gcc warnings */
	( void ) obj;

	return CapTouch_GetChBaseline(CAPTOUCH_DEV, Channel);
}

/**
  * @brief  read average data from specified channel.
   * @param  obj: captouch object define in application software.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval average data
  */
u32 captouch_get_ch_data(captouch_t *obj, u8 Channel)
{
	/* To avoid gcc warnings */
	( void ) obj;

	return CapTouch_GetChAveData(CAPTOUCH_DEV, Channel);
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
