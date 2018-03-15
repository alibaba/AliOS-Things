/**
  ******************************************************************************
  * @file    rtl8711b_diag.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for UART LOG firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _8710B_DIAG_H_
#define _8710B_DIAG_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup DIAG 
  * @brief DIAG driver modules
  * @{
  */

/** @addtogroup DIAG
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-Control API for LOGUART.
  *		-These API is used by system, user should not use these API if not needed.
  *		-LOGUART is UART2.
  *
  *****************************************************************************************
  * pinmux
  *****************************************************************************************
  *		-S0: GPIOA_16/17: QFN48, QFN68, QFN48-MCM.
  *		-S1: GPIOA_29/30: QFN32.
  *		-EFUSE 0x19[6]: 0: S1 PA29 & PA30, 1: S0 PA16 & PA17.
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DIAG_Exported_Constants DIAG Exported Constants
  * @{
  */ 

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup DIAG_Exported_Functions DIAG Exported Functions
  * @{
  */
_LONG_CALL_ void DIAG_UartInit(BOOL InitConsol);
_LONG_CALL_ void UartLogIrqHandle(void * Data);
_LONG_CALL_ void DIAG_UartReInit(IRQ_FUN IrqFun);

_LONG_CALL_ void LOGUART_PutChar(u8 c);
_LONG_CALL_ u8 LOGUART_GetChar(BOOL    PullMode);
_LONG_CALL_ u32 LOGUART_GetIMR(void);
_LONG_CALL_ void LOGUART_SetIMR (u32 SetValue);
_LONG_CALL_ void LOGUART_WaitBusy(void);

#define DiagPutChar		LOGUART_PutChar
#define DiagGetChar		LOGUART_GetChar
#define DiagGetIsrEnReg	LOGUART_GetIMR
#define DiagSetIsrEnReg	LOGUART_SetIMR
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
typedef struct _COMMAND_TABLE_ {
    const   u8* cmd;
    u16     ArgvCnt;
    u32     (*func)(u16 argc, u8* argv[]);
    const   u8* msg;
}COMMAND_TABLE, *PCOMMAND_TABLE;

typedef u32 (*monitor_cmd_handler)(u16 argc, u8* argv[]);

extern COMMAND_TABLE  UartLogRomCmdTable[];

#endif //_8710B_DIAG_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
