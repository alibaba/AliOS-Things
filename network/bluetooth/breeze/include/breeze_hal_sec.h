/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_SECURITY_H_
#define _AIS_SECURITY_H_

typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

#define SHA256_BLOCK_SIZE 32

typedef struct {
        BYTE data[64];
        WORD datalen;
        unsigned long long bitlen;
        WORD state[8];
} SHA256_CTX;

/**
 * @brief  SHA256 initialization AIS wrapper.
 *
 * @param[in] ctx: context
 * @param[in] dir:
 * @return: None 
 * @see None.
 * @note None.
 */

void sec_sha256_init(SHA256_CTX *ctx);

/**
 * @brief  SHA256 update AIS wrapper.
 *
 * @param[in] ctx:
 * @param[in] data:
 * @param[in] len: 
 * @return : None
 * @see None.
 * @note None.
 */

void sec_sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);

/**
 * @brief  SHA256 final AIS wrapper. 
 * @param[in] ctx:
 * @param[in] hash:
 * @return : None.
 * @see None.
 * @note None.
 */
void sec_sha256_final(SHA256_CTX *ctx, BYTE hash[]);


/**
 * @brief  Initialize the aes context, which includes key/iv info.
 *         The aes context is implementation specific.
 *
 * @param[in] key:
 * @param[in] iv:
 * @param[in] dir: AIS_AES_ENCRYPTION or AIS_AES_DECRYPTION
 * @return p_ais_aes128_t
   @verbatim None
   @endverbatim
 * @see None.
 * @note None.
 */
void *sec_aes128_init(const uint8_t *key, const uint8_t *iv);

/**
 * @brief   Destroy the aes context.
 *
 * @param[in] aes: the aex context.
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int sec_aes128_destroy(void *aes);

/**
 * @brief   Do aes-128 cbc encryption.
 *          No padding is required inside the implementation.
 *
 * @param[in] aes: AES handler
 * @param[in] src: plain data
 * @param[in] block_num: plain data number of 16 bytes size
 * @param[out] dst: cipher data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int sec_aes128_cbc_encrypt(void *aes, const void *src, size_t block_num,
                           void *dst);

/**
 * @brief   Do aes-128 cbc decryption.
 *          No padding is required inside the implementation.
 *
 * @param[in] aes: AES handler
 * @param[in] src: cipher data
 * @param[in] block_num: plain data number of 16 bytes size
 * @param[out] dst: plain data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int sec_aes128_cbc_decrypt(void *aes, const void *src, size_t block_num,
                           void *dst);

#endif
