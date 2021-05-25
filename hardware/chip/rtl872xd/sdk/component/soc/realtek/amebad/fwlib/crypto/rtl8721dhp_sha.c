/**
  ******************************************************************************
  * @file    rtl8721dhp_sha.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - SHA1/SHA2
  *           - HMAC SHA1/SHA2
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

unsigned char *hmac_sha1_key = NULL;
uint32_t hmac_sha1_keylen = 0;
unsigned char *hmac_sha2_key = NULL;
uint32_t hmac_sha2_keylen = 0;
/**
  * @brief  SHA1 init
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha1_init(void)
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
	
	return CRYPTO_SetSecurityModeAD(pIE, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_SHA1,
		pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);
}

/**
  * @brief  SHA1 process.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of SHA1 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha1_process(
			IN const u8* message, 	IN const u32 msglen, 
			OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return CRYPTO_ProcessAD(pIE, message, msglen, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  SHA1 update.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @note    Use in sequential hash
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha1_update(IN const u8* message, IN const u32 msglen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int ret = SUCCESS;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	assert_param(message != NULL);
	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1) 
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
  * @brief  SHA1 final process.
  * @param    pDigest	: the result of SHA1 function 
  * @note    Use in sequential hash, process the last block
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha1_final(OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	if ( pIE->hmac_seq_is_recorded == 0 ) return _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE;

	//return CRYPTO_SendSeqBuf(pDigest);
	return CRYPTO_ProcessAD(pIE, (const uint8_t *)pIE->hmac_seq_buf, pIE->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @ SHA1 init & process
  *
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of SHA1 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha1(IN const u8* message, IN const u32 msglen, OUT u8* pDigest)
{
	int ret;

	ret = rtl_crypto_sha1_init();
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_sha1_process(message, msglen, pDigest);

	return ret;
}

/**
  * @brief  SHA2 init
  * @param	sha2type	: This parameter can be one of the following values:
  *		 @arg SHA2_224
  *		 @arg SHA2_256
  *		 @arg SHA2_384
  *		 @arg SHA2_512
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha2_init(IN const SHA2_TYPE sha2type)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int auth_type;
	const u8* pCipherKey 	= NULL;
	const u32 lenCipherKey 	= 0;
	const u8* pAuthKey 		= NULL;
	const u32 lenAuthKey 	= 0;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );

	switch (sha2type) {
		case SHA2_224 :
			auth_type = AUTH_TYPE_SHA2_224_ALL;
			break;
		case SHA2_256 : 
			auth_type = AUTH_TYPE_SHA2_256_ALL;
			break;
		case SHA2_384 : 
		case SHA2_512 : 
		default: 
			return FAIL;
	}

	// for sequential hash
	pIE->hmac_seq_is_recorded = 0;
	pIE->hmac_seq_buf_is_used_bytes = 0;
	
	return CRYPTO_SetSecurityModeAD(pIE, CIPHER_TYPE_NO_CIPHER, auth_type,
		pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);
}

/**
  * @brief  SHA2 process.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of SHA2 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return CRYPTO_ProcessAD(pIE, message, msglen, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  SHA2 update.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @note    Use in sequential hash
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha2_update(IN const u8* message, IN const u32 msglen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int ret = SUCCESS;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	assert_param(message != NULL);
	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2) 
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
  * @brief  SHA2 final process.
  * @param    pDigest	: the result of SHA2 function 
  * @note    Use in sequential hash, process the last block
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha2_final(OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	if ( pIE->hmac_seq_is_recorded == 0 ) return _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE;

	//return CRYPTO_SendSeqBuf(pDigest);
	return CRYPTO_ProcessAD(pIE, (const uint8_t *)pIE->hmac_seq_buf, pIE->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  SHA2 init & process
  * @param	sha2type	: This parameter can be one of the following values:
  *		 @arg SHA2_224
  *		 @arg SHA2_256
  *		 @arg SHA2_384
  *		 @arg SHA2_512
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of SHA2 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_sha2(IN const SHA2_TYPE sha2type, IN const u8* message, IN const u32 msglen, 
	OUT u8* pDigest)
{
	int ret;

	ret = rtl_crypto_sha2_init(sha2type);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_sha2_process(message, msglen, pDigest);

	return ret;
}

/**
  * @brief  HMAC-SHA1 start, get key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha1_start(IN const u8* key, IN const u32 keylen)
{
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	for (uint32_t i = 0; i < keylen; ++i) {
		hmac_sha1_key[i] = key[i];
	}
	hmac_sha1_keylen = keylen;
	return SUCCESS;
}

/**
  * @brief  HMAC-SHA1 init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha1_init(IN const u8* key, IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	u8* pCipherKey = NULL;
	u32 lenCipherKey = 0;
	u8* pAuthKey = NULL;
	u32 lenAuthKey = 0;
	
	if (key == NULL) {
		if (hmac_sha1_key == NULL) return _ERRNO_CRYPTO_NULL_POINTER;
		memcpy(pAuthKey, hmac_sha1_key, hmac_sha1_keylen);
		lenAuthKey = hmac_sha1_keylen;
	} else {
		memcpy(pAuthKey, key, keylen);
		lenAuthKey = keylen;
	}
	if ( (u32)(pAuthKey) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
	if ( lenAuthKey > CRYPTO_AUTH_PADDING ) return _ERRNO_CRYPTO_KEY_OutRange;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );		

	// for sequential hash
	pIE->hmac_seq_is_recorded = 0;
	pIE->hmac_seq_buf_is_used_bytes = 0;

	return CRYPTO_SetSecurityModeAD(pIE, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_HMAC_SHA1,
		pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);
}

/**
  * @brief  HMAC-SHA1 process.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of HMAC-SHA1 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha1_process(
			IN const u8* message, 	IN const u32 msglen, 
			OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return CRYPTO_ProcessAD(pIE, message, msglen, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  HMAC-SHA1 update.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @note    Use in sequential hash
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha1_update(IN const u8* message, IN const u32 msglen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int ret = SUCCESS;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	assert_param(message != NULL);
	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1) 
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
  * @brief  HMAC-SHA1 final process.
  * @param    pDigest	: the result of HMAC-SHA1 function 
  * @note    Use in sequential hash, process the last block
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha1_final(OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	if ( pIE->hmac_seq_is_recorded == 0 ) return _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE;

	//return CRYPTO_SendSeqBuf(pDigest);
	return CRYPTO_ProcessAD(pIE, (const uint8_t *)pIE->hmac_seq_buf, pIE->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  HMAC-SHA1 init & process
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of HMAC-SHA1 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha1(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* key, 		IN const u32 keylen, 
	OUT u8* pDigest)
{
	int ret;

	ret = rtl_crypto_hmac_sha1_init(key, keylen);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_hmac_sha1_process(message, msglen, pDigest);

	return ret;
}

/**
  * @brief  HMAC-SHA2 start, get key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha2_start(IN const u8* key, IN const u32 keylen)
{
	if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
	for (uint32_t i = 0; i < keylen; ++i) {
		hmac_sha2_key[i] = key[i];
	}
	hmac_sha2_keylen = keylen;
	return SUCCESS;
}

/**
  * @brief  HMAC-SHA2 init, set key
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @param	sha2type	: This parameter can be one of the following values:
  *		 @arg SHA2_224
  *		 @arg SHA2_256
  *		 @arg SHA2_384
  *		 @arg SHA2_512
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha2_init(
	IN const SHA2_TYPE sha2type, 	
	IN const u8* key, 	IN const u32 keylen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int auth_type;
	u8* pCipherKey = NULL;
	u32 lenCipherKey = 0;
	u8* pAuthKey = NULL;
	u32 lenAuthKey = 0;

	if (key == NULL) {
		if (hmac_sha2_key == NULL) return _ERRNO_CRYPTO_NULL_POINTER;
		memcpy(pAuthKey, hmac_sha2_key, hmac_sha2_keylen);
		lenAuthKey = hmac_sha2_keylen;
	} else {
		memcpy(pAuthKey, key, keylen);
		lenAuthKey = keylen;
	}

	if ( (u32)(pAuthKey) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
	if ( lenAuthKey > CRYPTO_AUTH_PADDING ) return _ERRNO_CRYPTO_KEY_OutRange;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );

	switch (sha2type) {
		case SHA2_224 :
			auth_type = AUTH_TYPE_HMAC_SHA2_224_ALL;
			break;
		case SHA2_256 : 
			auth_type = AUTH_TYPE_HMAC_SHA2_256_ALL;
			break;
		case SHA2_384 : 
		case SHA2_512 : 
		default: 
			return FAIL;
	}

	// for sequential hash
	pIE->hmac_seq_is_recorded = 0;
	pIE->hmac_seq_buf_is_used_bytes = 0;

	return CRYPTO_SetSecurityModeAD(pIE, CIPHER_TYPE_NO_CIPHER, auth_type,
		pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);
}

/**
  * @brief  HMAC-SHA2 process.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of HMAC-SHA2 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha2_process(
			IN const u8* message, 	IN const u32 msglen, 
			OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	return CRYPTO_ProcessAD(pIE, message, msglen, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  HMAC-SHA2 update.
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @note    Use in sequential hash
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha2_update(IN const u8* message, IN const u32 msglen)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int ret = SUCCESS;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	assert_param(message != NULL);
	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 ) 
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
  * @brief  HMAC-SHA2 final process.
  * @param    pDigest	: the result of HMAC-SHA2 function 
  * @note    Use in sequential hash, process the last block
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha2_final(OUT u8* pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	assert_param( (pIE != NULL) && (pIE->isInit == _TRUE) );
	if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 ) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2) 
		return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

	if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

	if ( pIE->hmac_seq_is_recorded == 0 ) return _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE;

	//return CRYPTO_SendSeqBuf(pDigest);
	return CRYPTO_ProcessAD(pIE, (const uint8_t *)pIE->hmac_seq_buf, pIE->hmac_seq_buf_is_used_bytes, NULL, 0, NULL, 0, pDigest, NULL);
}

/**
  * @brief  HMAC-SHA2 init & process
  * @param	key		: need to be 4 byte alignment
  * @param	keylen	: key length
  * @param	sha2type	: This parameter can be one of the following values:
  *		 @arg SHA2_224
  *		 @arg SHA2_256
  *		 @arg SHA2_384
  *		 @arg SHA2_512
  * @param 	message : input buffer
  * @param    msglen 	: input buffer length
  * @param    pDigest	: the result of HMAC-SHA2 function 
  * @retval	0		: SUCCESS <br>
  *			others	: fail, refer to ERRNO 
  */
int rtl_crypto_hmac_sha2(
	IN const SHA2_TYPE sha2type, 
	IN const u8* message, 			IN const u32 msglen, 
	IN const u8* key, 				IN const u32 keylen,
	OUT u8* pDigest)
{
	int ret;

	ret = rtl_crypto_hmac_sha2_init(sha2type, key, keylen);
	if ( ret != SUCCESS ) return ret;

	ret = rtl_crypto_hmac_sha2_process(message, msglen, pDigest);

	return ret;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
