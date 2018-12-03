/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "impl_crypto.h"
#include "hal_crypto.h"
#include "sal_crypto.h"

ali_crypto_result hal_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    ALI_CRYPTO_DBG_I("ali_aes_get_ctx_size: aes type(%d)\n", type);
    UNUSED(type);
    *size = 128;
    return ALI_CRYPTO_SUCCESS;         
}

ali_crypto_result hal_aes_init(aes_type_t type,         bool is_enc,
                                    const uint8_t *key1,    const uint8_t *key2,
                                    size_t keybytes,        const uint8_t *iv,      
                                    void *context)
{
    UNUSED(type);
    UNUSED(is_enc);
    UNUSED(key1);
    UNUSED(key2);
    UNUSED(keybytes);
    UNUSED(iv);
    UNUSED(context);

    ALI_CRYPTO_DBG_I("hal_aes_init:  aes type(%d)\n", type);

    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_aes_process(const uint8_t    *src, 
                                        uint8_t         *dst, 
                                        size_t          size, 
                                        void            *context)
{
    UNUSED(src);
    UNUSED(dst);
    UNUSED(size);
    UNUSED(context);

    ALI_CRYPTO_DBG_I("hal_aes_process\n");

    return ALI_CRYPTO_SUCCESS;

}
                    

ali_crypto_result hal_aes_finish(const uint8_t   *src, 
                                    size_t          src_size, 
                                    uint8_t         *dst, 
                                    size_t          *dst_size, 
                                    sym_padding_t   padding,
                                    void            *context)
{
    UNUSED(src);
    UNUSED(src_size);
    UNUSED(dst);
    UNUSED(dst_size);
    UNUSED(padding);
    UNUSED(context);
    ALI_CRYPTO_DBG_I("hal_aes_finish\n");
    
    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_aes_reset(void *context)
{
    UNUSED(context);
    ALI_CRYPTO_DBG_I("hal_aes_reset\n");

    return ALI_CRYPTO_SUCCESS;
}





