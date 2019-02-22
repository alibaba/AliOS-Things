/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */
#include "ali_crypto.h"
#include "sal_crypto.h"
#if (CONFIG_HAL_MD5 || CONFIG_HAL_SHA1 || CONFIG_HAL_SHA256 )
#include "ls_hal_crypt.h"
#endif

typedef struct {
    hash_type_t            type;
    union{
#if (CONFIG_HAL_MD5)
        hal_md5_ctx_t      md5_ctx;
#endif
#if (CONFIG_HAL_SHA1)
        hal_sha1_ctx_t     sha1_ctx;
#endif
#if (CONFIG_HAL_SHA256 )
        hal_sha256_ctx_t   sha256_ctx;
#endif
        hash_ctx_t         sal_ctx;
    };
} api_hash_ctx_t;

ali_crypto_result ali_hash_get_ctx_size(hash_type_t type, size_t *size)
{
    ali_crypto_result result = 0;

    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input!\n");
    }
    switch(type){
#if (CONFIG_HAL_MD5 || CONFIG_SAL_MD5)
        case MD5:
            *size = sizeof(api_hash_ctx_t);
            break;
#endif
#if (CONFIG_HAL_SHA1 || CONFIG_SAL_SHA1)
        case SHA1:
            *size = sizeof(api_hash_ctx_t);
            break;
#endif
#if (CONFIG_HAL_SHA256 || CONFIG_SAL_SHA256)
        case SHA256:
            *size = sizeof(api_hash_ctx_t);
            break;
#endif
        default:
            PRINT_RET(-1, "ali_hash_init: invalid type(%d)\n", type);
    }
    return result;
}

ali_crypto_result ali_hash_init(hash_type_t type, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    api_hash_ctx_t *ctx;

    if (NULL == context) {
        ALI_CRYPTO_DBG_E("bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    ctx = (api_hash_ctx_t *) context;
    ctx->type = type;
    switch(type){
#if (CONFIG_HAL_MD5 || CONFIG_SAL_MD5)
        case MD5:
#if CONFIG_HAL_MD5
            result = ls_hal_md5_init(&(ctx->md5_ctx));
#else
            result = sal_hash_init(type, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA1 || CONFIG_SAL_SHA1)
        case SHA1:
#if CONFIG_HAL_SHA1
            result = ls_hal_sha1_init(&(ctx->sha1_ctx));
#else
            result = sal_hash_init(type, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA256 || CONFIG_SAL_SHA256)
        case SHA256:
#if CONFIG_HAL_SHA256
            result = ls_hal_sha256_init(&(ctx->sha256_ctx));
#else
            result = sal_hash_init(type, &(ctx->sal_ctx));
#endif
            break;
#endif
        default:
            PRINT_RET(-1, "ali_hash_init: invalid type(%d)\n", type);
    }
    return result;
}

ali_crypto_result ali_hash_update(const uint8_t *src, size_t size, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hash_type_t type;
    api_hash_ctx_t *ctx;
    
    if (context == NULL) {
        ALI_CRYPTO_DBG_E("bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    if (src == NULL && size != 0) {
        ALI_CRYPTO_DBG_E("bad args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    ctx = (api_hash_ctx_t *) context;
    type = ctx->type;
    switch(type){
#if (CONFIG_HAL_MD5 || CONFIG_SAL_MD5)
        case MD5:
#if CONFIG_HAL_MD5
            result = ls_hal_md5_update(&(ctx->md5_ctx), src, size);
#else
            result = sal_hash_update(src, size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA1 || CONFIG_SAL_SHA1)
        case SHA1:
#if CONFIG_HAL_SHA1
            result = ls_hal_sha1_update(&(ctx->sha1_ctx), src, size);
#else
            result = sal_hash_update(src, size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA256 || CONFIG_SAL_SHA256)
        case SHA256:
#if CONFIG_HAL_SHA256
            result = ls_hal_sha256_update(&(ctx->sha256_ctx),src,size);
#else
            result = sal_hash_update(src, size, &(ctx->sal_ctx));
#endif
            break;
#endif
        default:
            PRINT_RET(-1, "ali_hash_update: invalid type(%d)\n", type);
    }
    return result;
}

ali_crypto_result ali_hash_final(uint8_t *dgst, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hash_type_t type;
    api_hash_ctx_t *ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "invalid context!\n");
    }
    if (dgst == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    ctx = (api_hash_ctx_t *) context;
    type = ctx->type;
    switch(type){
#if (CONFIG_HAL_MD5 || CONFIG_SAL_MD5)
        case MD5:
#if CONFIG_HAL_MD5
            result = ls_hal_md5_finish(&(ctx->md5_ctx), dgst);
#else
            result = sal_hash_final(dgst, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA1 || CONFIG_SAL_SHA1)
        case SHA1:
#if CONFIG_HAL_SHA1
            result = ls_hal_sha1_finish(&(ctx->sha1_ctx), dgst);
#else
            result = sal_hash_final(dgst, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA256 || CONFIG_SAL_SHA256)
        case SHA256:
#if CONFIG_HAL_SHA256
            result = ls_hal_sha256_finish(&(ctx->sha256_ctx), dgst);
#else
            result = sal_hash_final(dgst, &(ctx->sal_ctx));
#endif
            break;
#endif
        default:
            PRINT_RET(-1, "ali_hash_final: invalid type(%d)\n", type);
    }
    return result;
}

ali_crypto_result ali_hash_digest(hash_type_t type, const uint8_t *src,
                                            size_t size, uint8_t *dgst)
{
    ali_crypto_result result = 0;
    size_t impl_ctx_size = 0;
    void *context = NULL; 
    
    if ((src == NULL && size != 0) || dgst == NULL) {
        ALI_CRYPTO_DBG_E("bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }
    result = ali_hash_get_ctx_size(type, &impl_ctx_size);
    if( ALI_CRYPTO_SUCCESS != result) {
       PRINT_RET(result,  "get ctx err!\n");
    }    
    
    context = osa_malloc(impl_ctx_size);
    if(!context){
        PRINT_RET(ALI_CRYPTO_OUTOFMEM,  "ctx malloc err!\n");
    }
    
    result = ali_hash_init(type, context);
    if( ALI_CRYPTO_SUCCESS != result) {
       GO_RET(result,  "err: init\n");
    }    

    result = ali_hash_update(src, size, context);
    if( ALI_CRYPTO_SUCCESS != result) {
       GO_RET(result,  "err: udpate\n");
    }        

    result = ali_hash_final(dgst, context);
    if( ALI_CRYPTO_SUCCESS != result) {
       GO_RET(result,  "err: final\n");
    }        
_OUT:
    if(context){
        osa_free(context);
    }
    return result;
}

ali_crypto_result ali_hash_reset(void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hash_type_t type;
    api_hash_ctx_t *ctx;
 
    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "invalid context!\n");
    }

    ctx = (api_hash_ctx_t *) context;
    type = ctx->type;
    switch(type){
#if (CONFIG_HAL_MD5 || CONFIG_SAL_MD5)
        case MD5:
#if CONFIG_HAL_MD5
            osa_memset(&(ctx->md5_ctx), 0, sizeof(ctx->md5_ctx));
#else
            result = sal_hash_reset(&(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA1 || CONFIG_SAL_SHA1)
        case SHA1:
#if CONFIG_HAL_SHA1
            osa_memset(&(ctx->sha1_ctx), 0, sizeof(ctx->sha1_ctx));
#else
            result = sal_hash_reset(&(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_SHA256 || CONFIG_SAL_SHA256)
        case SHA256:
#if CONFIG_HAL_SHA256
            osa_memset(&(ctx->sha256_ctx), 0, sizeof(ctx->sha256_ctx));
#else
            result = sal_hash_reset(&(ctx->sal_ctx));
#endif
            break;
#endif
        default:
            PRINT_RET(-1, "ali_hash_reset: invalid type(%d)\n", type);
    }
    return result;
}

