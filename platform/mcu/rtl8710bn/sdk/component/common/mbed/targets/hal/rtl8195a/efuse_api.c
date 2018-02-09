/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "rtl8195a.h"

#ifdef CONFIG_EFUSE_EN

extern VOID  ReadEfuseContant1(OUT u8 *pContant);
extern u8 WriteEfuseContant1(IN u8 CodeWordNum, IN u8 WordEnable, IN u8 *pContant);
extern VOID ReadEOTPContant(OUT u8 *pContant);
extern u32  WriteEOTPContant(IN u8 *pContant);
extern u32 EOTPChkContant(IN u8 * pContant);
extern u32  WriteKEY1(IN u8 *pContant);
extern u32  WriteKEY2(IN u8 *pContant);
extern u8 GetRemainingEfuseLength(void);
extern VOID HALJtagOff(VOID);

/**
  * @brief  get remaining efuse length 
  * @retval  remaining efuse length 
  */
int  efuse_get_remaining_length(void)
{
    return GetRemainingEfuseLength();
}


/**
  * @brief  Read efuse contant of specified user 
  * @param  data: Specified the address to save the readback data.
  */
void  efuse_mtp_read(uint8_t * data)
{
    ReadEfuseContant1(data);
    return;
}

/**
  * @brief  Write user's contant to efuse
  * @param  *data: Specified the data to be programmed.
  * @param len: Specifies the data length of programmed data.
  * @retval   status: Success:0~32 or Failure: -1.
  */
int  efuse_mtp_write(uint8_t *data, uint8_t len)
{
    
    u8 len_low, len_high, word_enable = 0;	
	
	if( (len & 0x01) == 1)
		len += 1;
	
 	if(len > 32){
 		return -1;
 	}
 	if(len == 0){
		return 0;
 	}
	
	len_low = len & 0x07;
	len_high = (len >> 3) & 0x07;
	
	if(len_low == 0)
		word_enable = 0;
	else if(len_low == 2)
		word_enable = 1;
	else if(len_low == 4)
		word_enable = 3;
	else if(len_low == 6)
		word_enable = 7;
		
	switch (len_high){
		case 0:
			WriteEfuseContant1(0, word_enable, data);
			break;
			
		case 1:
			WriteEfuseContant1(0, 0xf, data);
			WriteEfuseContant1(1, word_enable, data+8);	
			break;

		case 2:
			WriteEfuseContant1(0, 0xf, data);
			WriteEfuseContant1(1, 0xf, data+8);
			WriteEfuseContant1(2, word_enable, data+8);
			break;

		case 3:
			WriteEfuseContant1(0, 0xf, data);
			WriteEfuseContant1(1, 0xf, data+8);
			WriteEfuseContant1(2, 0xf, data+16);
			WriteEfuseContant1(3, word_enable, data+24);
			break;

		case 4:
			WriteEfuseContant1(0, 0xf, data);
			WriteEfuseContant1(1, 0xf, data+8);
			WriteEfuseContant1(2, 0xf, data+16);
			WriteEfuseContant1(3, 0xf, data+24);
	}		
	return len;
}


/**
 * @brief  Read efuse OTP contant 
 * @param  address: Specifies the offset of the OTP.
 * @param len: Specifies the length of readback data.
 * @param  buf: Specified the address to save the readback data.
 */
int efuse_otp_read(u8 address, u8 len, u8 *buf)
{
	u8 content[32];	// the OTP max length is 32
	
	if((address+len) > 32)
		return -1;
	ReadEOTPContant(content);
	_memcpy(buf, content+address, len);
	return 0;
}


/**
  * @brief  Write user's contant to OTP efuse
  * @param  address: Specifies the offset of the programmed OTP.
  * @param len: Specifies the data length of programmed data.
  * @param  *buf: Specified the data to be programmed.
  * @retval   status: Success:0 or Failure: -1.
  */
int efuse_otp_write(u8 address, u8 len, u8 *buf)
{
	u8 content[32];	// the OTP max length is 32
	u32 result;

	if((address+len) > 32)
		return -1;
	_memset(content, 0xFF, 32);
	_memcpy(content+address, buf, len);
	result = WriteEOTPContant(content);

    return (result? 0 : -1);
}


/**
  * @brief  ckeck user's contant to OTP efuse
  * @param  *buf: Specified the data to be programmed.
  * @param len: Specifies the data length of programmed data.
  * @retval   status: Success:0 or Failure: -1.
  */
int efuse_otp_chk(u8 len, u8 *buf)
{
	u8 content[32];	// the OTP max length is 32
	u32 result;

	_memset(content, 0xFF, 32);
	_memcpy(content, buf, len);
	result = EOTPChkContant(content);

    return (result? 0 : -1);
}


/**
  * @brief  Write key1 to efuse
  * @param  address: Specifies the offset of the programmed efuse.
  * @param len: Specifies the data length of programmed data.
   * @param  *buf: Specified the data to be programmed.
  * @retval   status: Success:0 or Failure: -1.
  */
int efuse_key1_write(u8 address, u8 len, u8 *buf)
{
	u8 content[16];	// the key max length is 16
	u32 result;

	if((address+len) > 16)
		return -1;
	_memset(content, 0xFF, 16);
	_memcpy(content+address, buf, len);
	result = WriteKEY1(content);

    return (result? 0 : -1);
}


/**
  * @brief  Write key2 to efuse
  * @param  address: Specifies the offset of the programmed efuse.
  * @param len: Specifies the data length of programmed data.
   * @param  *buf: Specified the data to be programmed.
  * @retval   status: Success:0 or Failure: -1.
  */
int efuse_key2_write(u8 address, u8 len, u8 *buf)
{
	u8 content[16];	// the key max length is 16
	u32 result;

	if((address+len) > 16)
		return -1;
	_memset(content, 0xFF, 16);
	_memcpy(content+address, buf, len);
	result = WriteKEY2(content);

    return (result? 0 : -1);
}


/**
  * @brief  Disable jtag
  */
int efuse_disable_jtag(void)
{
    HALJtagOff();
    return 0;
}
#endif
