/**
  ******************************************************************************
  * @file    rtl8710b_delay.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the systimer & delay:
  *           - SYSTIMER_GetPassTime
  *           - SYSTIMER_TickGet
  *           - DelayUs
  *           - DelayMs
  *           - DelayNop
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _8710B_DELAY_H_
#define _8710B_DELAY_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup DELAY 
  * @brief DELAY driver modules
  * @{
  */

/** @addtogroup DELAY
  * @verbatim
  *****************************************************************************************
  * Delay
  *****************************************************************************************
  * 	- DelayUs
  * 	- DelayMs
  * 	- DelayNop
  *
  *****************************************************************************************
  * Sys Timer
  *****************************************************************************************
  * 	- TIM0 is used as systimer, so TIM0 can not be used for other purpose
  * 	- init when boot in rom code
  *	- resolution is 31us
  *	- you can get timer tick use SYSTIMER_TickGet, every tick is 31us
  *	- you can get passing time use SYSTIMER_GetPassTime in ms
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @defgroup DELAY_Exported_Functions DELAY Exported Functions
  * @{
  */
_LONG_CALL_ void SYSTIMER_Init(void);
_LONG_CALL_ u32 SYSTIMER_TickGet(void);
_LONG_CALL_ u32 SYSTIMER_GetPassTime(u32 start);
_LONG_CALL_ void DelayUs(u32 us);
_LONG_CALL_ void DelayMs(u32 ms);
_LONG_CALL_ void DelayNop(u32 count);
#define HalDelayUs DelayUs
/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */ 

#endif//_8710B_DELAY_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
