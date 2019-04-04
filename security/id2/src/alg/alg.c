/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "config.h"
#include "error_code.h"
#include "irot_pal.h"
#include "alg.h"
#include "log/log.h"

#if (ID2_USE_ALI_CRYPTO)

#include "ali_crypto.h"
#include "crypto.h"

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
irot_result_t des3_ecb(const uint8_t* key, uint32_t key_len, const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, uint8_t is_enc)
{
    id2_log_debug("crypto lib: 3DES algorithm not supported.\n");
    return IROT_ERROR_NOT_SUPPORTED;
}
#endif

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
irot_result_t aes_ecb(const uint8_t* key, uint32_t key_len, const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, uint8_t is_enc)
{
    irot_result_t ret = IROT_SUCCESS;
    size_t ctx_size;
    ali_crypto_result result;
    void* aes_ctx = NULL;
    int crypto_mode;

    id2_log_debug("crypto lib: AES ecb algorithm.\n");

    if (is_enc)
    {
        crypto_mode = 1;
    }
    else
    {
        crypto_mode = 0;
    }

    result = crypto_aes_get_ctx_size(AES_ECB, &ctx_size);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    aes_ctx = irot_pal_memory_malloc(ctx_size);
    if (aes_ctx == NULL)
    {
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    result = crypto_aes_init(AES_ECB, crypto_mode, key, NULL, key_len, NULL, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    result = crypto_aes_process(in, out, in_len, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }

    result = crypto_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        ret = IROT_ERROR_GENERIC;
        goto EXIT;
    }
    *out_len = in_len;
EXIT:
    irot_pal_memory_free(aes_ctx);
    return ret;
}
#endif

#else

#include <mbedtls/des.h>
#include <mbedtls/aes.h>
#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
irot_result_t des3_ecb(const uint8_t* key, uint32_t key_len, const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, uint8_t is_enc)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t i;
    mbedtls_des3_context des_context;

    mbedtls_des3_init(&des_context);
    if ((in_len % DES_BLOCK_SIZE != 0) || (*out_len < in_len))
    {
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }
    if (key_len == 0x10)
    {
        if (is_enc)
        {
            mbedtls_des3_set2key_enc(&des_context, key);
        }
        else
        {
            mbedtls_des3_set2key_dec(&des_context, key);
        }
    }
    else if (key_len == 0x18)
    {
        if (is_enc)
        {
            mbedtls_des3_set3key_enc(&des_context, key);
        }
        else
        {
            mbedtls_des3_set3key_dec(&des_context, key);
        }
    }
    else
    {
        ret = IROT_ERROR_BAD_PARAMETERS;
        goto EXIT;
    }

    i = 0;
    while (i < in_len)
    {
        mbedtls_des3_crypt_ecb(&des_context, in + i, out + i);
        i += DES_BLOCK_SIZE;
    }

    *out_len = in_len;
EXIT:
    mbedtls_des3_free(&des_context);
    return ret;
}
#endif

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
irot_result_t aes_ecb(const uint8_t* key, uint32_t key_len, const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, uint8_t is_enc)
{
    irot_result_t ret = IROT_SUCCESS;
    uint32_t i;
    mbedtls_aes_context aes_context;

    mbedtls_aes_init(&aes_context);
    if ((in_len % AES_BLOCK_SIZE != 0) || (*out_len < in_len))
    {
        ret = IROT_ERROR_SHORT_BUFFER;
        goto EXIT;
    }
    i = 0;
    if (is_enc)
    {
        mbedtls_aes_setkey_enc(&aes_context, key, key_len * 8);
        while (i < in_len)
        {
            mbedtls_aes_crypt_ecb(&aes_context, MBEDTLS_AES_ENCRYPT, in + i, out + i);
            i += AES_BLOCK_SIZE;
        }
    }
    else
    {
        mbedtls_aes_setkey_dec(&aes_context, key, key_len * 8);
        while (i < in_len)
        {
            mbedtls_aes_crypt_ecb(&aes_context, MBEDTLS_AES_DECRYPT, in + i, out + i);
            i += AES_BLOCK_SIZE;
        }
    }

    *out_len = in_len;
EXIT:
    mbedtls_aes_free(&aes_context);
    return ret;
}
#endif

#endif
