/**
  ******************************************************************************
  * @file    rtl8710b_backup_reg.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the 16bytes backup registers
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_BACKUP_REG_H_
#define _RTL8710B_BACKUP_REG_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup BKUP_REG 
  * @brief BKUP_REG driver modules
  * @{
  */

/** @addtogroup BKUP_REG
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * backup register size:
  *		-16bytes (4 dwords)
  *
  * usage:
  *		- user can use this registers to save some data before reset happens
  *
  * backup register can not be reset by following functions:
  *		- cpu reset
  *		- system reset
  *		- soc sleep mode
  *		- soc deep standby mode
  *
  * backup register will be reset by following functions:
  *		- soc deep sleep mode
  *		- soc power down reset
  *		- soc power off
  *
  * system defined bits (other bits are reserved for user):
  *		- dword0[0]: watchdog reset or system reset (distinguish from BIT(5) by Software)
  *		- dword0[1]: BOR2 HW temp bit
  *		- dword0[2]: SW reserved
  *		- dword0[3]: this is SW set bit before reboot, for uart download
  *		- dword0[4]: this is SW set bit before reboot, for uart download debug
  *		- dword0[5]: this is SW set bit before reboot, for distinguish watchdog and system reset
  *		- dword0[6]: BOR2 HW temp bit
  *		- dword0[7]: 1: enable bor2 detection;  0: disable
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		BKUP_Write: write a dword backup register
  *		BKUP_Read: read a dword backup register
  *		BKUP_Set: set 1 to some bits of backup register
  *		BKUP_Clear: set 0 to some bits of backup register
  *****************************************************************************************     
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup BKUP_REG_Exported_Constants BKUP_REG Exported Constants
  * @{
  */ 

/** @defgroup BKUP_REG_Idx_definitions 
  * @{
  */ 
#define BKUP_REG0				((u32)0x00000000)	/*!< bit0~bit5 is defined by system, other bits can be used by user */
#define BKUP_REG1				((u32)0x00000001)	/*!< all bits can be used by user */
#define BKUP_REG2				((u32)0x00000002)	/*!< all bits can be used by user */
#define BKUP_REG3				((u32)0x00000003)	/*!< all bits can be used by user */
#define IS_BKUP_REG(IDX)	(((IDX) == BKUP_REG0) || \
							((IDX) == BKUP_REG1) ||\
							((IDX) == BKUP_REG2) ||\
							((IDX) == BKUP_REG3))
/**
  * @}
  */
/**
  * @}
  */


/** @defgroup BKUP_REG_Exported_Functions BKUP_REG Exported Functions
  * @{
  */
_LONG_CALL_ void BKUP_Write(u32 DwordIdx, u32 WriteVal);
_LONG_CALL_ u32 BKUP_Read(u32 DwordIdx);
_LONG_CALL_ void BKUP_Set(u32 DwordIdx, u32 BitMask);
_LONG_CALL_ void BKUP_Clear(u32 DwordIdx, u32 BitMask);
/**
  * @}
  */
  
/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup BKUP_REG_Register_Definitions BKUP_REG Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup BKUP_REG_WDORD0
 * @{
 *****************************************************************************/
#define BIT_SYS_BOR_DETECION				BIT(7)	/*!<  1: enable bor2 detection;  0: disable */
#define BIT_BOR2_RESET_TEMP				BIT(6)	/*!<  BOR2 HW temp bit */
#define BIT_SYS_RESET_HAPPEN				BIT(5)	/*!<  this is SW set bit before reboot, for distinguish watchdog and system reset */
#define BIT_UARTBURN_DEBUG				BIT(4)	/*!<  this is SW set bit before reboot, for uart download debug */
#define BIT_UARTBURN_BOOT					BIT(3)	/*!<  this is SW set bit before reboot, for uart download */
#define BIT_RTC_RESTORE						BIT(2)	/*!<  this is SW set bit after rtc init */
#define BIT_BOR2_RESET_HAPPEN				BIT(1)	/*!<  BOR2 (brown out reset) happen */
#define BIT_CPU_RESET_HAPPEN				BIT(0)	/*!<  watchdog reset or system reset (distinguish from BIT(5) by Software) */

#define BIT_RTC_BACKUP					((u32)0x0000FF00)	/*!< used to backup rtc year before reset */

/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */

#endif //_RTL8710B_BACKUP_REG_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
