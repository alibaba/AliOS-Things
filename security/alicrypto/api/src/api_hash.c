/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */
#include "ali_crypto.h"
#include "impl_crypto.h"
#include "sal_crypto.h"

ali_crypto_result ali_hash_get_ctx_size(hash_type_t type, size_t *size)
{
    ali_crypto_result result = 0;
    hash_impl_t *p_impl = NULL;
    size_t impl_ctx_size = 0;
    
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input!\n");
    }

    p_impl = ali_crypto_hash_get_impl(type);
    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }
    if(NULL == p_impl->ops.hash_get_ctx_size) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.hash_get_ctx_size(type, &impl_ctx_size);
    
    /*store the type in the head of context*/
    *size = impl_ctx_size + 4;
    return result;

}

ali_crypto_result ali_hash_init(hash_type_t type, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hash_impl_t *p_impl = NULL;

    if (NULL == context) {
        ALI_CRYPTO_DBG_E("bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }
    
    p_impl = ali_crypto_hash_get_impl(type);

    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }
    if(NULL == p_impl->ops.hash_init) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    *(uint32_t*)context = type;
    
    result = p_impl->ops.hash_init(type, (((uint8_t *)context) + 4));
    return result;
}

ali_crypto_result ali_hash_update(const uint8_t *src, size_t size, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hash_impl_t *p_impl = NULL;
    hash_type_t type = 0;

    if (context == NULL) {
        ALI_CRYPTO_DBG_E("bad ctx!\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        ALI_CRYPTO_DBG_E("bad args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    type = *(uint32_t*)context;
    
    p_impl = ali_crypto_hash_get_impl(type);

    if(NULL == p_impl) {
       PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_impl->ops.hash_update) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.hash_update(src, size, (((uint8_t *)context) + 4));

    return result;
}

ali_crypto_result ali_hash_final(uint8_t *dgst, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hash_impl_t *p_impl = NULL;
    hash_type_t type = 0;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "invalid context!\n");
    }
    if (dgst == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    type = *(uint32_t*)context;
    
    p_impl = ali_crypto_hash_get_impl(type);

    if(NULL == p_impl) {
       PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }
    if(NULL == p_impl->ops.hash_final) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.hash_final(dgst, (((uint8_t *)context) + 4));

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
    hash_impl_t *p_impl = NULL;
    hash_type_t type = 0;
    
    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "invalid context!\n");
    }

    type = *(uint32_t*)context;

    p_impl = ali_crypto_hash_get_impl(type);

    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_impl->ops.hash_reset) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.hash_reset((((uint8_t *)context) + 4));

    return result;
}

