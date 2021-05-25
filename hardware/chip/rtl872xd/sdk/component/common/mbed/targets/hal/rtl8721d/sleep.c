/** mbed Microcontroller Library
  ******************************************************************************
  * @file    sleep.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for SLEEP.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "sleep_ex_api.h"
#include "cmsis.h"

/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_SLEEP
 *  @brief      MBED_SLEEP driver modules.
 *  @{
 */

/** @defgroup MBED_SLEEP_Exported_Functions MBED_SLEEP Exported Functions
  * @{
  */

/**
  * @brief  AmebaD don't support sleep api, please refer to AN400 power save Section
  */
void sleep_ex(uint32_t wakeup_event,  uint32_t sleep_duration)
{
	/* To avoid gcc warnings */
	( void ) wakeup_event;
	( void ) sleep_duration;
	
	assert_param(0);
}

/**
  * @brief  AmebaD don't support sleep api, please refer to AN400 power save Section
  */
void sleep_ex_selective(uint32_t wakeup_event,  uint32_t sleep_duration, uint32_t clk_sourec_enable, uint32_t sdr_enable)
{
	/* To avoid gcc warnings */
	( void ) wakeup_event;
	( void ) sleep_duration;
	( void ) clk_sourec_enable;
	( void ) sdr_enable;
	
	assert_param(0);
}

/**
  * @brief  AmebaD don't support deep sleep api, please refer to AN400 power save Section
  */
void deepsleep_ex(uint32_t wakeup_event, uint32_t sleep_duration)
{
	/* To avoid gcc warnings */
	( void ) wakeup_event;
	( void ) sleep_duration;
	
	assert_param(0);
}

/**
  * @brief  AmebaD don't support deep sleep api, please refer to AN400 power save Section
  */
int deepsleep_get_bootup_reason(void)
{
	assert_param(0);
	return 0;
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
