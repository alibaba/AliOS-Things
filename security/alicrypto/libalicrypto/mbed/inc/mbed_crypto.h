/*
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#ifndef _MBED_CRYPTO_H_
#define _MBED_CRYPTO_H_

#include "ali_crypto.h"
#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aes.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"
#include "md.h"
#include "hash.h"
#include "md5.h"
#include "rsa.h"
#include "hmac.h"

#if CONFIG_DBG_CRYPT
#define MBED_DBG_E(_f, _a ...) \
        printf("E %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#define MBED_DBG_I(_f, ...) \
        printf("I %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#define MBED_DBG_E(_f, _a...)
#define MBED_DBG_I(_f, _a...)
#endif

#define PRINT_RET(_ret, _f, ...)        \
    do {                                \
        MBED_DBG_E(_f, ##__VA_ARGS__);  \
        return (ali_crypto_result)_ret; \
    } while (0);

#define GO_RET(_ret, _f, ...)             \
    do {                                  \
        MBED_DBG_E(_f, ##__VA_ARGS__);    \
        result = (ali_crypto_result)_ret; \
        goto _OUT;                        \
    } while (0);

#define INIT_CTX_MAGIC(m) (m = 0x12345678)
#define IS_VALID_CTX_MAGIC(m) (0x12345678 == m)
#define CLEAN_CTX_MAGIC(m) (m = 0x0)

#ifdef MBEDTLS_IOT_PLAT_AOS
#include <aos/kernel.h>
#define OSA_malloc(_size) aos_malloc(_size)
#define OSA_free(_ptr) aos_free(_ptr)
#else
#define OSA_malloc(_size) malloc(_size)
#define OSA_free(_ptr) free(_ptr)
#endif

#define OSA_memcpy(_dst, _src, _size) memcpy(_dst, _src, _size)
#define OSA_memset(_src, _val, _size) memset(_src, _val, _size)
#define OSA_memcmp(_dst, _src, _size) memcmp(_dst, _src, _size)
#define OSA_strlen(_str) strlen(_str)

enum
{
    PK_PUBLIC  = 0,
    PK_PRIVATE = 1
};

typedef struct _hash_ctx_t
{
    uint32_t    magic;
    uint32_t    status;
    hash_type_t type;

    union
    {
        uint8_t                sym_ctx[1];
        mbedtls_md5_context    md5_ctx;
        mbedtls_sha1_context   sha1_ctx;
        mbedtls_sha256_context sha256_ctx;
        mbedtls_sha512_context sha512_ctx;
    };
} hash_ctx_t;

typedef struct _hmac_ctx_t
{
    uint32_t    magic;
    uint32_t    status;
    hash_type_t type;

    union
    {
        uint8_t                sym_ctx[1];
        mbedtls_hash_context_t ctx;
    };
} hmac_ctx_t;

typedef struct _cts_ctx_t
{
    uint32_t is_ecb;
} cts_ctx_t;

typedef struct _xts_ctx_t
{
    uint8_t tweak[16];
} xts_ctx_t;

typedef struct _aes_ctx_t
{
    uint32_t   magic;
    uint32_t   status;
    aes_type_t type;
    uint32_t   is_enc;
    uint8_t    iv[AES_IV_SIZE];
    size_t     offset;
    uint8_t    stream_block[AES_BLOCK_SIZE];
    union
    {
        uint8_t             sym_ctx[1];
        mbedtls_aes_context ctx;
    };
} aes_ctx_t;


typedef struct _des_ctx_t
{
    uint32_t   magic;
    uint32_t   status;
    des_type_t type;
    uint32_t   is_enc;
    union
    {
        uint8_t sym_ctx[1];
    };
} des_ctx_t;

typedef struct _ae_ctx_t
{
    uint32_t       magic;
    uint32_t       status;
    authenc_type_t type;
    uint32_t       is_enc;
    uint32_t       tag_len;
} ae_ctx_t;

ali_crypto_result mbed_crypto_init(void);
void              mbed_crypto_cleanup(void);

#endif /* _MBED_CRYPTO_H_ */
