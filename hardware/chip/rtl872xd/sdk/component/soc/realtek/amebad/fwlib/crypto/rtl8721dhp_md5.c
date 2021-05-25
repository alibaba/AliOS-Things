/**
  ******************************************************************************
  * @file    rtl8721dhp_md5.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - MD5
  *           - HMAC MD5
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
#include "hal_crypto.h"

static const u8 md5_null_msg_result[16] = {
	0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
	0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E 
};

/**
  * @brief  MD5 Init.
  * @param  none
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_md5_init(void)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	const u8* pCipherKey 	= NULL;
	const u32 lenCipherKey 	= 0;
	const u8* pAuthKey 		= NULL;
	const u32 lenAuthKey 	= 0;
	
	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );

	// for sequential hash
	pIE->hmac_seq_is_recorded = 0;
	pIE->hmac_seq_buf_is_used_bytes = 0;
	
	return CRYPTO_SetSecurityModeAD(pIE, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_MD5,
		pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);
}

/**
  * @brief  MD5 process.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of MD5 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_md5_process(
			IN const u8* message, IN const u32 msglen, 
			OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( message == NULL ) {
		if ( msglen > 0 ) {
		    return _ERRNO_CRYPTO_NULL_POINTER;
		} else {
			_memcpy(pDigest, md5_null_msg_result, 16);
		}
	}

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return CRYPTO_ProcessAD(pIE, message, msglen, NULL, 0, NULL, 0, pDigest, NULL);	
}

/**
  * @brief  MD5 update.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @note    Use in sequential hash
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_md5_update(IN const u8* message, IN const u32 msglen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int ret = SUCCESS;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	assert_param(message != NULL);
	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
/*
	if ( pIE->hmac_seq_is_recorded != 0 )  {
		ret = CRYPTO_SendSeqBuf(NULL);
		if ( ret != SUCCESS ) return ret;
	}*/

	pIE->hmac_seq_last_message = (u8*)message;
	pIE->hmac_seq_last_msglen = msglen;
	pIE->hmac_seq_is_recorded = 1;
	ret = CRYPTO_SendSeqBuf(NULL);

	return ret;
}

/**
  * @brief  MD5 final process.
  * @param    pDigest	: the result of MD5 function 
  * @note    Use in sequential hash, process the last block
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_md5_final(OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	if ( pIE->hmac_seq_is_recorded == 0 ) return _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE;

	//return CRYPTO_SendSeqBuf(pDigest);
	return CRYPTO_ProcessAD(pIE, (const uint8_t *)pIE->hmac_seq_buf, pIE->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  MD5 init & process
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of MD5 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_md5(IN const u8* message, IN const u32 msglen, OUT u8* pDigest)
{
	int ret;

	ret = rtl_crypto_md5_init();
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_md5_process(message, msglen, pDigest);

	return ret;
}

/**
  * @brief  HMAC-MD5 init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_md5_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	u8* pCipherKey = NULL;
	u32 lenCipherKey = 0;

	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
	if ( keylen > CRYPTO_AUTH_PADDING ) return _ERRNO_CRYPTO_KEY_OutRange;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );

	// for sequential hash
	pIE->hmac_seq_is_recorded = 0;
	pIE->hmac_seq_buf_is_used_bytes = 0;

	return CRYPTO_SetSecurityModeAD(pIE, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_HMAC_MD5,
		pCipherKey, lenCipherKey, key, keylen);
}

/**
  * @brief  brief  HMAC-MD5 process.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of HMAC-MD5 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_md5_process(
	IN const u8* message, 	IN const u32 msglen, 
	OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return CRYPTO_ProcessAD(pIE, message, msglen, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  HMAC-MD5 update.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @note    Use in sequential hash
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_md5_update(IN const u8* message, IN const u32 msglen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int ret = SUCCESS;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	assert_param(message != NULL);
	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
/*
	if ( pIE->hmac_seq_is_recorded != 0 )  {
		ret = CRYPTO_SendSeqBuf(NULL);
		if ( ret != SUCCESS ) return ret;
	}
*/
	pIE->hmac_seq_last_message = (u8*)message;
	pIE->hmac_seq_last_msglen = msglen;
	pIE->hmac_seq_is_recorded = 1;
	ret = CRYPTO_SendSeqBuf(NULL);

	return ret;
}

/**
  * @brief  HMAC-MD5 final process.
  * @param    pDigest	: the result of HMAC-MD5 function 
  * @note    Use in sequential hash, process the last block
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_md5_final(OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	if ( pIE->hmac_seq_is_recorded == 0 ) return _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE;

	//return CRYPTO_SendSeqBuf(pDigest);
	return CRYPTO_ProcessAD(pIE, (const uint8_t *)pIE->hmac_seq_buf, pIE->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  HMAC-MD5 init & process
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of HMAC-MD5 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_md5(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* key, 		IN const u32 keylen, 
	OUT u8* pDigest)
{
	int ret;

	ret = rtl_crypto_hmac_md5_init(key, keylen);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_hmac_md5_process(message, msglen, pDigest);

	return ret;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
