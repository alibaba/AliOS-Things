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
#include "rtl8711b_efuse.h"
#include "rom_aes.h"
//#ifdef CONFIG_EFUSE_EN

typedef union
{   unsigned int l;
    unsigned char b[4];
} aes_inf;

typedef struct
{
    aes_context ctx;
    aes_inf inf;
} aes_encrypt_ctx;

#define RDP_KEY_CHECK_TIMES   (3)

/*plaintext image*/
static u8 rdp_bin_test_plaintext[16]= {0x08,0x18,0x70,0x47,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
/*encrypt image buffer*/
static u8 rdp_bin_test_enc[16] = {0};
/*checksum*/
static u32 rdp_bin_checksum = 0;
/*RDP key*/
static u8 * RDP_key = NULL;

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
			ret = EFUSE_USER1_Write_ROM(2, word_enable, data+16);	if (ret == _FAIL) return -1;
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
	RDP_key = rdp_key;
}

/**
  * @brief  encypt rdp check image.
  * @param  SW_Key: key for encyption
  * @note  none
  */
void efuse_rdp_check_image_encrypt(u8* SW_Key)
{

	int i;
	aes_encrypt_ctx enc_ctx;
	u8 * pByte;

	pByte = (u8 *)&rdp_bin_checksum;
	memset(&enc_ctx, 0, sizeof(enc_ctx));

	/*calculate checksum using plaintext*/
	for(i=0; i<4; i++) {
		pByte[0] ^= rdp_bin_test_plaintext[i*4];
		pByte[1] ^= rdp_bin_test_plaintext[i*4 + 1];
		pByte[2] ^= rdp_bin_test_plaintext[i*4 + 2];
		pByte[3] ^= rdp_bin_test_plaintext[i*4 + 3];
	}
	
	/*encrypt image*/
	aes_init();
	aes_encrypt_key(SW_Key, 128, &enc_ctx);	
	aes_ecb_encrypt(rdp_bin_test_plaintext, rdp_bin_test_enc, 16, &enc_ctx);

	DBG_8195A("[%s] rdp_bin_checksum = %x\n", __func__, rdp_bin_checksum);
}

/**
  * @brief  check RDP key.
  * @param  SW_key: 16B EFUSE SW-KEY
  * @note  only after calling API "efuse_rdp_enable()" and "efuse_rdp_keyset()" can this API be called.
  */
void efuse_rdp_check(u8* SW_key)
{
	u32 temp = 0;
	u32 ret = TRUE;
	SYSTEM_DATA* pSysData = (SYSTEM_DATA*)(SPI_FLASH_BASE + FLASH_SYSTEM_DATA_ADDR);
	u32 RDPFlashAddr = pSysData->RDP_FlashAddr;
	u32 RDPLen = pSysData->RDP_Len;
	u32 checksum_ipsec, checksum_rdp_flash;
	u8 aes_ecb_key_dummy[16]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	u8 *rdp_bin_data_en = (u8*)((u32)__rom_top_4k_start_ - 16 * 1024);
	u32 PollTimes;
	u32 RdpEnable = 0;
	u32 WriteKeyState = 0;
	u32 RDP_ERR_Flag = 0;

	EFUSE_WRITE_CHECK(ENABLE);

	/*check RDP enable bit*/
	for(PollTimes = 0; PollTimes < RDP_KEY_CHECK_TIMES; PollTimes++) {
		/* load RDP EN from efuse */
		ret = RDP_EN_Request();

		if (ret == FALSE) {
			DBG_8195A("BIT_RDP_EN_LOAD timeout.\n");
			RDP_ERR_Flag = RDP_KEY_REQUEST_TIMEOUT;
			goto exit;
		}

		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_FW_PPROTECT_KEY_CTRL);

		if (temp & BIT_RDP_EN) {
			RdpEnable = 1;
			DBG_8195A("[%s] PG RDP enable bit ok: %d\n", __func__, PollTimes);
			break;
		} else {
			EFUSE_RDP_EN();
			DBG_8195A("[%s] PG RDP enable bit again: %d\n", __func__, PollTimes);
		}
	}
	if(RdpEnable == 0) {
		DBG_8195A("PG RDP enable bit fail!\n");
		goto exit;
	}

	/*if software key is not NULL, use the encrypt firmware in ram to check the RDP-key; if NULL, 
	use the 4kB encrypt firmare burnt in flash to check the RDP-key*/
	if(SW_key != NULL) {
		efuse_rdp_check_image_encrypt(SW_key);
		RDPFlashAddr = (u32)&rdp_bin_test_enc ;
		RDPLen = 16;
		checksum_rdp_flash = rdp_bin_checksum;
		DBG_8195A("software key is valid, use firmware in ram to check RDP-key!\n");
	} else {
		checksum_rdp_flash = HAL_READ32(RDPFlashAddr, RDPLen);
		DBG_8195A("software key is not NULL, use the 4KB firmware in flash to check RDP-key!\n");
	}

	if(RDP_key == NULL) {
		DBG_8195A("[%s]: RDP key is NULL\n", __func__);
		goto exit;
	}

	/*if RDP enable, check the RDP key*/
	if (RdpEnable) {
		for(PollTimes = 0; PollTimes < RDP_KEY_CHECK_TIMES; PollTimes++) {
			DBG_8195A("BOOT_FLASH_RDP RDP enable \n");		
			/* enable the key request from efuse */
			KEY_Request(BIT_RDP_KEY_REQ);

			/* read encrypted rdp code and data from flash or ram*/
			_memcpy(rdp_bin_data_en, (const void*)RDPFlashAddr, RDPLen);
			
			if(0xFFFFFFFF == *((u32*)rdp_bin_data_en)) {
				DBG_8195A("RDP.bin Empty.\n");
				RDP_ERR_Flag = RDP_RDPBIN_WRONG;
				goto exit;
			}
			/* decrypt using ctr method and DMA to RAM rdp region using IPSEC */
			rtl_cryptoEngine_init();
			/* RDP key will be use when RDPEN, here we just set a dummy key */
			rtl_crypto_aes_ecb_init(aes_ecb_key_dummy, 16);
			CRYPTO_SetCheckSumEn(1);
			rtl_crypto_aes_ecb_decrypt(rdp_bin_data_en,  RDPLen , NULL, 0, (u8*)0x1003F000); 
			CRYPTO_GetCheckSumData(&checksum_ipsec);

			/* read checksum from rdp flash region and compare with checksum calculated by IPSEC */
			//checksum_rdp_flash = HAL_READ32(RDPFlashAddr, RDPLen);
				
			if(checksum_rdp_flash == checksum_ipsec){
				WriteKeyState = 1;
				DBG_8195A("[%s] PG write RDP key ok: %d\n", __func__, PollTimes);
				DBG_8195A("RDP bin decryption OK!\n");
				break;
			}else{
				EFUSE_RDP_KEY(RDP_key);
				DBG_8195A("write RDP key again: %d \n", PollTimes);
				DBG_8195A("checksum_ipsec = 0x%x, checksum_rdp_flash = 0x%x\n", checksum_ipsec, checksum_rdp_flash);
				RDP_ERR_Flag = RDP_CHECKSUM_ERROR;
			}
		}

		if(WriteKeyState == 0) {
			DBG_8195A("write RDP key failed\n");
			goto exit;
		}
		
	} else {
		RDP_ERR_Flag = RDP_NOT_ENABLE;
		goto exit;
	}
	
exit:
	if(RDP_ERR_Flag != 0) {
		DBG_8195A("RDP_ERR_Flag: %d\n", RDP_ERR_Flag);
	}
	EFUSE_WRITE_CHECK(DISABLE);
}

/**
  * @brief  Enable/Disable OTF function.
  * @param  newStatus: can be ENABLE/DISABLE 
  */
void efuse_otf_cmd(u32 NewState)
{
	u32 value = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6);
	u8 ret = _SUCCESS;
	
	if(NewState != DISABLE){
		value |= BIT_SYS_FLASH_ENCRYPT_EN;
	}else{
		value &= ~BIT_SYS_FLASH_ENCRYPT_EN;
	}

	ret = EFUSE_LMAP_WRITE(0x18, 2, (u8*)&value);
	if (ret == _FAIL) {
		DBG_8195A("Set OTF control bit fail \n");
	}
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
