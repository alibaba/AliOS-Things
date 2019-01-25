/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __LS_HAL_CRYPT_H__
#define __LS_HAL_CRYPT_H__

#include "ls_osa.h"

/* HAL Crypto Error Code */
#define HAL_CRYPT_SUCCESS                   (0x00000000)
#define HAL_CRYPT_ERROR_GENERIC             (0xFFFF0000)
#define HAL_CRYPT_ERROR_BAD_PARAMETERS      (0xFFFF0001)
#define HAL_CRYPT_ERROR_SHORT_BUFFER        (0xFFFF0002)
#define HAL_CRYPT_ERROR_OUT_OF_MEMORY       (0xFFFF0003)
#define HAL_CRYPT_ERROR_NOT_SUPPORTED       (0xFFFF0004)

#define HAL_RSA_MAX_KEY_SIZE    1024

typedef struct _rsa_priv_key_t {
    size_t   key_bits;
    uint8_t  n[HAL_RSA_MAX_KEY_SIZE >> 3]; 
    uint8_t  d[HAL_RSA_MAX_KEY_SIZE >> 3]; 
    uint8_t  p[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  q[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  qp[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  dp[HAL_RSA_MAX_KEY_SIZE >> 4]; 
    uint8_t  dq[HAL_RSA_MAX_KEY_SIZE >> 4]; 
} rsa_priv_key_t;

typedef struct _rsa_pubkey_t {
    size_t   key_bits;
    uint8_t  n[HAL_RSA_MAX_KEY_SIZE >> 3]; 
    uint8_t  e[HAL_RSA_MAX_KEY_SIZE >> 3]; 
} rsa_pub_key_t;

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

int ls_hal_rsa_public(rsa_pub_key_t *pub_key,
           const uint8_t *src, uint8_t *dst, size_t size);
int ls_hal_rsa_private(rsa_priv_key_t *priv_key,
           int (*f_rng)(void *, uint8_t *, size_t),
           const uint8_t *src, uint8_t *dst, size_t size);

int ls_hal_get_random(uint8_t* buf, size_t len);

#endif /*__LS_HAL_CRYPT_H__ */
