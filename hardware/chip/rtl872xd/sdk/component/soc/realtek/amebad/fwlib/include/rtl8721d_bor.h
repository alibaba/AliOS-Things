/**
  ******************************************************************************
  * @file    rtl8721d_bor.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the bor.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_BOR_H_
#define _RTL8721D_BOR_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup BOR
  * @brief BOR driver modules
  * @{
  */

/** @addtogroup BOR
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * functions:
  * 	- set bor mode
  * 	- bor high and low threshold set for reset or interrupt mode
  * 	- bor debounce timer set
  * 	- bor interrupt clear
  *
  *****************************************************************************************     
  * How to use BOR
  *****************************************************************************************
  *      To use the BOR mode, the following steps are mandatory:
  *       
  *      1. Choose bor mode using the follwoing functions.
  *			BOR_ModeSet(u32 Option, u32 NewStatus);
  *
  *      2. If Choose bor Interrupt mode, register bor Interrupt handler using the follwoing functions.
  *			InterruptRegister() : register bor Interrupt handler
  *			InterruptEn() : enable bor Interrupt
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup BOR_Exported_Types BOR Exported Types
  * @{
  */
  
/** @defgroup BOR_Exported_Constants BOR Exported Constants
  * @{
  */ 

/* Exported constants ------------------------------------------------------------*/

/** @defgroup BOR_Mode_definitions 
  * @{
  */
#define	BOR_RESET							((u32)(0x00000001 << 0))
#define	BOR_INTR							((u32)(0x00000001 << 1))
/**
  * @}
  */

/** @defgroup  BOR_Debounce_Threshold_definitions 
  * @{
  */ 
#define IS_BOR_INTR_DBNC_THRES(DBNC)    ((DBNC) <= 0x7FFF)
/**
  * @}
  */

/** @defgroup BOR_Threshold_HIGH_definitions 
  * @{
  */ 
#define BOR_TH_HIGH0			((u32)0x000000000)
#define BOR_TH_HIGH1			((u32)0x000000001)
#define BOR_TH_HIGH2			((u32)0x000000002)
#define BOR_TH_HIGH3			((u32)0x000000003)
#define BOR_TH_HIGH4			((u32)0x000000004)
#define BOR_TH_HIGH5			((u32)0x000000005)
#define BOR_TH_HIGH6			((u32)0x000000006)
#define BOR_TH_HIGH7			((u32)0x000000007)
#define IS_BOR_TH_HIGH(TYPE)	(((TYPE) == BOR_TH_HIGH0) || ((TYPE) == BOR_TH_HIGH1) || \
									((TYPE) == BOR_TH_HIGH2) || ((TYPE) == BOR_TH_HIGH3) || \
									((TYPE) == BOR_TH_HIGH4) || ((TYPE) == BOR_TH_HIGH5) || \
									((TYPE) == BOR_TH_HIGH6) || ((TYPE) == BOR_TH_HIGH7)) 
/**
  * @}
  */

/** @defgroup BOR_Threshold_LOW_definitions 
  * @{
  */ 
#define BOR_TH_LOW0			((u32)0x000000000)
#define BOR_TH_LOW1			((u32)0x000000001)
#define BOR_TH_LOW2			((u32)0x000000002)
#define BOR_TH_LOW3			((u32)0x000000003)
#define BOR_TH_LOW4			((u32)0x000000004)
#define BOR_TH_LOW5			((u32)0x000000005)
#define BOR_TH_LOW6			((u32)0x000000006)
#define BOR_TH_LOW7			((u32)0x000000007)
#define IS_BOR_TH_LOW(TYPE)		(((TYPE) == BOR_TH_LOW0) || ((TYPE) == BOR_TH_LOW1) || \
									((TYPE) == BOR_TH_LOW2) || ((TYPE) == BOR_TH_LOW3) || \
									((TYPE) == BOR_TH_LOW4) || ((TYPE) == BOR_TH_LOW5) || \
									((TYPE) == BOR_TH_LOW6) || ((TYPE) == BOR_TH_LOW7)) 
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup BOR_Exported_Functions BOR Exported Functions
  * @{
  */
VOID BOR_ThresholdSet(u32 Thres_Low, u32 Thres_High);
VOID BOR_DbncSet(u32 Option, u32 Dbnc_Value);
VOID BOR_ClearINT(void);
VOID BOR_ModeSet(u32 Option, u32 NewStatus);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

#endif //_RTL8721D_BOR_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

