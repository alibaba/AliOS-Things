/*
 *  FIPS-180-1 compliant SHA-1 implementation
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
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA1_C)

#include "mbedtls/sha1.h"

#include <string.h>

#if defined(MBEDTLS_SHA1_ALT)

#if defined(CONFIG_TEE_CRYPTO)
#include "drv/tee.h"
#endif

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = (unsigned char *)v;

    while (n--) {
        *p++ = 0;
    }
}

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sha1_context));
    sc_sha_init(&ctx->sc_sha, 0);
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    sc_sha_uninit(&ctx->sc_sha);
    mbedtls_zeroize(ctx, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst, const mbedtls_sha1_context *src)
{
    *dst = *src;
}

/*
 * SHA-1 context setup
 */
void mbedtls_sha1_starts(mbedtls_sha1_context *ctx)
{
    sc_sha_start(&ctx->sc_sha, &ctx->sc_ctx, SHA_MODE_1);
}

void mbedtls_sha1_process(mbedtls_sha1_context *ctx, const unsigned char data[64])
{
    (void)ctx;
    (void)data;
    //TODO
    return;
}

/*
 * SHA-1 process buffer
 */
void mbedtls_sha1_update(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    if (!ilen) {
        return;
    }
    sc_sha_update(&ctx->sc_sha, &ctx->sc_ctx, input, ilen);
}

/*
 * SHA-1 final digest
 */
void mbedtls_sha1_finish(mbedtls_sha1_context *ctx, unsigned char output[20])
{
    uint32_t len;
    sc_sha_finish(&ctx->sc_sha, &ctx->sc_ctx, output, &len);
}

#endif /* !MBEDTLS_SHA1_ALT */
#endif /* !MBEDTLS_SHA1_C */
