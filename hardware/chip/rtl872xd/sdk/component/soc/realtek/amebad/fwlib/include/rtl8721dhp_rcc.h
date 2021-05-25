/**
  ******************************************************************************
  * @file    rtl8721dhp_rcc.h
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

#ifndef _RTL8721D_HP_RCC_H_
#define _RTL8721D_HP_RCC_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup RCC 
  * @brief RCC driver modules
  * @{
  */


/** @defgroup HS_RCC
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

/** @addtogroup HS_RCC 
  * @brief HS_RCC driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup HS_RCC_CLK_Exported_Constants HS_RCC CLK Exported Constants
  * @{
  */

#define SYS_CLK_CTRL1					0x03 //0x210
#define SYS_CLK_CTRL2					0x02
#define SYS_CLK_CTRL3					0x01
#define SYS_CLK_CTRL4					0x00
#define APBPeriph_CLOCK_NULL			0 //if you dont want to set any clock, you can use this

#define APBPeriph_PSRAM_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HSYS_PSRAM_CKE | BIT_SHIFT_HSYS_PSRAM_CKSL_100)
#define APBPeriph_AUDIOC_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HSYS_AC_CK)
#define APBPeriph_VENDOR_REG_CLOCK	(SYS_CLK_CTRL1  << 30 | BIT_HS_VNDR_CKE)
#define APBPeriph_USI_CLOCK				(SYS_CLK_CTRL1  << 30 | BIT_HS_USI_CKE)
#define APBPeriph_IRDA_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HS_IRDA_CKE)
#define APBPeriph_IPC_CLOCK				(SYS_CLK_CTRL1  << 30 | BIT_HS_IPC_CKE)
#define APBPeriph_GTIMER_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HS_TIMER0_CKE)
#define APBPeriph_SPI1_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HS_SPI1_CKE)
#define APBPeriph_SPI0_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HS_SPI0_CKE)
#define APBPeriph_UART1_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HS_UART1_CKE)
#define APBPeriph_UART0_CLOCK			(SYS_CLK_CTRL1  << 30 | BIT_HS_UART0_CKE)

#define APBPeriph_BT_CLOCK				(SYS_CLK_CTRL2  << 30 | BIT_HSYS_BT_CKE)
#define APBPeriph_WL_CLOCK				(SYS_CLK_CTRL2  << 30 | BIT_HSYS_WLAN_CKSL_AXIF | BIT_HSYS_WLAN_CKE_AXIF)
#define APBPeriph_GDMA0_CLOCK			(SYS_CLK_CTRL2  << 30 | BIT_HSYS_GDMA0_CKE)
#define APBPeriph_LCDC_CLOCK			(SYS_CLK_CTRL2  << 30 | BIT_HSYS_LCDC_CKE)
#define APBPeriph_I2S0_CLOCK			(SYS_CLK_CTRL2  << 30 | BIT_HSYS_I2S0_CKE)
#define APBPeriph_SEC_ENG_CLOCK		(SYS_CLK_CTRL2  << 30 | BIT_HSYS_IPSEC_CKE)
#define APBPeriph_LXBUS_CLOCK			(SYS_CLK_CTRL2  << 30 | BIT_HSYS_LX1BUS_CKE)

#define APBPeriph_SPORT_CLOCK			(SYS_CLK_CTRL3  << 30 | BIT_HSYS_SPORT_CKE)
#define APBPeriph_OTG_CLOCK			(SYS_CLK_CTRL3  << 30 | BIT_HSYS_USBOTG_CKE)
#define APBPeriph_SDIOH_CLOCK			(SYS_CLK_CTRL3  << 30 | BIT_HSYS_SDH_CKE_SCLK | BIT_HSYS_SDH_CKE_BCLK)
#define APBPeriph_SDIOD_CLOCK			(SYS_CLK_CTRL3  << 30 | BIT_HSYS_SDD_CKE)
/**
  * @}
  */

/** @defgroup AON_RCC_CLK_Exported_Constants Exported Constants
  * @{
  */ 
#define APBPeriph_RTC_CLOCK				(BIT_AON_RTC_CKE)
#define APBPeriph_CTOUCH_CLOCK			(BIT_AON_CTOUCH_CKE)
#define APBPeriph_CK32KGEN_CLOCK		(BIT_AON_CK32KGEN_CKE)
#define APBPeriph_KEYSCAN_CLOCK		(BIT_AON_KEYSCAN_CKE)
#define APBPeriph_TSF_CLOCK				(BIT_AON_TSF_CKE)
/**
  * @}
  */

/** @defgroup HS_RCC_FUNC_Exported_Constants HS_RCC FUNC Exported Constants
  * @{
  */ 

#define SYS_FUNC_EN1					0x03 //0x200
#define SYS_FUNC_EN2					0x02 //0x204
#define SYS_FUNC_EN3					0x01 //0x208
#define SYS_FUNC_EN4					0x00 //0x
#define APBPeriph_NULL					0 //if you dont want to set any function, you can use this

#define APBPeriph_PSRAM					(SYS_FUNC_EN1  << 30 | BIT_HSYS_PSRAM_FEN)
#define APBPeriph_AUDIOC				(SYS_FUNC_EN1  << 30 | BIT_HSYS_AC_FEN)
#define APBPeriph_VENDOR_REG			(SYS_FUNC_EN1  << 30 | BIT_HS_VNDR_FEN)
#define APBPeriph_USI_REG				(SYS_FUNC_EN1  << 30 | BIT_HS_USI_FEN)
#define APBPeriph_IRDA_REG				(SYS_FUNC_EN1  << 30 | BIT_HS_IRDA_FEN)
#define APBPeriph_IPC					(SYS_FUNC_EN1  << 30 | BIT_HS_IPC_FEN)
#define APBPeriph_GTIMER				(SYS_FUNC_EN1  << 30 | BIT_HS_TIMER0_FEN)
#define APBPeriph_SPI1					(SYS_FUNC_EN1  << 30 | BIT_HS_SPI1_FEN)
#define APBPeriph_SPI0					(SYS_FUNC_EN1  << 30 | BIT_HS_SPI0_FEN)
#define APBPeriph_UART1					(SYS_FUNC_EN1  << 30 | BIT_HS_UART1_FEN_FUN | BIT_HS_UART1_FEN_GLB)
#define APBPeriph_UART0					(SYS_FUNC_EN1  << 30 | BIT_HS_UART0_FEN_FUN | BIT_HS_UART0_FEN_GLB)

#define APBPeriph_BT						(SYS_FUNC_EN2  << 30 | BIT_HSYS_BT_FEN)
#define APBPeriph_WL					(SYS_FUNC_EN2  << 30 | BIT_HSYS_WLAN_FEN_AXIF)
#define APBPeriph_GDMA0				(SYS_FUNC_EN2  << 30 | BIT_HSYS_GDMA0_FEN)
#define APBPeriph_LCDC					(SYS_FUNC_EN2  << 30 | BIT_HSYS_LCDC_FEN)
#define APBPeriph_I2S0					(SYS_FUNC_EN2  << 30 | BIT_HSYS_I2S0_FEN)
#define APBPeriph_SECURITY_ENGINE		(SYS_FUNC_EN2  << 30 | BIT_HSYS_IPSEC_FEN)
#define APBPeriph_LXBUS					(SYS_FUNC_EN2  << 30 | BIT_HSYS_LX1BUS_FEN)

#define APBPeriph_SPORT					(SYS_FUNC_EN3  << 30 | BIT_HSYS_SPORT_FEN)
#define APBPeriph_OTG					(SYS_FUNC_EN3  << 30 | BIT_HSYS_USBOTG_FEN)
#define APBPeriph_SDIOH					(SYS_FUNC_EN3  << 30 | BIT_HSYS_SDH_FEN_SCKGEN | BIT_HSYS_SDH_FEN)
#define APBPeriph_SDIOD					(SYS_FUNC_EN3  << 30 | BIT_HSYS_SDD_FEN_OFF | BIT_HSYS_SDD_FEN_ON)
/**
  * @}
  */

/** @defgroup AON_RCC_FUNC_Exported_Constants Exported Constants
  * @{
  */ 
#define APBPeriph_RTC					(BIT_AON_RTC_FEN)
#define APBPeriph_CTOUCH				(BIT_AON_CTOUCH_FEN)
#define APBPeriph_CK32KGEN				(BIT_AON_CK32KGEN_FEN)
#define APBPeriph_KEYSCAN				(BIT_AON_KEYSCAN_FEN)
#define APBPeriph_TSF				  		(BIT_AON_TSF_FEN)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HS_RCC_Exported_Functions HS_RCC Exported Functions
  * @{
  */
_LONG_CALL_ void RCC_PeriphClockCmd(u32 APBPeriph, u32 APBPeriph_Clock, u8 NewState);
_LONG_CALL_ void RCC_WIFIClockCmd(u8 NewState);
_LONG_CALL_ void RCC_PeriphClockSource_RTC(u32 Xtal);
_LONG_CALL_ void RCC_PeriphClockSource_I2C(UNUSED_WARN_DIS u32 Idx, u32 Source);
_LONG_CALL_ void RCC_PeriphClockSource_QDEC(UNUSED_WARN_DIS u32 Idx, u32 Source);
_LONG_CALL_ void RCC_PeriphClockSource_UART (UART_TypeDef* UARTx, u32 Source);

/**
  * @brief  Enables or disables the AON APB peripheral clock and function
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of @ref AON_RCC_FUNC_Exported_Constants
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of @ref AON_RCC_CLK_Exported_Constants
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  */
__STATIC_INLINE void RCC_PeriphClockCmd_AON(u32 APBPeriph, u32 APBPeriph_Clock, u8 NewState)
{
	u32 TempVal = 0;

	//clock
	if (APBPeriph_Clock != APBPeriph_CLOCK_NULL) {
		if(NewState == ENABLE)
		{
			TempVal = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL);
			TempVal |= APBPeriph_Clock;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL, TempVal);
		} else {
			TempVal = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL);
			TempVal &= ~APBPeriph_Clock;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL, TempVal);
		}
	}

	if (APBPeriph == APBPeriph_NULL)
		return;

	//function
	if(NewState == ENABLE)
	{
		TempVal = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL);
		TempVal |= APBPeriph;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL, TempVal);
	} else {
		TempVal = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL);
		TempVal &= ~APBPeriph;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_ISO_CTRL, TempVal);
	}

	return;
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

#endif /* _RTL8721D_HP_RCC_H_ */
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

