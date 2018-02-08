/** mbed Microcontroller Library
  ******************************************************************************
  * @file    efuse_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed EFUSE API.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
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

#if CONFIG_PLATFORM_8195A
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

#if CONFIG_PLATFORM_8711B
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

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif // MBED_EXT_EFUSE_API_EXT_H
