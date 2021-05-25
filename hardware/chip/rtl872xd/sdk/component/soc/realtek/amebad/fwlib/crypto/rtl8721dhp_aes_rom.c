/**
  ******************************************************************************
  * @file    rtl8721dhp_aes_rom.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - Initialization
  *           - AES CBC/ECB/CTR/CFB/OFB/GCM
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

HAL_ROM_DATA_SECTION
static const u8 gcm_iv_tail[] __attribute__((aligned(32))) = {0x00, 0x00, 0x00, 0x01};

/**
  * @brief  AES-CBC init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_cbc_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	
	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_AES_CBC, key, keylen);
}

/**
  * @brief  AES-CBC encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen != 16 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}


/**
  * @brief  AES-CBC decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen != 16 ) return _ERRNO_CRYPTO_IV_OutRange;
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-ECB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_AES_ECB, key, keylen);
}

/**
  * @brief  AES-ECB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ecb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	//if ( ivlen != 0 ) return _ERRNO_CRYPTO_IV_OutRange; // ECB mode : ivlen = 0
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-ECB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ecb_decrypt(
		IN const u8* message, 	IN const u32 msglen, 
		IN const u8* iv, 		IN const u32 ivlen, 
		OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-CTR init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ctr_init(IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_AES_CTR, key, keylen);
}

/**
  * @brief  AES-CTR encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ctr_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-CTR decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ctr_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-CFB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_cfb_init(IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_AES_CFB, key, keylen);
}

/**
  * @brief  AES-CFB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_cfb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-CFB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_cfb_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-OFB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ofb_init(IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_AES_OFB, key, keylen);
}

/**
  * @brief  AES-OFB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ofb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-OFB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_ofb_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  AES-GCM init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_gcm_init(IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_AES_GCM, key, keylen);
}

/**
  * @brief  AES-GCM encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param  aad: Point to AAD(additional authentication data). 
  * @param  aadlen: AAD length. 
  * @param  pResult: Point to cipher result. 
  * @param  pTag: Point to MAC(Message Authentication Code) .
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_gcm_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 	IN const u8 *aad, IN const u32 aadlen,
	OUT u8* pResult, OUT u8* pTag)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(aad) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pTag == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	_memcpy((void*)(pIE->gcm_iv), (const void*)(iv), 96/8);
	_memcpy((void*)(&(pIE->gcm_iv)[12]), (const void*)(gcm_iv_tail), 4);
	return CRYPTO_CipherEncryptAD(pIE, message, msglen, (u8*)(pIE->gcm_iv), 16, aad, aadlen, pResult, pTag);
}

/**
  * @brief  AES-GCM decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param  aad: Point to AAD(additional authentication data). 
  * @param  aadlen: AAD length. 
  * @param  pResult: Point to cipher result. 
  * @param  pTag: Point to MAC(Message Authentication Code) .
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_aes_gcm_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 	IN const u8 *aad, IN const u32 aadlen,
	OUT u8* pResult, OUT u8* pTag)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(aad) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pTag == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	_memcpy((void*)(pIE->gcm_iv), (const void*)(iv), 96/8);
	_memcpy((void*)(&(pIE->gcm_iv)[12]), (const void*)(gcm_iv_tail), 4);
	return CRYPTO_CipherDecryptAD(pIE, message, msglen, (u8*)(pIE->gcm_iv), 16, aad, aadlen, pResult, pTag);
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
