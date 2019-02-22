/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"

#define HAL_AES_BLOCK_SIZE    16

#if CONFIG_HAL_AES_ECB
int ls_hal_aes_ecb_get_size(void)
{
#ifdef CONFIG_HAL_DEMO
    int size = sizeof(hal_aes_ctx_t);

    return size;
#else
    HAL_DBG_LOG("not supported!\n");

    return -1;
#endif
}

int ls_hal_aes_ecb_init(void *ctx,
           int is_enc, const uint8_t *key, size_t size)
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_aes_ctx_t *hal_ctx = (hal_aes_ctx_t *)ctx;

    if (ctx == NULL || key == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    if (size != 16 &&
        size != 24 && size != 32) {
        HAL_DBG_LOG("invalid key size, %d\n", (int)size);
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    hal_aes_init(&hal_ctx->ctx);
    if (0 == is_enc) {
        hal_ctx->mode = HAL_AES_DECRYPT;
        ret = hal_aes_setkey_dec(&hal_ctx->ctx, key, size << 3);
        if (ret != 0) {
            HAL_DBG_LOG("setkey_dec fail.\n");
            return HAL_CRYPT_ERROR;
        }
    } else {
        hal_ctx->mode = HAL_AES_ENCRYPT;
        ret = hal_aes_setkey_enc(&hal_ctx->ctx, key, size << 3);
        if (ret != 0) {
            HAL_DBG_LOG("setkey_enc fail.\n");
            return HAL_CRYPT_ERROR;
        }
    }

    return HAL_CRYPT_SUCCESS;
#else /* CONFIG_HAL_DEMO */

    (void)ctx;
    (void)is_enc;
    (void)key;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;

#endif /* CONFIG_HAL_DEMO */
}

int ls_hal_aes_ecb_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_aes_ctx_t *hal_ctx = (hal_aes_ctx_t *)ctx;

    if (ctx == NULL || src == NULL || dst == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    if (size % HAL_AES_BLOCK_SIZE != 0) {
        HAL_DBG_LOG("invalid size, %d\n", size);
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    while(size > 0) {
        ret = hal_aes_crypt_ecb(&hal_ctx->ctx, hal_ctx->mode, src, dst);
        if (ret != 0) {
            HAL_DBG_LOG("invalid size, %d\n", size);
            return HAL_CRYPT_ERROR;
        }

        src += HAL_AES_BLOCK_SIZE;
        dst += HAL_AES_BLOCK_SIZE;
        size -= HAL_AES_BLOCK_SIZE;
    }

    return HAL_CRYPT_SUCCESS;

#else /* CONFIG_HAL_DEMO */
    (void)ctx;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif
}
#endif

#if CONFIG_HAL_AES_CBC
int ls_hal_aes_cbc_get_size(void)
{
#ifdef CONFIG_HAL_DEMO
    int size = sizeof(hal_aes_ctx_t);

    return size;
#else
    HAL_DBG_LOG("not supported!\n");

    return -1;
#endif
}

int ls_hal_aes_cbc_init(void *ctx, int is_enc,
           const uint8_t *key, size_t key_size, uint8_t iv[AES_IV_SIZE])
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_aes_ctx_t *hal_ctx = (hal_aes_ctx_t *)ctx;

    if (ctx == NULL || key == NULL || iv == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    if (key_size != 16 &&
        key_size != 24 && key_size != 32) {
        HAL_DBG_LOG("invalid key_size, %d\n", (int)key_size);
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    memcpy(hal_ctx->iv, iv, AES_IV_SIZE);
    hal_aes_init(&hal_ctx->ctx);

    if (0 == is_enc) {
        hal_ctx->mode = HAL_AES_DECRYPT;

        ret = hal_aes_setkey_dec(&hal_ctx->ctx, key, key_size << 3);
        if (ret != 0) {
            HAL_DBG_LOG("setkey_dec fail.\n");
            return HAL_CRYPT_ERROR;
        }
    } else {
        hal_ctx->mode = HAL_AES_ENCRYPT;

        ret = hal_aes_setkey_enc(&hal_ctx->ctx, key, key_size << 3);
        if (ret != 0) {
            HAL_DBG_LOG("setkey_enc fail.\n");
            return HAL_CRYPT_ERROR;
        }
    }

    return HAL_CRYPT_SUCCESS;
#else /* CONFIG_HAL_DEMO */
    (void)ctx;
    (void)is_enc;
    (void)key;
    (void)key_size;
    (void)iv;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif /* CONFIG_HAL_DEMO */
}

int ls_hal_aes_cbc_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
#ifdef CONFIG_HAL_DEMO
    int        ret;
    hal_aes_ctx_t *hal_ctx;
    int        mode;

    if (ctx == NULL) {
        return HAL_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    hal_ctx = (hal_aes_ctx_t *)ctx;
    mode = hal_ctx->mode;

    if (size % AES_BLOCK_SIZE != 0) {
          return HAL_CRYPTO_LENGTH_ERR;
    }

    ret = hal_aes_crypt_cbc(&(hal_ctx->ctx), mode, size, (unsigned char *)hal_ctx->iv, (const unsigned char *)src, (unsigned char *)dst);
    if (ret != HAL_CRYPT_SUCCESS) {
        if (hal_ctx->mode) {
            HAL_DBG_LOG("hal_aes_cbc_process: encrypt fail!\n");
        } else {
            HAL_DBG_LOG("hal_aes_cbc_process: decrypt fail!\n");
        }
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
#else
    (void)ctx;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");
#endif
    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
}
#endif

#if CONFIG_HAL_AES_CTR
int ls_hal_aes_ctr_get_size(void)
{
#ifdef CONFIG_HAL_DEMO
    int size = sizeof(hal_aes_ctx_t);

    return size;
#else
    HAL_DBG_LOG("not supported!\n");

    return -1;
#endif
}

int ls_hal_aes_ctr_init(void *ctx, int is_enc,
           const uint8_t *key, size_t size, uint8_t iv[AES_IV_SIZE])
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_aes_ctx_t *hal_ctx = (hal_aes_ctx_t *)ctx;

    if (ctx == NULL || key == NULL || iv == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    if (size != 16 &&
        size != 24 && size != 32) {
        HAL_DBG_LOG("invalid key size, %d\n", (int)size);
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    memcpy(hal_ctx->iv, iv, AES_IV_SIZE);
    hal_aes_init(&hal_ctx->ctx);
    if (0 == is_enc) {
        hal_ctx->mode = HAL_AES_DECRYPT;
    } else {
        hal_ctx->mode = HAL_AES_ENCRYPT;
    }
    ret = hal_aes_setkey_enc(&hal_ctx->ctx, key, size << 3);
    if (ret != 0) {
        HAL_DBG_LOG("setkey fail.\n");
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
#else
    (void)ctx;
    (void)is_enc;
    (void)key;
    (void)key_size;
    (void)iv;

    HAL_DBG_LOG("not supported!\n");
    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif
}

int ls_hal_aes_ctr_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_aes_ctx_t *hal_ctx;

    if (ctx == NULL) {
        return HAL_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    hal_ctx = (hal_aes_ctx_t *)ctx;
    ret = hal_aes_crypt_ctr(
              &(hal_ctx->ctx), size, &(hal_ctx->offset),
              (unsigned char *)hal_ctx->iv,
              (unsigned char *)hal_ctx->stream_block,
              (const unsigned char *)src, (unsigned char *)dst);
    if (ret != HAL_CRYPT_SUCCESS) {
        if (hal_ctx->mode) {
            HAL_DBG_LOG("hal_aes_ctr_process: encrypt fail!\n");
        } else {
            HAL_DBG_LOG("hal_aes_ctr_process: decrypt fail!\n");
        }
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
#else
    (void)ctx;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");
    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif
}

int ls_hal_aes_cfb_get_size(void)
{
#ifdef CONFIG_HAL_DEMO
    int size = sizeof(hal_aes_ctx_t);

    return size;
#else
    HAL_DBG_LOG("not supported!\n");

    return -1;
#endif
}
#endif

#if CONFIG_HAL_AES_CFB
int ls_hal_aes_cfb_init(void *ctx, int is_enc,
           const uint8_t *key, size_t size, uint8_t iv[16])
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_aes_ctx_t *hal_ctx = (hal_aes_ctx_t *)ctx;

    if (ctx == NULL || key == NULL) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    if (size != 16 &&
        size != 24 && size != 32) {
        HAL_DBG_LOG("invalid key size, %d\n", (int)size);
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    memcpy(hal_ctx->iv, iv, AES_IV_SIZE);
    hal_aes_init(&hal_ctx->ctx);
    if (0 == is_enc) {
        hal_ctx->mode = HAL_AES_DECRYPT;
    } else {
        hal_ctx->mode = HAL_AES_ENCRYPT;
    }
    ret = hal_aes_setkey_enc(&hal_ctx->ctx, key, size << 3);
    if (ret != 0) {
        HAL_DBG_LOG("setkey fail.\n");
        return HAL_CRYPT_ERROR;
    }
    return HAL_CRYPT_SUCCESS;
#else
    (void)ctx;
    (void)is_enc;
    (void)key;
    (void)key_size;
    (void)iv;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif
}

int ls_hal_aes_cfb_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
#ifdef CONFIG_HAL_DEMO
    int ret;
    hal_aes_ctx_t *hal_ctx;
    int        mode;

    if (ctx == NULL) {
        return HAL_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        return HAL_CRYPT_ERROR_BAD_PARAMETERS;
    }

    hal_ctx = (hal_aes_ctx_t *)ctx;
    mode = hal_ctx->mode;
    ret = hal_aes_crypt_cfb8(
              &(hal_ctx->ctx), mode, size,
              (unsigned char *)hal_ctx->iv,
              (const unsigned char *)src, (unsigned char *)dst);
    if (ret != HAL_CRYPT_SUCCESS) {
        if (hal_ctx->mode) {
            HAL_DBG_LOG("hal_aes_ctr_process: encrypt fail!\n");
        } else {
            HAL_DBG_LOG("hal_aes_ctr_process: decrypt fail!\n");
        }
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
#else
    (void)ctx;
    (void)src;
    (void)dst;
    (void)size;

    HAL_DBG_LOG("not supported!\n");

    return HAL_CRYPT_ERROR_NOT_SUPPORTED;
#endif
}
#endif

