/**
 * \file hash_wrap.c
 *
 * \brief Generic message digest wrapper for mbed TLS
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_MD_C)

#include "ali_crypto.h"
#include "mbedtls/md.h"
#include "mbedtls/hash.h"
#include "mbedtls/md_internal.h"

#if defined(MBEDTLS_MD5_C)
#include "mbedtls/md5.h"
#endif

#if defined(MBEDTLS_SHA1_C)
#include "mbedtls/sha1.h"
#endif

#if defined(MBEDTLS_SHA256_C)
#include "mbedtls/sha256.h"
#endif

#if defined(MBEDTLS_SHA512_C)
#include "mbedtls/sha512.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#include <stdio.h>
#define mbedtls_printf printf
#ifdef MBEDTLS_IOT_PLAT_AOS
#include <aos/kernel.h>
#define mbedtls_malloc   aos_malloc
#define mbedtls_free     aos_free
#else
#define mbedtls_malloc malloc
#define mbedtls_free free
#endif /* MBEDTLS_IOT_PLAT_AOS */
#endif

#include <string.h>

#if defined(MBEDTLS_FS_IO)
#include <stdio.h>
#endif

extern ali_crypto_result ali_hash_init(hash_type_t type, void *context);
extern ali_crypto_result ali_hash_update(const uint8_t *src, size_t size,
                                         void *context);
extern ali_crypto_result ali_hash_final(uint8_t *dgst, void *context);

#if defined(MBEDTLS_MD5_C)
static void _md5_starts_wrap(void *ctx)
{
    ali_hash_init(MD5, ctx);
}
#endif

#if defined(MBEDTLS_SHA1_C)
static void _sha1_starts_wrap(void *ctx)
{
    ali_hash_init(SHA1, ctx);
}
#endif

#if defined(MBEDTLS_SHA256_C)
static void _sha224_starts_wrap(void *ctx)
{
    ali_hash_init(SHA224, ctx);
}

static void _sha256_starts_wrap(void *ctx)
{
    ali_hash_init(SHA256, ctx);
}
#endif

#if defined(MBEDTLS_SHA512_C)
static void _sha384_starts_wrap(void *ctx)
{
    ali_hash_init(SHA384, ctx);
}

static void _sha512_starts_wrap(void *ctx)
{
    ali_hash_init(SHA512, ctx);
}
#endif

static void _hash_update_wrap(void *ctx, const unsigned char *input,
                              size_t ilen)
{
    int ret;
    ret = ali_hash_update(input, ilen, ctx);
    if (ret != ALI_CRYPTO_SUCCESS) {
        mbedtls_printf("failed\n");
    }
}

static void _hash_finish_wrap(void *ctx, unsigned char *output)
{
    ali_hash_final(output, ctx);
}

#if defined(MBEDTLS_MD5_C)
void _mbedtls_md5(const unsigned char *input, size_t ilen,
                  unsigned char *output)
{
    ali_hash_digest(MD5, input, ilen, output);
}
#endif

#if defined(MBEDTLS_SHA1_C)
void _mbedtls_sha1(const unsigned char *input, size_t ilen,
                   unsigned char *output)
{
    ali_hash_digest(SHA1, input, ilen, output);
}
#endif

#if defined(MBEDTLS_SHA256_C)
void _mbedtls_sha224(const unsigned char *input, size_t ilen,
                     unsigned char *output)
{
    ali_hash_digest(SHA224, input, ilen, output);
}

void _mbedtls_sha256(const unsigned char *input, size_t ilen,
                     unsigned char *output)
{
    ali_hash_digest(SHA256, input, ilen, output);
}
#endif

#if defined(MBEDTLS_SHA512_C)
void _mbedtls_sha384(const unsigned char *input, size_t ilen,
                     unsigned char *output)
{
    ali_hash_digest(SHA384, input, ilen, output);
}

void _mbedtls_sha512(const unsigned char *input, size_t ilen,
                     unsigned char *output)
{
    ali_hash_digest(SHA512, input, ilen, output);
}
#endif

#if defined(MBEDTLS_MD5_C)
static void *_md5_ctx_alloc(void)
{
    size_t  len;
    int32_t ret;
    void *  ctx;

    ret = ali_hash_get_ctx_size(MD5, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = mbedtls_malloc(len);
    if (ctx == NULL) {
        return NULL;
    }
	memset(ctx, 0, len);

    return (ctx);
}

static void _md5_ctx_free(void *ctx)
{
    mbedtls_free(ctx);
}
#endif

#if defined(MBEDTLS_SHA1_C)
static void *sha1_ctx_alloc(void)
{
    size_t  len;
    int32_t ret;
    void *  ctx;

    ret = ali_hash_get_ctx_size(SHA1, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = mbedtls_malloc(len);
    if (ctx == NULL) {
        return NULL;
    }
	memset(ctx, 0, len);

    return (ctx);
}

static void sha1_ctx_free(void *ctx)
{
    mbedtls_free(ctx);
}
#endif

#if defined(MBEDTLS_SHA256_C)
static void *_sha224_ctx_alloc(void)
{
    size_t  len;
    int32_t ret;
    void *  ctx;

    ret = ali_hash_get_ctx_size(SHA224, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = mbedtls_malloc(len);
    if (ctx == NULL) {
        return NULL;
    }
	memset(ctx, 0, len);

    return (ctx);
}

static void _sha224_ctx_free(void *ctx)
{
    mbedtls_free(ctx);
}

static void *_sha256_ctx_alloc(void)
{
    size_t  len;
    int32_t ret;
    void *  ctx;

    ret = ali_hash_get_ctx_size(SHA256, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = mbedtls_malloc(len);
    if (ctx == NULL) {
        return NULL;
    }
	memset(ctx, 0, len);

    return (ctx);
}

static void _sha256_ctx_free(void *ctx)
{
    mbedtls_free(ctx);
}
#endif

#if defined(MBEDTLS_SHA512_C)
static void *_sha384_ctx_alloc(void)
{
    size_t  len;
    int32_t ret;
    void *  ctx;

    ret = ali_hash_get_ctx_size(SHA384, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = mbedtls_malloc(len);
    if (ctx == NULL) {
        return NULL;
    }
	memset(ctx, 0, len);

    return (ctx);
}

static void _sha384_ctx_free(void *ctx)
{
    mbedtls_free(ctx);
}

static void *_sha512_ctx_alloc(void)
{
    size_t  len;
    int32_t ret;
    void *  ctx;

    ret = ali_hash_get_ctx_size(SHA512, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = mbedtls_malloc(len);
    if (ctx == NULL) {
        return NULL;
    }
	memset(ctx, 0, len);

    return (ctx);
}

static void _sha512_ctx_free(void *ctx)
{
    mbedtls_free(ctx);
}
#endif

#if defined(MBEDTLS_MD5_C)
static const mbedtls_md_info_t _mbedtls_md5_info = {
    MBEDTLS_MD_MD5,
    "MD5",
    16,
    64,
    _md5_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _mbedtls_md5,
    _md5_ctx_alloc,
    _md5_ctx_free,
    NULL,
    NULL,
};
#endif

#if defined(MBEDTLS_SHA1_C)
static const mbedtls_md_info_t _mbedtls_sha1_info = {
    MBEDTLS_MD_SHA1,
    "SHA1",
    20,
    64,
    _sha1_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _mbedtls_sha1,
    sha1_ctx_alloc,
    sha1_ctx_free,
    NULL,
    NULL,
};
#endif

#if defined(MBEDTLS_SHA256_C)
static const mbedtls_md_info_t _mbedtls_sha224_info = {
    MBEDTLS_MD_SHA224,
    "SHA224",
    28,
    64,
    _sha224_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _mbedtls_sha224,
    _sha224_ctx_alloc,
    _sha224_ctx_free,
    NULL,
    NULL,
};

static const mbedtls_md_info_t _mbedtls_sha256_info = {
    MBEDTLS_MD_SHA256,
    "SHA256",
    32,
    64,
    _sha256_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _mbedtls_sha256,
    _sha256_ctx_alloc,
    _sha256_ctx_free,
    NULL,
    NULL,
};
#endif

#if defined(MBEDTLS_SHA512_C)
static const mbedtls_md_info_t _mbedtls_sha384_info = {
    MBEDTLS_MD_SHA384,
    "SHA384",
    48,
    128,
    _sha384_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _mbedtls_sha384,
    _sha384_ctx_alloc,
    _sha384_ctx_free,
    NULL,
    NULL,
};

static const mbedtls_md_info_t _mbedtls_sha512_info = {
    MBEDTLS_MD_SHA512,
    "SHA512",
    64,
    128,
    _sha512_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _mbedtls_sha512,
    _sha512_ctx_alloc,
    _sha512_ctx_free,
    NULL,
    NULL,
};
#endif

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = v;
    while (n--) {
        *p++ = 0;
    }
}

const mbedtls_hash_info_t *mbedtls_hash_info_from_type(
  mbedtls_md_type_t md_type)
{
    switch (md_type) {
#if defined(MBEDTLS_MD5_C)
        case MBEDTLS_MD_MD5:
            return (&_mbedtls_md5_info);
#endif
#if defined(MBEDTLS_SHA1_C)
        case MBEDTLS_MD_SHA1:
            return (&_mbedtls_sha1_info);
#endif
#if defined(MBEDTLS_SHA256_C)
        case MBEDTLS_MD_SHA224:
            return (&_mbedtls_sha224_info);
        case MBEDTLS_MD_SHA256:
            return (&_mbedtls_sha256_info);
#endif
#if defined(MBEDTLS_SHA512_C)
        case MBEDTLS_MD_SHA384:
            return (&_mbedtls_sha384_info);
        case MBEDTLS_MD_SHA512:
            return (&_mbedtls_sha512_info);
#endif
        default:
            return (NULL);
    }
}

void mbedtls_hash_init(mbedtls_hash_context_t *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_hash_context_t));
}

void mbedtls_hash_free(mbedtls_hash_context_t *ctx)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return;
    }

    if (ctx->md_ctx != NULL) {
        ctx->md_info->ctx_free_func(ctx->md_ctx);
        ctx->md_ctx = NULL;
    }

    if (ctx->hmac_ctx != NULL) {
        mbedtls_zeroize(ctx->hmac_ctx, 2 * ctx->md_info->block_size);
        mbedtls_free(ctx->hmac_ctx);
        ctx->hmac_ctx = NULL;
    }

    mbedtls_zeroize(ctx, sizeof(mbedtls_hash_context_t));
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
int mbedtls_hash_init_ctx(mbedtls_hash_context_t * ctx,
                          const mbedtls_md_info_t *md_info)
{
    return mbedtls_hash_setup(ctx, md_info, 1);
}
#endif

int mbedtls_hash_setup(mbedtls_hash_context_t * ctx,
                       const mbedtls_md_info_t *md_info, int hmac)
{
    if (md_info == NULL || ctx == NULL) {
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);
    }

    if ((ctx->md_ctx = md_info->ctx_alloc_func()) == NULL) {
        return (MBEDTLS_ERR_MD_ALLOC_FAILED);
    }

    if (hmac != 0) {
        ctx->hmac_ctx = mbedtls_malloc(2 * md_info->block_size);
        if (ctx->hmac_ctx == NULL) {
            md_info->ctx_free_func(ctx->md_ctx);
            ctx->md_ctx = NULL;
            return (MBEDTLS_ERR_MD_ALLOC_FAILED);
        }
		memset(ctx->hmac_ctx, 0, 2 * md_info->block_size);
    }

    ctx->md_info = md_info;

    return (0);
}

int mbedtls_hash_starts(mbedtls_hash_context_t *ctx)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);
    }

    ctx->md_info->starts_func(ctx->md_ctx);

    return (0);
}

int mbedtls_hash_update(mbedtls_hash_context_t *ctx, const unsigned char *input,
                        size_t ilen)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);
    }

    ctx->md_info->update_func(ctx->md_ctx, input, ilen);

    return (0);
}

int mbedtls_hash_finish(mbedtls_hash_context_t *ctx, unsigned char *output)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);
    }

    ctx->md_info->finish_func(ctx->md_ctx, output);

    return (0);
}

int mbedtls_hash(const mbedtls_hash_info_t *md_info, const unsigned char *input,
                 size_t ilen, unsigned char *output)
{
    if (md_info == NULL) {
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);
    }

    md_info->digest_func(input, ilen, output);

    return (0);
}

unsigned char mbedtls_hash_get_size(const mbedtls_md_info_t *md_info)
{
    if (md_info == NULL) {
        return (0);
    }

    return md_info->size;
}

mbedtls_md_type_t mbedtls_hash_get_type(const mbedtls_md_info_t *md_info)
{
    if (md_info == NULL) {
        return (MBEDTLS_MD_NONE);
    }

    return md_info->type;
}

#endif /* MBEDTLS_MD_C */
