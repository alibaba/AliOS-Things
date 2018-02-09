/**
  ******************************************************************************
  * @file    rtl8711b_rtc.h
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

#ifndef _RTL8710B_DONGLE_H_
#define _RTL8710B_DONGLE_H_

/** @addtogroup AmebaZ_Periph_Driver
  * @{
  */

/** @defgroup DONGLE 
  * @brief DONGLE driver modules
  * @{
  */

/** @addtogroup DONGLE
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * functions:
  * 	- init usb to dongle mode
  * 	- usb dongle read/write efuse
  * 	- usb dongle read/write flash
  *
  *****************************************************************************************
  * How to use DONGLE
  *****************************************************************************************
  * 	  ROM boot use it, user don't use if no needed.
  *
  *****************************************************************************************
  * Host indrect access guide
  *****************************************************************************************
  *
  * read flash
  * 1) write 0x80F8: 0x00xxxxxx =>xxxxxx is flash address
  * 2) write 0x806C: 0x80000000
  * 3) poll bit31
  * 4) read 0x809C: 0xyyyyyyyy => yyyyyyyy is flash data
  *
  * write flash
  * 1) write 0x80F8: 0x00xxxxxx =>xxxxxx is flash address
  * 2) write 0x809C: 0xyyyyyyyy =>yyyyyyyy is flash data
  * 3) write 0x806C: 0x81000000
  * 4) poll bit31
  *
  * flash erase
  * 1) write 0x80F8: 0x00xxxxxx =>xxxxxx is flash address
  * 2) write 0x806C: 0x82000000
  * 3) poll bit31
  *
  *
  * read any register
  * 1) write 0x80F8: 0xxxxxxxxx =>xxxxxx is register complete address
  * 2) write 0x806C: 0x83000000
  * 3) poll bit31
  * 4) read 0x809C: 0xyyyyyyyy => yyyyyyyy is register data
  *
  * write any register
  * 1) write 0x80F8: 0xxxxxxxxx =>xxxxxx is register complete address
  * 2) write 0x809C: 0xyyyyyyyy =>yyyyyyyy is register data
  * 3) write 0x806C: 0x84000000
  * 4) poll bit31
  *
  * efuse read
  * 1) write 0x80F8: 0x000000xx =>xx is efuse physical address
  * 2) write 0x806C: 0x85000000
  * 3) poll bit31
  * 4) read 0x809C: 0x000000yy => yy is efuse physical data
  *
  * efuse write
  * 1) write 0x80F8: 0x000000xx =>xx is efuse physical address
  * 2) write 0x809C: 0x000000yy => yy is efuse physical data
  * 3) write 0x806C: 0x86000000
  * 4) poll bit31
  *
  *****************************************************************************************
  
  * @endverbatim
  */


/* Exported types ------------------------------------------------------------*/

/** @defgroup DONGLE_Exported_Types DONGLE Exported Types
  * @{
  */

/** 
  * @brief  DONGLE Init structure definition  
  */
typedef struct {
	void (*usb_dongle_interrupt)(void);   /*!< USOC_DongleIsr_RAM function in RAM */
} DONGLE_InitTypeDef;

/** 
  * @brief  DONGLE ROM_Patch function definition  
  */
typedef void (*DONGLE_PATCH_FUN)(void);
/**
  * @}
  */

/** @defgroup DONGLE_Exported_Constants DONGLE Exported Constants
  * @{
  */ 

/* Exported constants ------------------------------------------------------------*/

/** @defgroup DONGLE_HOST_SpecialCmd_definitions 
  * @{
  */
#define USB_HOST_SPECIALCMD_FLASH_READ		0
#define USB_HOST_SPECIALCMD_FLASH_WRITE		1
#define USB_HOST_SPECIALCMD_FLASH_ERASE		2
#define USB_HOST_SPECIALCMD_REG_READ			3
#define USB_HOST_SPECIALCMD_REG_WRITE		4
#define USB_HOST_SPECIALCMD_EFUSE_READ		5
#define USB_HOST_SPECIALCMD_EFUSE_WRITE		6
/**
  * @}
  */

/**
  * @}
  */


/** @defgroup DONGLE_Exported_Functions DONGLE Exported Functions
  * @{
  */
_LONG_CALL_ void USOC_DongleSpecialCmd(u8 cmd_val);
_LONG_CALL_ void USOC_DongleCmd(void);
_LONG_CALL_ void USOC_DongleIsr(void *DATA);
_LONG_CALL_ u32 USOC_PHY_Write(u8 addr, u8 data);
_LONG_CALL_ u32 USOC_PHY_Read(u8 addr, u8 *data);
_LONG_CALL_ void USOC_PHY_Autoload(void);
_LONG_CALL_ void USOC_DongleInit(DONGLE_PATCH_FUN patch_func);
_LONG_CALL_ void USOC_SIE_INTConfig(u32 SIE_INT, u32 NewState);
_LONG_CALL_ void USOC_SIE_INTClear(void);
void USOC_DongleIsr_RAM(void);
VOID USOC_DongleReOpen_RAM(void);
VOID USOC_DongleInitPatch_RAM(void);
VOID USOC_Dongle_InitThread(void *param);
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
/********************  REG definition for SIE register  *******************/
#define USB_SIE_REG_40_ADDR		0x40 /*!< VCONTROL: uphy_reg_addr, Haddr[7:4] and Laddr[3:0], 4-bit wide respectively. The Haddr value is restricted in 8~F while LADDR in 0~7 */
#define USB_SIE_REG_41_DATAIN		0x41 /*!< VSTATUS_IN[7:0]: uphy_reg_data_input  */
#define USB_SIE_REG_42_CMD			0x42 /*!< VLOADM: 42[0]=0: VSTATUS serves as input for PHY. Otherwise, VSTATUS serves as output that passes the selected internal signals out.
												42[7]: uphy_reg_en, enable to load uphy_reg_addr. This bit will be cleared at the end of accessing usbphy register */
#define USB_SIE_REG_43_DATAOUT	0x43 /*!< VSTATUS_OUT[7:0]: uphy_reg_data_output */

#define USB_SIE_REG_64		0x64

#define USB_SIE_REG_E0		0xE0
#define USB_SIE_REG_E1		0xE1
#define USB_SIE_REG_E2		0xE2
#define USB_SIE_REG_E3		0xE3
#define USB_SIE_REG_E4		0xE4
#define USB_SIE_REG_E5		0xE5
#define USB_SIE_REG_E6		0xE6
#define USB_SIE_REG_E7		0xE7
#define USB_SIE_REG_E8		0xE8
#define USB_SIE_REG_E9		0xE9
#define USB_SIE_REG_EA		0xEA
#define USB_SIE_REG_EB		0xEB
#define USB_SIE_REG_EC		0xEC
#define USB_SIE_REG_ED		0xED
#define USB_SIE_REG_EE		0xEE
#define USB_SIE_REG_EF		0xEF
#define USB_SIE_REG_F0		0xF0
#define USB_SIE_REG_F1		0xF1
#define USB_SIE_REG_F2		0xF2
#define USB_SIE_REG_F3		0xF3
#define USB_SIE_REG_F4		0xF4
#define USB_SIE_REG_F5		0xF5
#define USB_SIE_REG_F6		0xF6
#define USB_SIE_REG_F7		0xF7

/* Other Definitions --------------------------------------------------------*/
#define USB_PHY_INDIRECT_ACCESS_POLL_TIMES	0xFFF

#ifdef CONFIG_CHIP_A_CUT
#define USOC_PHY_AUTOLOAD		USOC_PHY_Autoload_ACUT
#else
#define USOC_PHY_AUTOLOAD		USOC_PHY_Autoload
#endif

#endif //_RTL8710B_DONGLE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
