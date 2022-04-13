/*
 *  FIPS-180-2 compliant SHA-256 implementation
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
/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA256_C)

#include "mbedtls/sha256.h"

#include <string.h>

#if defined(MBEDTLS_SHA256_ALT)


#if defined(CONFIG_TEE_SHA256)
#include "drv/tee.h"
#endif



/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = v;

    while (n--) {
        *p++ = 0;
    }
}

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sha256_context));

    sc_sha_init(&ctx->sc_sha, 0);
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    sc_sha_uninit(&ctx->sc_sha);
    mbedtls_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst, const mbedtls_sha256_context *src)
{
    memcpy(&dst->sc_ctx, &src->sc_ctx, sizeof(sc_sha_context_t));

#if defined(CONFIG_TEE_SHA256)
    *dst = *src;
#endif
}

int mbedtls_sha256_starts_ret(mbedtls_sha256_context *ctx, int is224)
{

#if defined(CONFIG_TEE_SHA256)
    if (is224) {
        csi_tee_sha_start(TEE_SHA224, (void *)ctx);
    } else {
        csi_tee_sha_start(TEE_SHA256, (void *)ctx);
    }
#endif
    if (is224) {
        sc_sha_start(&ctx->sc_sha, &ctx->sc_ctx, SC_SHA_MODE_224);
    } else {
        sc_sha_start(&ctx->sc_sha, &ctx->sc_ctx, SC_SHA_MODE_256);
    }

    return 0;
}

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
int mbedtls_sha256_update_ret(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    if (!ilen) {
        return 0;
    }

#if defined(CONFIG_TEE_SHA256)
    csi_tee_sha_update(input, ilen, (void *)ctx);
#endif
    sc_sha_update(&ctx->sc_sha, &ctx->sc_ctx, input, ilen);

    return 0;
}

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
int mbedtls_sha256_finish_ret(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    uint32_t len;

    (void)len;
#if defined(CONFIG_TEE_SHA256)
    csi_tee_sha_finish(output, (void *)ctx);
#endif
    sc_sha_finish(&ctx->sc_sha, &ctx->sc_ctx, output, &len);

    return 0;
}

/* Internal use */
void mbedtls_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
    (void)ctx;
    (void)data;
    return;
}

int mbedtls_internal_sha256_process( mbedtls_sha256_context *ctx,
                                     const unsigned char data[64] )
{
    (void)ctx;
    (void)data;
    return 0;
}

#endif /* MBEDTLS_SHA256_ALT */

#endif /* MBEDTLS_SHA256_C */
