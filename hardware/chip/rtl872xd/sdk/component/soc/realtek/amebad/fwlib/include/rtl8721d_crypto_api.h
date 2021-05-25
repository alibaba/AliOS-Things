/**
  ******************************************************************************
  * @file    rtl_8721d_crypto_api.h
  * @author
  * @version V1.0.0
  * @date    2017-10-26
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - Initialization
  *           - MD5
  *           - SHA1/SHA2
  *           - HMAC
  *           - AES CBC/ECB/CFB/OFB/CTR/GCM
  *           - 3DES CBC/ECB/CFB/OFB/CTR
  *           - DES CBC/ECB/CFB/OFB/CTR
  *           - Chacha20-poly1305
  *           - Sequential hash
  *           - CRC
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef __RTL8721D_CRYPTO_API_H__
#define __RTL8721D_CRYPTO_API_H__
#include "rtl8721d_crypto.h"
/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup CRYPTO
  * @brief CRYPTO driver modules
  * @{
  */

/** @addtogroup CRYPTO
  * @verbatim
  *****************************************************************************************  
  * Method 2 (use upper level API, for user use)
  *****************************************************************************************    
  *	-call rtl_cryptoEngine_init to open IPSEC function & clock
  *
  *	-call following API for set key:
  *		-rtl_crypto_xxx_init
  *
  *	-call following API for authentication/encrypt/decrypt:
  *		authentication		-rtl_crypto_xxx_process or
  *		sequential hash		-rtl_crypto_xxx_update and
  *						-rtl_crypto_xxx_final
  *
  *		encrypt			-rtl_crypto_xxx_encrypt
  *		decrypt			-rtl_crypto_xxx_decrypt
  *
  *****************************************************************************************   
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup CRYPTO_Exported_Constants CRYPTO Exported Constants
  * @{
  */ 

/** @defgroup CRYPTO_Process_Status_definitions 
  * @{
  */ 
#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange 	-2
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange	-3
#define _ERRNO_CRYPTO_NULL_POINTER				-4
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT			-5
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned	-6
#define _ERRNO_CRYPTO_KEY_OutRange				-7
#define _ERRNO_CRYPTO_MSG_OutRange				-8
#define _ERRNO_CRYPTO_IV_OutRange				-9
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH		-10
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 	-11
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF			-12
#define _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE		-13
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CRYPTO_Exported_Functions CRYPTO Exported Functions
 * @{
 *****************************************************************************/

/** @defgroup Authentication_Functions
  * @{
  */
_LONG_CALL_ int rtl_crypto_md5(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_md5_init(void);
_LONG_CALL_ int rtl_crypto_md5_process(IN const u8* message, const IN u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_md5_update(IN const u8* message, IN const u32 msglen);
_LONG_CALL_ int rtl_crypto_md5_final(OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_sha1(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_sha1_init(void);
_LONG_CALL_ int rtl_crypto_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_sha1_update(IN const u8* message, IN const u32 msglen);
_LONG_CALL_ int rtl_crypto_sha1_final(OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_sha2(IN const SHA2_TYPE sha2type, IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_sha2_init(IN const SHA2_TYPE sha2type);
_LONG_CALL_ int rtl_crypto_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_sha2_update(IN const u8* message, IN const u32 msglen);
_LONG_CALL_ int rtl_crypto_sha2_final(OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_hmac_md5(IN const u8* message, IN const u32 msglen, IN const u8* key, IN const u32 keylen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_md5_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_md5_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_md5_update(IN const u8* message, IN const u32 msglen);
_LONG_CALL_ int rtl_crypto_hmac_md5_final(OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_hmac_sha1(IN const u8* message, IN const u32 msglen, IN const u8* key, IN const u32 keylen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha1_start(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha1_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha1_update(IN const u8* message, IN const u32 msglen);
_LONG_CALL_ int rtl_crypto_hmac_sha1_final(OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_hmac_sha2(IN const SHA2_TYPE sha2type, IN const u8* message, IN const u32 msglen, IN const u8* key, IN const u32 keylen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha2_start(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha2_init(IN const SHA2_TYPE sha2type, IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha2_update(IN const u8* message, IN const u32 msglen);
_LONG_CALL_ int rtl_crypto_hmac_sha2_final(OUT u8* pDigest);
/**
  * @}
  */

/** @defgroup Cipher_Functions
  * @{
  */
_LONG_CALL_ int rtl_crypto_aes_cbc_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_cbc_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_aes_cbc_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_ecb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_aes_ecb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_aes_ctr_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_ctr_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_aes_ctr_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_aes_cfb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_cfb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_aes_cfb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_aes_ofb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_ofb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_aes_ofb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_aes_gcm_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_aes_gcm_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u8* aad, IN const u32 aadlen, OUT u8* pResult, OUT u8* pTag);
_LONG_CALL_ int rtl_crypto_aes_gcm_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u8* aad, IN const u32 aadlen, OUT u8* pResult, OUT u8* pTag);

_LONG_CALL_ int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_cbc_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_3des_cbc_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_3des_ecb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_ecb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_3des_ecb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_3des_ctr_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_ctr_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_3des_ctr_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_3des_cfb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_cfb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_3des_cfb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_3des_ofb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_ofb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_3des_ofb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_cbc_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_des_cbc_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_des_ecb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_ecb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_des_ecb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_des_ctr_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_ctr_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_des_ctr_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_des_cfb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_cfb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_des_cfb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_des_ofb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_ofb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_des_ofb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_poly1305_init(IN const u8 *key);
_LONG_CALL_ int rtl_crypto_poly1305_process(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest);
_LONG_CALL_ int rtl_crypto_poly1305(IN const u8* message, IN const u32 msglen, IN const u8* key, OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_chacha_init(IN const u8* key);
_LONG_CALL_ int rtl_crypto_chacha_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 count, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_chacha_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 count, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_chacha_poly1305_init(IN const u8* key);
_LONG_CALL_ int rtl_crypto_chacha_poly1305_encrypt(IN const u8* message, 	IN const u32 msglen, IN const u8* nonce, IN const u8* aad, IN const u32 aadlen, OUT u8* pResult, OUT u8 *pTag);
_LONG_CALL_ int rtl_crypto_chacha_poly1305_decrypt(IN const u8* message, 	IN const u32 msglen, IN const u8* nonce, IN const u8* aad, IN const u32 aadlen, OUT u8* pResult, OUT u8 *pTag);
/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */

#endif /* __RTL8721D_CRYPTO_API_H__ */

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/

