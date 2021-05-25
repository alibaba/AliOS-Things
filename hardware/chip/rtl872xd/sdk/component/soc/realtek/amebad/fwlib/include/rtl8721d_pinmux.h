/**
  ******************************************************************************
  * @file    rtl8721d_pinmux.h
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

#ifndef _HAL_8721D_PINMUX_
#define _HAL_8721D_PINMUX_

/** @addtogroup AmebaD_Platform
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
  *		-[31:16]		reserved
  *		-[15]			pad shut down enable
  *		-[14]			H3L1 for SDIO pad, other pad reserved
  *		-[13]			pull resistor selection
  *		-[12]			schmitt trigger enable 
  *		-[11:10]		pad driving strength	
  *		-[9]			pull down resistor enable
  *		-[8]			pull up resistor enable
  *		-[7:5]			reserved for function id extend
  *		-[4:0]			function id
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
#define _PA_0		(0x00)	//0x400
#define _PA_1		(0x01)	//0x404
#define _PA_2		(0x02)	//0x408
#define _PA_3		(0x03)	//0x40C
#define _PA_4		(0x04)	//0x410
#define _PA_5		(0x05)	//0x414
#define _PA_6		(0x06)	//0x418
#define _PA_7		(0x07)	//0x41C
#define _PA_8		(0x08)	//0x420
#define _PA_9		(0x09)	//0x424
#define _PA_10		(0x0A)	//0x428
#define _PA_11		(0x0B)	//0x42C
#define _PA_12		(0x0C)	//0x430
#define _PA_13		(0x0D)	//0x434
#define _PA_14		(0x0E)	//0x438
#define _PA_15		(0x0F)	//0x43C
#define _PA_16		(0x10)	//0x440
#define _PA_17		(0x11)	//0x444
#define _PA_18		(0x12)	//0x448
#define _PA_19		(0x13)	//0x44C
#define _PA_20		(0x14)	//0x450
#define _PA_21		(0x15)	//0x454
#define _PA_22		(0x16)	//0x458
#define _PA_23		(0x17)	//0x45C
#define _PA_24		(0x18)	//0x460
#define _PA_25		(0x19)	//0x464
#define _PA_26		(0x1A)	//0x468
#define _PA_27		(0x1B)	//0x46C
#define _PA_28		(0x1C)	//0x470
#define _PA_29		(0x1D)	//0x474
#define _PA_30		(0x1E)	//0x478
#define _PA_31		(0x1F)	//0x47C

#define _PB_0		(0x20)	//0x480
#define _PB_1		(0x21)	//0x484
#define _PB_2		(0x22)	//0x488
#define _PB_3		(0x23)	//0x48C
#define _PB_4		(0x24)	//0x490
#define _PB_5		(0x25)	//0x494
#define _PB_6		(0x26)	//0x498
#define _PB_7		(0x27)	//0x49C
#define _PB_8		(0x28)	//0x4A0
#define _PB_9		(0x29)	//0x4A4
#define _PB_10		(0x2A)	//0x4A8
#define _PB_11		(0x2B)	//0x4AC
#define _PB_12		(0x2C)	//0x4B0
#define _PB_13		(0x2D)	//0x4B4
#define _PB_14		(0x2E)	//0x4B8
#define _PB_15		(0x2F)	//0x4Bc
#define _PB_16		(0x30)	//0x4C0
#define _PB_17		(0x31)	//0x4C4
#define _PB_18		(0x32)	//0x4C8
#define _PB_19		(0x33)	//0x4CC
#define _PB_20		(0x34)	//0x4D0
#define _PB_21		(0x35)	//0x4D4
#define _PB_22		(0x36)	//0x4D8
#define _PB_23		(0x37)	//0x4DC
#define _PB_24		(0x38)	//0x4E0
#define _PB_25		(0x39)	//0x4E4
#define _PB_26		(0x3A)	//0x4E8
#define _PB_27		(0x3B)	//0x4EC
#define _PB_28		(0x3C)	//0x4F0
#define _PB_29		(0x3D)	//0x4F4
#define _PB_30		(0x3E)	//0x4F8
#define _PB_31		(0x3F)	//0x4FC

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

/** @defgroup PINMUX_PAD_Control_definitions 
  * @{
  */
#define PAD_BIT_SHUT_DWON					BIT(15)		
#define PAD_BIT_SDIO_H3L1					BIT(14)
#define PAD_BIT_PULL_RESISTOR_SEL			BIT(13)				
#define PAD_BIT_SCHMITT_TRIGGER_EN		BIT(12)	
#define PAD_BIT_PULL_RESISTOR_SMALL		BIT(11) /* for PAD C/F/G */
#define PAD_BIT_SHIFT_DRIVING_STRENGTH	10	
#define PAD_BIT_MASK_DRIVING_STRENGTH	0x03				
#define PAD_BIT_PULL_DOWN_RESISTOR_EN	BIT(9)				
#define PAD_BIT_PULL_UP_RESISTOR_EN		BIT(8)
#define PAD_BIT_SHIFT_FUNCTION_ID			0
#define PAD_BIT_MASK_FUNCTION_ID			0x1F		
/**
  * @}
  */

/** @defgroup GPIO_Pull_Resistor_definitions 
  * @{
  */ 
#define GPIO_Resistor_LARGE		0x00 /*!< GPIO Resistor LARGE */
#define GPIO_Resistor_SMALL		0x01 /*!< GPIO Resistor SMALL */
/**
  * @}
  */

/** @defgroup PINMUX_Function_definitions 
  * @{
  */
#define PINMUX_FUNCTION_GPIO				(0)
#define PINMUX_FUNCTION_UART				(1)

#define PINMUX_FUNCTION_UART_RTSCTS		(2)
#define PINMUX_FUNCTION_LOGUART			(2)

#define PINMUX_FUNCTION_SPIM				(3)
#define PINMUX_FUNCTION_SPIS				(3)

#define PINMUX_FUNCTION_RTC				(4)
#define PINMUX_FUNCTION_TIMINPUT			(4)
#define PINMUX_FUNCTION_EXT32K			(28)
#define PINMUX_FUNCTION_RTCOUT			(28)
#define PINMUX_FUNCTION_TIMINPUT_HS		(22)

#define PINMUX_FUNCTION_IR					(5)
#define PINMUX_FUNCTION_SPIF				(6)
#define PINMUX_FUNCTION_I2C				(7)

#define PINMUX_FUNCTION_SDIOD				(8)
#define PINMUX_FUNCTION_SDIOH				(8)

#define PINMUX_FUNCTION_PWM				(9)
#define PINMUX_FUNCTION_PWM_HS			(9)
#define PINMUX_FUNCTION_PWM_LP			(10)
#define PINMUX_FUNCTION_SWD				(11)

#define PINMUX_FUNCTION_I2S				(12)
#define PINMUX_FUNCTION_DMIC				(12)

#define PINMUX_FUNCTION_KEYSCAN_ROW		(29)
#define PINMUX_FUNCTION_KEYSCAN_COL		(30)
#define PINMUX_FUNCTION_LCD				(13)
#define PINMUX_FUNCTION_USB				(14)
#define PINMUX_FUNCTION_QDEC				(15)
#define PINMUX_FUNCTION_SGPIO				(16)
#define PINMUX_FUNCTION_RFE				(18)
#define PINMUX_FUNCTION_BTCOEX			(19)

#define PINMUX_FUNCTION_WIFIFW			(20)
#define PINMUX_FUNCTION_EXT_PCM			(20)
#define PINMUX_FUNCTION_BB_PIN			(21)
#define PINMUX_FUNCTION_SIC				(22)
#define PINMUX_FUNCTION_WAKEUP			(31)
#define PINMUX_FUNCTION_DBGPORT			(23)
#define PINMUX_FUNCTION_BBDBG				(25)

/**
  * @}
  */

/** @defgroup PINMUX_Peripheral_Location_definitions
  * @note just used by function PINMUX_Ctrl
  * @{
  */
#define PINMUX_S0		(0)
#define PINMUX_S1		(1)
#define PINMUX_S2		(2)
#define PINMUX_S3		(3)
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
_LONG_CALL_ void Pinmux_UartLogCtrl(u32  PinLocation, BOOL   Operation);
_LONG_CALL_ void Pinmux_SpicCtrl(u32  PinLocation, BOOL Operation);
/**
  *  @brief Set the Internal pad Resistor type.
  *  @param PinName : value of @ref PINMUX_Pin_Name_definitions.
  *  @param PullType : the pull type for the pin.This parameter can be one of the following values:
  *		 @arg GPIO_Resistor_LARGE
  *		 @arg GPIO_Resistor_SMALL
  *  @retval None
  *  @note Just for PAD C/F/G:
  *  @note PA[12]/PA[13]/PA[14]/PA[15]/PA[16]/PA[17]/PA[18]/PA[19]/PA[20]/PA[21]/PA[25]/PA[26] 4.7K/50K
  *  @note PA[29]/PA[30]/PA[31] 4.7K/10K
  */
static inline void PAD_ResistorCtrl(u8 PinName, u8 RType)
{
	u32 Temp = 0;

	/* get PADCTR */
	Temp = PINMUX->PADCTR[PinName];

	/* set resistor small */
	Temp |= PAD_BIT_PULL_RESISTOR_SMALL; /* by default is small */

	/* set large if needed */
	if (RType == GPIO_Resistor_LARGE) {
		Temp &= ~PAD_BIT_PULL_RESISTOR_SMALL;
	}

	/* set PADCTR register */
	PINMUX->PADCTR[PinName] = Temp;
}


/**
  *  @brief Turn off pinmux SWD function.
  *  @retval None
  */
static inline void Pinmux_Swdoff(void)
{
	u32 Temp = 0;
	 
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SWD_PMUX_EN);	
	Temp &= (~BIT_LSYS_SWD_PMUX_EN);	
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SWD_PMUX_EN, Temp);
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

/* Other definations --------------------------------------------------------*/
#define FLASH_S0_CS_GPIO		_PB_18
#define FLASH_S1_CS_GPIO		_PB_16



#endif   //_HAL_8721D_PINMUX_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

