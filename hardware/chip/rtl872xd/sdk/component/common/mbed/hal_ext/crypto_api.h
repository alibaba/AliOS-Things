/** mbed Microcontroller Library
 ******************************************************************************
 * @file    crypto_api.h
 * @brief   This file provides mbed API for CRYPTO.
 *
 * @author
 * @version V1.0.0
 * @date     2017-12-11
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************
 */
#ifndef MBED_CRYPTO_API_H
#define MBED_CRYPTO_API_H

#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
#include "device.h"
#include "basic_types.h"

#ifdef  __cplusplus
extern "C" {
#endif


/**************[Error index]***********************************
 *
 * _ERRNO_CRYPTO_DESC_NUM_SET_OutRange 		                -2
 * _ERRNO_CRYPTO_BURST_NUM_SET_OutRange		                -3
 * _ERRNO_CRYPTO_NULL_POINTER					            -4
 * _ERRNO_CRYPTO_ENGINE_NOT_INIT				            -5
 * _ERRNO_CRYPTO_ADDR_NOT_32Byte_Aligned		            -6
 * _ERRNO_CRYPTO_KEY_OutRange					            -7
 * _ERRNO_CRYPTO_MSG_OutRange					            -8
 * _ERRNO_CRYPTO_IV_OutRange					            -9
 * _ERRNO_CRYPTO_AAD_OutRange					            -10
 * _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH			            -11
 * _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 		                -12
 * _ERRNO_CRYPTO_KEY_IV_LEN_DIFF				            -13
 * _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned	            -14
 * _ERRNO_CRYPTO_CHACHA_MSGLEN_NOT_16Byte_Aligned	        -15
 * _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned	            -16
 * _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE		                -17
 * _ERRNO_CRYPTO_CACHE_HANDLE			                    -18
 * _ERRNO_CRYPTO_CIPHER_DECRYPT_MSGLEN_NOT_8Byte_Aligned	-19
 *
 *
 **************************************************************/

/**
 * @brief Define SHA2 different type
 * used for compatible ameba1 sha2 APIs
 */
typedef enum _SHA2_TYPE_ {
		MBED_SHA2_NONE 	= 0,
		MBED_SHA2_224 	= 224/8,
		MBED_SHA2_256 	= 256/8,
		MBED_SHA2_384 	= 384/8,
		MBED_SHA2_512 	= 512/8
} SHA2_TYPE;

/** @addtogroup crypto CRYPTO
 *  @ingroup    hal
 *  @brief      crypto functions
 *  @{
 */

/// Crypto doesn't support this feature.
#define _ERRNO_CRYPTO_NOT_SUPPORT_THIS_FEATURE      -30

///@name Ameba Common
///@{

// Ameba_pro mbed api
//Crypto engine init/deinit
/**
 * @brief  Initializes the CRYPTO, including clock/function/interrupt/CRYPTO Engine registers.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_init(void);

/**
 * @brief  Deinitializes the CRYPTO, including clock/function/interrupt/CRYPTO Engine registers.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_deinit(void);

//Auth md5
/**
 * @brief  MD5 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of MD5 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_md5(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Initializes the MD5 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_md5_init(void);

/**
 * @brief  MD5 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of MD5 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_md5_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update MD5 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_md5_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get MD5 final hash result.
 * @param  pDigest: the result buffer of MD5 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_md5_final(uint8_t *pDigest);

//Auth SHA1
/**
 * @brief  SHA1 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA1 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha1(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Initializes the SHA1 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha1_init(void);

/**
 * @brief  SHA1 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA1 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha1_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update SHA1 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha1_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get SHA1 final hash result.
 * @param  pDigest: the result buffer of SHA1 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha1_final(uint8_t *pDigest);

//Auth SHA2_224
/**
 * @brief  SHA2_224 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA2_224 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_224(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Initializes the SHA2_224 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_224_init(void);

/**
 * @brief  SHA2_224 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA2_224 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_224_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update SHA2_224 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_224_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get SHA2_224 final hash result.
 * @param  pDigest: the result buffer of SHA2_224 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_224_final(uint8_t *pDigest);

//Auth SHA2_256
/**
 * @brief  SHA2_256 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA2_256 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_256(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Initializes the SHA2_256 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_256_init(void);

/**
 * @brief  SHA2_256 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA2_256 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_256_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update SHA2_256 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_256_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get SHA2_256 final hash result.
 * @param  pDigest: the result buffer of SHA2_256 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_sha2_256_final(uint8_t *pDigest);

//Auth HMAC_MD5
/**
 * @brief  HMAC_MD5 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @param  pDigest: the result buffer of HMAC_MD5 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_md5(const uint8_t *message, const uint32_t msglen,
                    const uint8_t *key, const uint32_t keylen, uint8_t *pDigest);

/**
 * @brief  Initializes the HMAC-MD5 function.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_md5_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  HMAC_MD5 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of HMAC_MD5 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_md5_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update HMAC_MD5 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_md5_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get HMAC_MD5 final hash result.
 * @param  pDigest: the result buffer of HMAC_MD5 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_md5_final(uint8_t *pDigest);

//Auth HMAC_SHA1
/**
 * @brief  HMAC_SHA1 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @param  pDigest: the result buffer of HMAC_SHA1 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha1(const uint8_t *message, const uint32_t msglen,
                     const uint8_t *key, const uint32_t keylen, uint8_t *pDigest);

/**
 * @brief  Initializes the HMAC_SHA1 function.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha1_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  HMAC_SHA1 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of HMAC_SHA1 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha1_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update HMAC_SHA1 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha1_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get HMAC_SHA1 final hash result.
 * @param  pDigest: the result buffer of HMAC_SHA1 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha1_final(uint8_t *pDigest);

//Auth HMAC_SHA2_224
/**
 * @brief  HMAC_SHA2_224 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @param  pDigest: the result buffer of HMAC_SHA2_224 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_224(const uint8_t *message, const uint32_t msglen,
                         const uint8_t *key, const uint32_t keylen, uint8_t *pDigest);

/**
 * @brief  Initializes the HMAC_SHA2_224 function.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_224_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  HMAC_SHA2_224 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of HMAC_SHA2_224 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_224_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update HMAC_SHA2_224 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_224_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get HMAC_SHA2_224 final hash result.
 * @param  pDigest: the result buffer of HMAC_SHA2_224 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_224_final(uint8_t *pDigest);

//Auth HMAC_SHA2_256
/**
 * @brief  HMAC_SHA2_256 message digest algorithm (hash function).
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @param  pDigest: the result buffer of HMAC_SHA2_256 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_256(const uint8_t *message, const uint32_t msglen,
                         const uint8_t *key, const uint32_t keylen, uint8_t *pDigest);

/**
 * @brief  Initializes the HMAC_SHA2_256 function.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_256_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  HMAC_SHA2_256 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of HMAC_SHA2_256 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_256_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest);

/**
 * @brief  Update HMAC_SHA2_256 with new buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_256_update(const uint8_t *message, const uint32_t msglen);

/**
 * @brief  Get HMAC_SHA2_256 final hash result.
 * @param  pDigest: the result buffer of HMAC_SHA2_256 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_hmac_sha2_256_final(uint8_t *pDigest);

// DES-CBC
/**
 * @brief  Initializes the DES-CBC function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_des_cbc_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  DES-CBC buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of DES-CBC encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_des_cbc_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  DES-CBC buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of DES-CBC decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_des_cbc_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);


// DES-ECB
/**
 * @brief  Initializes the DES-ECB function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_des_ecb_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  DES-ECB buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of DES-ECB encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_des_ecb_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  DES-ECB buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of DES-ECB decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_des_ecb_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// 3DES-CBC
/**
 * @brief  Initializes the 3DES-CBC function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_3des_cbc_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  3DES-CBC buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of 3DES-CBC encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_3des_cbc_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  3DES-CBC buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of 3DES-CBC decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_3des_cbc_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// 3DES-ECB
/**
 * @brief  Initializes the 3DES-ECB function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_3des_ecb_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  3DES-ECB buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of 3DES-ECB encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_3des_ecb_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  3DES-ECB buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of 3DES-ECB decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_3des_ecb_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// AES-CBC
/**
 * @brief  Initializes the AES-CBC function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_cbc_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  AES-CBC buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult  the result buffer of AES-CBC encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_cbc_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  AES-CBC buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-CBC decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_cbc_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// AES-ECB
/**
 * @brief  Initializes the AES-ECB function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ecb_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  AES-ECB buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-ECB encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ecb_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  AES-ECB buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-ECB decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ecb_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// AES-CTR
/**
 * @brief  Initializes the AES-CTR function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ctr_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  AES-CTR buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-CTR encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ctr_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  AES-CTR buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-CTR decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ctr_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// AES-CFB
/**
 * @brief  Initializes the AES-CFB function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_cfb_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  AES-CFB buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-CFB encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_cfb_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  AES-CFB buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-CFB decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_cfb_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// AES-OFB
/**
 * @brief  Initializes the AES-OFB function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ofb_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  AES-OFB buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-OFB encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ofb_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

/**
 * @brief  AES-OFB buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  ivlen:  length of the initial vector.
 * @param  pResult:  the result buffer of AES-OFB decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_ofb_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t ivlen,
    uint8_t *pResult);

// AES-GCM
/**
 * @brief  Initializes the AES-GCM function with a secret key.
 * @param  key: secret key buffer.
 * @param  keylen: length of the key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_gcm_init(const uint8_t *key, const uint32_t keylen);

/**
 * @brief  AES-GCM buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  aad:  additional data buffer.
 * @param  aadlen:  length of additional data.
 * @param  pResult:  the result buffer of AES-GCM encrypt function(Ciphertext).
 * @param  pTag:  buffer for holding the tag(Authentication code).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_gcm_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv,
    const uint8_t *aad, const uint32_t aadlen,
    uint8_t *pResult, uint8_t *pTag);

/**
 * @brief  AES-GCM buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  aad:  additional data buffer.
 * @param  aadlen:  length of additional data.
 * @param  pResult:  the result buffer of AES-GCM decrypt function(Plaintext).
 * @param  pTag:  buffer for holding the tag(Authentication code).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_aes_gcm_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv,
    const uint8_t *aad, const uint32_t aadlen,
    uint8_t *pResult, uint8_t *pTag);

// CHACHA20
/**
 * @brief  Initializes the Chacha20 function with a secret key.
 * @param  key: secret key buffer.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_chacha_init(const uint8_t *key);

/**
 * @brief  Chacha20 buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  count:  initial counter value.
 * @param  pResult:  the result buffer of Chacha20 encrypt function(Ciphertext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_chacha_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t count,
    uint8_t *pResult);

/**
 * @brief  Chacha20 buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  iv:  buffer holding the initial vector data.
 * @param  count:  initial counter value.
 * @param  pResult:  the result buffer of Chacha20 decrypt function(Plaintext).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_chacha_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *iv, const uint32_t count,
    uint8_t *pResult);

// POLY1305
/**
 * @brief  Poly1305 message digest algorithm.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  key:  secret key buffer.
 * @param  pDigest:  the result buffer of Poly1305 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_poly1305(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *key,
    uint8_t *pDigest);

/**
 * @brief  Initializes the Poly1305 function with a secret key.
 * @param  key:  secret key buffer.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_poly1305_init(const uint8_t *key);

/**
 * @brief  Poly1305 process buffer.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  pDigest:  the result buffer of Poly1305 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_poly1305_process(
    const uint8_t *message, const uint32_t msglen,
	uint8_t *pDigest);

// CHACHA+POLY1305
/**
 * @brief  Initializes the Chacha20_Poly1305 function with a secret key.
 * @param  key: secret key buffer.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_chacha_poly1305_init(const uint8_t *key);

/**
 * @brief  Chacha20_Poly1305 buffer encryption.
 * @param  message:  input buffer(Plaintext).
 * @param  msglen:   input buffer length.
 * @param  nonce:  numbers used once buffer.
 * @param  aad:  additional data buffer.
 * @param  aadlen:  length of additional data.
 * @param  pResult:  the result buffer of Chacha20_Poly1305 encrypt function(Ciphertext).
 * @param  pTag:  buffer for holding the tag(Authentication code).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_chacha_poly1305_encrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *nonce,
    const uint8_t *aad, const uint32_t aadlen,
    uint8_t *pResult, uint8_t *pTag);

/**
 * @brief  Chacha20_Poly1305 buffer decryption.
 * @param  message:  input buffer(Ciphertext).
 * @param  msglen:   input buffer length.
 * @param  nonce:  numbers used once buffer.
 * @param  aad:  additional data buffer.
 * @param  aadlen:  length of additional data.
 * @param  pResult:  the result buffer of Chacha20_Poly1305 decrypt function(Plaintext).
 * @param  pTag:  buffer for holding the tag(Authentication code).
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_chacha_poly1305_decrypt(
    const uint8_t *message, const uint32_t msglen,
    const uint8_t *nonce,
    const uint8_t *aad, const uint32_t aadlen,
    uint8_t *pResult, uint8_t *pTag);

// crc
/**
 * @brief  Calculate CRC32 value using command mode.
 * @param  message:  input buffer.
 * @param  msglen:   input buffer length.
 * @param  pResult:  the result value of CRC32.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_crc32_cmd(const uint8_t *message, const uint32_t msglen, uint32_t *pCrc);

/**
 * @brief  Calculate CRC32 value using DMA mode.
 * @param  message:  input buffer.
 * @param  msglen:   input buffer length.
 * @param  pResult:  the result value of CRC32.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_crc32_dma(const uint8_t *message, const uint32_t msglen, uint32_t *pCrc);

/**
 *  @brief  Set the CRC basic parameters
 *  @param  order:  CRC polynomial order
 *  @param  polynom:  CRC polynomial coefficients
 *  @param  crcinit:  CRC initial value
 *  @param  crcxor:  CRC XOR output value
 *  @param  refin:  CRC input swap value
 *  @param  refout:  CRC output swap value
 *  @retval 0: SUCCESS
 *  @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_crc_setting(int order, unsigned long polynom, unsigned long crcinit,
                       unsigned long crcxor, int refin, int refout);

/**
 * @brief  Calculate CRC value using command mode.
 * @param  message:  input buffer.
 * @param  msglen:   input buffer length.
 * @param  pResult:  the result value of CRC.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_crc_cmd(const uint8_t *message, const uint32_t msglen, uint32_t *pCrc);

/**
 * @brief  Calculate CRC value using DMA mode.
 * @param  message:  input buffer.
 * @param  msglen:   input buffer length.
 * @param  pResult:  the result value of CRC.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
int crypto_crc_dma(const uint8_t *message, const uint32_t msglen, uint32_t *pCrc);

///@}

/*\@}*/
//----------------------------------------------------------------------------------//
// Compatible with Ameba1 hal apis

// Redefine function for Ameba1 crypto hal APIs
/// Redefine Ameba1 crypto initialize function.
#define rtl_cryptoEngine_init               crypto_init

/// Redefine Ameba1 crypto md5 hash function.
#define rtl_crypto_md5                      crypto_md5
/// Redefine Ameba1 crypto md5 hash initialize function.
#define rtl_crypto_md5_init                 crypto_md5_init
/// Redefine Ameba1 crypto md5 hash process function.
#define rtl_crypto_md5_process              crypto_md5_process
/// Redefine Ameba1 crypto sha1 hash function.
#define rtl_crypto_sha1                     crypto_sha1
/// Redefine Ameba1 crypto sha1 hash initialize function.
#define rtl_crypto_sha1_init                crypto_sha1_init
/// Redefine Ameba1 crypto sha1 hash process function.
#define rtl_crypto_sha1_process             crypto_sha1_process

/// Redefine Ameba1 crypto hmac_md5 hash function.
#define rtl_crypto_hmac_md5                 crypto_hmac_md5
/// Redefine Ameba1 crypto hmac_md5 hash initialize function.
#define rtl_crypto_hmac_md5_init            crypto_hmac_md5_init
/// Redefine Ameba1 crypto hmac_md5 hash process function.
#define rtl_crypto_hmac_md5_process         crypto_hmac_md5_process
/// Redefine Ameba1 crypto hmac_sha1 hash function.
#define rtl_crypto_hmac_sha1                crypto_hmac_sha1
/// Redefine Ameba1 crypto hmac_sha1 hash initialize function.
#define rtl_crypto_hmac_sha1_init           crypto_hmac_sha1_init
/// Redefine Ameba1 crypto hmac_sha1 hash process function.
#define rtl_crypto_hmac_sha1_process        crypto_hmac_sha1_process

/// Redefine Ameba1 crypto des_cbc cipher initialize function.
#define rtl_crypto_des_cbc_init             crypto_des_cbc_init
/// Redefine Ameba1 crypto des_cbc cipher encrypt function.
#define rtl_crypto_des_cbc_encrypt          crypto_des_cbc_encrypt
/// Redefine Ameba1 crypto des_cbc cipher decrypt function.
#define rtl_crypto_des_cbc_decrypt          crypto_des_cbc_decrypt
/// Redefine Ameba1 crypto des_ecb cipher initialize function.
#define rtl_crypto_des_ecb_init             crypto_des_ecb_init
/// Redefine Ameba1 crypto des_ecb cipher encrypt function.
#define rtl_crypto_des_ecb_encrypt          crypto_des_ecb_encrypt
/// Redefine Ameba1 crypto des_ecb cipher decrypt function.
#define rtl_crypto_des_ecb_decrypt          crypto_des_ecb_decrypt
/// Redefine Ameba1 crypto 3des_cbc cipher initialize function.
#define rtl_crypto_3des_cbc_init            crypto_3des_cbc_init
/// Redefine Ameba1 crypto 3des_cbc cipher encrypt function.
#define rtl_crypto_3des_cbc_encrypt         crypto_3des_cbc_encrypt
/// Redefine Ameba1 crypto 3des_cbc cipher decrypt function.
#define rtl_crypto_3des_cbc_decrypt         crypto_3des_cbc_decrypt
/// Redefine Ameba1 crypto 3des_ecb cipher initialize function.
#define rtl_crypto_3des_ecb_init            crypto_3des_ecb_init
/// Redefine Ameba1 crypto 3des_ecb cipher encrypt function.
#define rtl_crypto_3des_ecb_encrypt         crypto_3des_ecb_encrypt
/// Redefine Ameba1 crypto 3des_ecb cipher decrypt function.
#define rtl_crypto_3des_ecb_decrypt         crypto_3des_ecb_decrypt

/// Redefine Ameba1 crypto aes_cbc cipher initialize function.
#define rtl_crypto_aes_cbc_init             crypto_aes_cbc_init
/// Redefine Ameba1 crypto aes_cbc cipher encrypt function.
#define rtl_crypto_aes_cbc_encrypt          crypto_aes_cbc_encrypt
/// Redefine Ameba1 crypto aes_cbc cipher decrypt function.
#define rtl_crypto_aes_cbc_decrypt          crypto_aes_cbc_decrypt
/// Redefine Ameba1 crypto aes_ecb cipher initialize function.
#define rtl_crypto_aes_ecb_init             crypto_aes_ecb_init
/// Redefine Ameba1 crypto aes_ecb cipher encrypt function.
#define rtl_crypto_aes_ecb_encrypt          crypto_aes_ecb_encrypt
/// Redefine Ameba1 crypto aes_ecb cipher decrypt function.
#define rtl_crypto_aes_ecb_decrypt          crypto_aes_ecb_decrypt
/// Redefine Ameba1 crypto aes_ctr cipher initialize function.
#define rtl_crypto_aes_ctr_init             crypto_aes_ctr_init
/// Redefine Ameba1 crypto aes_ctr cipher encrypt function.
#define rtl_crypto_aes_ctr_encrypt          crypto_aes_ctr_encrypt
/// Redefine Ameba1 crypto aes_ctr cipher decrypt function.
#define rtl_crypto_aes_ctr_decrypt          crypto_aes_ctr_decrypt

/**
 * @brief  Compatible function: Ameba1 SHA2 message digest algorithm (hash function).
 * @param  sha2type:  different sha2 type.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA2 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 * @note   Input sha2 type:
 *         - SHA2_224: (224/8)
 *         - SHA2_256: (256/8)
 */
__STATIC_INLINE int rtl_crypto_sha2(const SHA2_TYPE sha2type, const uint8_t *message,
                                    const uint32_t msglen, uint8_t *pDigest)
{
    if (sha2type == MBED_SHA2_224) {
        return crypto_sha2_224(message, msglen, pDigest);
    } else if (sha2type == MBED_SHA2_256) {
        return crypto_sha2_256(message, msglen, pDigest);
    }else {
        return _ERRNO_CRYPTO_NOT_SUPPORT_THIS_FEATURE;
    }
}

/**
 * @brief  Compatible function: Ameba1 initializes the SHA2 function.
 * @param  sha2type:  different sha2 type.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 * @note   Input sha2 type:
 *         - SHA2_224: (224/8)
 *         - SHA2_256: (256/8)
 */
__STATIC_INLINE int rtl_crypto_sha2_init(const SHA2_TYPE sha2type)
{
    if (sha2type == MBED_SHA2_224) {
        return crypto_sha2_224_init();
    } else if (sha2type == MBED_SHA2_256) {
        return crypto_sha2_256_init();
    }else {
        return _ERRNO_CRYPTO_NOT_SUPPORT_THIS_FEATURE;
    }
}

/**
 * @brief  Compatible function: Ameba1 SHA2 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of SHA2 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
__STATIC_INLINE int rtl_crypto_sha2_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest)
{
    return crypto_sha2_256_process(message, msglen, pDigest);
}

/**
 * @brief  Compatible function: Ameba1 HMAC_SHA2 message digest algorithm (hash function).
 * @param  sha2type:  different sha2 type.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @param  pDigest: the result buffer of HMAC_SHA2 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 * @note   Input sha2 type:
 *         - SHA2_224: (224/8)
 *         - SHA2_256: (256/8)
 */
__STATIC_INLINE int rtl_crypto_hmac_sha2(const SHA2_TYPE sha2type, const uint8_t *message,const uint32_t msglen,
                                         const uint8_t *key, const uint32_t keylen, uint8_t *pDigest)
{
    if (sha2type == MBED_SHA2_224) {
        return crypto_hmac_sha2_224(message, msglen, key, keylen, pDigest);
    } else if (sha2type == MBED_SHA2_256){
        return crypto_hmac_sha2_256(message, msglen, key, keylen, pDigest);
    } else {
        return _ERRNO_CRYPTO_NOT_SUPPORT_THIS_FEATURE;
    }
}

/**
 * @brief  Compatible function: Ameba1 initializes the HMAC_SHA2 function.
 * @param  sha2type:  different sha2 type.
 * @param  key: HMAC secret key buffer.
 * @param  keylen: length of the HMAC key.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 * @note   Input sha2 type:
 *         - SHA2_224: (224/8)
 *         - SHA2_256: (256/8)
 */
__STATIC_INLINE int rtl_crypto_hmac_sha2_init(const SHA2_TYPE sha2type, const uint8_t *key, const uint32_t keylen)
{
    if (sha2type == MBED_SHA2_224) {
        return crypto_hmac_sha2_224_init(key, keylen);
    } else if (sha2type == MBED_SHA2_256) {
        return crypto_hmac_sha2_256_init(key, keylen);
    } else {
        return _ERRNO_CRYPTO_NOT_SUPPORT_THIS_FEATURE;
    }
}

/**
 * @brief  Compatible function: Ameba1 HMAC_SHA2 process buffer.
 * @param  message: input buffer.
 * @param  msglen: input buffer length.
 * @param  pDigest: the result buffer of HMAC_SHA2 function.
 * @retval 0: SUCCESS
 * @retval < 0: FAIL(Refer to ERRNO)
 */
__STATIC_INLINE int rtl_crypto_hmac_sha2_process(const uint8_t *message, const uint32_t msglen, uint8_t *pDigest)
{
    return crypto_hmac_sha2_256_process(message, msglen, pDigest);
}

#ifdef  __cplusplus
}
#endif
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"
#endif
