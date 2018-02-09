/**
  ******************************************************************************
  * @file    rtl8711b_efuse.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the EFUSE firmware
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

#ifndef _8710B_EFUSE_H_
#define _8710B_EFUSE_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup EFUSE 
  * @brief EFUSE driver modules
  * @{
  */

/** @addtogroup EFUSE
  * @verbatim
  *****************************************************************************************
  * logical map (512B)
  *****************************************************************************************
  *
  * 0x00~0x1F		32bytes system autoload
  * 0x20~0xCF	WIFI calibration data
  * 0xD0~0x11F	HCI CIS
  * 0x130~0x13F	SW/RF  Reserved
  * 0x160~0x17F	32bytes USER1
  * 0x180~0x19F	32bytes USER2
  * 0x1A0~0x1Bf	32bytes USER3
  * 0x1C0~0x1Df	USB HCI
  *
  *****************************************************************************************
  * physical map (256B)
  *****************************************************************************************
  *
  * 0x00~0x7E	127bytes for logical efuse, user can read
  * 0x80~0x9F		32bytes for user OTP, user can read
  * 0xA0~0xAF 	16bytes OTF KEY, can not read by user
  * 0xB0~0xBF 	16bytes RDP KEY, can not read by user
  * 0xC0 			1byte RDP EN, can not read by user
  * 0xC1~0xD2 	18bytes for Security section
  * 0xD3 			1byte JTAG ON/OFF
  * 0xD4~0xEF	29bytes RF rsvd, user can read
  * 0xF0~0xFF		16bytes RTK rsvd, user can read
  *
  *****************************************************************************************
  * USER Section (3 * 32B)
  *****************************************************************************************
  * can be changed after write
  *
  * USER1 32B = 4 sections * 8B
  * USER2 32B = 4 sections * 8B
  * USER3 32B = 4 sections * 8B
  *
  *****************************************************************************************
  * OTP Section (32B)
  *****************************************************************************************
  *
  * can not be changed after write
  *
  * OTP 32B
  *
  *****************************************************************************************
  * FW protection
  *****************************************************************************************
  *
  * can not be changed after write
  *
  * OTF KEY: 16B, can not read
  * RDP KEY: 16B, can not read
  * RDP EN: 1B, can not read
  * JTAG OFF: 1B
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup EFUSE_Exported_Constants EFUSE Exported Constants
  * @{
  */ 

/** @defgroup EFUSE_LOGICAL_definitions 
  * @{
  */ 
#define EFUSE_MAP_LEN_8711B			512 /*!< logical map len in byte */
#define EFUSE_MAX_SECTION_8711B		(EFUSE_MAP_LEN_8711B >> 3) /*!< logical map len in section */
#define PGPKT_DATA_SIZE				8 /*!< logical map section len */

/* logical EFUSE User area */
#define USER_SECTION					(0x160 >> 3)/*!< user area section index  */
/**
  * @}
  */

/** @defgroup EFUSE_PHYSICAL_definitions 
  * @{
  */ 
#define OTP_SECTION						0x80 /*!< EFUSE OTP area: physical address */
#define OTP_SECTION_LEN				0x20 /*!< EFUSE OTP area: 32 bytes */

/*  physical EFUSE len */
#define EFUSE_REAL_CONTENT_LEN_8711B	256
#define AVAILABLE_EFUSE_ADDR(addr)		(addr < EFUSE_REAL_CONTENT_LEN_8711B)

/* physical EFUSE read forbid */
#define READ_FORBID_SECTION_START		0xA0 /*!< EFUSE Security area: physical start */
#define READ_FORBID_SECTION_END		0xD4 /*!< EFUSE Security area: physical end */

/* physical EFUSE write forbid */
#define LOGICAL_MAP_SECTION_LEN		0x7E /*!< logical mapping efuse len in physical address */
#define EFUSE_OOB_PROTECT_BYTES		(EFUSE_REAL_CONTENT_LEN_8711B - LOGICAL_MAP_SECTION_LEN) // Security + RF + MAC + OTP
/**
  * @}
  */

/** @defgroup EFUSE_CHIPID_definitions 
  * @{
  */ 
//#define CHIPID_8710BN		0xFF /* PACKAGE_QFN32 */
//#define CHIPID_8710BU		0xFE /* PACKAGE_QFN48_MCM */
//#define CHIPID_8711BN		0xFD /* PACKAGE_QFN48 */
//#define CHIPID_8711BG		0xFC /* PACKAGE_QFN68 */
//#define CHIPID_8710BN_L0	0xFB /* PACKAGE_QFN32 L0 */
/**
  * @}
  */

/** @defgroup EFUSE_VOLTAGE_definitions 
  * @{
  */
#define L25EOUTVOLTAGE						7
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup EFUSE_Exported_Functions EFUSE Exported Functions
  * @{
  */
/** @defgroup EFUSE_Physical_Address_functions 
  * @{
  */
_LONG_CALL_ extern void EFUSE_PowerSwitchROM(u8 bWrite, u8 PwrState, u8 L25OutVoltage);
_LONG_CALL_ extern u32 EFUSE_OneByteReadROM(u32 CtrlSetting, u16 Addr, u8 *Data, u8 L25OutVoltage);
_LONG_CALL_ extern u32 EFUSE_OneByteWriteROM(u32 CtrlSetting, u16 Addr, u8 Data, u8 L25OutVoltage);
_LONG_CALL_ extern u8 EFUSE_LogicalMap_Read(u8 *pbuf);
_LONG_CALL_ extern u8 EFUSE_LogicalMap_Write(u32 addr, u32 cnts, u8 *data);
_LONG_CALL_ extern  u32 EFUSE_WRITE_Check(u32 NewStatus);
u32 EFUSE_OneByteRead_ACUT(u32 CtrlSetting, u16 Addr, u8 *Data, u8 L25OutVoltage);
u8 EFUSE_LogicalMap_Read_ACUT(u8 *pbuf);
u32 EFUSE_WRITE_Check_ACUT(u32 NewStatus);
u8 EFUSE_LogicalMap_Write_Check(u32 addr, u32 cnts, u8 *data);
#define EFUSEPowerSwitch	EFUSE_PowerSwitchROM
#define EFUSERead8			EFUSE_OneByteReadROM
#define EFUSEWrite8			EFUSE_OneByteWriteROM
/**
  * @}
  */

/** @defgroup EFUSE_USER_Area_functions 
  * @{
  */
_LONG_CALL_ void EFUSE_USER1_Read(u8 *pContant);
_LONG_CALL_ void EFUSE_USER2_Read(u8 *pContant);
_LONG_CALL_ void EFUSE_USER3_Read(u8 *pContant);
_LONG_CALL_ u8 EFUSE_USER1_Write(u8 SectionIdx, u8 WordEnable, u8 *pContant);
_LONG_CALL_ u8 EFUSE_USER2_Write(u8 SectionIdx, u8 WordEnable, u8 *pContant);
_LONG_CALL_ u8 EFUSE_USER3_Write(u8 SectionIdx, u8 WordEnable, u8 *pContant);
u8 EFUSE_USER1_Write_ACUT(u8 SectionIdx, u8 WordEnable, u8 *pContant);
/**
  * @}
  */

/** @defgroup EFUSE_OTP_Area_functions 
  * @{
  */
_LONG_CALL_ u32 EFUSE_OTP_Read1B(u32 CtrlSetting, u16 Addr, u8 *Data, u8 L25OutVoltage);
_LONG_CALL_ u32 EFUSE_OTP_Write1B(u32 CtrlSetting, u16 Addr, u8 Data, u8 L25OutVoltage);
_LONG_CALL_ void EFUSE_OTP_Read32B(u8 *pContant);
_LONG_CALL_ u32 EFUSE_OTP_Write32B(u8 *pContant);
u32 EFUSE_OTP_Write1B_ACUT(u32 CtrlSetting, u16 Addr, u8 Data, u8 L25OutVoltage);
u32 EFUSE_OTP_Write32B_ACUT(u8 *pContant);
/**
  * @}
  */

/** @defgroup EFUSE_FW_Protection_functions 
  * @{
  */
_LONG_CALL_ void EFUSE_RDP_EN(void);
_LONG_CALL_ void EFUSE_RDP_KEY(u8 *RdpKey);
_LONG_CALL_ void EFUSE_OTF_KEY(u8 *OtfKey);
_LONG_CALL_ u32 EFUSE_JTAG_OFF(void);
u32 EFUSE_JTAG_OFF_ACUT(void);
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


/* Other functions --------------------------------------------------------*/

#define EFUSE_POLL_TIMES	20000

/* physical map bit Macro */
/* 0xD3 */
#define EFUSE_PHYSICAL_JTAG_ON		BIT(0)	/* 0: JTAG OFF, default 1: JTAG ON */

extern u8 EFUSE_MAP[512];

#ifdef CONFIG_CHIP_A_CUT
#define EFUSE_JTAG_OFF_ROM		EFUSE_JTAG_OFF_ACUT
#define EFUSE_OTP_Write32B_ROM	EFUSE_OTP_Write32B_ACUT
#define EFUSE_OTP_Write1B_ROM		EFUSE_OTP_Write1B_ACUT
#define EFUSE_USER1_Write_ROM		EFUSE_USER1_Write_ACUT
#define EFUSE_LMAP_READ			EFUSE_LogicalMap_Read_ACUT
#define EFUSE_LMAP_WRITE			EFUSE_LogicalMap_Write_Check
#define EFUSE_WRITE_CHECK			EFUSE_WRITE_Check_ACUT
#else
#define EFUSE_JTAG_OFF_ROM		EFUSE_JTAG_OFF
#define EFUSE_OTP_Write32B_ROM	EFUSE_OTP_Write32B
#define EFUSE_OTP_Write1B_ROM		EFUSE_OTP_Write1B
#define EFUSE_USER1_Write_ROM		EFUSE_USER1_Write
#define EFUSE_LMAP_READ			EFUSE_LogicalMap_Read
#define EFUSE_LMAP_WRITE			EFUSE_LogicalMap_Write
#define EFUSE_WRITE_CHECK			EFUSE_WRITE_Check
#endif

#endif //_8710B_EFUSE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
