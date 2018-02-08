/**
  ******************************************************************************
  * @file    rtl8710b_pinmux.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the pinmux firmware
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

#ifndef _HAL_8710B_PINMUX_
#define _HAL_8710B_PINMUX_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup PIN
  * @brief PIN driver modules
  * @{
  */

/** @defgroup PINMUX 
  * @brief PINMUX modules
  * @{
  */

/** @addtogroup PINMUX
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-Every GPIO pin can be set to some function based on pinmux spec.
  *		-Every GPIO pin can set internal pull-up, pull-down based on pinmux spec.
  *
  *====================================================================
  * pad control Spec.
  *====================================================================
  *		-[15]		reservd
  *		-[14]		Schmitt trigger enable
  *		-[13]		H3L1 for SDIO pad, other pad reservd
  *		-[12]		slew rate
  *		-[11:9]		pad driving strength
  *		-[8]			pad shut down enable, 0: is shut down
  *		-[7]			pull down resistor enable
  *		-[6]			pull up resistor enable
  *		-[5:4]		rserved for function id extend
  *		-[3:0]		function id
  *
  *****************************************************************************************
  * How to use Pinmux
  ***************************************************************************************** 
  *		1. Set the Internal pad function type for  each pin using the follwoing function:
  *			Pinmux_Config(u8 PinName, u32 PinFunc)
  *
  *		2. Set the Internal pad pull type for each pin using the follwoing function:
  *			PAD_PullCtrl(u8 PinName, u8 PullType) 
  *
  *****************************************************************************************
  * @endverbatim
  */
  
/* Exported constants --------------------------------------------------------*/

/** @defgroup PINMUX_Exported_Constants PINMUX Exported Constants
  * @{
  */ 

/** @defgroup PINMUX_Pin_Name_definitions 
  * @note: Pin_Name = (((port)<<5)|(pin))
  * @{
  */ 
#define _PA_0		(0x00)
#define _PA_1		(0x01)
#define _PA_2		(0x02)
#define _PA_3		(0x03)
#define _PA_4		(0x04)
#define _PA_5		(0x05)
#define _PA_6		(0x06)
#define _PA_7		(0x07)
#define _PA_8		(0x08)
#define _PA_9		(0x09)
#define _PA_10		(0x0A)
#define _PA_11		(0x0B)
#define _PA_12		(0x0C)
#define _PA_13		(0x0D)
#define _PA_14		(0x0E)
#define _PA_15		(0x0F)
#define _PA_16		(0x10)
#define _PA_17		(0x11)
#define _PA_18		(0x12)
#define _PA_19		(0x13)
#define _PA_20		(0x14)
#define _PA_21		(0x15)
#define _PA_22		(0x16)
#define _PA_23		(0x17)
#define _PA_24		(0x18)
#define _PA_25		(0x19)
#define _PA_26		(0x1A)
#define _PA_27		(0x1B)
#define _PA_28		(0x1C)
#define _PA_29		(0x1D)
#define _PA_30		(0x1E)
#define _PA_31		(0x1F)
#define _PB_0		(0x20)
#define _PB_1		(0x21)
#define _PB_2		(0x22)
#define _PB_3		(0x23)
#define _PB_4		(0x24)
#define _PB_5		(0x25)
#define _PB_6		(0x26)
#define _PB_7		(0x27)
#define _PB_8		(0x28)
#define _PNC		(0xFFFFFFFF)
/**
  * @}
  */

/** @defgroup PINMUX_Port_And_Pin_definitions 
  * @{
  */
#define PORT_NUM(pin)	((pin>>5) & 0x03)
#define PIN_NUM(pin)		(pin & 0x1f)
/**
  * @}
  */

/** @defgroup PINMUX_Function_definitions 
  * @{
  */
#define PINMUX_FUNCTION_GPIO				(0)
#define PINMUX_FUNCTION_UART				(1)
#define PINMUX_FUNCTION_SPIM				(2)
#define PINMUX_FUNCTION_SPIS				(3)
#define PINMUX_FUNCTION_SPIF				(4)
#define PINMUX_FUNCTION_I2C				(5)
#define PINMUX_FUNCTION_SDIOD				(6)

#define PINMUX_FUNCTION_PWM				(7)
#define PINMUX_FUNCTION_TIMINPUT			(7)

#define PINMUX_FUNCTION_SWD				(8)
#define PINMUX_FUNCTION_EXT32K			(8)
#define PINMUX_FUNCTION_RTCOUT			(8)

#define PINMUX_FUNCTION_I2S				(9)
#define PINMUX_FUNCTION_COEX_EXT32K		(10)
#define PINMUX_FUNCTION_BTCOEX			(10)
#define PINMUX_FUNCTION_WLLED			(10)
/**
  * @}
  */

/** @defgroup PINMUX_Peripheral_definitions
  * @note just used by function PINMUX_Ctrl or PinCtrl
  * @{
  */
#define PERIPHERAL_UART0		(0)
#define PERIPHERAL_UART1		(1)
#define PERIPHERAL_SPI0			(3)
#define PERIPHERAL_SPI1			(4)
#define PERIPHERAL_I2C0			(5)
#define PERIPHERAL_I2C1			(6)
#define PERIPHERAL_I2S0			(7)
#define PERIPHERAL_LOG_UART	(8)
#define PERIPHERAL_SPI_FLASH	(10)
#define PERIPHERAL_SDIOD		(11)
#define PERIPHERAL_JTAG			(13)
/**
  * @}
  */


/** @defgroup PINMUX_Peripheral_Location_definitions
  * @note just used by function PINMUX_Ctrl or PinCtrl
  * @{
  */
#define S0		(0)
#define S1		(1)
#define S2		(2)
#define S3		(3)
/**
  * @}
  */

/** @defgroup PINMUX_PAD_DrvStrength_definitions 
  * @{
  */
#define PAD_DRV_STRENGTH_0			(0x00000000 << 9)
#define PAD_DRV_STRENGTH_1			(0x00000001 << 9)
#define PAD_DRV_STRENGTH_2			(0x00000002 << 9)
#define PAD_DRV_STRENGTH_3			(0x00000003 << 9)
#define PAD_DRV_STRENGTH_4			(0x00000004 << 9)
#define PAD_DRV_STRENGTH_5			(0x00000005 << 9)
#define PAD_DRV_STRENGTH_6			(0x00000006 << 9)
#define PAD_DRV_STRENGTH_7			(0x00000007 << 9)
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup PINMUX_Exported_Functions PINMUX Exported Functions
  * @{
  */
_LONG_CALL_ void PAD_CMD(u8 PinName, u8 NewStatus);
_LONG_CALL_ void PAD_DrvStrength(u8 PinName, u32 DrvStrength);
_LONG_CALL_ void PAD_PullCtrl(u8 PinName, u8 PullType);
_LONG_CALL_ void Pinmux_Config(u8 PinName, u32 PinFunc);
_LONG_CALL_ u32 Pinmux_ConfigGet(u8 PinName);
_LONG_CALL_ void Pinmux_Deinit(u8 PinName);
_LONG_CALL_ void PINMUX_Ctrl(u32  Function, u32  PinLocation, BOOL Operation);
_LONG_CALL_ void PINMUX_SWD_OFF(void);
_LONG_CALL_ BOOL PINMUX_SWD_REG(u32 Addr);
#define PinCtrl PINMUX_Ctrl
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup PIN
  * @brief PIN driver modules
  * @{
  */
/* Other definations --------------------------------------------------------*/



#endif   //_HAL_8710B_PINMUX_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

