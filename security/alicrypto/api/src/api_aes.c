/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "sal_crypto.h"
#include "ali_crypto.h"
#include "impl_crypto.h"


ali_crypto_result ali_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    ali_crypto_result result = 0;
    aes_impl_t *p_aes_impl = NULL;
    size_t impl_size = 0;
    
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input!\n");
    }

    p_aes_impl = ali_crypto_aes_get_impl(type);
    if(NULL == p_aes_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes impl\n");
    }
    
    if(NULL == p_aes_impl->ops.aes_get_ctx_size) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }

    result = p_aes_impl->ops.aes_get_ctx_size(type, &impl_size);

    *size = impl_size + 4;
    return result;
}

ali_crypto_result ali_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    aes_impl_t *p_aes_impl = NULL;

    if (key1 == NULL || context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    if (keybytes != 16 && keybytes != 24 && keybytes != 32) {
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR, "bad key lenth(%d)\n", (int)keybytes);
    }
    
    p_aes_impl = ali_crypto_aes_get_impl(type);

    if(NULL == p_aes_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }
    if(NULL == p_aes_impl->ops.aes_init) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }

    *(uint32_t*)context = type;
    
    result = p_aes_impl->ops.aes_init(type, is_enc, key1, key2, keybytes, iv,(((uint8_t*) context) + 4));
    return result;
}

ali_crypto_result ali_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    aes_impl_t *p_aes_impl = NULL;
    aes_type_t type = 0;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "bad ctx!\n");
    }

    if (src == NULL || dst == NULL || size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad args!\n");
    }

    type = *(uint32_t*)context;
    
    p_aes_impl = ali_crypto_aes_get_impl(type);

    if(NULL == p_aes_impl) {
       PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_aes_impl->ops.aes_process) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }

    result = p_aes_impl->ops.aes_process(src, dst, size, (((uint8_t *)context) + 4));

    return result;
}

ali_crypto_result ali_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    aes_impl_t *p_aes_impl = NULL;
    aes_type_t type = 0;

    if ((src == NULL && src_size != 0) ||
        ((dst_size != NULL) && (dst == NULL && *dst_size != 0)) ||
        context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    type = *(uint32_t*)context;

    p_aes_impl = ali_crypto_aes_get_impl(type);

    if(NULL == p_aes_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_aes_impl->ops.aes_finish) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }

    result = p_aes_impl->ops.aes_finish(src, src_size, dst, dst_size, padding, (((uint8_t *)context) + 4));

    return result;
}

ali_crypto_result ali_aes_reset(void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    aes_impl_t *p_aes_impl = NULL;
    aes_type_t type = 0;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    type = *(uint32_t*)context;

    p_aes_impl = ali_crypto_aes_get_impl(type);

    if(NULL == p_aes_impl) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid impl\n");
    }

    if(NULL == p_aes_impl->ops.aes_reset) {
        PRINT_RET(ALI_CRYPTO_ERROR,  "invalid aes ops\n");
    }

    result = p_aes_impl->ops.aes_reset((((uint8_t *)context) + 4));

    return result;

}


