/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include "mbed_crypto.h"
#include "ali_crypto.h"
#include "drv_tee.h"

static uint8_t  key_copy[32];
static uint32_t keylen_copy;

ali_crypto_result ali_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "aes_get_ctx_size: bad input!\n");
    }
    switch (type) {
        case AES_ECB:
        case AES_CBC:
        case AES_CTR:
#if defined(MBEDTLS_CIPHER_MODE_CFB)
        case AES_CFB8:
        case AES_CFB128:
#endif
            break;
        case AES_CTS:
        case AES_XTS:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_init: invalid aes type(%d)\n", type);
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                      "ali_aes_init: invalid aes type(%d)\n", type);
    }
    *size = sizeof(aes_ctx_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context)
{
    aes_ctx_t *aes_ctx;

    (void)key2;
    if (key1 == NULL || context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_aes_init: bad input args!\n");
    }

    if (keybytes != 16 && keybytes != 24 && keybytes != 32) {
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR, "ali_aes_init: bad key lenth(%d)\n",
                  (int)keybytes);
    }

    aes_ctx = (aes_ctx_t *)context;
    if ((IS_VALID_CTX_MAGIC(aes_ctx->magic) &&
         aes_ctx->status != CRYPTO_STATUS_FINISHED) &&
        aes_ctx->status != CRYPTO_STATUS_CLEAN) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "ali_aes_init: bad status(%d)\n",
                  (int)aes_ctx->status);
    }

    switch (type) {
        case AES_ECB:
            break;
        case AES_CBC: {
            if (iv == NULL) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "ali_aes_init: cbc iv is null\n");
            }

            OSA_memcpy(aes_ctx->iv, iv, 16);
            break;
        }
        case AES_CTR: {
            if (iv == NULL) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "ali_aes_init: ctr iv is null\n");
            }
            OSA_memcpy(aes_ctx->iv, iv, 16);
            break;
        }
#if defined(MBEDTLS_CIPHER_MODE_CFB)
        case AES_CFB8:
        case AES_CFB128: {
            if (iv == NULL) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "ali_aes_init: cfb iv is null\n");
            }
            OSA_memcpy(aes_ctx->iv, iv, 16);
            break;
        }
#endif
        case AES_CTS:
        case AES_XTS:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_init: not support aes type(%d)\n", type);
            break;
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                      "ali_aes_init: invalid aes type(%d)\n", type);
    }

    // mbedtls_aes_init(&(aes_ctx->ctx));
    aes_ctx->is_enc = is_enc;

    OSA_memcpy(key_copy, key1, keybytes);
    keylen_copy = keybytes;

    aes_ctx->offset = 0;
    aes_ctx->type   = type;
    aes_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(aes_ctx->magic);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context)
{
    int        ret = ALI_CRYPTO_ERROR;
    aes_ctx_t *aes_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_aes_process: bad ctx!\n");
    }

    if (src == NULL || dst == NULL || size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_aes_process: bad args!\n");
    }

    aes_ctx = (aes_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(aes_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_aes_process: bad magic!\n");
    }

    if ((aes_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (aes_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "ali_aes_update: bad status(%d)\n",
                  (int)aes_ctx->status);
    }

    switch (aes_ctx->type) {
        /* FIXME, limitation, size must be block size aigned */
        case AES_ECB: {
            size_t cur_len = 0;
            if (size % AES_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                          "ali_aes_process: invalid size(%d)\n", (int)size);
            }

            while (cur_len < size) {
                if (aes_ctx->is_enc) {
                    ret = csi_tee_aes_encrypt_ecb(src + cur_len, AES_BLOCK_SIZE,
                                                  key_copy, keylen_copy,
                                                  dst + cur_len);
                } else {
                    ret = csi_tee_aes_decrypt_ecb(src + cur_len, AES_BLOCK_SIZE,
                                                  key_copy, keylen_copy,
                                                  dst + cur_len);
                }

                if (ret != 0) {
                    printf("aes engine fail \n");
                }

                cur_len += AES_BLOCK_SIZE;
            }

            break;
        }
        case AES_CBC: {
            if (size % AES_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                          "ali_aes_process: invalid size(%d)\n", (int)size);
            }

            if (aes_ctx->is_enc) {
                ret = csi_tee_aes_encrypt_cbc(src, size, key_copy, keylen_copy,
                                              aes_ctx->iv, dst);
            } else {
                ret = csi_tee_aes_decrypt_cbc(src, size, key_copy, keylen_copy,
                                              aes_ctx->iv, dst);
            }

#if 0 /* mbedtls have copy it */
            if (ret == ALI_CRYPTO_SUCCESS) {
                OSA_memcpy(aes_ctx->iv, src - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
            }
#endif

            break;
        }
        case AES_CTR: {

            break;
        }
#if defined(MBEDTLS_CIPHER_MODE_CFB)
        case AES_CFB8: {

            break;
        }
        case AES_CFB128: {

            break;
        }
#endif
        case AES_CTS:
        case AES_XTS:
        default:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_process: invalid hash type(%d)\n",
                      aes_ctx->type);
    }

    if (ret != ALI_CRYPTO_SUCCESS) {
        if (aes_ctx->is_enc) {
            MBED_DBG_E("ali_aes_process: encrypt(%d) fail!\n", aes_ctx->type);
        } else {
            MBED_DBG_E("ali_aes_process: decrypt(%d) fail!\n", aes_ctx->type);
        }
        return ALI_CRYPTO_ERROR;
    }

    aes_ctx->status = CRYPTO_STATUS_PROCESSING;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context)
{
    ali_crypto_result ret = ALI_CRYPTO_ERROR;
    aes_ctx_t *       aes_ctx;

    (void)padding;

    if ((src == NULL && src_size != 0) ||
        ((dst_size != NULL) && (dst == NULL && *dst_size != 0)) ||
        context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_aes_finish: bad input args!\n");
    }

    aes_ctx = (aes_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(aes_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_aes_finish: bad magic!\n");
    }

    if ((aes_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (aes_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "ali_aes_finish: bad status(%d)\n",
                  (int)aes_ctx->status);
    }

    switch (aes_ctx->type) {
        case AES_ECB: {
            if (aes_ctx->is_enc) {
                ret = csi_tee_aes_encrypt_ecb(src, src_size, key_copy,
                                              keylen_copy, dst);
            } else {
                ret = csi_tee_aes_decrypt_ecb(src, src_size, key_copy,
                                              keylen_copy, dst);
            }
            break;
        }
        case AES_CBC: {
            if (aes_ctx->is_enc) {
                ret = csi_tee_aes_encrypt_cbc(src, src_size, key_copy,
                                              keylen_copy, aes_ctx->iv, dst);
            } else {
                ret = csi_tee_aes_decrypt_cbc(src, src_size, key_copy,
                                              keylen_copy, aes_ctx->iv, dst);
            }
            break;
        }
        case AES_CTR: {
            break;
        }
#if defined(MBEDTLS_CIPHER_MODE_CFB)
        case AES_CFB8:
        case AES_CFB128: {
            break;
        }
#endif
        case AES_CTS:
        case AES_XTS:
        default:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_finish: invalid aes type(%d)\n", aes_ctx->type);
    }

    if (ret != ALI_CRYPTO_SUCCESS) {
        mbedtls_aes_free(&(aes_ctx->ctx));
        PRINT_RET(ret, "ali_aes_process: aes type(%d) final fail(%08x)\n",
                  aes_ctx->type, ret);
    }

    CLEAN_CTX_MAGIC(aes_ctx->magic);
    aes_ctx->status = CRYPTO_STATUS_FINISHED;
    aes_ctx->offset = 0;

    mbedtls_aes_free(&(aes_ctx->ctx));
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_aes_reset(void *context)
{
    aes_ctx_t *aes_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_aes_reset: bad input args!\n");
    }

    aes_ctx = (aes_ctx_t *)context;
#if 0
    if (!IS_VALID_CTX_MAGIC(aes_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ali_aes_reset: bad magic!\n");
    }
#endif

    OSA_memset(aes_ctx, 0, sizeof(aes_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_aes_copy_context(void *dst_ctx, void *src_ctx)
{
    aes_ctx_t *aes_ctx_src, *aes_ctx_dst;

    if ((dst_ctx == NULL) || (src_ctx == NULL)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                  "ali_aes_copy_context: bad input args!\n");
    }

    aes_ctx_src = (aes_ctx_t *)src_ctx;
    if (!IS_VALID_CTX_MAGIC(aes_ctx_src->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT,
                  "ali_aes_copy_context: bad magic!\n");
    }

    /* only can copy to one un-initialized context */
    aes_ctx_dst = (aes_ctx_t *)dst_ctx;
    if ((IS_VALID_CTX_MAGIC(aes_ctx_dst->magic)) &&
        ((aes_ctx_dst->status == CRYPTO_STATUS_INITIALIZED) ||
         (aes_ctx_dst->status == CRYPTO_STATUS_PROCESSING) ||
         (aes_ctx_dst->status == CRYPTO_STATUS_FINISHED))) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "ali_aes_init: bad dst status(%d)\n",
                  (int)aes_ctx_dst->status);
    }

    OSA_memcpy(aes_ctx_dst, aes_ctx_src, sizeof(aes_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}
