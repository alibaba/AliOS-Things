/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include "ls_hal.h"
#include "aes.h"

#define HAL_AES_BLOCK_SIZE    16

int ls_hal_aes_ecb_get_size(void)
{
    return sizeof(impl_aes_ctx_t);
}

int ls_hal_aes_ecb_init(void *ctx,
           int is_enc, const uint8_t *key, size_t size)
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;

    if (ctx == NULL || key == NULL) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    if (size != 16 && size != 24 && size != 32) {
        LS_HAL_LOG("invalid key size, %d\n", (int)size);
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    impl_aes_init(&(impl_ctx->ctx));

    if (0 == is_enc) {
        impl_ctx->mode = HAL_AES_DECRYPT;
        ret = impl_aes_setkey_dec(&impl_ctx->ctx, key, size << 3);
        if (ret != 0) {
            LS_HAL_LOG("setkey_dec fail.\n");
            return HAL_CRYPT_ERROR;
        }
    } else {
        impl_ctx->mode = HAL_AES_ENCRYPT;
        ret = impl_aes_setkey_enc(&(impl_ctx->ctx), key, size << 3);
        if (ret != 0) {
            LS_HAL_LOG("setkey_enc fail.\n");
            return HAL_CRYPT_ERROR;
        }
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_ecb_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;

    if (ctx == NULL || src == NULL || dst == NULL) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    if (size % HAL_AES_BLOCK_SIZE != 0) {
        LS_HAL_LOG("invalid size, %d\n", size);
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;

    while(size > 0) {
        ret = impl_aes_crypt_ecb(&(impl_ctx->ctx), impl_ctx->mode, src, dst);
        if (ret != 0) {
            LS_HAL_LOG("invalid size, %d\n", size);
            return HAL_CRYPT_ERROR;
        }

        src += HAL_AES_BLOCK_SIZE;
        dst += HAL_AES_BLOCK_SIZE;
        size -= HAL_AES_BLOCK_SIZE;
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_cbc_get_size(void)
{
    return sizeof(impl_aes_ctx_t);
}

int ls_hal_aes_cbc_init(void *ctx, int is_enc,
           const uint8_t *key, size_t key_size, uint8_t iv[AES_IV_SIZE])
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;

    if (ctx == NULL || key == NULL || iv == NULL) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    if (key_size != 16 && key_size != 24 && key_size != 32) {
        LS_HAL_LOG("invalid key_size, %d\n", (int)key_size);
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    memcpy(impl_ctx->iv, iv, AES_IV_SIZE);
    impl_aes_init(&(impl_ctx->ctx));

    if (0 == is_enc) {
        impl_ctx->mode = HAL_AES_DECRYPT;

        ret = impl_aes_setkey_dec(&impl_ctx->ctx, key, key_size << 3);
        if (ret != 0) {
            LS_HAL_LOG("setkey_dec fail.\n");
            return HAL_CRYPT_ERROR;
        }
    } else {
        impl_ctx->mode = HAL_AES_ENCRYPT;

        ret = impl_aes_setkey_enc(&impl_ctx->ctx, key, key_size << 3);
        if (ret != 0) {
            LS_HAL_LOG("setkey_enc fail.\n");
            return HAL_CRYPT_ERROR;
        }
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_cbc_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
    int ret;
    impl_aes_ctx_t *impl_ctx;
    int mode;

    if (ctx == NULL) {
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    mode = impl_ctx->mode;

    if (size % AES_BLOCK_SIZE != 0) {
          return HAL_CRYPT_LENGTH_ERR;
    }

    ret = impl_aes_crypt_cbc(&(impl_ctx->ctx), mode, size, (unsigned char *)impl_ctx->iv, (const unsigned char *)src, (unsigned char *)dst);
    if (ret != HAL_CRYPT_SUCCESS) {
        if (impl_ctx->mode) {
            LS_HAL_LOG("impl_aes_cbc_process: encrypt fail!\n");
        } else {
            LS_HAL_LOG("impl_aes_cbc_process: decrypt fail!\n");
        }
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_ctr_get_size(void)
{
    return sizeof(impl_aes_ctx_t);
}

int ls_hal_aes_ctr_init(void *ctx, int is_enc,
           const uint8_t *key, size_t size, uint8_t iv[AES_IV_SIZE])
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;

    if (ctx == NULL || key == NULL || iv == NULL) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    if (size != 16 && size != 24 && size != 32) {
        LS_HAL_LOG("invalid key size, %d\n", (int)size);
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    memcpy(impl_ctx->iv, iv, AES_IV_SIZE);
    impl_aes_init(&(impl_ctx->ctx));

    if (0 == is_enc) {
        impl_ctx->mode = HAL_AES_DECRYPT;
    } else {
        impl_ctx->mode = HAL_AES_ENCRYPT;
    }
    ret = impl_aes_setkey_enc(&impl_ctx->ctx, key, size << 3);
    if (ret != 0) {
        LS_HAL_LOG("setkey fail.\n");
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_ctr_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;

    if (ctx == NULL) {
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    ret = impl_aes_crypt_ctr(
              &(impl_ctx->ctx), size, &(impl_ctx->offset),
              (unsigned char *)impl_ctx->iv,
              (unsigned char *)impl_ctx->stream_block,
              (const unsigned char *)src, (unsigned char *)dst);
    if (ret != HAL_CRYPT_SUCCESS) {
        if (impl_ctx->mode) {
            LS_HAL_LOG("impl_aes_ctr_process: encrypt fail!\n");
        } else {
            LS_HAL_LOG("impl_aes_ctr_process: decrypt fail!\n");
        }
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_cfb_get_size(void)
{
    return sizeof(impl_aes_ctx_t);
}

int ls_hal_aes_cfb_init(void *ctx, int is_enc,
           const uint8_t *key, size_t size, uint8_t iv[16])
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;

    if (ctx == NULL || key == NULL) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    if (size != 16 && size != 24 && size != 32) {
        LS_HAL_LOG("invalid key size, %d\n", (int)size);
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    memcpy(impl_ctx->iv, iv, AES_IV_SIZE);
    impl_aes_init(&(impl_ctx->ctx));

    if (0 == is_enc) {
        impl_ctx->mode = HAL_AES_DECRYPT;
    } else {
        impl_ctx->mode = HAL_AES_ENCRYPT;
    }

    ret = impl_aes_setkey_enc(&(impl_ctx->ctx), key, size << 3);
    if (ret != 0) {
        LS_HAL_LOG("setkey fail.\n");
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_cfb_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;
    int mode;

    if (ctx == NULL) {
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    mode = impl_ctx->mode;
    ret = impl_aes_crypt_cfb8(
              &(impl_ctx->ctx), mode, size,
              (unsigned char *)impl_ctx->iv,
              (const unsigned char *)src, (unsigned char *)dst);
    if (ret != HAL_CRYPT_SUCCESS) {
        if (impl_ctx->mode) {
            LS_HAL_LOG("impl_aes_ctr_process: encrypt fail!\n");
        } else {
            LS_HAL_LOG("impl_aes_ctr_process: decrypt fail!\n");
        }
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}

int ls_hal_aes_cfb128_process(void *ctx, const uint8_t *src, uint8_t *dst, size_t size)
{
    int ret;
    impl_aes_ctx_t *impl_ctx = NULL;
    int mode;

    if (ctx == NULL) {
        return HAL_CRYPT_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        return HAL_CRYPT_BAD_PARAMETERS;
    }

    impl_ctx = (impl_aes_ctx_t *)ctx;
    mode = impl_ctx->mode;
    ret = impl_aes_crypt_cfb128(
              &(impl_ctx->ctx), mode, size,
              &(impl_ctx->offset), (unsigned char *)impl_ctx->iv,
              (const unsigned char *)src, (unsigned char *)dst);
    if (ret != HAL_CRYPT_SUCCESS) {
        if (impl_ctx->mode) {
            LS_HAL_LOG("impl_aes_ctr_process: encrypt fail!\n");
        } else {
            LS_HAL_LOG("impl_aes_ctr_process: decrypt fail!\n");
        }
        return HAL_CRYPT_ERROR;
    }

    return HAL_CRYPT_SUCCESS;
}
