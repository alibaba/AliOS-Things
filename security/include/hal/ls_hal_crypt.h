/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __LS_HAL_CRYPT_H__
#define __LS_HAL_CRYPT_H__

#include "ls_osa.h"
#include "demo/inc/aes.h"
#include "demo/inc/md5.h"
#include "demo/inc/sha1.h"
#include "demo/inc/sha256.h"

/* HAL Crypto Error Code */
#define HAL_CRYPT_SUCCESS                   (0x00000000)
#define HAL_CRYPT_ERROR                     (0xFFFF0000)
#define HAL_CRYPT_ERROR_BAD_PARAMETERS      (0xFFFF0001)
#define HAL_CRYPT_ERROR_SHORT_BUFFER        (0xFFFF0002)
#define HAL_CRYPT_ERROR_OUT_OF_MEMORY       (0xFFFF0003)
#define HAL_CRYPT_ERROR_NOT_SUPPORTED       (0xFFFF0004)
#define HAL_CRYPTO_INVALID_PADDING          (0xFFFF0005)
#define HAL_CRYPTO_INVALID_CONTEXT          (0xFFFF0006)
#define HAL_CRYPTO_LENGTH_ERR               (0xFFFF0007)

#define HAL_RSA_MAX_KEY_SIZE    1024
#define AES_IV_SIZE             16
#define AES_BLOCK_SIZE          16

#if CONFIG_DBG_CRYPT
#ifndef PLATFORM_ANDROID
#define HAL_CRYPTO_DBG_E(_f, ...) \
    printf("E %s %d: "_f, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HAL_CRYPTO_DBG_I(_f, ...) \
    printf("I %s %d: "_f, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else /* PLATFORM_ANDROID */
#include <android/log.h>
#define __LOG_TAG__             "ICRYPTO_LOG"

#define LOG_INF(...)            __android_log_print(        \
                                        ANDROID_LOG_INFO,   \
                                        __LOG_TAG__,        \
                                        __VA_ARGS__)

#define HAL_CRYPTO_DBG_E(_f, ...)  \
            LOG_INF("E %s %d: "_f,  __FUNCTION__, __LINE__, ##_VA_ARGS__)
#define HAL_CRYPTO_DBG_I(_f, ...)  \
            LOG_INF("I %s %d: "_f,  __FUNCTION__, __LINE__, ##_VA_ARGS__)
#endif /* PLATFORM_ANDROID */

#else /* CONFIG_DBG_CRYPT */
#define HAL_CRYPTO_DBG_E(_f, _a...)
#define HAL_CRYPTO_DBG_I(_f, _a...)
#endif  /* CONFIG_DBG_CRYPT */

#define HAL_PRINT_RET(_ret, _f, ...)        \
    do {                                \
        HAL_CRYPTO_DBG_E(_f, ##__VA_ARGS__);  \
        return _ret; \
    } while (0);

#define HAL_GO_RET(_ret, _f, ...)             \
    do {                                  \
        HAL_CRYPTO_DBG_E(_f, ##__VA_ARGS__);    \
        result = _ret; \
        goto _OUT;                        \
    } while (0);

typedef enum _hal_aes_type_t {
    HAL_AES_ECB         = 0,
    HAL_AES_CBC         = 1,
    HAL_AES_CTR         = 2,
    HAL_AES_CFB         = 3,
} hal_aes_type_t;

typedef struct {
    uint32_t             mode;
    hal_aes_type_t       type;
    uint8_t              iv[AES_IV_SIZE];
    size_t               offset;
    uint8_t              stream_block[AES_BLOCK_SIZE];
    hal_aes_context      ctx;
} hal_aes_ctx_t;

typedef struct {
    hal_md5_context context;
} hal_md5_ctx_t;

typedef struct {
    hal_sha1_context context;
} hal_sha1_ctx_t;

typedef struct {
    hal_sha256_context context;
} hal_sha256_ctx_t;

typedef enum _hal_sym_padding_t {
    HAL_SYM_NOPAD       = 0,
    HAL_SYM_PKCS5_PAD   = 1,
    HAL_SYM_ZERO_PAD    = 2,
} hal_sym_padding_t;

typedef struct _rsa_priv_key_t {
    size_t   key_bytes;
    uint8_t  n[HAL_RSA_MAX_KEY_SIZE >> 3]; 
    uint8_t  d[HAL_RSA_MAX_KEY_SIZE >> 3]; 
    uint8_t  p[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  q[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  dp[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  dq[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  qp[HAL_RSA_MAX_KEY_SIZE >> 4]; 
} hal_rsa_keypair_t;

typedef struct _hal_rsa_pubkey_t {
    size_t   key_bytes;
    uint8_t  n[HAL_RSA_MAX_KEY_SIZE >> 3]; 
    uint8_t  e[HAL_RSA_MAX_KEY_SIZE >> 3]; 
} hal_rsa_pubkey_t;


/* internal data types */
typedef struct __rsa_keypair  rsa_keypair_t;
typedef struct __rsa_pubkey   rsa_pubkey_t;


int ls_hal_aes_ecb_get_size(void);
int ls_hal_aes_ecb_init(void *ctx,
           int is_enc, const uint8_t *key, size_t size);
int ls_hal_aes_ecb_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size);

int ls_hal_aes_cbc_get_size(void);
int ls_hal_aes_cbc_init(void *ctx, int is_enc,
           const uint8_t *key, size_t key_size, uint8_t iv[16]);
int ls_hal_aes_cbc_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size);

int ls_hal_aes_ctr_get_size(void);
int ls_hal_aes_ctr_init(void *ctx, int is_enc,
           const uint8_t *key, size_t key_size, uint8_t iv[16]);
int ls_hal_aes_ctr_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size);

int ls_hal_aes_cfb_get_size(void);
int ls_hal_aes_cfb_init(void *ctx, int is_enc,
           const uint8_t *key, size_t key_size, uint8_t iv[16]);
int ls_hal_aes_cfb_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size);

int ls_hal_md5_get_size(void);
int ls_hal_md5_init(void *ctx);
int ls_hal_md5_update(void *ctx, const uint8_t *src, size_t size);
int ls_hal_md5_finish(void *ctx, uint8_t digest[16]);

int ls_hal_sha1_get_size(void);
int ls_hal_sha1_init(void *ctx);
int ls_hal_sha1_update(void *ctx, const uint8_t *src, size_t size);
int ls_hal_sha1_finish(void *ctx, uint8_t digest[20]);

int ls_hal_sha256_get_size(void);
int ls_hal_sha256_init(void *ctx);
int ls_hal_sha256_update(void *ctx, const uint8_t *src, size_t size);
int ls_hal_sha256_finish(void *ctx, uint8_t digest[32]);

#if CONFIG_HAL_RSA
int ls_hal_rsa_get_ctx_size(size_t *size);
int ls_hal_rsa_init(rsa_padding_t padding, void *context);
int ls_hal_rsa_public(const rsa_pubkey_t *pubkey,
           const uint8_t *src, uint8_t *dst, size_t size, void *context);
int ls_hal_rsa_private(const rsa_keypair_t *privkey,
           int (*f_rng)(void *, uint8_t *, size_t),
           const uint8_t *src, uint8_t *dst, size_t size, void *context);
#endif

int ls_hal_get_random(uint8_t* buf, size_t len);

#endif /*__LS_HAL_CRYPT_H__ */
