/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AIS_SECURITY_H_
#define _AIS_SECURITY_H_

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
void *ais_aes128_init(const uint8_t *key, const uint8_t *iv);

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
int ais_aes128_destroy(void *aes);

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
int ais_aes128_cbc_encrypt(void *aes, const void *src, size_t block_num,
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
int ais_aes128_cbc_decrypt(void *aes, const void *src, size_t block_num,
                           void *dst);

#endif
