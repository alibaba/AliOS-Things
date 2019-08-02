/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "ali_crypto.h"
#include "ls_hal_crypt.h"

// hash crypto api get ctx size according to hash mode
ali_crypto_result ali_sha1_get_ctx_size(size_t *size)
{
    if (size == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    *size = sizeof(api_hash_ctx_t) + ls_hal_sha1_get_size();
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_sha256_get_ctx_size(size_t *size)
{
    if (size == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    *size = sizeof(api_hash_ctx_t) + ls_hal_sha256_get_size();
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_md5_get_ctx_size(size_t *size)
{
    if (size == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    *size = sizeof(api_hash_ctx_t) + ls_hal_md5_get_size();
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_hash_get_ctx_size(hash_type_t type, size_t *size)
{

    ali_crypto_result result;

    if (size == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch(type) {
        case SHA1:
            result = ali_sha1_get_ctx_size(size);
            break;
        case SHA256:
            result = ali_sha256_get_ctx_size(size);
            break;
        case MD5:
            result = ali_md5_get_ctx_size(size);
            break;
        default:
            CRYPTO_DBG_LOG("invalid type(%d)\n", type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    return result;
}

// hash crypto api hash init according to hash mode
ali_crypto_result ali_sha1_init(void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;

    if (NULL == context) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ctx->type = SHA1;
    ctx->hal_size = ls_hal_sha1_get_size();
    ret = ls_hal_sha1_init(ctx->hal_ctx);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_sha256_init(void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;

    if (NULL == context) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    
    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ctx->type = SHA256;
    ctx->hal_size = ls_hal_sha256_get_size();
    ret = ls_hal_sha256_init(ctx->hal_ctx);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_md5_init(void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;

    if (NULL == context) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ctx->type = MD5;
    ctx->hal_size = ls_hal_md5_get_size();
    ret = ls_hal_md5_init(ctx->hal_ctx);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_hash_init(hash_type_t type, void *context)
{
    ali_crypto_result result;

    if (NULL == context) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    switch(type) {
        case SHA1:
            result = ali_sha1_init(context);
            break;
        case SHA256:
            result = ali_sha256_init(context);
            break;
        case MD5:
            result = ali_md5_init(context);
            break;
        default:
            CRYPTO_DBG_LOG("not support this type(%d)\n", type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    return result;
}

// hash crypto api hash update according to hash mode
ali_crypto_result ali_sha1_update(const uint8_t *src, size_t size, void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;
    
    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ret = ls_hal_sha1_update(ctx->hal_ctx, src, size);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_sha256_update(const uint8_t *src, size_t size, void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ret = ls_hal_sha256_update(ctx->hal_ctx, src, size);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_md5_update(const uint8_t *src, size_t size, void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;
    
    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ret = ls_hal_md5_update(ctx->hal_ctx, src, size);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_hash_update(const uint8_t *src, size_t size, void *context)
{
    ali_crypto_result  result;
    api_hash_ctx_t *ctx;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    switch(ctx->type) {
        case SHA1:
            result = ali_sha1_update(src, size, ctx);
            break;
        case SHA256:
            result = ali_sha256_update(src, size, ctx);
            break;
        case MD5:
            result = ali_md5_update(src, size, ctx);
            break;
        default:
            CRYPTO_DBG_LOG("invalid type(%d)\n", ctx->type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    return result;
}

// hash crypto api hash final according to hash mode
ali_crypto_result ali_sha1_final(uint8_t *dgst, void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    if (dgst == NULL) {
        CRYPTO_DBG_LOG("bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ret = ls_hal_sha1_finish(ctx->hal_ctx, dgst);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_sha256_final(uint8_t *dgst, void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    if (dgst == NULL) {
        CRYPTO_DBG_LOG("bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ret = ls_hal_sha256_finish(ctx->hal_ctx, dgst);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_md5_final(uint8_t *dgst, void *context)
{
    ls_hal_crypt_result ret;
    api_hash_ctx_t *ctx;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    if (dgst == NULL) {
        CRYPTO_DBG_LOG("bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ret = ls_hal_md5_finish(ctx->hal_ctx, dgst);

    return ali_crypt_get_errcode(ret);
}

ali_crypto_result ali_hash_final(uint8_t *dgst, void *context)
{
    ali_crypto_result  result;
    api_hash_ctx_t *ctx;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    if (dgst == NULL) {
        CRYPTO_DBG_LOG("bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_hash_ctx_t *) context;
    switch(ctx->type) {
        case SHA1:
            result = ali_sha1_final(dgst, ctx);
            break;
        case SHA256:
            result = ali_sha256_final(dgst, ctx);
            break;
        case MD5:
            result = ali_md5_final(dgst, ctx);
            break;
        default:
            CRYPTO_DBG_LOG("not support this type(%d)\n", ctx->type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    return result;
}

// hash crypto api hash digest according to hash mode
ali_crypto_result ali_sha1_digest(const uint8_t *src,
                                  size_t size, uint8_t *dgst)
{
    ali_crypto_result result;
    api_hash_ctx_t *ctx;
    size_t ctx_size;

    if ((src == NULL && size != 0) || dgst == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    result = ali_sha1_get_ctx_size(&ctx_size);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("get ctx err!\n");
        return result;
    }

    ctx = ls_osa_malloc(ctx_size);
    if (!ctx) {
        CRYPTO_DBG_LOG("ctx malloc(%d) failed!\n", ctx_size);
        result = ALI_CRYPTO_OUTOFMEM;
        goto _out;
    }

    result = ali_sha1_init(ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("init failed(0x%08x)\n", result);
        goto _out;
    }

    result = ali_sha1_update(src, size, ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("update failed(0x%08x)\n", result);
        goto _out;
    }

    result = ali_sha1_final(dgst, ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("final failed(0x%08x)\n", result);
        goto _out;
    }

_out:
    if (ctx) {
        ls_osa_free(ctx);
    }

    return result;
}

ali_crypto_result ali_sha256_digest(const uint8_t *src,
                                  size_t size, uint8_t *dgst)
{
    ali_crypto_result result;
    api_hash_ctx_t *ctx;
    size_t ctx_size;

    if ((src == NULL && size != 0) || dgst == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    result = ali_sha256_get_ctx_size(&ctx_size);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("get ctx err!\n");
        return result;
    }

    ctx = ls_osa_malloc(ctx_size);
    if (!ctx) {
        CRYPTO_DBG_LOG("ctx malloc(%d) failed!\n", ctx_size);
        result = ALI_CRYPTO_OUTOFMEM;
        goto _out;
    }

    result = ali_sha256_init(ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("init failed(0x%08x)\n", result);
        goto _out;
    }

    result = ali_sha256_update(src, size, ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("update failed(0x%08x)\n", result);
        goto _out;
    }

    result = ali_sha256_final(dgst, ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("final failed(0x%08x)\n", result);
        goto _out;
    }

_out:
    if (ctx) {
        ls_osa_free(ctx);
    }

    return result;
}

ali_crypto_result ali_md5_digest(const uint8_t *src,
                                  size_t size, uint8_t *dgst)
{
    ali_crypto_result result;
    api_hash_ctx_t *ctx;
    size_t ctx_size;

    if ((src == NULL && size != 0) || dgst == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    result = ali_md5_get_ctx_size(&ctx_size);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("get ctx err!\n");
        return result;
    }

    ctx = ls_osa_malloc(ctx_size);
    if (!ctx) {
        CRYPTO_DBG_LOG("ctx malloc(%d) failed!\n", ctx_size);
        result = ALI_CRYPTO_OUTOFMEM;
        goto _out;
    }

    result = ali_md5_init(ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("init failed(0x%08x)\n", result);
        goto _out;
    }

    result = ali_md5_update(src, size, ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("update failed(0x%08x)\n", result);
        goto _out;
    }

    result = ali_md5_final(dgst, ctx);
    if (ALI_CRYPTO_SUCCESS != result) {
        CRYPTO_DBG_LOG("final failed(0x%08x)\n", result);
        goto _out;
    }

_out:
    if (ctx) {
        ls_osa_free(ctx);
    }

    return result;
}

ali_crypto_result ali_hash_digest(hash_type_t type, const uint8_t *src,
                                  size_t size, uint8_t *dgst)
{
    ali_crypto_result result;

    if ((src == NULL && size != 0) || dgst == NULL) {
        CRYPTO_DBG_LOG("bad input\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch(type) {
        case SHA1:
            result = ali_sha1_digest(src, size, dgst);
            break;
        case SHA256:
            result = ali_sha256_digest(src, size, dgst);
            break;
        case MD5:
            result = ali_md5_digest(src, size, dgst);
            break;
        default:
            CRYPTO_DBG_LOG("not support this type(%d)\n", type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    return result;
}

ali_crypto_result ali_hash_reset(void *context)
{
    ali_crypto_result  ret = ALI_CRYPTO_SUCCESS;
    api_hash_ctx_t *ctx;
 
    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid ctx\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    ctx = (api_hash_ctx_t *) context;
    memset(ctx->hal_ctx, 0, ctx->hal_size);

    return ret;
}
