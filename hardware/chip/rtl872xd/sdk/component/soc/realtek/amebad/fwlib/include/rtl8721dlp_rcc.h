/**
  ******************************************************************************
  * @file    rtl8721dlp_rcc.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for peripheral reset and clock control driver.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_LP_RCC_H_
#define _RTL8721D_LP_RCC_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup RCC 
  * @brief RCC driver modules
  * @{
  */


/** @defgroup LP_RCC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		- functions prototypes for peripheral reset and clock control driver.
  *
  *****************************************************************************************
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  * use UART0 as example:
  *		RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @addtogroup LP_RCC 
  * @brief LP_RCC driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup LP_RCC_CLK_Exported_Constants LP_RCC CLK Exported Constants
  * @{
  */
#define SYS_CLK_CTRL0					0x00 //0x210
#define SYS_CLK_CTRL1					0x01 //0x214
#define SYS_CLK_CTRL2					0x02 //0x004 AON
#define SYS_CLK_CTRL3					0x03 //TODO
#define APBPeriph_CLOCK_NULL			0 //if you dont want to set any clock, you can use this

#define APBPeriph_WLON_CLOCK			(SYS_CLK_CTRL0  << 30 | BIT_LSYS_WLON_CKE)
#define APBPeriph_FLASH_CLOCK			(SYS_CLK_CTRL0  << 30 | BIT_FLASH_CKE)
#define APBPeriph_GDMA0_CLOCK			(SYS_CLK_CTRL0  << 30 | BIT_LSYS_GDMA0_CKE)
#define APBPeriph_EFUSE_CLOCK			(SYS_CLK_CTRL0  << 30 | BIT_SYSON_CK_EELDR_EN)

#define APBPeriph_GPIO_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_LSYS_GPIO0_CKE)
#define APBPeriph_QDEC0_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_LSYS_QDEC0_CKE)
#define APBPeriph_SGPIO_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_LSYS_SPGIO0_CKE)
#define APBPeriph_I2C0_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_LSYS_I2C0_CKE)
#define APBPeriph_ADC_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_LSYS_ADC_CKE)
#define APBPeriph_UART1_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_LSYS_UART1_CKE)
#define APBPeriph_LOGUART_CLOCK		(SYS_CLK_CTRL1  << 30 | BIT_LSYS_UART0_CKE)
#define APBPeriph_GTIMER_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_LSYS_TIMER0_CKE)
#define APBPeriph_IPC_CLOCK				(SYS_CLK_CTRL1  << 30 | BIT_LSYS_IPC_CKE)
#define APBPeriph_VENDOR_REG_CLOCK	(SYS_CLK_CTRL1  << 30 | BIT_VENDOR_CKE)

#define APBPeriph_RTC_CLOCK				(SYS_CLK_CTRL2  << 30 | BIT_AON_RTC_CKE)
#define APBPeriph_CTOUCH_CLOCK			(SYS_CLK_CTRL2  << 30 | BIT_AON_CTOUCH_CKE)
#define APBPeriph_CK32KGEN_CLOCK		(SYS_CLK_CTRL2  << 30 | BIT_AON_CK32KGEN_CKE)
#define APBPeriph_KEYSCAN_CLOCK		    (SYS_CLK_CTRL2  << 30 | BIT_AON_KEYSCAN_CKE)
#define APBPeriph_TSF_CLOCK			    (SYS_CLK_CTRL2  << 30 | BIT_AON_TSF_CKE)

/* Switch SPIC clock using RCC_PeriphClockSource_SPIC(), don't use the following macro. Because SPIC has RCC_PeriphClockCmd has bug. */
#define APBPeriph_FLASH_CLOCK_ANA4M	(SYS_CLK_CTRL0  << 30 | BIT_FLASH_CKE | BIT_SHIFT_FLASH_CLK_ANA4M)
#define APBPeriph_FLASH_CLOCK_XTAL	(SYS_CLK_CTRL0  << 30 | BIT_FLASH_CKE | BIT_SHIFT_FLASH_CLK_XTAL)
#define APBPeriph_FLASH_CLOCK_PLL		(SYS_CLK_CTRL0  << 30 | BIT_FLASH_CKE | BIT_SHIFT_FLASH_CLK_PLL)

/**
  * @}
  */

/** @defgroup LP_RCC_FUNC_Exported_Constants  LP_RCC FUNC Exported Constants
  * @{
  */ 
#define SYS_FUNC_EN0					0x03 //0x208
#define SYS_FUNC_EN1					0x02 //0x20C
#define SYS_FUNC_EN2					0x01 //0x004 AON
#define SYS_FUNC_EN3					0x00 //TODO
#define APBPeriph_NULL					0 //if you dont want to set any function, you can use this

#define APBPeriph_WLON					(SYS_FUNC_EN0  << 30 | BIT_LSYS_WLON_FEN)
#define APBPeriph_FLASH					(SYS_FUNC_EN0  << 30 | BIT_FLASH_FUN_EN)
#define APBPeriph_GDMA0				(SYS_FUNC_EN0  << 30 | BIT_LSYS_GDMA0_FEN)
#define APBPeriph_EFUSE					(SYS_FUNC_EN0  << 30 | BIT_SYS_FEN_EELDR)

#define APBPeriph_GPIO					(SYS_FUNC_EN1  << 30 | BIT_LSYS_GPIO0_FEN)
#define APBPeriph_QDEC0					(SYS_FUNC_EN1  << 30 | BIT_LSYS_QDEC0_FEN)
#define APBPeriph_SGPIO					(SYS_FUNC_EN1  << 30 | BIT_LSYS_SPGIO0_FEN)
#define APBPeriph_I2C0					(SYS_FUNC_EN1  << 30 | BIT_LSYS_I2C0_FEN)
#define APBPeriph_ADC					(SYS_FUNC_EN1  << 30 | BIT_LSYS_ADC_FEN)
#define APBPeriph_UART1					(SYS_FUNC_EN1  << 30 | BIT_LSYS_UART1_FEN_GLB | BIT_LSYS_UART1_FEN_FUN)
#define APBPeriph_LOGUART				(SYS_FUNC_EN1  << 30 | BIT_LSYS_UART0_FEN_GLB | BIT_LSYS_UART0_FEN_FUN)
#define APBPeriph_GTIMER				(SYS_FUNC_EN1  << 30 | BIT_LSYS_TIMER0_FEN)
#define APBPeriph_IPC					(SYS_FUNC_EN1  << 30 | BIT_LSYS_IPC_FEN)
#define APBPeriph_VENDOR_REG			(SYS_FUNC_EN1  << 30 | BIT_VENDOR_EN)

#define APBPeriph_RTC					(SYS_FUNC_EN2  << 30 | BIT_AON_RTC_FEN)
#define APBPeriph_CTOUCH				(SYS_FUNC_EN2  << 30 | BIT_AON_CTOUCH_FEN)
#define APBPeriph_CK32KGEN				(SYS_FUNC_EN2  << 30 | BIT_AON_CK32KGEN_FEN)
#define APBPeriph_KEYSCAN				(SYS_FUNC_EN2  << 30 | BIT_AON_KEYSCAN_FEN)
#define APBPeriph_TSF				    (SYS_FUNC_EN2  << 30 | BIT_AON_TSF_FEN)

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup LP_RCC_Exported_Functions LP_RCC Exported Functions
  * @{
  */
_LONG_CALL_ void RCC_PeriphClockCmd(u32 APBPeriph, u32 APBPeriph_Clock, u8 NewState);
_LONG_CALL_ void RCC_PeriphClockSource_RTC(u32 Xtal);
_LONG_CALL_ void RCC_PeriphClockSource_I2C(UNUSED_WARN_DIS u32 Idx, u32 Source);
_LONG_CALL_ void RCC_PeriphClockSource_QDEC(UNUSED_WARN_DIS u32 Idx, u32 Source);
_LONG_CALL_ void RCC_PeriphClockSource_UART (UART_TypeDef* UARTx, u32 Source);

/**
  * @brief    Configure SPIC Clock.
  * @param  Source:  This parameter can be one of the following values:
  *                            @arg BIT_SHIFT_FLASH_CLK_ANA4M
  *                            @arg BIT_SHIFT_FLASH_CLK_XTAL
  *                            @arg BIT_SHIFT_FLASH_CLK_PLL
  * @retval   None
  * @note    Used to switch SPIC clock
  */
__STATIC_INLINE void RCC_PeriphClockSource_SPIC (u32 Source)
{
	u32 Temp = 0;

	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0);
	Temp &= ~ (BIT_MASK_FLASH_CLK_SEL << BIT_SHIFT_FLASH_CLK_SEL);
	Temp |= Source;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0, Temp);
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

/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/* Other definations --------------------------------------------------------*/

#endif /* _RTL8721D_LP_RCC_H_ */
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
