/**
  ******************************************************************************
  * @file    rtl8721dhp_chacha_poly1305.c
  * @author
  * @version V1.0.0
  * @date    2017-10-25
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - CHACHA
  *           - POLY1305
  *           - CHACHA-POLY1305
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "ameba_soc.h"
#include "hal_crypto.h"

/**
  * @brief  Poly1305 initialize.
  * @param  key: Cipher key
  * @retval Initialize status.
  */
int rtl_crypto_poly1305_init(IN const u8 *key)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_POLY1305, key, 32);
}

/**
  * @brief  Poly1305 process
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: output digest buffer 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_poly1305_process(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, NULL, 0, NULL, 0, NULL, pDigest);
}

/**
  * @brief  Poly1305 init & process
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of Poly1305 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_poly1305(IN const u8* message, IN const u32 msglen, IN const u8* key,
    OUT u8* pDigest)
{
	int ret;

	ret = rtl_crypto_poly1305_init(key);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_poly1305_process(message, msglen, pDigest);

	return ret;
}

/**
  * @brief  ChaCha initialize.
  * @param  key: Cipher key
  * @retval Initialize status.
  */
int rtl_crypto_chacha_init(IN const u8* key)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_CHACHA, key, 32);
}

/**
  * @brief  ChaCha encryption.
  * @param  message: Point to message. 
  * @param  msglen: Message length. 
  * @param  iv: Point to IV(initialize vector). 
  * @param  count: Counter value. 
  * @param  pResult: Point to cipher result. 
  * @retval Process status.
  */
int rtl_crypto_chacha_encrypt(IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 count,
    OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	u8  count_str[4];

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	//rtk_cpu_to_be32(count);
	_memcpy((void*)count_str, (const void*)(&count), sizeof(u32));
	_memcpy((void*)(&(pIE->gcm_iv)[0]), count_str, 4);
	_memcpy((void*)(&(pIE->gcm_iv)[4]), iv, 96/8);

	return CRYPTO_CipherEncryptAD(pIE, message, msglen, (u8*)(pIE->gcm_iv), 16, NULL, 0, pResult, NULL);
}

/**
  * @brief  ChaCha decryption.
  * @param  message: Point to message. 
  * @param  msglen: Message length. 
  * @param  iv: Point to IV(initialize vector). 
  * @param  count: Counter value. 
  * @param  pResult: Point to cipher result. 
  * @retval Process status.
  */
int rtl_crypto_chacha_decrypt(IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 count,
    OUT u8* pResult)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	u8  count_str[4];

	if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	//rtk_cpu_to_be32(count);
	_memcpy((void*)count_str, (const void*)(&count), sizeof(u32));
	_memcpy((void*)(&(pIE->gcm_iv)[0]), count_str, 4);
	_memcpy((void*)(&(pIE->gcm_iv)[4]), iv, 96/8);

	return CRYPTO_CipherDecryptAD(pIE, message, msglen, (u8*)(pIE->gcm_iv), 16, NULL, 0, pResult, NULL);
}

/**
  * @brief  ChaCha Poly1305 initialize.
  * @param  key: Cipher key
  * @retval Initialize status.
  */
int rtl_crypto_chacha_poly1305_init(IN const u8* key)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

	return CRYPTO_CipherInit(pIE, CIPHER_TYPE_CHACHA_POLY1305, key, 32);
}

/**
  * @brief  ChaCha Poly1305 encryption.
  * @param  message: Point to message. 
  * @param  msglen: Message length. 
  * @param  nonce: Random value. 
  * @param  aad: Point to AAD(additional authentication data). 
  * @param  aadlen: AAD length. 
  * @param  pResult: Point to cipher result. 
  * @param  pTag: Point to MAC(Message Authentication Code) .
  * @retval Process status.
  */
int rtl_crypto_chacha_poly1305_encrypt(IN const u8* message, 	IN const u32 msglen,
    IN const u8* nonce,
    IN const u8* aad, IN const u32 aadlen,
    OUT u8* pResult, OUT u8 *pTag)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	u32 count_be;
	u8  count_str[4];
	
	if ( nonce == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(nonce) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pTag == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	
	count_be = 1;
	//rtk_cpu_to_be32(count_be);
	_memcpy((void*)count_str, (const void*)(&count_be), sizeof(u32));
	_memcpy((void*)(&(pIE->gcm_iv)[0]), count_str, 4);
	_memcpy((void*)(&(pIE->gcm_iv)[4]), nonce, 96/8);
	CRYPTO_CipherEncryptAD(pIE, message, msglen, (u8*)(pIE->gcm_iv), 16, aad, aadlen, pResult, pTag);

	return 0;
}

/**
  * @brief  ChaCha Poly1305 decryption.
  * @param  message: Point to message. 
  * @param  msglen: Message length. 
  * @param  nonce: Random value. 
  * @param  aad: Point to AAD(additional authentication data). 
  * @param  aadlen: AAD length. 
  * @param  pResult: Point to cipher result. 
  * @param  pTag: Point to MAC(Message Authentication Code) .
  * @retval Process status.
  */
int rtl_crypto_chacha_poly1305_decrypt(IN const u8* message, 	IN const u32 msglen,
    IN const u8* nonce,
    IN const u8* aad, IN const u32 aadlen,
    OUT u8* pResult, OUT u8 *pTag)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	u32 count_be;
	u8  count_str[4];
	
	if ( nonce == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(nonce) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
	if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(aad) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
	if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( pTag == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	
	count_be = 1;
	//rtk_cpu_to_be32(count_be);
	_memcpy((void*)count_str, (const void*)(&count_be), sizeof(u32));
	_memcpy((void*)(&(pIE->gcm_iv)[0]), count_str, 4);
	_memcpy((void*)(&(pIE->gcm_iv)[4]), nonce, 96/8);
	CRYPTO_CipherDecryptAD(pIE, message, msglen, (u8*)(pIE->gcm_iv), 16, aad, aadlen, pResult, pTag);

	return 0;
}

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/

