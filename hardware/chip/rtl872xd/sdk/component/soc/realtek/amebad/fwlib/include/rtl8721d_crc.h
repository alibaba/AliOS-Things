/**
  ******************************************************************************
  * @file    rtl8721d_crc.h
  * @author
  * @version V1.0.0
  * @date    2017-10-10
  * @brief   This file contains all the functions prototypes for the IPsec firmware
  *          library
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_CRC_H_
#define _RTL8721D_CRC_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup CRC
  * @brief CRYPTO driver modules
  * @{
  */

/** @addtogroup CRC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *	-CRC engine only used for IPsec secure mode
  *	-Register base address: 0x5007xxxx
  *	-CRC support mode: command mode and DMA mode
  *	-support programmable Polynomial coefficients, initial value, and XOR output value
  *****************************************************************************************    
  * How to use crc
  *****************************************************************************************    
  *	-open CRC function & clock
  *
  *	-call following API for set crc parameter:
  *		-CRC_Init(CRC_InitTypeDef* CRC_InitStruct)
  *
  *	-call following API for crc calculation:
  *		-CRC_Calculate
  *
  *****************************************************************************************     
  * @endverbatim
  */

#define CRC_S_BASE                 0x50022100
#define CRC_S_MODULE		((volatile CRC_TypeDef*)CRC_S_BASE)
/* Exported Types --------------------------------------------------------*/
/** @defgroup CRC_Exported_Types CRC Exported Types
  * @{
  */

/**
  * @brief	CRC Init structure definition
  */
typedef struct {
	u32 CRC_Type;		/*!< Specifies CRC type
								This parameter can be a value of @ref CRC_type */

	u32 CRC_Polynom;		/*!< Specifies CRC polynom
								This parameter can be set by user according to CRC order */

	u32 CRC_InitVal; 		/*!< Specifies CRC intial value
								This parameter can be set by user according to CRC order */

	u32 CRC_Xor; 			/*!< Specifies CRC xor output value
								This parameter can be set by user according to CRC order */

	u32 CRC_Iswap;		/*!< Specifies CRC input swap
								This parameter must be set to a value in the 0~7 range */

	u32 CRC_Oswap;		/*!< Specifies CRC output swap
								This parameter must be set to a value 0(not swap) or 1(swap) */

	u32 CRC_DmaMode;	/*!< Specifies CRC mode
								This parameter can be a value of @ref CRC_dma_mode */
} CRC_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup CRC_Exported_Constants CRC Exported Constants
  * @{
  */
  
/** @defgroup CRC_type CRC Type
  * @{
  */
#define CRC_TYPE_32					((u32)0x00000000)
#define CRC_TYPE_24					((u32)0x00000001)
#define CRC_TYPE_16					((u32)0x00000002)
#define CRC_TYPE_12					((u32)0x00000003)
#define CRC_TYPE_10					((u32)0x00000004)
#define CRC_TYPE_8					((u32)0x00000005)
#define CRC_TYPE_7					((u32)0x00000006)
#define CRC_TYPE_5					((u32)0x00000007)

#define IS_CRC_TYPE(TYPE) (((TYPE) == CRC_TYPE_32) || \
							((TYPE) == CRC_TYPE_24) || \
							((TYPE) == CRC_TYPE_16) || \
							((TYPE) == CRC_TYPE_12) || \
							((TYPE) == CRC_TYPE_10) || \
							((TYPE) == CRC_TYPE_8) || \
							((TYPE) == CRC_TYPE_7) || \
							((TYPE) == CRC_TYPE_5))

/**
  * @}
  */

/** @defgroup CRC_dma_mode CRC dma mode
  * @{
  */
#define CRC_CMD_MODE					((u32)0x00000000)
#define CRC_DMA_MODE					((u32)0x00000001)

#define IS_CRC_MODE(MODE) (((MODE) == CRC_CMD_MODE) || \
							((MODE) == CRC_DMA_MODE))

/**
  * @}
  */
  
/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/** @defgroup CRC_Exported_Functions CRC Exported Functions
  * @{
  */
_LONG_CALL_ void CRC_StructInit(CRC_InitTypeDef* CRC_InitStruct);
_LONG_CALL_ int CRC_Init(CRC_InitTypeDef* CRC_InitStruct);
_LONG_CALL_ int CRC_Calculate(CRC_InitTypeDef* CRC_InitStruct, IN const u8* message, IN const u32 msglen, OUT u32* pCrc);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CRC_Register_Definitions CRC Register Definitions
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup CRC_RST
 * @{
 *****************************************************************************/
#define CRC_RESET								((u32)0x00000001)		/*BIT[0], CRC reset*/
/** @} */

/**************************************************************************//**
 * @defgroup CRC_OP
 * @{
 *****************************************************************************/
#define CRC_ISWAP							((u32)0x00000007)		/* Bit[2:0], swap input data swap	*/
#define CRC_OSWAP							((u32)0x00000001<<3)	/* Bit[3], swap output data swap	*/
#define CRC_SEL								((u32)0x00000007<<4)	/* Bit[6:4], crc sel				*/
#define CRC_DMA								((u32)0x00000001<<7)	/* Bit[7], DMA mode 			*/
#define CRC_BE								((u32)0x00000003<<8)	/* Bit[9:8], byte type			*/
#define CRC_LAST								((u32)0x00000001<<12)	/* Bit[12], crc last				*/
#define CRC_LENGTH							((u32)0xFFFF0000)		/* Bit[31:16],  length			*/
/** @} */

/**************************************************************************//**
 * @defgroup CRC_STAT
 * @{
 *****************************************************************************/
#define CRC_BUSY								((u32)0x00000001)		/* Bit[0], still busy in previous execution	*/
#define CRC_OK								((u32)0x00000001<<1)	/* Bit[1], finish execution				*/
#define CRC_INTR_MASK						((u32)0x00000001<<2)	/* Bit[2], interrupt mask				*/
#define CRC_LITTLE_ENDIAN					((u32)0x00000001<<3)	/* Bit[3], little endian				*/
#define CRC_STAT_MASK						((u32)0x0000000E)		/* Bit[3:1], status mask				*/
/** @} */

/**************************************************************************//**
 * @defgroup CRC_CNT
 * @{
 *****************************************************************************/
#define CRC_COUNT								((u32)0x0000FFFF)		/*BIT[15:0], count number in bytes*/
/** @} */

/** @} */

/**
  * @}
  */

/**
  * @}
  */ 

/* Other Definitions --------------------------------------------------------*/
#define CRC_TIMEOUT  			10000

#endif
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/


