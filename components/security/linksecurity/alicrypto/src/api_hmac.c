/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "ali_crypto.h"

ali_crypto_result ali_hmac_get_ctx_size(hash_type_t type, size_t *size)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
    if (size == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (type != MD5 && type != SHA1 && type != SHA256) {
        CRYPTO_DBG_LOG("no supported type(%d)\n", type);
        return ALI_CRYPTO_NOSUPPORT;
    }

    ret = ali_hash_get_ctx_size(type, size);
    if (ret) {
        CRYPTO_DBG_LOG("get ctx size failed(0x%08x)\n", ret);
        return ret;
    }

    *size += sizeof(api_hmac_ctx_t);
    return ret;
}

ali_crypto_result ali_hmac_init(hash_type_t type, const uint8_t *key,
                                size_t keylen, void *ctx)
{
    size_t            block_size;
    uint8_t           sum[MAX_BLOCK_SIZE], ipad[MAX_BLOCK_SIZE], opad[MAX_BLOCK_SIZE];
    int               i;
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
    api_hmac_ctx_t    *context = (api_hmac_ctx_t *)ctx;

    if (context == NULL || ((key == NULL) && (keylen != 0))) {
        CRYPTO_DBG_LOG("bad input args\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (type == MD5 || type == SHA1 || type == SHA256) {
        block_size = 64;
    } else {
        CRYPTO_DBG_LOG("no supported type(%d)\n", type);
        return ALI_CRYPTO_NOSUPPORT;
    }

    if (block_size > MAX_BLOCK_SIZE) {
        CRYPTO_DBG_LOG("invalid MAX_BLOCK_SIZE\n", type);
        return ALI_CRYPTO_INVALID_ARG;
    }
    memset(sum, 0, block_size);
    if (keylen > block_size) {
        ret = ali_hash_digest(type, key, keylen, sum);
        if (ALI_CRYPTO_SUCCESS != ret) {
            CRYPTO_DBG_LOG("hash digest failed(0x%08x)\n", ret);
            return ret;
        }
    } else {
        memcpy(sum, key, keylen);
    }
    keylen = block_size;

    memset(ipad, 0x36, block_size);
    memset(opad, 0x5c, block_size);
    for (i = 0; i < keylen; i++) {
        opad[i] = (opad[i] ^ sum[i]);
        ipad[i] = (ipad[i] ^ sum[i]);
    }
    memcpy(&context->opad, opad, block_size);
    context->opad_size = block_size;

    ret = ali_hash_init(type, &context->hash_ctx);
    if( ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash init failed(0x%08x)\n", ret);
        return ret;
    }
    /* hash(K'\xor ipad) */
    ret = ali_hash_update(ipad, block_size, &context->hash_ctx);
    if( ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash update failed(0x%08x)\n", ret);
        return ret;
    }

    context->type = type;
    return ret;
}

ali_crypto_result ali_hmac_update(const uint8_t *src, size_t size, void *ctx)
{
    api_hmac_ctx_t   *context = (api_hmac_ctx_t *) ctx;
    ali_crypto_result  ret = ALI_CRYPTO_SUCCESS;

    if (context == NULL) {
        CRYPTO_DBG_LOG("bad ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        CRYPTO_DBG_LOG("bad args\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ret = ali_hash_update(src, size, &context->hash_ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash update failed\n");
        return ret;
    }
    return ret;
}

ali_crypto_result ali_hmac_final(uint8_t *dgst, void *ctx)
{
    api_hmac_ctx_t     *context = (api_hmac_ctx_t *) ctx;
    ali_crypto_result  ret = ALI_CRYPTO_SUCCESS;
    api_hash_ctx_t     *hash_ctx;
    size_t             size;

    if (context == NULL || dgst == NULL) {
        CRYPTO_DBG_LOG("bad input args\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    
    ret = ali_hash_get_ctx_size(context->type, &size);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("get ctx err(0x%08x)\n", ret);
        return ret;
    }

    hash_ctx = ls_osa_malloc(size);
    if (!hash_ctx) {
        CRYPTO_DBG_LOG("malloc %d failed\n", size);
        return ALI_CRYPTO_OUTOFMEM;
    }
    memset(hash_ctx, 0, size);

    /* hash(hash((K'\xor ipad)||m)) */
    ret = ali_hash_final(dgst, &context->hash_ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash final failed(0x%08x)\n", ret);
        goto cleanup;
    }
    size = HASH_SIZE(context->type);

    ret = ali_hash_init(context->type, &context->hash_ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash init failed(0x%08x)\n", ret);
        goto cleanup;
    }

    /* hash((K'\xor opad)) */
    ret = ali_hash_update(context->opad, context->opad_size, &context->hash_ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash update failed(0x%08x)\n", ret);
        goto cleanup;
    }
    
    ret = ali_hash_update(dgst, size, &context->hash_ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash update failed(0x%08x)\n", ret);
        goto cleanup;
    }

    ret = ali_hash_final(dgst, &context->hash_ctx);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hash final failed(0x%08x)\n", ret);
    }

cleanup:
    if (hash_ctx) {
        ls_osa_free(hash_ctx);
        hash_ctx = NULL;
    }
    return ret;
}

ali_crypto_result ali_hmac_digest(hash_type_t type, const uint8_t *key,
                                  size_t keybytes, const uint8_t *src,
                                  size_t size, uint8_t *dgst)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
    size_t            ctx_size;
    api_hmac_ctx_t    *context;

    if ((src == NULL && size != 0) ||
        key == NULL || keybytes == 0 ||
        dgst == NULL) {
        CRYPTO_DBG_LOG("bad input args\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ret = ali_hmac_get_ctx_size(type, &ctx_size);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("get ctx err(0x%08x)\n", ret);
        return ret;
    }
    
    context = ls_osa_malloc(ctx_size);
    if (!context) {
        CRYPTO_DBG_LOG("malloc %d failed\n", size);
        return ALI_CRYPTO_OUTOFMEM;
    }
    
    ret = ali_hmac_init(type, key, keybytes, context);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("init failed(0x%08x)\n", ret);
        goto _cleanup;
    }

    ret = ali_hmac_update(src, size, context);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("udpate failed(0x%08x)\n", ret);
        goto _cleanup;
    }

    ret = ali_hmac_final(dgst, context);
    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("final failed(0x%08x)\n", ret);
        goto _cleanup;
    }
    
_cleanup:
    if (context) {
        ls_osa_free(context);
    }
    return ret;
}

ali_crypto_result ali_hmac_reset(void *context)
{
    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    memset(context, 0 , sizeof(api_hmac_ctx_t));
    return 0;
}
