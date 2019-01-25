/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */
#include "ali_crypto.h"
#include "impl_crypto.h"
#include "sal_crypto.h"

#ifdef ALI_CRYPTO_HMAC
ali_crypto_result ali_hmac_get_ctx_size(hash_type_t type, size_t *size)
{
    ali_crypto_result result = 0;
    hmac_impl_t *p_impl = NULL;
    size_t impl_ctx_size = 0;
    
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input!\n");
    }

    p_impl = ali_crypto_hmac_get_impl(type);
    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }
    if(NULL == p_impl->ops.hmac_get_ctx_size) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
    
    result = p_impl->ops.hmac_get_ctx_size(type, &impl_ctx_size);
    
    /*store the type in the head of context*/
    *size = impl_ctx_size + 4;
    return result;
}

ali_crypto_result ali_hmac_init(hash_type_t type, const uint8_t *key,
                                size_t keybytes, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hmac_impl_t *p_impl = NULL;

    if (context == NULL || ((key == NULL) && (keybytes != 0))) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    p_impl = ali_crypto_hmac_get_impl(type);

    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }
    if(NULL == p_impl->ops.hmac_init) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    *(uint32_t*)context = type;
    
    result = p_impl->ops.hmac_init(type, key, keybytes, (((uint8_t *)context) + 4));
    return result;
}

ali_crypto_result ali_hmac_update(const uint8_t *src, size_t size, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hmac_impl_t *p_impl = NULL;
    hash_type_t type = 0;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "bad ctx!\n");
    }

    if (src == NULL && size != 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad args!\n");
    }
    
    type = *(uint32_t*)context;
    
    p_impl = ali_crypto_hmac_get_impl(type);

    if(NULL == p_impl) {
       PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_impl->ops.hmac_update) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.hmac_update(src, size, (((uint8_t *)context) + 4));

    return result;
}

ali_crypto_result ali_hmac_final(uint8_t *dgst, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hmac_impl_t *p_impl = NULL;
    hash_type_t type = 0;

    if (context == NULL || dgst == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }
    
    type = *(uint32_t*)context;
    
    p_impl = ali_crypto_hmac_get_impl(type);

    if(NULL == p_impl) {
       PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }
    if(NULL == p_impl->ops.hmac_final) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
	
    result = p_impl->ops.hmac_final(dgst, (((uint8_t *)context) + 4));

    return result;
}

ali_crypto_result ali_hmac_digest(hash_type_t type, const uint8_t *key,
                                  size_t keybytes, const uint8_t *src,
                                  size_t size, uint8_t *dgst)
{
    ali_crypto_result result = 0;
    size_t impl_ctx_size = 0;
    void *context = NULL; 

    if ((src == NULL && size != 0) || key == NULL || keybytes == 0 ||
        dgst == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }
	
    result = ali_hmac_get_ctx_size(type, &impl_ctx_size);
    if( ALI_CRYPTO_SUCCESS != result) {
       PRINT_RET(result,  "get ctx err!\n");
    }    
    
    context = osa_malloc(impl_ctx_size);
    if(!context){
        PRINT_RET(ALI_CRYPTO_OUTOFMEM,  "ctx malloc err!\n");
    }
    
    result = ali_hmac_init(type, key, keybytes, context);
    if( ALI_CRYPTO_SUCCESS != result) {
       GO_RET(result,  "err: init\n");
    }    

    result = ali_hmac_update(src, size, context);
    if( ALI_CRYPTO_SUCCESS != result) {
       GO_RET(result,  "err: udpate\n");
    }        

    result = ali_hmac_final(dgst, context);
    if( ALI_CRYPTO_SUCCESS != result) {
       GO_RET(result,  "err: final\n");
    }        
    
_OUT:
    if(context){
        osa_free(context);
    }
    return result;
}


ali_crypto_result ali_hmac_reset(void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    hmac_impl_t *p_impl = NULL;
    hash_type_t type = 0;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "invalid context!\n");
    }

    type = *(uint32_t*)context;

    p_impl = ali_crypto_hmac_get_impl(type);

    if(NULL == p_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_impl->ops.hmac_reset) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }
    result = p_impl->ops.hmac_reset((((uint8_t *)context) + 4));

    return result;

}
#endif /* ALI_CRYPTO_HMAC */
