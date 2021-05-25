/**
  ******************************************************************************
  * @file    rtl8721d_ipc.h
  * @author
  * @version V1.0.0
  * @date    2017-11-06
  * @brief   This file contains all the functions prototypes for the Internal Processor Communication(IPC)
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_IPC_H_
#define _RTL8721D_IPC_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup IPC 
  * @brief IPC driver modules
  * @{
  */

/** @addtogroup IPC
  * @verbatim
  *****************************************************************************************
  * Internal Processor Communication(IPC) Introduction
  *****************************************************************************************
  *		-32 core-to-core interrupts.
  *		-16 hardware semephone.
  *		-CPU ID get.
  *****************************************************************************************
  * @endverbatim
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */
void IPC_INTConfig(IPC_TypeDef *IPCx, u8 IPC_ChNum, u32 NewState);
void IPC_IERSet(IPC_TypeDef *IPCx, u32 IPC_Chs);
u32 IPC_IERGet(IPC_TypeDef *IPCx);
void IPC_INTRequest(IPC_TypeDef *IPCx, u8 IPC_ChNum);
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_ChNum);
u32 IPC_INTGet(IPC_TypeDef *IPCx);
u32 IPC_CPUID(void);
u32 IPC_INTHandler(void *Data);
void IPC_INTUserHandler(u8 IPC_ChNum, VOID *IrqHandler, VOID *IrqData);
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */ 

/* Channel Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup IPC_Channel_Definitions IPC Channel Definitions
 * @{
 *****************************************************************************/
#define IPC_INT_CHAN_SHELL_SWITCH				0	/*!<  KM0 <--> KM4 Switch shell */
#define IPC_INT_CHAN_WIFI_FW					1	/*!<  KM0 <--  KM4 FW INFO*/
#define IPC_INT_CHAN_FLASHPG_REQ				2	/*!<  KM0 <-->  KM4 Flash Program REQUEST*/
#define IPC_INT_KM4_TICKLESS_INDICATION		3	/*!<  KM0 <--  KM4 tickless indicate */
/** @} */

/**************************************************************************//**
 * @defgroup IPC0_USER_BUF_Definitions LP IPC User IDX Definitions
 * @{
 *****************************************************************************/
#define IPC_USER_BUF_LOG_RP					7	/*!<  logbuf write pointer */
#define IPC_USER_BUF_LOG_WP					8	/*!<  logbuf read pointer */
/** @} */

/**************************************************************************//**
 * @defgroup IPC_SEM_IDX_Definitions LP IPC SEM IDX Definitions
 * @{
 *****************************************************************************/
#define IPC_SEM_INDEX_LOG							0	/*!<  KM0 <-- KM4 log print */

/** @} */


/**
  * @}
  */

/* Exported Types --------------------------------------------------------*/

/** @defgroup IPC_Exported_Constants IPC Exported Types
  * @{
  */ 

/* Other definations --------------------------------------------------------*/
typedef void (*IPC_IRQ_FUN)(VOID *Data, u32 IrqStatus, u32 ChanNum);

typedef enum
{
	IPC_USER_POINT = 0,
	IPC_USER_DATA	= 1
} USER_MSG_TYP_DEF;

typedef struct _IPC_INIT_TABLE_ {
	u32 USER_MSG_TYPE;
	void (*func)(VOID *Data, u32 IrqStatus, u32 ChanNum);
	VOID *IrqData;
}IPC_INIT_TABLE, *PIPC_INIT_TABLE;

extern const IPC_INIT_TABLE  ipc_init_config[];

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
