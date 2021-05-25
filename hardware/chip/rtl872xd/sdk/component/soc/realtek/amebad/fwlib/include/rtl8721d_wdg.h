/**
  ******************************************************************************
  * @file    rtl8721d_wdg.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the WDG firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_WDG_H_
#define _RTL8721D_WDG_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup WDG 
  * @brief WDG driver modules
  * @{
  */

/** @addtogroup WDG
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * WDG:
  * 	- Base Address: VENDOR_REG_BASE
  * 	- Timer clk: SDM 32k
  * 	- Generates MCU reset or WDG interrupt on expiry of a programmed time period,
  * 	  unless the program refreshes the watchdog
  * 	- IRQ: WDG_IRQ
  *
  *****************************************************************************************
  * WDG Register
  *****************************************************************************************
  *  [31]		 R/W1C	 Wdt_to 			 Watch dog timer timeout. 1 cycle pulse
  *  [30]		 R/W	 Wdt_mode			 1: Reset system, 0: Interrupt CPU
  *  [29]		 R/W	 RSVD
  *  [28:25]	 R/W	 Cnt_limit			 0: 0x001
  * 										 1: 0x003
  * 										 2: 0x007
  * 										 3: 0x00F
  * 										 4: 0x01F
  * 										 5: 0x03F
  * 										 6: 0x07F
  * 										 7: 0x0FF
  * 										 8: 0x1FF
  * 										 9: 0x3FF
  * 										 10: 0x7FF
  * 										 11~15: 0xFFF
  *  [24]		 W		 Wdt_clear			 Write 1 to clear timer
  *  [23:17]	 R/W	 RSVD
  *  [16]		 R/W	 Wdt_en_byte			 Set 0x1 to enable watch dog timer
  *  [15:0] 	 R/W	 BIT_VNDR_divfactor  "Dividing factor.Watch dog timer is count with 32.768KHz/(divfactor+1).
  * 										 Minimum dividing factor is 1."
  *
  *****************************************************************************************
  * How to use WGD
  *****************************************************************************************
  *      To use WDG peripheral, the following steps are mandatory:
  *
  *      1.  Get count ID and divisor factor according to WDG timeout period using
  *			WDG_Scalar(WDG_TEST_TIMEOUT, &CountProcess, &DivFacProcess);
  *
  *      2. Configure WDG with the corresponding configuration.
  *			WDG_Init(&WDG_InitStruct)
  *
  *      3. Activate the WDG peripheral:
  			WDG_Cmd(ENABLE).
  *
  *      @note	In interrupt mode, call WDG_IrqInit() function after WDG_Init()
  *
  *      @note	WDG_Refresh() function is used to clear timer, if call this function before timeout period,
  *			then MCU reset or WDG interrupt won't generate
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup WDG_Exported_Types WDG Exported Types
  * @{
  */

/** 
  * @brief  WDG Init structure definition  
  */
typedef struct
{
	u32 CountProcess; 		/*!< WDG parameter get from WDG_Scalar, Specifies count id of WDG
								This parameter must be set to a value in the 0-11 range */

	u32 DivFacProcess; 	/*!< WDG parameter get from WDG_Scalar, Specifies WDG timeout count divisor factor
								This parameter must be set to a value in the 1-65535 range */

	u32 RstAllPERI;		/*!< WDG parameter, Specifies WDG reset all the PERIs in HS or not
								This parameter must be set to a value of 0 or 1 */
} WDG_InitTypeDef;
/**
  * @}
  */

/** @defgroup WDG_Exported_Functions WDG Exported Functions
  * @{
  */
_LONG_CALL_ void WDG_Scalar(u32 Period, u32 *pCountProcess, u32 *pDivFacProcess);
_LONG_CALL_ void WDG_Init(WDG_InitTypeDef *WDG_InitStruct);
_LONG_CALL_ void WDG_IrqInit(void *handler, u32 Id);
_LONG_CALL_ void WDG_Cmd(u32 NewState);
_LONG_CALL_ void WDG_Refresh(void);
/**
  * @}
  */
  

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup WDG_Register_Definitions WDG Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup WDG_REG
 * @{
 *****************************************************************************/
#define WDG_BIT_ENABLE			((u32)0x00000001 << 16)
#define WDG_BIT_CLEAR			((u32)0x00000001 << 24)
#define WDG_BIT_RST_MODE		((u32)0x00000001 << 30)
#define WDG_BIT_ISR_CLEAR		((u32)0x00000001 << 31)
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */ 

#endif //_RTL8721D_WDG_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
