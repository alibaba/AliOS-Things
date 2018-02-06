/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include "mbed_crypto.h"
#include "ali_crypto.h"

ali_crypto_result ali_hash_get_ctx_size(hash_type_t type, size_t *size)
{
    if (NULL == size) {
        MBED_DBG_E("get_ctx_size: bad input!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch(type) {
        case SHA1:
        case SHA224:
        case SHA256:
        case SHA384:
        case SHA512:
        case MD5:
            break;

        default:
            MBED_DBG_E("get_ctx_size: invalid hash type(%d)\n", type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    *size = sizeof(hash_ctx_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_hash_init(hash_type_t type, void *context)
{
    hash_ctx_t *hash_ctx;

    if (NULL == context) {
        MBED_DBG_E("ali_hash_init: bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    hash_ctx = (hash_ctx_t *)context;
    if ((IS_VALID_CTX_MAGIC(hash_ctx->magic) &&
         hash_ctx->status != CRYPTO_STATUS_FINISHED) &&
         hash_ctx->status != CRYPTO_STATUS_CLEAN) {
         MBED_DBG_E("ali_hash_init: bad status(%d)\n", (int)hash_ctx->status);
         return ALI_CRYPTO_ERR_STATE;
    }

    switch(type) {
#ifdef MBEDTLS_SHA1_C
        case SHA1: {
            mbedtls_sha1_init(&hash_ctx->sha1_ctx);
            mbedtls_sha1_starts(&hash_ctx->sha1_ctx);
            break;
        }
#endif

#ifdef MBEDTLS_SHA256_C
        case SHA224: {
            mbedtls_sha256_init(&hash_ctx->sha256_ctx);
            mbedtls_sha256_starts(&hash_ctx->sha256_ctx, 1);
            break;
        }

        case SHA256: {
            mbedtls_sha256_init(&hash_ctx->sha256_ctx);
            mbedtls_sha256_starts(&hash_ctx->sha256_ctx, 0);
            break;
        }
#endif

#ifdef MBEDTLS_SHA512_C
        case SHA384: {
            mbedtls_sha512_init(&hash_ctx->sha512_ctx);
            mbedtls_sha512_starts(&hash_ctx->sha512_ctx, 1);
            break;
        }

        case SHA512: {
            mbedtls_sha512_init(&hash_ctx->sha512_ctx);
            mbedtls_sha512_starts(&hash_ctx->sha512_ctx, 0);
            break;
        }
#endif

#ifdef MBEDTLS_MD5_C
        case MD5: {
            mbedtls_md5_init(&hash_ctx->md5_ctx);
            mbedtls_md5_starts(&hash_ctx->md5_ctx);
            break;
        }
#endif

        default:
            MBED_DBG_E("ali_hash_init: invalid hash type(%d)\n", type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->type = type;
    hash_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(hash_ctx->magic);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_hash_update(const uint8_t *src, size_t size, void *context)
{
    hash_ctx_t *hash_ctx;

    if (context == NULL) {
        MBED_DBG_E("ali_hash_update: bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        MBED_DBG_E("ali_hash_update: bad args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    hash_ctx = (hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        MBED_DBG_E("ali_hash_update: bad magic!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
         MBED_DBG_E("ali_hash_update: bad status(%d)\n", (int)hash_ctx->status);
         return ALI_CRYPTO_ERR_STATE;
    }

    switch(hash_ctx->type) {
#ifdef MBEDTLS_SHA1_C
        case SHA1: {
            mbedtls_sha1_update(&hash_ctx->sha1_ctx,
                    (const unsigned char *)src, size);
            break;
        }
#endif

#ifdef MBEDTLS_SHA256_C
        case SHA224: {
            mbedtls_sha256_update(&hash_ctx->sha256_ctx,
                    (const unsigned char *)src, size);
            break;
        }

        case SHA256: {
            mbedtls_sha256_update(&hash_ctx->sha256_ctx,
                    (const unsigned char *)src, size);
            break;
        }
#endif

#ifdef MBEDTLS_SHA512_C
        case SHA384: {
            mbedtls_sha512_update(&hash_ctx->sha512_ctx,
                    (const unsigned char *)src, size);
            break;
        }

        case SHA512: {
            mbedtls_sha512_update(&hash_ctx->sha512_ctx,
                    (const unsigned char *)src, size);
            break;
        }
#endif

#ifdef MBEDTLS_MD5_C
        case MD5: {
            mbedtls_md5_update(&hash_ctx->md5_ctx,
                    (const unsigned char *)src, size);
            break;
        }
#endif

        default:
            MBED_DBG_E("ali_hash_update: invalid hash type(%d)\n", hash_ctx->type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->status = CRYPTO_STATUS_PROCESSING;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_hash_final(uint8_t *dgst, void *context)
{
    hash_ctx_t *hash_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_hash_final: invalid context!\n");
    }
    if (dgst == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_hash_final: bad input args!\n");
    }

    hash_ctx = (hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        MBED_DBG_E("ali_hash_final: bad magic!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
         MBED_DBG_E("ali_hash_final: bad status(%d)\n", (int)hash_ctx->status);
         return ALI_CRYPTO_ERR_STATE;
    }

    switch(hash_ctx->type) {
#ifdef MBEDTLS_SHA1_C
        case SHA1: {
            mbedtls_sha1_finish(&hash_ctx->sha1_ctx, (unsigned char *)dgst);
            mbedtls_sha1_free(&hash_ctx->sha1_ctx);
            break;
        }
#endif

#ifdef MBEDTLS_SHA256_C
        case SHA224: {
            mbedtls_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
            mbedtls_sha256_free(&hash_ctx->sha256_ctx);
            break;
        }

        case SHA256: {
            mbedtls_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
            mbedtls_sha256_free(&hash_ctx->sha256_ctx);
            break;
        }
#endif

#ifdef MBEDTLS_SHA512_C
        case SHA384: {
            mbedtls_sha512_finish(&hash_ctx->sha512_ctx, (unsigned char *)dgst);
            mbedtls_sha512_free(&hash_ctx->sha512_ctx);
            break;
        }

        case SHA512: {
            mbedtls_sha512_finish(&hash_ctx->sha512_ctx, (unsigned char *)dgst);
            mbedtls_sha512_free(&hash_ctx->sha512_ctx);
            break;
        }
#endif

#ifdef MBEDTLS_MD5_C
        case MD5: {
            mbedtls_md5_finish(&hash_ctx->md5_ctx, (unsigned char *)dgst);
            mbedtls_md5_free(&hash_ctx->md5_ctx);
            break;
        }
#endif

        default:
            MBED_DBG_E("ali_hash_final: invalid hash type(%d)\n", hash_ctx->type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    CLEAN_CTX_MAGIC(hash_ctx->magic);
    hash_ctx->status = CRYPTO_STATUS_FINISHED;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_hash_digest(hash_type_t type,
        const uint8_t *src, size_t size, uint8_t *dgst)
{
    hash_ctx_t hash_ctx;

    if ((src == NULL && size != 0) || dgst == NULL) {
        MBED_DBG_E("ali_hash_digest: bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch(type) {
#ifdef MBEDTLS_SHA1_C
        case SHA1: {
            mbedtls_sha1_init(&hash_ctx.sha1_ctx);
            mbedtls_sha1_starts(&hash_ctx.sha1_ctx);
            mbedtls_sha1_update(&hash_ctx.sha1_ctx,
                    (const unsigned char *)src, size);
            mbedtls_sha1_finish(&hash_ctx.sha1_ctx, (unsigned char *)dgst);
            mbedtls_sha1_free(&hash_ctx.sha1_ctx);
            break;
        }
#endif

#ifdef MBEDTLS_SHA256_C
        case SHA224: {
            mbedtls_sha256_init(&hash_ctx.sha256_ctx);
            mbedtls_sha256_starts(&hash_ctx.sha256_ctx, 1);
            mbedtls_sha256_update(&hash_ctx.sha256_ctx,
                    (const unsigned char *)src, size);
            mbedtls_sha256_finish(&hash_ctx.sha256_ctx, (unsigned char *)dgst);
            mbedtls_sha256_free(&hash_ctx.sha256_ctx);
            break;
        }

        case SHA256: {
            mbedtls_sha256_init(&hash_ctx.sha256_ctx);
            mbedtls_sha256_starts(&hash_ctx.sha256_ctx, 0);
            mbedtls_sha256_update(&hash_ctx.sha256_ctx,
                    (const unsigned char *)src, size);
            mbedtls_sha256_finish(&hash_ctx.sha256_ctx, (unsigned char *)dgst);
            mbedtls_sha256_free(&hash_ctx.sha256_ctx);
            break;
        }
#endif

#ifdef MBEDTLS_SHA512_C
        case SHA384: {
            mbedtls_sha512_init(&hash_ctx.sha512_ctx);
            mbedtls_sha512_starts(&hash_ctx.sha512_ctx, 1);
            mbedtls_sha512_update(&hash_ctx.sha512_ctx,
                    (const unsigned char *)src, size);
            mbedtls_sha512_finish(&hash_ctx.sha512_ctx, (unsigned char *)dgst);
            mbedtls_sha512_free(&hash_ctx.sha512_ctx);
            break;
        }

        case SHA512: {
            mbedtls_sha512_init(&hash_ctx.sha512_ctx);
            mbedtls_sha512_starts(&hash_ctx.sha512_ctx, 0);
            mbedtls_sha512_update(&hash_ctx.sha512_ctx,
                    (const unsigned char *)src, size);
            mbedtls_sha512_finish(&hash_ctx.sha512_ctx, (unsigned char *)dgst);
            mbedtls_sha512_free(&hash_ctx.sha512_ctx);
            break;
        }
#endif

#ifdef MBEDTLS_MD5_C
        case MD5: {
            mbedtls_md5_init(&hash_ctx.md5_ctx);
            mbedtls_md5_starts(&hash_ctx.md5_ctx);
            mbedtls_md5_update(&hash_ctx.md5_ctx,
                    (const unsigned char *)src, size);
            mbedtls_md5_finish(&hash_ctx.md5_ctx, (unsigned char *)dgst);
            mbedtls_md5_free(&hash_ctx.md5_ctx);
            break;
        }
#endif

        default:
            MBED_DBG_E("ali_hash_digest: invalid hash type(%d)\n", type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_hash_reset(void *context)
{
    hash_ctx_t *hash_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_hash_reset: invalid context!\n");
    }

    hash_ctx = (hash_ctx_t *)context;
#if 0
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_hash_reset: bad magic!");
    }
#endif

    OSA_memset(hash_ctx, 0, sizeof(hash_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_hash_copy_context(void *dst_ctx, void *src_ctx)
{
    hash_ctx_t *hash_ctx_src, *hash_ctx_dst;

    if ((src_ctx == NULL) || (dst_ctx == NULL)) {
        MBED_DBG_E("ali_hash_copy_context: bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    hash_ctx_src = (hash_ctx_t *)src_ctx;
    if (!IS_VALID_CTX_MAGIC(hash_ctx_src->magic)) {
        MBED_DBG_E("ali_hash_copy_context: bad magic!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    /* only can copy to one un-initialized context */
    hash_ctx_dst = (hash_ctx_t *)dst_ctx;
    if ((IS_VALID_CTX_MAGIC(hash_ctx_dst->magic)) &&
        ((hash_ctx_dst->status == CRYPTO_STATUS_INITIALIZED) ||
         (hash_ctx_dst->status == CRYPTO_STATUS_PROCESSING) ||
         (hash_ctx_dst->status == CRYPTO_STATUS_FINISHED))) {
         MBED_DBG_E("ali_hash_copy_context: bad status(%d)\n",
                 (int)hash_ctx_dst->status);
         return ALI_CRYPTO_ERR_STATE;
    }

    OSA_memcpy(hash_ctx_dst, hash_ctx_src, sizeof(hash_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}
