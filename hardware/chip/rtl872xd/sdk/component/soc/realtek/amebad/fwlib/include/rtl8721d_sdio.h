/**
  ******************************************************************************
  * @file    rtl8721d_sdio.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the SDIO firmware
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

#ifndef _RTL8710B_SDIO_H_
#define _RTL8710B_SDIO_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup SDIO 
  * @brief SDIO driver modules
  * @{
  */

/* Exported Types --------------------------------------------------------*/
/** @defgroup SDIO_Exported_Types SDIO Exported Types
  * @{
  */
/**
  * @brief	 SDIO Init structure definition
  */
typedef struct {
	/* TXBD */
	u32 TXBD_BAR;			/*!< Specifies TXBD base address */
	u32 TXBD_RING_SIZE; 	/*!< Specifies TXBD ring size, This parameter must be set to a value in the 0-0xFFFF range. */
	u32 TX_BUFFER_SIZE; 	/*!< Specifies TX buffer size, This parameter must be set to a value in the 0-0xFF range. */

	/* RXBD */
	u32 RXBD_BAR;			/*!< Specifies RXBD base address */
	u32 RXBD_RING_SIZE; 	/*!< Specifies RXBD ring size, This parameter must be set to a value in the 0-0xFFFF range. */
	u32 RXBD_FREE_TH;		/*!< the threshold of free RX BD count to trigger interrupt */
} SDIO_InitTypeDef;
/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/

/** @defgroup SDIO_Exported_Constants SDIO Exported Constants
  * @{
  */

/** @defgroup SDIO_MP_CMD_definitions The SDIO MP CMD definations
  * @{
  */ 
#define SDIO_MP_START			1
#define SDIO_MP_STOP			2
#define SDIO_MP_LOOPBACK		3
#define SDIO_MP_STATUS			4
#define SDIO_MP_READ_REG8		5
#define SDIO_MP_READ_REG16	6
#define SDIO_MP_READ_REG32	7
#define SDIO_MP_WRITE_REG8	8
#define SDIO_MP_WRITE_REG16	9
#define SDIO_MP_WRITE_REG32	10
#define SDIO_MP_WAKEUP		11	// wakeup the SDIO task manually, for debugging
#define SDIO_MP_DUMP			12    // start/stop to dump the SDIO status periodically
#define SDIO_MP_CTX				13     // setup continue TX test
#define SDIO_MP_CRX				14     // setup continue RX test
#define SDIO_MP_CRX_DA			15    // setup continue RX with dynamic allocate RX Buf test
#define SDIO_MP_CRX_STOP		16    // setup continue RX test
#define SDIO_MP_DBG_MSG		17    // Debug message On/Off
/**
  * @}
  */


/** @defgroup SDIO_RPWM_definitions The SDIO RPWM definations
  * @{
  */ 
#define RPWM2_ACT_BIT			(0x00000001 << 0)     // Active
#define RPWM2_SLEEP_BIT		0        // Sleep
#define RPWM2_DSTANDBY_BIT	(0x00000001 << 1)     // Deep Standby
#define RPWM2_PG_BIT			0        // Power Gated
#define RPWM2_FBOOT_BIT		(0x00000001 << 2)     // fast reboot
#define RPWM2_NBOOT_BIT		0        // normal reboot
#define RPWM2_WKPIN_0_BIT		(0x00000001 << 3)     // enable GPIO wakeup pin 0
#define RPWM2_WKPIN_1_BIT		(0x00000001 << 4)     // enable GPIO wakeup pin 1
#define RPWM2_WKPIN_2_BIT		(0x00000001 << 5)     // enable GPIO wakeup pin 2
#define RPWM2_WKPIN_3_BIT		(0x00000001 << 6)     // enable GPIO wakeup pin 3
#define RPWM2_WKPIN_0_LV_BIT	(0x00000001 << 7)     // GPIO wakeup pin 0 wakeup level
#define RPWM2_WKPIN_1_LV_BIT	(0x00000001 << 8)     // GPIO wakeup pin 1 wakeup level
#define RPWM2_WKPIN_2_LV_BIT	(0x00000001 << 9)     // GPIO wakeup pin 2 wakeup level
#define RPWM2_WKPIN_3_LV_BIT	(0x00000001 << 10)    // GPIO wakeup pin 3 wakeup level
#define RPWM2_CG_BIT			(0x00000001 << 11)    // Clock Gated
#define RPWM2_ACK_BIT			(0x00000001 << 14)    // Acknowledge
#define RPWM2_TOGGLE_BIT		(0x00000001 << 15)    // Toggle bit
/**
  * @}
  */

/** @defgroup SDIO_CPWM2_definitions The SDIO CPWM2 definations
  * @{
  */ 
#define CPWM2_ACT_BIT			(0x00000001 << 0)     // Active
#define CPWM2_DSTANDBY_BIT	(0x00000001 << 1)     // Deep Standby
#define CPWM2_FBOOT_BIT		(0x00000001 << 2)     // fast reboot
#define CPWM2_INIC_FW_RDY_BIT	(0x00000001 << 3)     // is the iNIC FW(1) or Boot FW(0)
#define CPWM2_TOGGLE_BIT		(0x00000001 << 15)    // Toggle bit
/**
  * @}
  */

/** @defgroup SDIO_CPWM1_definitions The SDIO CPWM1 definations
  * @{
  */ 
#define CPWM1_TOGGLE_BIT		(0x00000001 << 7)    // Toggle bit
/**
  * @}
  */

/** @defgroup SDIO_EVENT_definitions The SDIO EVENT definations
  * @{
  */ 
#define SDIO_EVENT_RX_PKT_RDY			(0x00000001 << 1)		// A new SDIO packet ready
#define SDIO_EVENT_DUMP                           (0x00000001 << 3)      // SDIO status dump periodically Enable
#define SDIO_EVENT_EXIT					(0x00000001 << 27)		// Request to exit the SDIO task
#define SDIO_EVENT_MP_STOPPED			(0x00000001 << 28)		// The SDIO task is stopped
#define SDIO_EVENT_IRQ_STOPPED		(0x00000001 << 29)		// The SDIO task is stopped
#define SDIO_EVENT_TX_STOPPED			(0x00000001 << 30)		// The SDIO task is stopped
#define SDIO_EVENT_RX_STOPPED			(0x00000001 << 31)		// The SDIO task is stopped
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SDIO_Exported_Functions SDIO Exported Functions
  * @{
  */
_LONG_CALL_ void SDIO_StructInit(SDIO_InitTypeDef* SDIO_InitStruct);
_LONG_CALL_ void SDIO_Init(SDIO_InitTypeDef* SDIOInit_Struct);
_LONG_CALL_ void SDIO_INTClear(void);
_LONG_CALL_ VOID SDIO_INTConfig(u16 IntMask, u32 NewState);
_LONG_CALL_ u8 SDIO_RPWM1_Get(void);
_LONG_CALL_ u16 SDIO_RPWM2_Get(void);
_LONG_CALL_ void SDIO_CPWM1_Set(u8 Val);
_LONG_CALL_ void SDIO_CPWM2_Set(u16 Val, u32 Newstate);
_LONG_CALL_ u16 SDIO_RXBD_RPTR_Get(void);
_LONG_CALL_ void SDIO_RXBD_WPTR_Set(u16 Val);
_LONG_CALL_ u16 SDIO_TXBD_WPTR_Get(void);
_LONG_CALL_ void SDIO_TXBD_RPTR_Set(u16 Val);
_LONG_CALL_ void SDIO_DMA_Reset(void);
/**
  * @}
  */

/**
  * @}
  */
/**
  * @}
  */

#define HAL_SDIO_READ32(addr)            			HAL_READ32(SDIO_DEVICE_REG_BASE, addr)
#define HAL_SDIO_WRITE32(addr, value)    			HAL_WRITE32(SDIO_DEVICE_REG_BASE, addr, value)
#define HAL_SDIO_READ16(addr)					HAL_READ16(SDIO_DEVICE_REG_BASE, addr)
#define HAL_SDIO_WRITE16(addr, value)    			HAL_WRITE16(SDIO_DEVICE_REG_BASE, addr, value)
#define HAL_SDIO_READ8(addr)					HAL_READ8(SDIO_DEVICE_REG_BASE, addr)
#define HAL_SDIO_WRITE8(addr, value)    			HAL_WRITE8(SDIO_DEVICE_REG_BASE, addr, value)

/* Registers Definitions --------------------------------------------------------*/
#define REG_SPDIO_TXBD_ADDR 						0xA0       // 4 Bytes
#define REG_SPDIO_TXBD_SIZE						0xA4       // 4 Bytes
#define REG_SPDIO_TXBD_WPTR						0xA8       // 2 Bytes
#define REG_SPDIO_TXBD_RPTR						0xAC       // 2 Bytes
#define REG_SPDIO_RXBD_ADDR						0xB0	// 4 Bytes
#define REG_SPDIO_RXBD_SIZE						0xB4	// 2 Bytes
#define REG_SPDIO_RXBD_C2H_WPTR					0xB6	// 2 Bytes
#define REG_SPDIO_RXBD_C2H_RPTR					0xB8	// 2 Bytes
#define REG_SPDIO_HCI_RX_REQ						0xBA	// 1 Byte
#define REG_SPDIO_CPU_RST_DMA						0xBB	// 1 Byte
#define REG_SPDIO_RX_REQ_ADDR						0xBC	// 2 Bytes
#define REG_SPDIO_CPU_INT_MASK					0xC0	// 2 Bytes
#define REG_SPDIO_CPU_INT_STAS					0xC2	// 2 Bytes
#define REG_SPDIO_CCPWM							0xC4	// 1 Byts
#define REG_SPDIO_CPU_IND							0xC5	// 1 Byte
#define REG_SPDIO_CCPWM2							0xC6	// 2 Bytes
#define REG_SPDIO_CPU_H2C_MSG						0xC8	// 4 Bytes
#define REG_SPDIO_CPU_C2H_MSG						0xCC	// 4 Bytes
#define REG_SPDIO_CRPWM							0xD0	// 1 Bytes
#define REG_SPDIO_CRPWM2							0xD2	// 2 Bytes
#define REG_SPDIO_AHB_DMA_CTRL					0xD4	// 4 Bytes
#define REG_SPDIO_RXBD_CNT					        	0xD8	// 4 Bytes
#define REG_SPDIO_TX_BUF_UNIT_SZ			        	0xD9	// 1 Bytes
#define REG_SPDIO_RX_BD_FREE_CNT			        	0xDA	// 2 Bytes
#define REG_SPDIO_CPU_H2C_MSG_EXT				0xDC	// 4 Bytes
#define REG_SPDIO_CPU_C2H_MSG_EXT				0xE0	// 4 Bytes

/********************  Bits definition for REG_SPDIO_CPU_RST_DMA register  *******************/
#define BIT_CPU_RST_SDIO_DMA						BIT(7)

/********************  Bits definition for REG_SPDIO_CPU_INT_MASK/REG_SPDIO_CPU_INT_STAS register  *******************/
#define BIT_TXFIFO_H2C_OVF							BIT(0)
#define BIT_H2C_BUS_RES_FAIL						BIT(1)
#define BIT_H2C_DMA_OK								BIT(2)
#define BIT_C2H_DMA_OK								BIT(3)
#define BIT_H2C_MSG_INT								BIT(4)
#define BIT_RPWM1_INT								BIT(5)
#define BIT_RPWM2_INT								BIT(6)
#define BIT_SDIO_RST_CMD_INT						BIT(7)
#define BIT_RXBD_FLAG_ERR_INT						BIT(8)
#define BIT_RX_BD_AVAI_INT							BIT(9)
#define BIT_HOST_WAKE_CPU_INT						BIT(10)

#define SDIO_INIT_INT_MASK							(BIT_H2C_DMA_OK | BIT_C2H_DMA_OK | \
													BIT_H2C_MSG_INT | BIT_RPWM1_INT | \
													BIT_RPWM2_INT |BIT_H2C_BUS_RES_FAIL | \
													BIT_RXBD_FLAG_ERR_INT)
/********************  Bits definition for REG_SPDIO_CPU_IND register  *******************/
#define BIT_SYSTEM_TRX_RDY_IND						BIT(0)

/********************  Bits definition for REG_SPDIO_HCI_RX_REQ register  *******************/
#define BIT_HCI_RX_REQ								BIT(0)

#endif	/* #ifndef _RTL8710B_SDIO_H_ */
