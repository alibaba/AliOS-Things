/**
  ******************************************************************************
  * @file    rtl8711b_rcc.h
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

#ifndef _RTL8711B_RCC_H_
#define _RTL8711B_RCC_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup RCC 
  * @brief RCC driver modules
  * @{
  */


/** @addtogroup RCC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		- functions prototypes for peripheral reset and clock control driver.
  *
  *****************************************************************************************
  * supported peripherals
  *****************************************************************************************
  *		-APBPeriph_SECURITY_ENGINE
  *		-APBPeriph_GTIMER
  *		-APBPeriph_GDMA1
  *		-APBPeriph_GDMA0
  *		-APBPeriph_FLASH
  *		-APBPeriph_OTG   
  *		-APBPeriph_UART0    
  *		-APBPeriph_UART1   
  *		-APBPeriph_LOGUART
  *		-APBPeriph_SPI0
  *		-APBPeriph_SPI1
  *		-APBPeriph_I2C0
  *		-APBPeriph_I2C1
  *		-APBPeriph_I2S0
  *		-APBPeriph_ADC
  *		-APBPeriph_GPIO
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


/* Exported constants --------------------------------------------------------*/

/** @defgroup RCC_CLK_Exported_Constants RCC Exported Constants
  * @{
  */

/** @defgroup CLK_CTRL_REG_Exported_Constants
  * @{
  */
#define PESOC_CLK_CTRL					0
#define PERI_CLK_CTRL0					1 //0x234
#define PERI_CLK_CTRL1					2 //0x238
#define PESOC_CLK_CTRL3					3 //0x23C
#define PESOC_HCI_CLK_CTRL0			4 //0x240
#define PESOC_COM_CLK_CTRL1			5 //0x244
/**
  * @}
  */

/** @defgroup REG_PESOC_CLK_CTRL_Exported_Constants 0x230 REG_PESOC_CLK_CTRL Exported Constants
  * @{
  */
#define APBPeriph_GPIO_CLOCK			(BIT_SOC_ACTCK_GPIO_EN)
#define APBPeriph_GDMA1_CLOCK			(BIT_SOC_ACTCK_GDMA1_EN)
#define APBPeriph_GDMA0_CLOCK			(BIT_SOC_ACTCK_GDMA0_EN)
#define APBPeriph_GTIMER_CLOCK			(BIT_SOC_ACTCK_TIMER_EN)
#define APBPeriph_LOGUART_CLOCK		(BIT_SOC_ACTCK_LOG_UART_EN)
#define APBPeriph_FLASH_CLOCK			(BIT_SOC_ACTCK_FLASH_EN)
#define APBPeriph_VENDOR_REG_CLOCK	(BIT_SOC_ACTCK_VENDOR_REG_EN)
#define APBPeriph_TRACE_CLOCK			(BIT_SOC_ACTCK_TRACE_EN)
/**
  * @}
  */

/** @defgroup REG_PESOC_PERI_CLK_CTRL0_Exported_Constants 234 REG_PESOC_PERI_CLK_CTRL0 Exported Constants
  * @{
  */
#define APBPeriph_UART0_CLOCK                 (BIT_SOC_ACTCK_UART0_EN | (PERI_CLK_CTRL0<<29))
#define APBPeriph_UART1_CLOCK                 (BIT_SOC_ACTCK_UART1_EN | (PERI_CLK_CTRL0<<29))
#define APBPeriph_SPI0_CLOCK                 (BIT_SOC_ACTCK_SPI0_EN | (PERI_CLK_CTRL0<<29))
#define APBPeriph_SPI1_CLOCK                 (BIT_SOC_ACTCK_SPI1_EN | (PERI_CLK_CTRL0<<29))
/**
  * @}
  */

/** @defgroup REG_PESOC_PERI_CLK_CTRL1_Exported_Constants 238 REG_PESOC_PERI_CLK_CTRL1 Exported Constants
  * @{
  */
#define APBPeriph_I2C0_CLOCK                 (BIT_SOC_ACTCK_I2C0_EN | (PERI_CLK_CTRL1<<29))
#define APBPeriph_I2C1_CLOCK                 (BIT_SOC_ACTCK_I2C1_EN | (PERI_CLK_CTRL1<<29))
#define APBPeriph_I2S0_CLOCK                 (BIT_SOC_ACTCK_I2S_EN | (PERI_CLK_CTRL1<<29))
#define APBPeriph_ADC_CLOCK                 (BIT_SOC_ACTCK_ADC_EN | (PERI_CLK_CTRL1<<29))
/**
  * @}
  */

/** @defgroup PESOC_HCI_CLK_CTRL0_Exported_Constants 240 PESOC_HCI_CLK_CTRL0 Exported Constants
  * @{
  */
#define APBPeriph_SDIOD_CLOCK			(BIT_SOC_ACTCK_SDIO_DEV_EN | (PESOC_HCI_CLK_CTRL0 << 29))
#define APBPeriph_SDIOH_CLOCK			(BIT_SOC_ACTCK_SDIO_HST_EN | (PESOC_HCI_CLK_CTRL0 << 29))
#define APBPeriph_OTG_CLOCK			(BIT_SOC_ACTCK_OTG_EN | (PESOC_HCI_CLK_CTRL0 << 29))
/**
  * @}
  */

/** @defgroup PESOC_COM_CLK_CTRL1_Exported_Constants 244 PESOC_COM_CLK_CTRL1 Exported Constants
  * @{
  */
#define APBPeriph_WL_CLOCK				(BIT_SOC_ACTCK_LXBUS_EN | (PESOC_COM_CLK_CTRL1 << 29))
#define APBPeriph_LXBUS_CLOCK			(BIT_SOC_ACTCK_LXBUS_EN | (PESOC_COM_CLK_CTRL1 << 29))
#define APBPeriph_SEC_ENG_CLOCK		(BIT_SOC_ACTCK_SECURITY_ENG_EN | (PESOC_COM_CLK_CTRL1 << 29))
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup RCC_CLK_Exported_Constants RCC Exported Constants
  * @{
  */ 

/** @defgroup FUNC_CTRL_REG_Exported_Constants
  * @{
  */
#define SOC_FUNC_EN					0 //210
#define SOC_HCI_COM_FUNC_EN			1 //214
#define SOC_PERI_FUNC0_EN				2 //218
#define SOC_PERI_FUNC1_EN				3 //21c
#define APBPeriph_NULL					0 //if you dont want to set any function, you can use this
#define APBPeriph_CLOCK_NULL			0 //if you dont want to set any clock, you can use this
/**
  * @}
  */

/** @defgroup REG_SOC_FUNC_EN_Exported_Constants 210 REG_SOC_FUNC_EN Exported Constants
  * @{
  */
#define APBPeriph_SECURITY_ENGINE		BIT_SOC_SECURITY_ENGINE_EN
#define APBPeriph_GTIMER				BIT_SOC_GTIMER_EN
#define APBPeriph_GDMA1				BIT_SOC_GDMA1_EN
#define APBPeriph_GDMA0				BIT_SOC_GDMA0_EN
#define APBPeriph_FLASH					BIT_SOC_FLASH_EN
#define APBPeriph_LXBUS					BIT_SOC_LXBUS_EN
/**
  * @}
  */

/** @defgroup REG_SOC_HCI_COM_FUNC_EN_Exported_Constants 214 REG_SOC_HCI_COM_FUNC_EN Exported Constants
  * @{
  */
#define APBPeriph_WLMAC			(BIT_SOC_HCI_WL_MACON_EN | SOC_HCI_COM_FUNC_EN << 30)
#define APBPeriph_SM				(BIT_SOC_HCI_SM_SEL | SOC_HCI_COM_FUNC_EN << 30)
#define APBPeriph_OTGRST			(BIT_SOC_HCI_OTG_RST_MUX | SOC_HCI_COM_FUNC_EN << 30)
#define APBPeriph_OTG				(BIT_SOC_HCI_OTG_EN | SOC_HCI_COM_FUNC_EN << 30)
#define APBPeriph_SDIOD_ON_RST		(BIT_SOC_HCI_SDIOD_ON_RST_MUX | SOC_HCI_COM_FUNC_EN << 30)
#define APBPeriph_SDIOD_OFF		(BIT_SOC_HCI_SDIOD_OFF_EN | SOC_HCI_COM_FUNC_EN << 30)
#define APBPeriph_SDIOD_ON			(BIT_SOC_HCI_SDIOD_ON_EN | SOC_HCI_COM_FUNC_EN << 30)
/**
  * @}
  */

/** @defgroup REG_SOC_PERI_FUNC0_EN_Exported_Constants 218 REG_SOC_PERI_FUNC0_EN Exported Constants
  * @{
  */
#define APBPeriph_UART0				(BIT_PERI_UART0_EN | SOC_PERI_FUNC0_EN << 30)
#define APBPeriph_UART1				(BIT_PERI_UART1_EN | SOC_PERI_FUNC0_EN << 30)
#define APBPeriph_LOGUART			(BIT_PERI_UART2_EN | SOC_PERI_FUNC0_EN << 30)

#define APBPeriph_SPI0                 (BIT_PERI_SPI0_EN | SOC_PERI_FUNC0_EN << 30)
#define APBPeriph_SPI1                 (BIT_PERI_SPI1_EN | SOC_PERI_FUNC0_EN << 30)

#define APBPeriph_I2C0                 (BIT_PERI_I2C0_EN | SOC_PERI_FUNC0_EN << 30)
#define APBPeriph_I2C1                 (BIT_PERI_I2C1_EN | SOC_PERI_FUNC0_EN << 30)

#define APBPeriph_I2S0                 (BIT_PERI_I2S0_EN | SOC_PERI_FUNC0_EN << 30)
/**
  * @}
  */

/** @defgroup REG_SOC_PERI_FUNC1_EN_Exported_Constants 21c REG_SOC_PERI_FUNC1_EN Exported Constants
  * @{
  */
#define APBPeriph_ADC                 (BIT_PERI_ADC0_EN | SOC_PERI_FUNC1_EN << 30)
#define APBPeriph_GPIO                 (BIT_PERI_GPIO_EN | SOC_PERI_FUNC1_EN << 30)
/**
  * @}
  */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup RCC_Exported_Functions RCC Exported Functions
  * @{
  */
_LONG_CALL_ void RCC_PeriphClockCmd(u32 APBPeriph, u32 APBPeriph_Clock, u8 NewState);
_LONG_CALL_ void FUNC_HCI_COM(u32 Function_Bit, u8 NewState);
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

#endif /* _RTL8711B_RCC_H_ */
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
