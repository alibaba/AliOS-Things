/**
  ******************************************************************************
  * @file    rtl8721dhp_des_rom.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - 3DES CBC/ECB
  *           - DES CBC/ECB
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

/**
  * @brief  3DES-CBC init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_3DES_CBC, key, keylen);
}

/**
  * @brief  3DES-CBC encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  3DES-CBC decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  3DES-ECB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ecb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_3DES_ECB, key, keylen);
}

/**
  * @brief  3DES-ECB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ecb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  3DES-ECB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ecb_decrypt(
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
  * @brief  3DES-CFB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_cfb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_3DES_CFB, key, keylen);
}

/**
  * @brief  3DES-CFB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_cfb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  3DES-CFB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_cfb_decrypt(
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
  * @brief  3DES-OFB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ofb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_3DES_OFB, key, keylen);
}

/**
  * @brief  3DES-OFB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ofb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  3DES-OFB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ofb_decrypt(
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
  * @brief  3DES-CTR init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ctr_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_3DES_CTR, key, keylen);
}

/**
  * @brief  3DES-CTR encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ctr_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  3DES-CTR decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_3des_ctr_decrypt(
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
  * @brief  DES-CBC init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_DES_CBC, key, keylen);
}

/**
  * @brief  DES-CBC encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-CBC decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
	if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-ECB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ecb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_DES_ECB, key, keylen);
}

/**
  * @brief  DES-ECB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ecb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-ECB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ecb_decrypt(
	IN const u8* message, 		IN const u32 msglen, 
	IN const u8* iv, 			IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-CFB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_cfb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_DES_CFB, key, keylen);
}

/**
  * @brief  DES-CFB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_cfb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-CFB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_cfb_decrypt(
	IN const u8* message, 		IN const u32 msglen, 
	IN const u8* iv, 			IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-OFB init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ofb_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_DES_OFB, key, keylen);
}

/**
  * @brief  DES-OFB encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ofb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-OFB decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ofb_decrypt(
	IN const u8* message, 		IN const u32 msglen, 
	IN const u8* iv, 			IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-CTR init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length <= 32
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ctr_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_DES_CTR, key, keylen);
}

/**
  * @brief  DES-CTR encrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ctr_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}

/**
  * @brief  DES-CTR decrypt
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pResult	: output result buffer 
  * @param    iv		: IV 
  * @param    ivlen		: IV length 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
__weak HAL_ROM_TEXT_SECTION
int rtl_crypto_des_ctr_decrypt(
	IN const u8* message, 		IN const u32 msglen, 
	IN const u8* iv, 			IN const u32 ivlen, 
	OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
