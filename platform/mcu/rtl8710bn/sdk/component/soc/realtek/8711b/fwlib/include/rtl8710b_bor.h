/**
  ******************************************************************************
  * @file    rtl8711b_bor.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the bor2.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_BOR_H_
#define _RTL8710B_BOR_H_

/** @addtogroup AmebaZ_Periph_Driver
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
  * 	- set bor2 mode
  * 	- enable/disable bor2 intr
  * 	- register bor2 intr handler
  *
  *****************************************************************************************     
  * How to use BOR2
  *****************************************************************************************
  *      To use the BOR2 mode, the following steps are mandatory:
  *       
  *      1. Choose bor2 mode using the follwoing functions.
  *			BOR2_ModeSet(u32 Option);
  *
  *      2. If Choose bor2 Interrupt mode, register bor2 Interrupt handler using the follwoing functions.
  *			BOR2_INTRegister(void *bor_intr_hook_fun);
  *        
  *      3. Enable bor2 using the follwoing functions.
  *			BOR2_INTCmd(ENABLE);
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup BOR2_Exported_Types BOR2 Exported Types
  * @{
  */

/** 
  * @brief  BOR_IRQ_FUN function definition  
  */
typedef void (*BOR_IRQ_FUN)(void);
/**
  * @}
  */

/**
  * @}
  */
  
/** @defgroup BOR2_Exported_Constants BOR2 Exported Constants
  * @{
  */ 

/* Exported constants ------------------------------------------------------------*/

/** @defgroup BOR2_Mode_definitions 
  * @{
  */
#define	BOR2_RESET							((u32)(0x00000001 << 0))
#define	BOR2_INTR							((u32)(0x00000001 << 1))
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup BOR2_Exported_Functions BOR2 Exported Functions
  * @{
  */

VOID BOR2_INTRegister(void *bor_intr_hook_fun);
VOID BOR2_INTCmd(u32 NewStatus);
VOID BOR2_ModeSet(u32 Option);
/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */ 

#endif //_RTL8710B_BOR_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

