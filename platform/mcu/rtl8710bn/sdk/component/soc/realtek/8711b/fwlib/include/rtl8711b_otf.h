/**
  ******************************************************************************
  * @file    rtl8711b_otf.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the flash run time decrypt firmware
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

#ifndef _RTL8711B_OTF_H_
#define _RTL8711B_OTF_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup PROTECTION 
  * @brief PROTECTION driver modules
  * @{
  */

/** @addtogroup PROTECTION
  * @verbatim
  *****************************************************************************************
  * RSIP(OTF) Introduction
  *****************************************************************************************
  *		-used for flash firmware protection, and flash firmware will be encrypted use AES.
  *		-16B KEY shoud be written to EFUSE OTP KEY area use EFUSE_OTF_KEY.
  *		-Enable should be write to EFUSE 0x19[5].
  *****************************************************************************************
  * RDP Introduction
  *****************************************************************************************
  *		-after enable RDP, top 4k RAM can not be read.
  *		-16B RDP key should be written to EFUSE RDP key area use EFUSE_RDP_KEY.
  *		-Enable bit should be write to EFUSE use EFUSE_RDP_EN.
  *****************************************************************************************
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PROTECTION_Exported_Constants OTF Exported Constants
  * @{
  */ 
#define OTF_LENGTH_UNIT				4096
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PROTECTION_Exported_Functions OTF Exported Functions
  * @{
  */
_LONG_CALL_ void OTF_init(u8* IV);
_LONG_CALL_ void OTF_Cmd(u32 NewStatus);
_LONG_CALL_ void OTF_Mask(u32 MaskIdx, u32 Addr, u32 Len, u32 NewStatus);
_LONG_CALL_ u32 KEY_Request(u32 KeyTypeBit);
_LONG_CALL_ u32 RDP_EN_Request(void);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup OTF_Register_Definitions OTF Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup OTF_DEC
 * @{
 *****************************************************************************/
#define REG_SYS_OTF_DEC_CTRL				0x02D8
#define REG_SYS_OTF_DEC_ADDR_MASK0		0x02DC
#define REG_SYS_OTF_DEC_ADDR_MASK1		0x02E4
#define REG_SYS_OTF_DEC_ADDR_MASK2		0x02E8
#define REG_SYS_OTF_DEC_ADDR_MASK3		0x02EC
#define REG_SYS_OTF_DEC_IV_EXT			0x02F0
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_OTF_DEC_CTRL
 * @{
 *****************************************************************************/
#define OTF_FEN_OTFDEC				((u32)0x00000001)            /*!<function enable of OTF decoder */
#define OTF_DEC_IV_BYTE_SWAP		((u32)0x00000002)            /*!<Big/little endian conversion for input OTF IV */
#define OTF_DEC_KEY_BYTE_SWAP		((u32)0x00000004)            /*!<Big/little endian conversion for input OTF KEY*/
#define OTF_DEC_CIPHER_BYTE_SWAP	((u32)0x00000008)	            /*!Big/little endian conversion for calculated cipher*/
/** @} */

/**************************************************************************//**
 * @defgroup OTF_DEC_ADDR_MASK
 * @{
 *****************************************************************************/
#define OTF_DEC_MASK_EN			((u32)0x00000001)            /*!<Decoder mask enable for address~address+length */
#define OTF_DEC_MASK_SA			((u32)0x01FFFFFE)            /*!<Start address for decoder mask, unit is byte */
#define OTF_DEC_MASK_LEN			((u32)0xFE000000)            /*!<Address range for decoder mask, unit is 4KB */
/** @} */

/**************************************************************************//**
 * @defgroup RDP_ERROR_STATUS
 * @{
 *****************************************************************************/
#define RDP_SYSTEMBIN_WRONG		((u32)0x00000001)            /*!<system.bin not load to flash */
#define RDP_RDPBIN_WRONG			((u32)0x00000002)            /*!<rdp.bin not load to flash */
#define RDP_KEY_REQUEST_TIMEOUT	((u32)0x00000003)            /*!<Key request timeout */
#define RDP_NOT_ENABLE				((u32)0x00000004)            /*!<RDP not enable in efuse */
#define RDP_CHECKSUM_ERROR		((u32)0x00000005)            /*!<Check sum error */
/** @} */

/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other definations --------------------------------------------------------*/
#define KEY_REQ_POLL_TIMES			0xFF

#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
