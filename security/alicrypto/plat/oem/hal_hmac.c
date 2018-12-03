/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "impl_crypto.h"
#include "hal_crypto.h"
#include "sal_crypto.h"


ali_crypto_result hal_hmac_get_ctx_size(hash_type_t type, size_t *size)
{
    ALI_CRYPTO_DBG_I("%s: aes type(%d)\n", __FUNCTION__, type);
    UNUSED(type);
    *size = 128;
    return ALI_CRYPTO_SUCCESS;         
}

ali_crypto_result hal_hmac_init(hash_type_t type, const uint8_t *key,
                                size_t keybytes, void *context)
{
    UNUSED(type);
    UNUSED(context);

    ALI_CRYPTO_DBG_I("%s: aes type(%d)\n", __FUNCTION__, type);

    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_hmac_update(const uint8_t *src, size_t size, void *context)
{
    UNUSED(src);
    UNUSED(size);
    UNUSED(context);

    ALI_CRYPTO_DBG_I("%s\n", __FUNCTION__);

    return ALI_CRYPTO_SUCCESS;

}
                    

ali_crypto_result hal_hmac_final(uint8_t *dgst, void *context)
{
    UNUSED(dgst);
    UNUSED(context);
    ALI_CRYPTO_DBG_I("%s\n", __FUNCTION__);
    
    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result hal_hmac_reset(void *context)
{
    UNUSED(context);
    ALI_CRYPTO_DBG_I("%s\n", __FUNCTION__);

    return ALI_CRYPTO_SUCCESS;
}





