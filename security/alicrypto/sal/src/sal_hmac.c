/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "sal_crypto.h"
#include "ali_crypto.h"

#ifdef ALI_CRYPTO_HMAC
ali_crypto_result sal_hmac_get_ctx_size(hash_type_t type, size_t *size)
{
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "get_ctx_size: bad input!\n");
    }

    switch (type) {
        case SHA1:
        case SHA224:
        case SHA256:
        case SHA384:
        case SHA512:
        case MD5:
            break;

        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                      "get_ctx_size: invalid hmac type(%d)\n", type);
    }

    *size = sizeof(hmac_ctx_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hmac_init(hash_type_t type, const uint8_t *key,
                                size_t keybytes, void *context)
{
    int                        ret;
    hmac_ctx_t *               hmac_ctx;
    const ali_algo_hash_info_t *md_info;
    ali_algo_md_type_t          md_type;
    int                        zero_tmp_key;

    if (context == NULL || ((key == NULL) && (keybytes != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_hmac_init: bad input args!\n");
    }

    hmac_ctx = (hmac_ctx_t *)context;
    if ((IS_VALID_CTX_MAGIC(hmac_ctx->magic) &&
         hmac_ctx->status != CRYPTO_STATUS_FINISHED) &&
        hmac_ctx->status != CRYPTO_STATUS_CLEAN) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "ali_hmac_init: bad status(%d)\n",
                  (int)hmac_ctx->status);
    }

    ali_algo_hash_init(&hmac_ctx->ctx);

    switch (type) {
        case SHA1: {
            md_type = ALI_ALGO_MD_SHA1;
            break;
        }

        case SHA224: {
            md_type = ALI_ALGO_MD_SHA224;
            break;
        }

        case SHA256: {
            md_type = ALI_ALGO_MD_SHA256;
            break;
        }

        case SHA384: {
            md_type = ALI_ALGO_MD_SHA384;
            break;
        }

        case SHA512: {
            md_type = ALI_ALGO_MD_SHA512;
            break;
        }

        case MD5: {
            md_type = ALI_ALGO_MD_MD5;
            break;
        }

        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                      "ali_hmac_init: invalid hash type(%d)\n", type);
    }

    md_info = ali_algo_hash_info_from_type(md_type);
    if (NULL == md_info) {
        PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                  "ali_hmac_init: invalid hash type(%d)\n", md_type);
    }

    ret = ali_algo_hash_setup(&hmac_ctx->ctx, md_info, 1);
    if (0 != ret) {
        PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                  "ali_hmac_init: invalid hash type(%d)\n", md_type);
    }

    if (keybytes) {
        ret = ali_algo_hmac_starts(&hmac_ctx->ctx, key, keybytes);
    } else {
        /* feed 4 bytes zero key */
        zero_tmp_key = 0;
        ret          = ali_algo_hmac_starts(&hmac_ctx->ctx,
                                  (const unsigned char *)&zero_tmp_key,
                                  (size_t)(sizeof(zero_tmp_key)));
    }

    if (ALI_CRYPTO_SUCCESS != ret) {
        ali_algo_hash_free(&hmac_ctx->ctx);
        PRINT_RET(ALI_CRYPTO_ERROR, "ali_hmac_init: fail to init hmac!\n");
    }

    hmac_ctx->type   = type;
    hmac_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(hmac_ctx->magic);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hmac_update(const uint8_t *src, size_t size,
                                  void *context)
{
    int         ret;
    hmac_ctx_t *hmac_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_hmac_update: bad ctx!\n");
    }

    if (src == NULL && size != 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_hmac_update: bad args!\n");
    }

    hmac_ctx = (hmac_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hmac_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_hmac_update: bad magic!\n");
    }

    if ((hmac_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hmac_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "ali_hmac_update: bad status(%d)\n",
                  (int)hmac_ctx->status);
    }

    ret = ali_algo_hmac_update(&hmac_ctx->ctx, (const unsigned char *)src, size);
    if (ALI_CRYPTO_SUCCESS != ret) {
        PRINT_RET(ALI_CRYPTO_ERROR, "ali_hmac_update: hmac_process fail!\n");
    }

    hmac_ctx->status = CRYPTO_STATUS_PROCESSING;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hmac_final(uint8_t *dgst, void *context)
{
    int         ret;
    hmac_ctx_t *hmac_ctx;

    if (context == NULL || dgst == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_hmac_final: bad input args!\n");
    }

    hmac_ctx = (hmac_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hmac_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_hmac_final: bad magic!\n");
    }

    if ((hmac_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hmac_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "ali_hmac_final: bad status(%d)\n",
                  (int)hmac_ctx->status);
    }

    ret = ali_algo_hmac_finish(&hmac_ctx->ctx, dgst);
    if (ALI_CRYPTO_SUCCESS != ret) {
        PRINT_RET(ALI_CRYPTO_ERROR, "ali_hmac_final: hmac_done fail!\n");
    }

    CLEAN_CTX_MAGIC(hmac_ctx->magic);
    hmac_ctx->status = CRYPTO_STATUS_FINISHED;

    ali_algo_hash_free(&hmac_ctx->ctx);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_hmac_reset(void *context)
{
    hmac_ctx_t *hmac_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    hmac_ctx = (hmac_ctx_t *)context;

    osa_memset(hmac_ctx, 0, sizeof(hmac_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}
#endif 
