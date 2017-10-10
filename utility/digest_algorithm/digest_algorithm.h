/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


//chf, cryptographic hash function

#ifndef __CRY_HASH_FUNC_H__
#define __CRY_HASH_FUNC_H__

#include <stdint.h>

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif
/** @defgroup group_chf group_chf
 *  @ingroup group_external_resource
 *  @{
 */

/**
 * @enum Type of digest.
 */
enum digest_type {
    DIGEST_TYPE_MD5,                /**< MD5 */
    //CHF_SHA1,
    DIGEST_TYPE_SHA256,             /**< SHA-256 */
    DIGEST_TYPE_SHA384,             /**< SHA-384 */
    DIGEST_TYPE_SHA512,             /**< SHA-512 */
};

/** @defgroup group_crypto_hash group_crypto_hash
 *  @{
 */

/** @defgroup group_crypto_hash_md5 group_crypto_hash_md5
 *  @{
 */

#define MD5_SIZE_BYTE    (16)    /*Size of MD5 result in byte*/
#define MD5_SIZE_BIT     (128)   /*Size of MD5 result in bit*/

/**
 * @brief initialize MD5.
 *
 * @param None.
 * @return @n MD5 handle.
 * @see None.
 * @note None.
 */
void *digest_md5_init(void);

/**
 * @brief Hash chunks of data with using MD5 digest type.
 *
 * @param[in] md5 @n The MD5 handle.
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note None.
 */
int digest_md5_update(void *md5, const void *data, uint32_t length);

/**
 * @brief Extract the MD5 result and place in digest.
 *
 * @param[in] md5 @n The MD5 handle.
 * @param[out] digest @n MD5 result.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_md5_final(void *md5, unsigned char *digest);

/**
 * @brief Compute MD5 once.
 *
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @param[out] digest @n MD5 result.
 *
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_md5(const void *data, uint32_t length, unsigned char *digest);


/**
 * @brief Compute the specific file's MD5 value.
 *
 * @param[in] path @n file path.
 * @param[out] digest @n MD5 result.
 *
 * @return
   @verbatim
     = 0, success.
     = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_md5_file(const char *path, unsigned char *digest);

/** @} */// end of group_crypt_hash_md5


/** @defgroup group_crypto_hash_sha256 group_crypto_hash_sha256
 *  @{
 */

#define SHA256_SIZE_BYTE    (32)    /*Size of SHA256 result in byte*/
#define SHA256_SIZE_BIT     (256)   /*Size of SHA256 result in bit*/

/**
 * @brief initialize sha256.
 *
 * @param None.
 * @return @n sha256 handle.
 * @see None.
 * @note None.
 */
void *digest_sha256_init(void);

/**
 * @brief Hash chunks of data with using sha256 digest type.
 *
 * @param[in] sha256 @n The sha256 handle.
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note None.
 */
int digest_sha256_update(void *sha256, const void *data, uint32_t length);

/**
 * @brief Extract the sha256 result and place in digest.
 *
 * @param[in] sha256 @n The sha256 handle.
 * @param[out] digest @n sha256 result.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_sha256_final(void *sha256, unsigned char *digest);

/**
 * @brief Compute sha256 once.
 *
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @param[out] digest @n sha256 result.
 *
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_sha256(const void *data, uint32_t length, unsigned char *digest);

/** @} */// end of group_crypt_hash_sha256

/** @defgroup group_crypto_hash_sha384 group_crypto_hash_sha384
 *  @{
 */

#define SHA384_SIZE_BYTE    (48)    /*Size of SHA384 result in byte*/
#define SHA384_SIZE_BIT     (384)   /*Size of SHA384 result in bit*/

/**
 * @brief initialize sha384.
 *
 * @param None.
 * @return @n sha384 handle.
 * @see None.
 * @note None.
 */
void *digest_sha384_init(void);

/**
 * @brief Hash chunks of data with using sha384 digest type.
 *
 * @param[in] sha384 @n The sha384 handle.
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note None.
 */
int digest_sha384_update(void *sha384, const void *data, uint32_t length);

/**
 * @brief Extract the sha384 result and place in digest.
 *
 * @param[in] sha384 @n The sha384 handle.
 * @param[out] digest @n sha384 result.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_sha384_final(void *sha384, unsigned char *digest);

/**
 * @brief Compute sha384 once.
 *
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @param[out] digest @n sha384 result.
 *
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_sha384(const void *data, uint32_t length, unsigned char *digest);

/** @} */// end of group_crypt_hash_sha384

/** @defgroup group_crypto_hash_sha512 group_crypto_hash_sha512
 *  @{
 */

#define SHA512_SIZE_BYTE    (64)    /*Size of SHA512 result in byte*/
#define SHA512_SIZE_BIT     (512)   /*Size of SHA512 result in bit*/


/**
 * @brief initialize sha512.
 *
 * @param None.
 * @return @n sha512 handle.
 * @see None.
 * @note None.
 */
void *digest_sha512_init(void);

/**
 * @brief Hash chunks of data with using sha512 digest type.
 *
 * @param[in] sha512 @n The sha512 handle.
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note None.
 */
int digest_sha512_update(void *sha512, const void *data, uint32_t length);

/**
 * @brief Extract the sha512 result and place in digest.
 *
 * @param[in] sha512 @n The sha512 handle.
 * @param[out] digest @n sha512 result.
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_sha512_final(void *sha512, unsigned char *digest);

/**
 * @brief Compute sha512 once.
 *
 * @param[in] data @n The data to be hashed.
 * @param[in] length @n The length of the data, in bytes.
 * @param[out] digest @n sha512 result.
 *
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_sha512(const void *data, uint32_t length, unsigned char *digest);

/** @} */// end of group_crypt_hash_sha512

/** @defgroup group_crypto_hash_hmac group_crypto_hash_hmac
 *  @{
 */

/**
 * @brief Compute hmac with specific digest type (MD5, sha256, .etc).
 *
 * @param[in] digest_type @n Specify the digest type for hmac.
 * @param[in] msg @n The data to be hashed.
 * @param[in] msg_len @n The length of the data, in bytes.
 * @param[in] key @n The hmac key.
 * @param[in] key_len @n The length of the hmac key, in bytes.
 * @param[out] digest @n hmac result.
 *
 * @return
   @verbatim
   = 0, success.
   = -1, failure.
   @endverbatim
 * @see None.
 * @note It is the caller that allocate memory space for digest.
 */
int digest_hmac(enum digest_type type,
                const unsigned char *msg, uint32_t msg_len,
                const unsigned char *key, uint32_t key_len,
                unsigned char *digest);

/** @} */// end of group_crypt_hash_hmac

/** @} */// end of group_crypt_hash

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif
#endif /* ALINK_AGENT_EXTERNAL_CHF_CHF_H_ */
