/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "sal_crypto.h"
#include "ali_crypto.h"

ali_crypto_result sal_hash_get_ctx_size(hash_type_t type, size_t *size)
{
    if (NULL == size) {
        ALI_CRYPTO_DBG_E("get_ctx_size: bad input!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    switch (type) {
        case SHA1:
        case SHA224:
        case SHA256:
        case SHA384:
        case SHA512:
        case MD5:
        case SM3:
            break;

        default:
            ALI_CRYPTO_DBG_E("get_ctx_size: invalid hash type(%d)\n", type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    *size = sizeof(hash_ctx_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hash_init(hash_type_t type, void *context)
{
    hash_ctx_t *hash_ctx;

    if (NULL == context) {
        ALI_CRYPTO_DBG_E("ali_hash_init: bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    hash_ctx = (hash_ctx_t *)context;
    if ((IS_VALID_CTX_MAGIC(hash_ctx->magic) &&
         hash_ctx->status != CRYPTO_STATUS_FINISHED) &&
        hash_ctx->status != CRYPTO_STATUS_CLEAN) {
        ALI_CRYPTO_DBG_E("ali_hash_init: bad status(%d)\n", (int)hash_ctx->status);
        return ALI_CRYPTO_ERR_STATE;
    }

    switch (type) {
#ifdef ALI_ALGO_SHA1_C
        case SHA1: {
            ali_algo_sha1_init(&hash_ctx->sha1_ctx);
            ali_algo_sha1_starts(&hash_ctx->sha1_ctx);
            break;
        }
#endif

#ifdef ALI_ALGO_SHA256_C
        case SHA224: {
            ali_algo_sha256_init(&hash_ctx->sha256_ctx);
            ali_algo_sha256_starts(&hash_ctx->sha256_ctx, 1);
            break;
        }

        case SHA256: {
            ali_algo_sha256_init(&hash_ctx->sha256_ctx);
            ali_algo_sha256_starts(&hash_ctx->sha256_ctx, 0);
            break;
        }
#endif

#ifdef ALI_ALGO_MD5_C
        case MD5: {
            ali_algo_md5_init(&hash_ctx->md5_ctx);
            ali_algo_md5_starts(&hash_ctx->md5_ctx);
            break;
        }
#endif

#ifdef ALI_ALGO_SM3_C
        case SM3: {
            ali_algo_sm3_init(&hash_ctx->sm3_ctx);
            ali_algo_sm3_starts(&hash_ctx->sm3_ctx);
            break;
        }
#endif
        default:
            ALI_CRYPTO_DBG_E("ali_hash_init: invalid hash type(%d)\n", type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->type   = type;
    hash_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(hash_ctx->magic);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hash_update(const uint8_t *src, size_t size,
                                  void *context)
{
    hash_ctx_t *hash_ctx;

    if (context == NULL) {
        ALI_CRYPTO_DBG_E("ali_hash_update: bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        ALI_CRYPTO_DBG_E("ali_hash_update: bad args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    hash_ctx = (hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        ALI_CRYPTO_DBG_E("ali_hash_update: bad magic!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        ALI_CRYPTO_DBG_E("ali_hash_update: bad status(%d)\n", (int)hash_ctx->status);
        return ALI_CRYPTO_ERR_STATE;
    }

    switch (hash_ctx->type) {
#ifdef ALI_ALGO_SHA1_C
        case SHA1: {
            ali_algo_sha1_update(&hash_ctx->sha1_ctx, (const unsigned char *)src,
                                size);
            break;
        }
#endif

#ifdef ALI_ALGO_SHA256_C
        case SHA224: {
            ali_algo_sha256_update(&hash_ctx->sha256_ctx,
                                  (const unsigned char *)src, size);
            break;
        }

        case SHA256: {
            ali_algo_sha256_update(&hash_ctx->sha256_ctx,
                                  (const unsigned char *)src, size);
            break;
        }
#endif

#ifdef ALI_ALGO_MD5_C
        case MD5: {
            ali_algo_md5_update(&hash_ctx->md5_ctx, (const unsigned char *)src,
                               size);
            break;
        }
#endif

#ifdef ALI_ALGO_SM3_C
        case SM3: {
            ali_algo_sm3_update(&hash_ctx->sm3_ctx, (const unsigned char *)src,
                               size);
            break;
        }
#endif
        default:
            ALI_CRYPTO_DBG_E("ali_hash_update: invalid hash type(%d)\n",
                       hash_ctx->type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->status = CRYPTO_STATUS_PROCESSING;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hash_final(uint8_t *dgst, void *context)
{
    hash_ctx_t *hash_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT,
                  "ali_hash_final: invalid context!\n");
    }
    if (dgst == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_hash_final: bad input args!\n");
    }

    hash_ctx = (hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        ALI_CRYPTO_DBG_E("ali_hash_final: bad magic!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        ALI_CRYPTO_DBG_E("ali_hash_final: bad status(%d)\n", (int)hash_ctx->status);
        return ALI_CRYPTO_ERR_STATE;
    }

    switch (hash_ctx->type) {
#ifdef ALI_ALGO_SHA1_C
        case SHA1: {
            ali_algo_sha1_finish(&hash_ctx->sha1_ctx, (unsigned char *)dgst);
            ali_algo_sha1_free(&hash_ctx->sha1_ctx);
            break;
        }
#endif

#ifdef ALI_ALGO_SHA256_C
        case SHA224: {
            ali_algo_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
            ali_algo_sha256_free(&hash_ctx->sha256_ctx);
            break;
        }

        case SHA256: {
            ali_algo_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
            ali_algo_sha256_free(&hash_ctx->sha256_ctx);
            break;
        }
#endif

#ifdef ALI_ALGO_MD5_C
        case MD5: {
            ali_algo_md5_finish(&hash_ctx->md5_ctx, (unsigned char *)dgst);
            ali_algo_md5_free(&hash_ctx->md5_ctx);
            break;
        }
#endif

#ifdef ALI_ALGO_SM3_C
        case SM3: {
            ali_algo_sm3_finish(&hash_ctx->sm3_ctx, (unsigned char *)dgst);
            ali_algo_sm3_free(&hash_ctx->sm3_ctx);
            break;
        }
#endif
        default:
            ALI_CRYPTO_DBG_E("ali_hash_final: invalid hash type(%d)\n",
                       hash_ctx->type);
            return ALI_CRYPTO_INVALID_TYPE;
    }

    CLEAN_CTX_MAGIC(hash_ctx->magic);
    hash_ctx->status = CRYPTO_STATUS_FINISHED;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hash_reset(void *context)
{
    hash_ctx_t *hash_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT,
                  "ali_hash_reset: invalid context!\n");
    }

    hash_ctx = (hash_ctx_t *)context;

    osa_memset(hash_ctx, 0, sizeof(hash_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}

