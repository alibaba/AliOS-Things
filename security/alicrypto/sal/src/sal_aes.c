/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "sal_crypto.h"
#include "ali_crypto.h"

ali_crypto_result sal_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "aes_get_ctx_size: bad input!\n");
    }
    switch (type) {
        case AES_ECB:
        case AES_CBC:
        case AES_CTR:
        case AES_CFB8:
        case AES_CFB128:
            break;
        case AES_CTS:
        case AES_XTS:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_get_ctx_size: invalid aes type(%d)\n", type);
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                      "ali_aes_get_ctx_size: invalid aes type(%d)\n", type);
    }
    *size = sizeof(aes_ctx_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context)
{
    int        ret = ALI_CRYPTO_SUCCESS;
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

            osa_memcpy(aes_ctx->iv, iv, 16);
            break;
        }
        case AES_CTR: {
            if (iv == NULL) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "ali_aes_init: ctr iv is null\n");
            }
            osa_memcpy(aes_ctx->iv, iv, 16);
	    osa_memset(aes_ctx->stream_block, 0, AES_BLOCK_SIZE);
            break;
        }
        case AES_CFB8:
        case AES_CFB128: {
            if (iv == NULL) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "ali_aes_init: cfb iv is null\n");
            }
            osa_memcpy(aes_ctx->iv, iv, 16);
            break;
        }
        case AES_CTS:
        case AES_XTS:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_init: not support aes type(%d)\n", type);
            break;
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                      "ali_aes_init: invalid aes type(%d)\n", type);
    }

    ali_algo_aes_init(&(aes_ctx->ctx));
    aes_ctx->is_enc = is_enc;

    if (aes_ctx->is_enc) {
        ret = ali_algo_aes_setkey_enc(&(aes_ctx->ctx), key1, keybytes * 8);
    } else {
        if (AES_CTR == type || AES_CFB8 == type || AES_CFB128 == type) {
            ret = ali_algo_aes_setkey_enc(&(aes_ctx->ctx), key1, keybytes * 8);
        } else {
            ret = ali_algo_aes_setkey_dec(&(aes_ctx->ctx), key1, keybytes * 8);
        }
    }

    if (ret != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR, "ALI_aes_init: start mode(%d) fail(%d)\n",
                  type, ret);
    }

    aes_ctx->offset = 0;
    aes_ctx->type   = type;
    aes_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(aes_ctx->magic);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context)
{
    int        ret;
    aes_ctx_t *aes_ctx;
    int        mode;

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

    if (aes_ctx->is_enc) {
        mode = ALI_ALGO_AES_ENCRYPT;
    } else {
        mode = ALI_ALGO_AES_DECRYPT;
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
                ret = ali_algo_aes_crypt_ecb(&(aes_ctx->ctx), mode,
                                            src + cur_len, dst + cur_len);
                if (0 != ret) {
                    PRINT_RET(ALI_CRYPTO_ERROR,
                              "ali_algo_aes_crypt_ecb fail(%d)\n", ret);
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

            ret = ali_algo_aes_crypt_cbc(
              &(aes_ctx->ctx), mode, size, (unsigned char *)aes_ctx->iv,
              (const unsigned char *)src, (unsigned char *)dst);

#if 0 /* ali_algo have copy it */
            if (ret == ALI_CRYPTO_SUCCESS) {
                osa_memcpy(aes_ctx->iv, src - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
            }
#endif

            break;
        }
        case AES_CTR: {
            ret = ali_algo_aes_crypt_ctr(
              &(aes_ctx->ctx), size, &(aes_ctx->offset),
              (unsigned char *)aes_ctx->iv,
              (unsigned char *)aes_ctx->stream_block,
              (const unsigned char *)src, (unsigned char *)dst);
            break;
        }
        case AES_CFB8: {
            ret = ali_algo_aes_crypt_cfb8(
              &(aes_ctx->ctx), mode, size, (unsigned char *)aes_ctx->iv,
              (const unsigned char *)src, (unsigned char *)dst);
            break;
        }
        case AES_CFB128: {
            ret = ali_algo_aes_crypt_cfb128(
              &(aes_ctx->ctx), mode, size, &(aes_ctx->offset),
              (unsigned char *)aes_ctx->iv, (const unsigned char *)src,
              (unsigned char *)dst);
            break;
        }
        case AES_CTS:
        case AES_XTS:
        default:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_process: invalid hash type(%d)\n",
                      aes_ctx->type);
    }

    if (ret != ALI_CRYPTO_SUCCESS) {
        if (aes_ctx->is_enc) {
            ALI_CRYPTO_DBG_E("ali_aes_process: encrypt(%d) fail!\n", aes_ctx->type);
        } else {
            ALI_CRYPTO_DBG_E("ali_aes_process: decrypt(%d) fail!\n", aes_ctx->type);
        }
        return ALI_CRYPTO_ERROR;
    }

    aes_ctx->status = CRYPTO_STATUS_PROCESSING;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_aes_reset(void *context)
{
    aes_ctx_t *aes_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_aes_reset: bad input args!\n");
    }

    aes_ctx = (aes_ctx_t *)context;

    osa_memset(aes_ctx, 0, sizeof(aes_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}
