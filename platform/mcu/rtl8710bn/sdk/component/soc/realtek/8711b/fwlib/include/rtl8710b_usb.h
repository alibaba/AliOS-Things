/**
  ******************************************************************************
  * @file    rtl8710b_usb.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the USOC firmware
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

#ifndef _RTL8710B_USOC_H_
#define _RTL8710B_USOC_H_

/** @addtogroup AmebaZ_Periph_Driver
  * @{
  */

/** @defgroup USOC 
  * @brief USOC driver modules
  * @{
  */

/** @addtogroup USOC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * USOC:
  * 	- Base Address: USOC_REG
  * 	- Clock domains: USOC domain use 62.5M divided by system clock. SIE domain uses 30M UTMI clock
  * 	- DMA: use descriptor ring, default ring size is 8
  * 	- IRQ: USB_IRQ
  * 	- Interrupt mitigation
  * 	- Error handling
  *
  *****************************************************************************************
  * How to use USOC in iNIC mode
  *****************************************************************************************
  * 	 To use the USOC in iNIC mode, the following steps are mandatory:
  *
  *      1. USOC power on using USOC_POWER_On().
  *
  *      2. Init TXBD and RXBD
  *
  *      3. Disable USOC using USOC_Cmd(USOC_REG, DISABLE).
  *
  *      4. Config USOC work mode using USOC_MODE_Cfg(USOC_REG, USB_INIC_MODE).
  *
  *      5. Call USOC_Init(USOC_REG, &USOCInit_Struct) to configure USOC register with the
  * 			corresponding configuration.
  *
  *      6. To configure interrupts using USOC_INTCfg() function.
  *
  *      7. Activate the USOC peripheral using USOC_Cmd(USOC_REG, ENABLE) function
  *
  *****************************************************************************************
  * How to use USOC in dongle mode
  *****************************************************************************************
  * 	 To use USOC dongle mode, the following steps are mandatory:
  *
  *      1.  Enable WL clock and Lx Bus.
  *
  *      2. USOC power on using USOC_POWER_On().
  *
  *      3. Disable USOC using USOC_Cmd(USOC_REG, DISABLE).
  *
  *      4. Open register(WLON) access channel using USOC_CH_Cmd(USOC_REG, USOC_CH2, ENABLE).
  *
  *      5. Config USOC work mode using USOC_MODE_Cfg(USOC_REG, USB_DONGLE_MODE).
  *
  *      @note  All other functions can be used separately to modify, if needed,
  *		   		a specific feature of the USOC
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup USOC_Exported_Types USOC Exported Types
  * @{
  */
/**
  * @brief	USOC Init structure definition
  */
typedef struct {
	/* TXBD */
	u32 TXBD_BAR;			/*!< Specifies TXBD base address */

	u32 TXBD_RING_SIZE; 	/*!< Specifies TXBD ring size
								This parameter must be set to a value in the 0-32 range. */

	u32 TX_BUFFER_SIZE; 	/*!< Specifies TX buffer size
								In iNIC mode, this parameter must be set to a value in the 0-1536 range. */

	u32 TX_BLK_SIZE;		/*!< Specifies AHB TX block size
								This parameter must be set to a value in the 0-15 range. */

	u32 TX_MIT_TIME;		/*!< Specifies TX time mitigation
								This parameter must be set to a value in the 0-65535 range. */

	u32 TX_MIT_PKT_CNT; 	/*!< Specifies TX packet count mitigation
								This parameter must be set to a value in the 0-63 range. */

	/* RXBD */
	u32 RXBD_BAR;			/*!< Specifies RXBD base address */

	u32 RXBD_RING_SIZE; 	/*!< Specifies RXBD ring size
								This parameter must be set to a value in the 0-32 range. */

	u32 RX_BUFFER_SIZE; 	/*!< Specifies RX buffer size
								In iNIC mode, this parameter must be set to a value in the 0-1536 range. */

	u32 RX_BLK_SIZE;		/*!< Specifies AHB RX block size
								This parameter must be set to a value in the 0-15 range. */

	u32 RX_MIT_TIME;		/*!< Specifies RX time mitigation
								This parameter must be set to a value in the 0-65535 range. */

	u32 RX_MIT_PKT_CNT; 	/*!< Specifies RX packet count mitigation
								This parameter must be set to a value in the 0-63 range. */
} USOC_InitTypeDef;

/**
  * @brief	USOC TXBD structure definition
  */
typedef struct {
	u32 pktSize:14; 		/*!< bit[0:13], specifies tx packet size */

	u32 status:2;			/*!< bit[14:15], specifies the txbd status
								This parameter can be set to 00(init), or 01(ready), or 10(ok), or 11(err) */

	u32 seqNum:16;			/*!< bit[16:31], specifies txbd sequence number */

	u32 address;			/*!< Specifies the TX buffer physical address
								@note This parameter must be 64-bytes aligned */
}USOC_TX_BD, *PUSOC_TX_BD;

/**
  * @brief	USOC RXBD structure definition
  */
typedef struct {
	u32 pktSize:14; 		/*!< bit[0:13], specifies rx packet size */

	u32 status:2;			/*!< bit[14:15], specifies the rxbd status
								This parameter can be set to 00(init), or 01(ready), or 10(ok), or 11(err) */

	u32 seqNum:16;			/*!< bit[16:31], specifies rxbd sequence number */

	u32 address;			/*!< Specifies the RX buffer physical address
								@note This parameter must be 64-bytes aligned */
} USOC_RX_BD, *PUSOC_RX_BD;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup USOC_Exported_Constants USOC Exported Constants
  * @{
  */

/** @defgroup USOC_TXBD_definitions The TX Buffer Descriptor format
  * @{
  */ 
#define TXBD_STAT_INIT				0
#define TXBD_STAT_RDY				1
#define TXBD_STAT_OK				2
#define TXBD_STAT_ERR				3

#define USOC_TXBD_SIZE				8	
/**
  * @}
  */

/** @defgroup USOC_RXBD_definitions The RX Buffer Descriptor format
  * @{
  */ 
#define RXBD_STAT_INIT				0
#define RXBD_STAT_RDY				1
#define RXBD_STAT_OK				2
#define RXBD_STAT_ERR				3

#define USOC_RXBD_SIZE				8
/**
  * @}
  */

/** @defgroup USOC_Mode_definitions
  * @{
  */
#define USB_INIC_MODE				0 /*!<  INIC */
#define USB_DONGLE_MODE			1 	  /*!<  WIFI dongle mode */
#define USB_AINST_MODE				2 /*!<  AUTO install */
/**
  * @}
  */

/** @defgroup USOC_Chan_definitions
  * @{
  */
#define USOC_CH0					0 /*!<  iTDE */
#define USOC_CH1					1 /*!<  iRDE */
#define USOC_CH2					2 /*!<  iREG */
/**
  * @}
  */

/** @defgroup USOC_TX_Ring_definitions
  * @{
  */
#define USOC_TXBD_RING_SIZE		8
#define USOC_TX_BUFFER_SIZE		1536
#define USOC_TX_BLK_SIZE			6
#define USOC_TX_BUFFER_SIZE_ALIGN	(USOC_TX_BUFFER_SIZE+(1 << USOC_TX_BLK_SIZE))
/**
  * @}
  */

/** @defgroup USOC_RX_Ring_definitions
  * @{
  */
#define USOC_RXBD_RING_SIZE		8
#define USOC_RX_BUFFER_SIZE		1536
#define USOC_RX_BLK_SIZE			6
#define USOC_RX_BUFFER_SIZE_ALIGN	(USOC_RX_BUFFER_SIZE+(1 << USOC_RX_BLK_SIZE))
/**
  * @}
  */

/** @defgroup USOC_INTR_MIT_definitions
  * @{
  */
#define USOC_INTR_TX_MIT_TIME		625
#define USOC_INTR_RX_MIT_TIME		625
#define USOC_INTR_TX_MIT_PKT_CNT	4
#define USOC_INTR_RX_MIT_PKT_CNT	4
/**
  * @}
  */

/** @defgroup USOC_STUCK_TIME_definitions
  * @{
  */
#define USOC_TX_STUCK_TIME		4096/*!< Specifies TX DMA stuck time, must be set to a value in the 0-65535 range. */
#define USOC_RX_STUCK_TIME		4096/*!< Specifies RX DMA stuck time, must be set to a value in the 0-65535 range. */
/**
  * @}
  */

/**
  * @}
  */ 

/** @defgroup USOC_Exported_Functions USOC Exported Functions
  * @{
  */
_LONG_CALL_ VOID USOC_Cmd(USOC_REG_TypeDef * usoc_reg, u32 NewStatus);
_LONG_CALL_ VOID USOC_PHY_Cmd(USOC_REG_TypeDef * usoc_reg, u32 NewStatus);
_LONG_CALL_ VOID USOC_MODE_Cfg(USOC_REG_TypeDef * usoc_reg, u32 mode);
_LONG_CALL_ VOID USOC_TXBD_SWIDX_Cfg(USOC_REG_TypeDef * usoc_reg, u32 index);
_LONG_CALL_ u32 USOC_TXBD_SWIDX_Get(USOC_REG_TypeDef * usoc_reg);
_LONG_CALL_ VOID USOC_RXBD_SWIDX_Cfg(USOC_REG_TypeDef * usoc_reg, u32 index);
_LONG_CALL_ u32 USOC_RXBD_SWIDX_Get(USOC_REG_TypeDef * usoc_reg);
_LONG_CALL_ u32 USOC_TXBD_HWIDX_Get(USOC_REG_TypeDef * usoc_reg);
_LONG_CALL_ u32 USOC_RXBD_HWIDX_Get(USOC_REG_TypeDef * usoc_reg);
_LONG_CALL_ VOID USOC_INTCfg(USOC_REG_TypeDef * usoc_reg, u32 mask);
_LONG_CALL_ VOID USOC_INTClr(USOC_REG_TypeDef * usoc_reg, u32 mask);
_LONG_CALL_ u32 USOC_INTGet(USOC_REG_TypeDef * usoc_reg);
_LONG_CALL_ VOID USOC_StructInit(USOC_InitTypeDef* USOCInit_Struct);
_LONG_CALL_ VOID USOC_Init(USOC_REG_TypeDef * usoc_reg, USOC_InitTypeDef* USOCInit_Struct);
_LONG_CALL_ VOID USOC_POWER_On(VOID);
_LONG_CALL_ VOID USOC_CH_Cmd(USOC_REG_TypeDef * usoc_reg, u32 ch, u32 NewStatus);
_LONG_CALL_ VOID USOC_SW_RST(USOC_REG_TypeDef * usoc_reg);
_LONG_CALL_ VOID USOC_TXSTUCK_Cfg(USOC_REG_TypeDef * usoc_reg, u32 NewStatus, u32 TVal);
_LONG_CALL_ VOID USOC_RXSTUCK_Cfg(USOC_REG_TypeDef * usoc_reg, u32 NewStatus, u32 RVal);

/**
  * @}
  */
  
/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup USOC_Register_Definitions USOC Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup REG_USOC_CR
 * @{
 *****************************************************************************/
#define BIT_CR_INIC_MODE_EN				((u32)0x00000001 << 0)	/*!< enable inic mode(usoc) */
#define BIT_CR_AUTO_LOAD_DONE			((u32)0x00000001 << 1)	/*!< indicate SIE autoload done.  HW autoclear. */
#define BIT_CR_USB_RF_EN				((u32)0x00000001 << 2)	/*!< CM4 read SIE EN */
#define BIT_CR_USB_PHY_EN				((u32)0x00000001 << 3)	/*!< enable USB PHY */
#define BIT_CR_SIE_RERUN				((u32)0x00000001 << 4)	/*!< Rerun USB SIE. HW autoclear. */
#define BIT_CR_WLAN_USB_EN				((u32)0x00000001 << 5)	/*!< enable SIE dongle mode */
#define BIT_CR_INIC_USB_EN				((u32)0x00000001 << 6)	/*!< enable SIE iNIC mode */
#define BIT_CR_AINST_USB_EN				((u32)0x00000001 << 7)	/*!< enable SIE autoinstall mode */
#define BIT_CR_USB1D1_EN				((u32)0x00000001 << 8)	/*!< enable SIE USB 1.1 */
#define BIT_CR_RST_SIE_TX				((u32)0x00000001 << 9)	/*!< SIE TX software reset. Active low. */
#define BIT_CR_RST_SIE_RX				((u32)0x00000001 << 10)	/*!< SIE RX software reset. Active low. */
#define BIT_CR_SIE_LOAD_FAIL			((u32)0x00000001 << 11)	/*!< SIE autoload fail */
/** @} */

/**************************************************************************//**
 * @defgroup CLK_RST_CTRL
 * @{
 *****************************************************************************/
#define BIT_RST_ITDE					((u32)0x00000001 << 24)
#define BIT_RST_IRDE					((u32)0x00000001 << 25)
#define BIT_RST_TX_HW_IDX				((u32)0x00000001 << 26)
#define BIT_RST_RX_HW_IDX				((u32)0x00000001 << 27)
#define BIT_RST_TX_IF					((u32)0x00000001 << 28)
#define BIT_RST_RX_IF					((u32)0x00000001 << 29)
#define BIT_RST_AHB_AUX					((u32)0x00000001 << 30)
#define BIT_RST_UTMI_AUX				((u32)0x00000001 << 31)

#define USOC_SW_RST_MASK				(BIT_RST_ITDE | BIT_RST_IRDE | \
										BIT_RST_TX_HW_IDX | BIT_RST_RX_HW_IDX | \
										BIT_RST_TX_IF | BIT_RST_RX_IF | \
										BIT_RST_UTMI_AUX)
/** @} */

/**************************************************************************//**
 * @defgroup CHANN_CTRL
 * @{
 *****************************************************************************/													
#define BIT_PRI_CH0_CFG					((u32)0x00000007 << 0)
#define BIT_PRI_CH1_CFG					((u32)0x00000007 << 4)
#define BIT_PRI_CH2_CFG					((u32)0x00000007 << 8)

#define BIT_EN_CH0						((u32)0x00000001 << 3)	/*!< enable iTDE */
#define BIT_EN_CH1						((u32)0x00000001 << 7)	/*!< enable iRDE */
#define BIT_EN_CH2						((u32)0x00000001 << 11)	/*!< enable iREG */

#define BIT_TX_BLK_SZ_CFG(x)			((u32)((x) & 0x0000000f) << 16)
#define BIT_RX_BLK_SZ_CFG(x)			((u32)((x) & 0x0000000f) << 20)
/** @} */

/**************************************************************************//**
 * @defgroup BUFFER_SIZE
 * @{
 *****************************************************************************/
#define BIT_TXBUF_SIZE_CFG(x)			((u32)((x) & 0x00000fff) << 0)
#define BIT_RXBUF_SIZE_CFG(x)			((u32)((x) & 0x00000fff) << 16)
/** @} */

/**************************************************************************//**
 * @defgroup RING_SIZE
 * @{
 *****************************************************************************/
#define BIT_TXBD_RING_SZ_CFG(x)			((u32)((x) & 0x000000ff) << 0)
#define BIT_RXBD_RING_SZ_CFG(x)			((u32)((x) & 0x000000ff) << 8)

#define BIT_MASK_TXBD_SW_IDX			((u32)0x000000ff)
#define BIT_MASK_RXBD_SW_IDX			((u32)0x000000ff)
#define BIT_MASK_TXBD_HW_IDX			((u32)0x000000ff)
#define BIT_MASK_RXBD_HW_IDX			((u32)0x000000ff)

/** @} */

/**************************************************************************//**
 * @defgroup REG_USOC_INTR_MASK_or_REG_USOC_INTR_STAT
 * @{
 *****************************************************************************/
#define BIT_INTR_TX_PKT_OK				((u32)0x00000001 << 0)
#define BIT_INTR_RX_PKT_OK				((u32)0x00000001 << 1)
#define BIT_INTR_SIE_TX_FAIL			((u32)0x00000001 << 2)
#define BIT_INTR_SIE_RX_FAIL			((u32)0x00000001 << 3)
#define BIT_INTR_TX_TIMEOUT				((u32)0x00000001 << 4)
#define BIT_INTR_RX_TIMEOUT				((u32)0x00000001 << 5)
#define BIT_INTR_TX_BUS_ERR				((u32)0x00000001 << 6)
#define BIT_INTR_RX_BUS_ERR				((u32)0x00000001 << 7)
#define BIT_INTR_BAD_TX_BD				((u32)0x00000001 << 8)
#define BIT_INTR_BAD_RX_BD				((u32)0x00000001 << 9)
#define BIT_INTR_NO_TX_BD				((u32)0x00000001 << 10)
#define BIT_INTR_NO_RX_BD				((u32)0x00000001 << 11)

#define BIT_INTR_TX_AB_WERR				((u32)0x00000001 << 16)
#define BIT_INTR_TX_AB_RERR				((u32)0x00000001 << 17)
#define BIT_INTR_TX_MB_WERR				((u32)0x00000001 << 18)
#define BIT_INTR_TX_MB_RERR				((u32)0x00000001 << 19)
#define BIT_INTR_TX_DB_WERR				((u32)0x00000001 << 20)
#define BIT_INTR_TX_DB_RERR				((u32)0x00000001 << 21)

#define BIT_INTR_RX_AB_WERR				((u32)0x00000001 << 24)
#define BIT_INTR_RX_AB_RERR				((u32)0x00000001 << 25)
#define BIT_INTR_RX_MB_WERR				((u32)0x00000001 << 26)
#define BIT_INTR_RX_MB_RERR				((u32)0x00000001 << 27)
#define BIT_INTR_RX_SB_WERR				((u32)0x00000001 << 28)
#define BIT_INTR_RX_SB_RERR				((u32)0x00000001 << 29)

#define USOC_INIT_INTR_MASK			(BIT_INTR_TX_PKT_OK | BIT_INTR_RX_PKT_OK | \
										BIT_INTR_SIE_TX_FAIL | BIT_INTR_SIE_RX_FAIL | \
										BIT_INTR_TX_TIMEOUT | BIT_INTR_RX_TIMEOUT| \
										BIT_INTR_TX_BUS_ERR | BIT_INTR_RX_BUS_ERR | \
										BIT_INTR_NO_TX_BD | BIT_INTR_NO_RX_BD | \
										BIT_INTR_BAD_TX_BD |BIT_INTR_BAD_RX_BD)
/** @} */

/**************************************************************************//**
 * @defgroup INTR_TX_MITIGATION
 * @{
 *****************************************************************************/
#define BIT_TX_MIT_TIME_CFG(x)			((u32)((x) & 0x0000ffff) << 0)
#define BIT_TX_MIT_PKT_CNT_CFG(x)		((u32)((x) & 0x0000003f) << 16)
#define BIT_TX_MIT_EN					((u32)0x00000001 << 31)
/** @} */

/**************************************************************************//**
 * @defgroup INTR_RX_MITIGATION
 * @{
 *****************************************************************************/
#define BIT_RX_MIT_TIME_CFG(x)			((u32)((x) & 0x0000ffff) << 0)
#define BIT_RX_MIT_PKT_CNT_CFG(x)		((u32)((x) & 0x0000003f) << 16)
#define BIT_RX_MIT_EN					((u32)0x00000001 << 31)
/** @} */

/**************************************************************************//**
 * @defgroup REG_TX_STUCK_TIMER
 * @{
 *****************************************************************************/
#define BIT_TX_STUCK_TIME_CFG(x)			((u32)((x) & 0x0000ffff) << 0)
#define BIT_TX_STUCK_TIMER_EN				((u32)0x00000001 << 16)
/** @} */

/**************************************************************************//**
 * @defgroup REG_RX_STUCK_TIMER
 * @{
 *****************************************************************************/
#define BIT_RX_STUCK_TIME_CFG(x)			((u32)((x) & 0x0000ffff) << 0)
#define BIT_RX_STUCK_TIMER_EN				((u32)0x00000001 << 16)
/** @} */

/**************************************************************************//**
 * @defgroup REG_OTG_PWCSEQ_PWC
 * @{
 *****************************************************************************/
#define BIT_PWC_USBD_EN					((u32)0x00000001 << 0)
#define BIT_PWC_UPLV_EN					((u32)0x00000001 << 1)
#define BIT_PWC_UPHV_EN					((u32)0x00000001 << 2)
/** @} */

/**************************************************************************//**
 * @defgroup REG_OTG_PWCSEQ_ISO
 * @{
 *****************************************************************************/
#define BIT_ISO_USBD_EN					((u32)0x00000001 << 0)
#define BIT_ISO_USBA_EN					((u32)0x00000001 << 1)
/** @} */

/**************************************************************************//**
 * @defgroup REG_OTG_PWCSEQ_OTG
 * @{
 *****************************************************************************/
#define BIT_UPLL_CKRDY					((u32)0x00000001 << 5)
#define BIT_USBOTG_EN					((u32)0x00000001 << 8)
#define BIT_USBPHY_EN					((u32)0x00000001 << 9)
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */ 

#endif //_RTL8710B_USOC_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
