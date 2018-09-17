#include "ota_rsa_param.h"
#include "ota_hash.h"

ota_crypto_result ota_hash_get_ctx_size(ota_hash_type_t type, unsigned int *size)
{
    if (NULL == size) {
        OTA_LOG_E("get_ctx_size: bad input!\n");
        return OTA_CRYPTO_INVALID_ARG;
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
            OTA_LOG_E("get_ctx_size: invalid hash type(%d)\n", type);
            return OTA_CRYPTO_INVALID_TYPE;
    }
    *size = sizeof(ota_hash_ctx_t);
    return OTA_CRYPTO_SUCCESS;
}

ota_crypto_result ota_hash_init(ota_hash_type_t type, void *context)
{
    ota_hash_ctx_t *hash_ctx;
    if (NULL == context) {
        OTA_LOG_E("ota_hash_init: bad ctx!\n");
        return OTA_CRYPTO_INVALID_CONTEXT;
    }

    hash_ctx = (ota_hash_ctx_t *)context;
    if ((IS_VALID_CTX_MAGIC(hash_ctx->magic) &&
         hash_ctx->status != CRYPTO_STATUS_FINISHED) &&
         hash_ctx->status != CRYPTO_STATUS_CLEAN) {
         OTA_LOG_E("ota_hash_init: bad status(%d)\n", (int)hash_ctx->status);
         return OTA_CRYPTO_ERR_STATE;
    }

    switch(type) {
        case SHA256: {
            ota_sha256_init(&hash_ctx->sha256_ctx);
            ota_sha256_starts(&hash_ctx->sha256_ctx, 0);
            break;
        }
        case MD5: {
            ota_md5_init(&hash_ctx->md5_ctx);
            ota_md5_starts(&hash_ctx->md5_ctx);
            break;
        }
        default:
            OTA_LOG_E("ota_hash_init: invalid hash type(%d)\n", type);
            return OTA_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->type = type;
    hash_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(hash_ctx->magic);
    return OTA_CRYPTO_SUCCESS;
}

ota_crypto_result ota_hash_update(const unsigned char *src, unsigned int size, void *context)
{
    ota_hash_ctx_t *hash_ctx;
    if (context == NULL) {
        OTA_LOG_E("ota_hash_update: bad ctx!\n");
        return OTA_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        OTA_LOG_E("ota_hash_update: bad args!\n");
        return OTA_CRYPTO_INVALID_ARG;
    }

    hash_ctx = (ota_hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        OTA_LOG_E("ota_hash_update: bad magic!\n");
        return OTA_CRYPTO_INVALID_CONTEXT;
    }
    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
         OTA_LOG_E("ota_hash_update: bad status(%d)\n", (int)hash_ctx->status);
         return OTA_CRYPTO_ERR_STATE;
    }
    switch(hash_ctx->type) {
        case SHA256: {
            ota_sha256_update(&hash_ctx->sha256_ctx,
                    (const unsigned char *)src, size);
            break;
        }
        case MD5: {
            ota_md5_update(&hash_ctx->md5_ctx,
                    (const unsigned char *)src, size);
            break;
        }
        default:
            OTA_LOG_E("ota_hash_update: invalid hash type(%d)\n", hash_ctx->type);
            return OTA_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->status = CRYPTO_STATUS_PROCESSING;
    return OTA_CRYPTO_SUCCESS;
}

ota_crypto_result ota_hash_final(unsigned char *dgst, void *context)
{
    ota_hash_ctx_t *hash_ctx;
    if (context == NULL) {
        OTA_LOG_E("ota_hash_final: invalid context!\n");
    }
    if (dgst == NULL) {
        OTA_LOG_E("ota_hash_final: bad input args!\n");
    }
    hash_ctx = (ota_hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        OTA_LOG_E("ota_hash_final: bad magic!\n");
        return OTA_CRYPTO_INVALID_CONTEXT;
    }
    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
         OTA_LOG_E("ota_hash_final: bad status(%d)\n", (int)hash_ctx->status);
         return OTA_CRYPTO_ERR_STATE;
    }
    switch(hash_ctx->type) {
        case SHA256: {
            ota_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
            ota_sha256_free(&hash_ctx->sha256_ctx);
            break;
        }  
        case MD5: {
            ota_md5_finish(&hash_ctx->md5_ctx, (unsigned char *)dgst);
            ota_md5_free(&hash_ctx->md5_ctx);
            break;
        }
        default:
            OTA_LOG_E("ota_hash_final: invalid hash type(%d)\n", hash_ctx->type);
            return OTA_CRYPTO_INVALID_TYPE;
    }

    CLEAN_CTX_MAGIC(hash_ctx->magic);
    hash_ctx->status = CRYPTO_STATUS_FINISHED;
    return OTA_CRYPTO_SUCCESS;
}

ota_crypto_result ota_hash_digest(ota_hash_type_t type,
        const uint8_t *src, size_t size, uint8_t *dgst)
{
    ota_hash_ctx_t hash_ctx;
    if ((src == NULL && size != 0) || dgst == NULL) {
        OTA_LOG_E("ota_hash_digest: bad input args!\n");
        return OTA_CRYPTO_INVALID_ARG;
    }
    switch(type) {
        case SHA256: {
            ota_sha256_init(&hash_ctx.sha256_ctx);
            ota_sha256_starts(&hash_ctx.sha256_ctx, 0);
            ota_sha256_update(&hash_ctx.sha256_ctx,
                    (const unsigned char *)src, size);
            ota_sha256_finish(&hash_ctx.sha256_ctx, (unsigned char *)dgst);
            ota_sha256_free(&hash_ctx.sha256_ctx);
            break;
        }
        case MD5: {
            ota_md5_init(&hash_ctx.md5_ctx);
            ota_md5_starts(&hash_ctx.md5_ctx);
            ota_md5_update(&hash_ctx.md5_ctx,
                    (const unsigned char *)src, size);
            ota_md5_finish(&hash_ctx.md5_ctx, (unsigned char *)dgst);
            ota_md5_free(&hash_ctx.md5_ctx);
            break;
        }

        default:
            OTA_LOG_E("ali_hash_digest: invalid hash type(%d)\n", type);
            return OTA_CRYPTO_INVALID_TYPE;
    }
    return OTA_CRYPTO_SUCCESS;
}
