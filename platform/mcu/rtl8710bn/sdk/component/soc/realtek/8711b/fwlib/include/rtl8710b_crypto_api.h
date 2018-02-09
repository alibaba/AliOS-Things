/**
  ******************************************************************************
  * @file    rtl8710b_crypto_api_rom.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - Initialization
  *           - MD5
  *           - SHA1/SHA2
  *           - HMAC
  *           - AES CBC/ECB/CTR
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

#ifndef __RTL8710B_CRYPTO_API_H__
#define __RTL8710B_CRYPTO_API_H__

/** @addtogroup AmebaZ_Periph_Driver
  * @{
  */

/** @defgroup CRYPTO
  * @brief CRYPTO driver modules
  * @{
  */

/** @addtogroup CRYPTO
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *	-Authentication or Cipher use hardware to improve data process performance
  *
  *****************************************************************************************
  * Authentication
  *****************************************************************************************
  *	-MD5
  *	-SHA1
  *	-SHA2
  *	-HMAC-MD5
  *	-HMAC-SHA1
  *	-HMAC-SHA2
  *
  *****************************************************************************************
  *****************************************************************************************      
  * Cipher
  *****************************************************************************************   
  *	-AES-CBC
  *	-AES-ECB
  *	-AES-CTR
  *	-3DES-CBC
  *	-3DES-ECB
  *	-DES-CBC
  *	-DES-ECB
  *
  *****************************************************************************************    
  * How to use
  *****************************************************************************************    
  *	-call rtl_cryptoEngine_init to open IPSEC function & clock
  *
  *	-call following API for set key:
  *		-rtl_crypto_xxx_init
  *
  *	-call following API for encrypt decrypt:
  *		-rtl_crypto_xxx_process or
  *
  *		-rtl_crypto_xxx_encrypt
  *		-rtl_crypto_xxx_decrypt
  *
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
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned		-6
#define _ERRNO_CRYPTO_KEY_OutRange				-7
#define _ERRNO_CRYPTO_MSG_OutRange				-8
#define _ERRNO_CRYPTO_IV_OutRange					-9
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH		-10
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 	-11
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF			-12
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup cryptoEngine_Exported_Functions cryptoEngine Exported Functions
  * @{
  */
_LONG_CALL_ int rtl_cryptoEngine_init(void);
_LONG_CALL_ void rtl_cryptoEngine_info(void);
/**
  * @}
  */

/** @defgroup Authentication_Exported_Functions Authentication Exported Functions
  * @{
  */
_LONG_CALL_ int rtl_crypto_md5(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_md5_init(void);
_LONG_CALL_ int rtl_crypto_md5_process(IN const u8* message, const IN u32 msglen, OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_sha1(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_sha1_init(void);
_LONG_CALL_ int rtl_crypto_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_sha2(IN const SHA2_TYPE sha2type, IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_sha2_init(IN const SHA2_TYPE sha2type);
_LONG_CALL_ int rtl_crypto_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_hmac_md5(IN const u8* message, IN const u32 msglen, IN const u8* key, IN const u32 keylen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_md5_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_md5_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_hmac_sha1(IN const u8* message, IN const u32 msglen, IN const u8* key, IN const u32 keylen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha1_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

_LONG_CALL_ int rtl_crypto_hmac_sha2(IN const SHA2_TYPE sha2type, IN const u8* message, IN const u32 msglen, IN const u8* key, IN const u32 keylen, OUT u8* pDigest);
_LONG_CALL_ int rtl_crypto_hmac_sha2_init(IN const SHA2_TYPE sha2type, IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_hmac_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);
/**
  * @}
  */

/** @defgroup Cipher_Exported_Functions Cipher Exported Functions
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

_LONG_CALL_ int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_cbc_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_3des_cbc_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_3des_ecb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_3des_ecb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_3des_ecb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_cbc_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_des_cbc_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

_LONG_CALL_ int rtl_crypto_des_ecb_init(IN const u8* key, IN const u32 keylen);
_LONG_CALL_ int rtl_crypto_des_ecb_encrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
_LONG_CALL_ int rtl_crypto_des_ecb_decrypt(IN const u8* message, IN const u32 msglen, IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);
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

#endif /* __RTL8710B_CRYPTO_API_H__ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
