/**
  ******************************************************************************
  * @file    rtl8721d_pinmap.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of pin control:
  *           - pinmux
  *           - active pad pull up & pull down
  *           - sleep pad pull up & pull down
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_PINMAP_H_
#define _RTL8710B_PINMAP_H_
  
/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup PIN
  * @brief PIN driver modules
  * @{
  */

/** @defgroup PINMAP
  * @brief PINMAP driver modules
  * @{
  */

/** @addtogroup PINMAP
  * @verbatim
  *****************************************************************************************
  * PINMAP
  *****************************************************************************************
  *   you can use pinmap to config pinmux instead of Pinmux_Config function
  *   you can use pinmap to config GPIO pull high/low status for power save when enter power save mode
  *****************************************************************************************
  * How To Use
  *****************************************************************************************
  *	-1) ENABLE MACRO: CONFIG_PINMAP_ENABLE
  *	-2) Set all pins function in pmap_func based on your board
  *	-3) Bootloader will call pinmap_init, then all pinmux will be set based on pmap_func
  *	-4) pinmap_sleep will be called when enter sleep mode, all GPIO will pull high or low based on pmap_func
  *	-5) pinmap_wake will be called when wake from sleep mode, all GPIO will back to active mode
  *****************************************************************************************     
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup PINMAP_Exported_Types PINMAP Exported Types
  * @{
  */

/** 
  * @brief  PINMAP Init structure definition  
  */
typedef struct
{
	u32 PinName; /*!< Specifies the pin name, This parameter can be a value of @ref PINMUX_Pin_Name_definitions */
	u32 FuncPuPd;/*!< Specifies the pin function PU/PD, This parameter can be a value of @ref GPIO_Pull_parameter_definitions */
	u32 SleepPuPd;/*!< Specifies the pin sleep PU/PD, This parameter can be a value of @ref GPIO_Pull_parameter_definitions */
	u32 DSleepPuPd;/*!< Specifies the pin deep sleep PU/PD, This parameter can be a value of @ref GPIO_Pull_parameter_definitions */
	u32 LowPowerPin;/*!< Specifies if it is a low power pin or touch pin, if so, this pin will keep state in DSLP state */
} PMAP_TypeDef;

/**
  * @}
  */
  
/* Exported functions --------------------------------------------------------*/
/** @defgroup PINMAP_Exported_Functions PINMAP Exported Functions
  * @{
  */
void pinmap_init(void);
void pinmap_sleep(void);
void pinmap_deepsleep(void);
void pinmap_wake(void);
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

/* Other definations --------------------------------------------------------*/
#define GPIO_PuPd_KEEP	0xFF /* keep pupd unchanged */
#endif //_RTL8710B_PINMAP_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
