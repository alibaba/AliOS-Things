/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/aos.h>
#include "iot_import.h"
#include "ali_crypto.h"

#define AES_BLOCK_SIZE 16

#define KEY_LEN 16 // aes 128 cbc
#define p_aes128_t p_HAL_Aes128_t
#define PLATFORM_AES_ENCRYPTION HAL_AES_ENCRYPTION
#define PLATFORM_AES_DECRYPTION HAL_AES_DECRYPTION
/**
 * @brief   初始化AES加密的结构体
 *
 * @param[in] key:
 * @param[in] iv:
 * @param[in] dir: AES_ENCRYPTION or AES_DECRYPTION
 * @return AES128_t
   @verbatim None
   @endverbatim
 * @see None.
 * @note None.
 */
p_HAL_Aes128_t HAL_Aes128_Init(_IN_ const uint8_t *key, _IN_ const uint8_t *iv,
                               _IN_ AES_DIR_t dir)
{
    ali_crypto_result result;
    void             *aes_ctx;
    size_t            aes_ctx_size, alloc_siz;
    uint8_t          *p;
    bool              is_en = true; // encrypto by default

    if (dir == PLATFORM_AES_DECRYPTION) {
        is_en = false;
    }

    result = ali_aes_get_ctx_size(AES_CBC, &aes_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "get ctx size fail(%08x)", result);
        return NULL;
    }

    alloc_siz = aes_ctx_size + KEY_LEN * 2 + sizeof(bool);
    aes_ctx   = HAL_Malloc(alloc_siz);
    if (aes_ctx == NULL) {
        LOGE("aos_awss", "kmalloc(%d) fail", (int)aes_ctx_size);
        return NULL;
    }
    memset(aes_ctx, 0, alloc_siz);

    p = (uint8_t *)aes_ctx + aes_ctx_size;
    memcpy(p, key, KEY_LEN);
    p += KEY_LEN;
    memcpy(p, iv, KEY_LEN);
    p += KEY_LEN;
    *((bool *)p) = is_en;

    return aes_ctx;
}

/**
 * @brief   销毁AES加密的结构体
 *
 * @param[in] aes:
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Destroy(_IN_ p_HAL_Aes128_t aes)
{
    if (aes) {
        HAL_Free(aes);
    }

    return 0;
}

static int platform_aes128_encrypt_decrypt(p_HAL_Aes128_t aes_ctx,
        const void *src, size_t siz,
        void *dst, aes_type_t t)
{
    ali_crypto_result result;
    size_t            dlen, in_len = siz, ctx_siz;
    uint8_t          *p, *key, *iv;
    bool              is_en;
    if (aes_ctx == NULL) {
        LOGE("aos_awss", "platform_aes128_encrypt_decrypt aes_ctx is NULL");
        return -1;
    }
    result = ali_aes_get_ctx_size(AES_CBC, &ctx_siz);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "get ctx size fail(%08x)", result);
        return 0;
    }

    p   = (uint8_t *)aes_ctx + ctx_siz;
    key = p;
    p += KEY_LEN;
    iv = p;
    p += KEY_LEN;
    is_en = *((uint8_t *)p);

    in_len <<= t == AES_CBC ? 4 : 0;
    dlen = in_len;

    result = ali_aes_init(t, is_en, key, NULL, KEY_LEN, iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "ali_aes_init fail(%08x)", result);
        return 0;
    }

    result = ali_aes_finish(src, in_len, dst, &dlen, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        LOGE("aos_awss", "aes_finish fail(%08x)", result);
        return -1;
    }

    memcpy(iv, dst + dlen - 16, 16);
    return 0;
}

/**
 * @brief   以`AES-CBC-128`方式,
 根据`HAL_Aes128_Init()`时传入的密钥加密指定的明文
 *
 * @param[in] aes: AES handler
 * @param[in] src: plain data
 * @param[in] blockNum: plain data number of 16 bytes size
 * @param[out] dst: cipher data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Cbc_Encrypt(_IN_ p_HAL_Aes128_t aes, _IN_ const void *src,
                           _IN_ size_t blockNum, _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, blockNum, dst, AES_CBC);
}

/**
 * @brief   以`AES-CBC-128`方式,
 根据`HAL_Aes128_Init()`时传入的密钥解密指定的密文
 *
 * @param[in] aes: AES handler
 * @param[in] src: cipher data
 * @param[in] blockNum: plain data number of 16 bytes size
 * @param[out] dst: plain data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Cbc_Decrypt(_IN_ p_HAL_Aes128_t aes, _IN_ const void *src,
                           _IN_ size_t blockNum, _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, blockNum, dst, AES_CBC);
}

/**
 * @brief   以`AES-CFB-128`方式,
 根据`HAL_Aes128_Init()`时传入的密钥加密指定的明文
 *
 * @param[in] aes: AES handler
 * @param[in] src: plain data
 * @param[in] length: plain data number of 16 bytes size
 * @param[out] dst: cipher data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */

int HAL_Aes128_Cfb_Encrypt(_IN_ p_HAL_Aes128_t aes, _IN_ const void *src,
                           _IN_ size_t length, _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, length, dst, AES_CFB128);
}

/**
 * @brief   以`AES-CFB-128`方式,
 根据`HAL_Aes128_Init()`时传入的密钥解密指定的密文
 *
 * @param[in] aes: AES handler
 * @param[in] src: cipher data
 * @param[in] length: plain data number of 16 bytes size
 * @param[out] dst: plain data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_Aes128_Cfb_Decrypt(_IN_ p_HAL_Aes128_t aes, _IN_ const void *src,
                           _IN_ size_t length, _OU_ void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, length, dst, AES_CFB128);
}
//#endif
