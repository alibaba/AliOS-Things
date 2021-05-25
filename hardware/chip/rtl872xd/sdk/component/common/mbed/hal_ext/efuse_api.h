/** mbed Microcontroller Library
  ******************************************************************************
  * @file    efuse_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed EFUSE API.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  ****************************************************************************** 
  */

#ifndef MBED_EXT_EFUSE_API_EXT_H
#define MBED_EXT_EFUSE_API_EXT_H
 
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup efuse EFUSE
 *  @ingroup    hal
 *  @brief      efuse functions
 *  @{
 */

///@name Ameba Common 
///@{

/**
  * @brief  Get remaining efuse length 
  * @retval : remaining efuse length 
  */
int efuse_get_remaining_length(void);

/**
  * @brief  Read efuse contant of specified user 
  * @param  data: Specified the address to save the readback data.
  * @retval none  
  */
void efuse_mtp_read(uint8_t * data);

/**
  * @brief  Write user's contant to efuse
  * @param  data: Specified the data to be programmed.
  * @param  len: Specifies the data length of programmed data.
  * @retval value:
  *          - 0~32: Success
  *          - -1: Failure   
  */
int efuse_mtp_write(uint8_t *data, uint8_t len);

/**
  * @brief  Read efuse OTP contant 
  * @param  address: Specifies the offset of the OTP.
  * @param  len: Specifies the length of readback data.
  * @param  buf: Specified the address to save the readback data.
  * @retval 0: Success
  * @retval -1: Failure
  */
int efuse_otp_read(u8 address, u8 len, u8 *buf);

/**
  * @brief  Write user's contant to OTP efuse
  * @param  address: Specifies the offset of the programmed OTP.
  * @param  len: Specifies the data length of programmed data.
  * @param  buf: Specified the data to be programmed.
  * @retval 0: Success
  * @retval -1: Failure
  */
int efuse_otp_write(u8 address, u8 len, u8 *buf);

/**
  * @brief  ckeck user's contant to OTP efuse
  * @param  buf: Specified the data to be programmed.
  * @param  len: Specifies the data length of programmed data.
  * @retval 0: Success
  * @retval -1: Failure
  */
int efuse_otp_chk(u8 len, u8 *buf);

/**
  * @brief  Disable jtag
  * @retval 0: Success
  */
int efuse_disable_jtag(void);


///@}

#if defined(CONFIG_PLATFORM_8195A) && (CONFIG_PLATFORM_8195A == 1)
///@name Ameba1 Only 
///@{
/**
  * @brief  Write key1 to efuse
  * @param  address: Specifies the offset of the programmed efuse.
  * @param  len: Specifies the data length of programmed data.
  * @param  buf: Specified the data to be programmed.
  * @retval 0: Success
  * @retval -1: Failure
  */
int efuse_key1_write(u8 address, u8 len, u8 *buf);

/**
  * @brief  Write key2 to efuse
  * @param  address: Specifies the offset of the programmed efuse.
  * @param  len: Specifies the data length of programmed data.
  * @param  buf: Specified the data to be programmed.
  * @retval 0: Success
  * @retval -1: Failure
  */
int efuse_key2_write(u8 address, u8 len, u8 *buf);
///@}
#endif //CONFIG_PLATFORM_8195A

#if defined(CONFIG_PLATFORM_8711B) && (CONFIG_PLATFORM_8711B == 1)
///@name AmebaZ Only 
///@{
/**
  * @brief  Set RDP Enable.
  * @param  none
  * @note  can not change or read back after write.
  */
void efuse_rdp_enable(void);

/**
  * @brief  Set 16B RDP key into EFUSE.
  * @param  rdp_key: 16B EFUSE KEY
  * @note  can not change or read back after write.
  */
void efuse_rdp_keyset(u8 *rdp_key);

/**
  * @brief  Set 16B OTF key into EFUSE.
  * @param  OtfKey: 16B EFUSE KEY
  * @note  can not change or read back after write.
  */

void efuse_otf_keyset(u8 *otf_key);
///@}
#endif //CONFIG_PLATFORM_8711B

#if defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1)
///@name AmebaPro Only 
///@{
/**
  * @brief  Disable LP jtag
  * @retval 0: Success
  */
int efuse_disable_lp_jtag(void);

/**
  * @brief  Disable HS secure jtag
  * @retval 0: Success
  */
int efuse_disable_sec_jtag(void);

/**
  * @brief  Disable HS nonsecure jtag
  * @retval 0: Success
  */
int efuse_disable_nonsec_jtag(void);
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BLP)"

#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
/**
  * @brief  Write secure key to efuse
  * @param  buf: Specified the 32-byte security key to be programmed.
  * @retval 0: Success
  * @retval -1: Failure
  */
int efuse_sec_key_write(u8 *buf);

/**
  * @brief  Write super secure key to efuse
  * @param  buf: Specified the 32-byte super security key to be programmed.
  * @retval 0: Success
  * @retval -1: Failure
  */
int efuse_susec_key_write(u8 *buf);
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif // MBED_EXT_EFUSE_API_EXT_H
