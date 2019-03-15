/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __IOT_IMPORT_CRYPT_H__
#define __IOT_IMPORT_CRYPT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_AES_ENCRYPTION = 0,
    HAL_AES_DECRYPTION = 1,
} AES_DIR_t;

typedef void *p_HAL_Aes128_t;

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
DLL_HAL_API p_HAL_Aes128_t HAL_Aes128_Init(
            _IN_ const uint8_t *key,
            _IN_ const uint8_t *iv,
            _IN_ AES_DIR_t dir);

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
DLL_HAL_API int HAL_Aes128_Destroy(_IN_ p_HAL_Aes128_t aes);

/**
 * @brief   以`AES-CBC-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥加密指定的明文
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
DLL_HAL_API int HAL_Aes128_Cbc_Encrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OU_ void *dst);

/**
 * @brief   以`AES-CBC-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥解密指定的密文
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
DLL_HAL_API int HAL_Aes128_Cbc_Decrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OU_ void *dst);

/**
 * @brief   以`AES-CFB-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥加密指定的明文
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

DLL_HAL_API int HAL_Aes128_Cfb_Encrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t length,
            _OU_ void *dst);

/**
 * @brief   以`AES-CFB-128`方式, 根据`HAL_Aes128_Init()`时传入的密钥解密指定的密文
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
DLL_HAL_API int HAL_Aes128_Cfb_Decrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t length,
            _OU_ void *dst);

#ifdef __cplusplus
}
#endif

#endif  /* __IOT_IMPORT_CRYPT_H__ */

