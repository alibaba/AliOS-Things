/** mbed Microcontroller Library
  ******************************************************************************
  * @file    efuse_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for EFUSE.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "ameba_soc.h"

//#ifdef CONFIG_EFUSE_EN

static u32 EFUSE_OTP_Check(u16 Addr, u8 Data)
{
	u8 L25OutVoltage = L25EOUTVOLTAGE;
	u32 CtrlSetting = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL);
	u32 bResult = _SUCCESS;
	u8 temp = 0xFF;

	EFUSE_WRITE_Check_ACUT(ENABLE);
	if ((Addr >= (OTP_SECTION)) && (Addr < (OTP_SECTION + OTP_SECTION_LEN))){
		
		EFUSE_OneByteRead_ACUT(CtrlSetting, Addr, &temp, L25OutVoltage);

		if (temp == 0xff) {
			bResult = EFUSE_OneByteWriteROM(CtrlSetting, Addr, Data, L25OutVoltage);
			if (bResult == _SUCCESS){
				EFUSE_OneByteRead_ACUT(CtrlSetting, Addr, &temp, L25OutVoltage);
				if (temp != Data){
					EFUSE_OneByteWriteROM(CtrlSetting, Addr, Data, L25OutVoltage);
					EFUSE_OneByteRead_ACUT(CtrlSetting, Addr, &temp, L25OutVoltage);
					if (temp != Data){
						bResult = _FAIL;
					}
				}
			}
		} else {
			if (temp != Data){
				EFUSE_OneByteWriteROM(CtrlSetting, Addr, Data, L25OutVoltage);
				EFUSE_OneByteRead_ACUT(CtrlSetting, Addr, &temp, L25OutVoltage);
				if (temp != Data){
					bResult = _FAIL;
				}
			}
		}
	}
	EFUSE_WRITE_Check_ACUT(DISABLE);
	
	return bResult;
}

/**
  * @brief  Get remaining efuse length 
  * @retval remaining efuse length 
  */
int  efuse_get_remaining_length(void)
{
	return EFUSE_RemainLength();
}


/**
  * @brief  Read efuse contant of specified user 
  * @param  data: Specified the address to save the readback data.
  * @retval none  
  */
void  efuse_mtp_read(uint8_t * data)
{
	EFUSE_USER1_Read(data);
	return;
}

/**
  * @brief  Write user's contant to efuse
  * @param  data: Specified the data to be programmed.
  * @param  len: Specifies the data length of programmed data.
  * @retval status value:
  *          - 0~32: Success
  *          - -1: Failure   
  */
int  efuse_mtp_write(uint8_t *data, uint8_t len)
{

	u8 len_low, section_num, word_enable = 0;
	u8 ret = 0;
	
	if( (len & 0x01) == 1)
		len += 1;

	if(len > 32){
		return -1;
	}
	if(len == 0){
		return 0;
	}

	/* 8bytes one section */
	len_low = len & 0x07;
	section_num = (len >> 3) & 0x07;

	if(len_low == 0)
		word_enable = 0; /* 0 word write enable */
	else if(len_low == 2)
		word_enable = 1; /* 1 word write enable */
	else if(len_low == 4)
		word_enable = 3; /* 2 word write enable */
	else if(len_low == 6)
		word_enable = 7; /* 3 word write enable */
		
	switch (section_num){
		case 0:
			ret = EFUSE_USER1_Write_ROM(0, word_enable, data);		if (ret == _FAIL) return -1;
			break;
			
		case 1:
			ret = EFUSE_USER1_Write_ROM(0, 0xf, data);				if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(1, word_enable, data+8);	if (ret == _FAIL) return -1;
			break;

		case 2:
			ret = EFUSE_USER1_Write_ROM(0, 0xf, data);				if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(1, 0xf, data+8);			if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(2, word_enable, data+8);	if (ret == _FAIL) return -1;
			break;

		case 3:
			ret = EFUSE_USER1_Write_ROM(0, 0xf, data);				if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(1, 0xf, data+8);			if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(2, 0xf, data+16);			if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(3, word_enable, data+24);	if (ret == _FAIL) return -1;
			break;

		case 4:
			ret = EFUSE_USER1_Write_ROM(0, 0xf, data);				if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(1, 0xf, data+8);			if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(2, 0xf, data+16);			if (ret == _FAIL) return -1;
			ret = EFUSE_USER1_Write_ROM(3, 0xf, data+24);			if (ret == _FAIL) return -1;
	}		
	return len;
}


/**
  * @brief  Read efuse OTP contant 
  * @param  address: Specifies the offset of the OTP.
  * @param  len: Specifies the length of readback data.
  * @param  buf: Specified the address to save the readback data.
  * @retval status value:
  *          - 0: Success
  *          - -1: Failure   
  */
int efuse_otp_read(u8 address, u8 len, u8 *buf)
{
	u8 content[32];	// the OTP max length is 32

	if((address+len) > 32) {
		return -1;
	}
	
	EFUSE_OTP_Read32B(content);
	_memcpy(buf, content+address, len);
	
	return 0;
}


/**
  * @brief  Write user's contant to OTP efuse
  * @param  address: Specifies the offset of the programmed OTP.
  * @param  len: Specifies the data length of programmed data.
  * @param  buf: Specified the data to be programmed.
  * @retval status value:
  *          - 0: Success
  *          - -1: Failure   
  */
int efuse_otp_write(u8 address, u8 len, u8 *buf)
{
	u32 ret = 0;
	u8 content[32];	// the OTP max length is 32
	
	if((address+len) > 32) {
		return -1;
	}
	
	_memset(content, 0xFF, 32);
	_memcpy(content+address, buf, len);
	
	ret = EFUSE_OTP_Write32B_ROM(content);
	
	if (ret == _SUCCESS) {
		return 0;
	} else {
		return -1;
	}
}

/**
  * @brief  check user's contant to OTP efuse
  * @param  *buf: Specified the data to be programmed.
  * @param len: Specifies the data length of programmed data.
  * @retval   status: Success:0 or Failure: -1.
  */
int efuse_otp_chk(u8 len, u8 *buf)
{
	u8 content[32];	// the OTP max length is 32
	u8 Idx = 0;
	u32 bResult = _SUCCESS;

	_memset(content, 0xFF, 32);
	_memcpy(content, buf, len);

	for (Idx = 0; Idx < OTP_SECTION_LEN; Idx++){
		bResult = EFUSE_OTP_Check((OTP_SECTION+Idx), (*(content+Idx)));
		if (bResult != _SUCCESS){
			break;
		}
	}

	return (bResult? 0 : -1);
}

/**
  * @brief  Disable jtag
  * @retval status: Success:0.
  */
int efuse_disable_jtag(void)
{
	u32 ret = 0;
	
	ret = EFUSE_JTAG_OFF_ROM();

	if (ret == _SUCCESS) {
		return 0;
	} else {
		return -1;
	}
}

/**
  * @brief  Set RDP Enable.
  * @param  none
  * @note  can not change or read back after write.
  */
void efuse_rdp_enable(void)
{
	EFUSE_RDP_EN();
}

/**
  * @brief  Set 16B RDP key into EFUSE.
  * @param  rdp_key: 16B EFUSE KEY
  * @note  can not change or read back after write.
  */
void efuse_rdp_keyset(u8 *rdp_key)
{
	EFUSE_RDP_KEY(rdp_key);
}


/**
  * @brief  Set 16B OTF key into EFUSE.
  * @param  OtfKey: 16B EFUSE KEY
  * @note  can not change or read back after write.
  */
void efuse_otf_keyset(u8 *otf_key)
{
	EFUSE_OTF_KEY(otf_key);
}

//#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
