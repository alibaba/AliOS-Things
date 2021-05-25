/**
  ******************************************************************************
  * @file    rtl8721d_efuse.h
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

/** @addtogroup AmebaD_Platform
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
#define EFUSE_MAP_LEN_8711B			1024 /*!< logical map len in byte */
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
#define EFUSE_REAL_CONTENT_LEN		512
#define AVAILABLE_EFUSE_ADDR(addr)		(addr < EFUSE_REAL_CONTENT_LEN)

/* physical EFUSE write forbid */
#define LOGICAL_MAP_SECTION_LEN		0x11E /*!< logical mapping efuse len in physical address */
#define EFUSE_OOB_PROTECT_BYTES		(EFUSE_REAL_CONTENT_LEN - LOGICAL_MAP_SECTION_LEN) // Security + RF + MAC + OTP
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
_LONG_CALL_ extern void EFUSEPowerSwitch(u8 bWrite, u8 PwrState, u8 L25OutVoltage);
_LONG_CALL_ extern u32 EFUSERead8(u32 CtrlSetting, u32 Addr, u8 *Data, u8 L25OutVoltage);
/* please use EFUSE_PMAP_WRITE8, dont use this API direclty, or chip will be damaged */
_LONG_CALL_ extern u32 EFUSEWrite8(u32 CtrlSetting, u32 Addr, u8 Data, u8 L25OutVoltage);
_LONG_CALL_ extern u32 EFUSE_LogicalMap_Read(u8 *pbuf);
/* please use EFUSE_LMAP_WRITE, dont use this API direclty, or chip will be damaged */
_LONG_CALL_ extern u32 EFUSE_LogicalMap_Write(u32 addr, u32 cnts, u8 *data);
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

#define EFUSE_SECURE_START					0x150	/* 0x150~0x17F: TrustZone Secure EFUSE */
#define EFUSE_SECURE_END					0x17F
#define EFUSE_RDP_KEY_ADDR					0x170	/* 0x170~0x17F: 16B*/

#define EFUSE_SWD_PWD_ADDR				0x180	/* 0x180~0x18F: 16B */
#define RSIP_KEY_ADDR						0x190	/* 0x190~0x19F: 16B */
#define SBOOT_PK_ADDR						0x1A0	/* 0x1A0~0x1BF: 32B*/

#define EFUSE_SEC_CONFIG_ADDR0			0x1C0	/* 0x1c0 & 0x1c1, security config, please ref REG_LP_EFUSE_PROTECTION for bit define */
#define EFUSE_SEC_CONFIG_ADDR1			0x1C1	/* 0x1c0 & 0x1c1, security config, please ref REG_LP_EFUSE_PROTECTION for bit define */

extern u8 EFUSE_MAP[1024];

__STATIC_INLINE u32 EFUSE_PMAP_READ8(u32 CtrlSetting, u32 Addr, u8 *Data, u8 L25OutVoltage)
{
	return EFUSERead8(CtrlSetting, Addr, Data, L25OutVoltage);
}

__STATIC_INLINE u32 EFUSE_PMAP_WRITE8(u32 CtrlSetting, u32 Addr, u8 Data, u8 L25OutVoltage)
{
	if (is_power_supply18() == TRUE) {
		DBG_8195A("Please Switch to 3.3V to PG EFUSE !!!!!");
		//while (1);

		return FALSE;
	} else {
		return EFUSEWrite8(CtrlSetting, Addr, Data, L25OutVoltage);
	}
}

__STATIC_INLINE u32 EFUSE_LMAP_READ(u8 *pbuf)
{
	return EFUSE_LogicalMap_Read(pbuf);
}

__STATIC_INLINE u32 EFUSE_LMAP_WRITE(u32 addr, u32 cnts, u8 *data)
{
	if (is_power_supply18() == TRUE) {
		DBG_8195A("Please Switch to 3.3V to PG EFUSE !!!!!");
		//while (1);

		return FALSE;
	} else {
		return EFUSE_LogicalMap_Write(addr, cnts, data);
	}
}

__STATIC_INLINE u32 EFUSE_IsSecure(u32 Addr)
{
	if (TrustZone_IsSecure()) {
		if ((Addr >= 0x150) && (Addr <= 0x17F)) {
			return TRUE;
		}
	}

	return FALSE;
}

/**
  * @brief  Get EFUSE physical address remain length.
  * @param  none
  * @retval EFUSE physical address remain length
  */

__STATIC_INLINE u32 EFUSE_RemainLength(void)
{
	u32 Idx = 0;
	u8 DataTemp0, WordEn;

	//find start add
	while(Idx < LOGICAL_MAP_SECTION_LEN){

		EFUSERead8(0, Idx, &DataTemp0, L25EOUTVOLTAGE);

		if (DataTemp0 != 0xff) {

			if((DataTemp0&0x0f) == 0xf){

				Idx++;
				EFUSERead8(0, Idx, &DataTemp0, L25EOUTVOLTAGE);
				WordEn = ((~DataTemp0)&0x0f);

				while(WordEn!=0){
					if (WordEn & BIT0) {
						Idx = Idx + 2;
					}
					WordEn = WordEn>>1;
				}
			}
			else {
				WordEn = ((~DataTemp0)&0x0f);
				while(WordEn!=0){
					if (WordEn & BIT0) {
						Idx = Idx + 2;
					}
					WordEn = WordEn>>1;
				}
			}
		}
		else {
			break;
		}

		Idx++;
	}

	return (LOGICAL_MAP_SECTION_LEN - Idx);
}

#endif //_8710B_EFUSE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
