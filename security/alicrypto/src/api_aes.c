/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "ali_crypto.h"
#include "ls_hal_crypt.h"

static int _get_pkcs_padding(unsigned char *input, size_t input_len, size_t *output_len)
{
    unsigned char i;
    unsigned char padding;

    padding = input[input_len - 1];

    for (i = padding; i > 0; i--) {
        if (input[input_len - i] != padding) {
            CRYPTO_DBG_LOG("pkcs unpadding fail\n");
            return -1;
        }
    }

    *output_len = input_len - padding;

    return 0;
}

ali_crypto_result ali_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    if (size == NULL) {
        CRYPTO_DBG_LOG("invalid size\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch(type) {
        case AES_ECB:
            *size = sizeof(api_aes_ctx_t) + ls_hal_aes_ecb_get_size();
            break;
        case AES_CBC:
            *size = sizeof(api_aes_ctx_t) + ls_hal_aes_cbc_get_size();
            break;
        case AES_CTR:
            *size = sizeof(api_aes_ctx_t) + ls_hal_aes_ctr_get_size();
            break;
        case AES_CFB8:
        case AES_CFB128:
            *size = sizeof(api_aes_ctx_t) + ls_hal_aes_cfb_get_size();
            break;
        default:
            CRYPTO_DBG_LOG("not support type(%d)\n", type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context)
{
    ali_crypto_result ret;
    api_aes_ctx_t *ctx = NULL;

    if (key1 == NULL || context == NULL) {
        CRYPTO_DBG_LOG("invalid key1/context\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (keybytes != 16 && keybytes != 24 && keybytes != 32) {
        CRYPTO_DBG_LOG("bad keybytes(%d)\n", keybytes);
        return ALI_CRYPTO_LENGTH_ERR;
    }

    ctx = (api_aes_ctx_t *)context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);
    ctx->is_enc = is_enc;
    ctx->type = type;

    switch(ctx->type) {
        case AES_ECB:
            ctx->hal_size = ls_hal_aes_ecb_get_size();
            ret = ls_hal_aes_ecb_init(ctx->hal_ctx, is_enc, key1, keybytes);
            break;
        case AES_CBC:
            ctx->hal_size = ls_hal_aes_cbc_get_size();
            ret = ls_hal_aes_cbc_init(ctx->hal_ctx, is_enc, key1, keybytes, (uint8_t *)iv);
            break;
        case AES_CTR:
            ctx->hal_size = ls_hal_aes_ctr_get_size();
            ret = ls_hal_aes_ctr_init(ctx->hal_ctx, is_enc, key1, keybytes, (uint8_t *)iv);
            break;
        case AES_CFB8:
        case AES_CFB128:
            ctx->hal_size = ls_hal_aes_cfb_get_size();
            ret = ls_hal_aes_cfb_init(ctx->hal_ctx, is_enc, key1, keybytes, (uint8_t *)iv);
            break;
        default:
            CRYPTO_DBG_LOG("not support type(%d)\n", ctx->type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hal aes(%d) get size fail, 0x%x\n", type, ret);
    }

    return ret;
}

ali_crypto_result ali_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context)
{
    ali_crypto_result  ret = ALI_CRYPTO_SUCCESS;
    api_aes_ctx_t *ctx = NULL;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid context\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL || dst == NULL || size == 0) {
        CRYPTO_DBG_LOG("invalid src/dst/size(%d)\n", size);
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_aes_ctx_t *)context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);

    switch(ctx->type){
        case AES_ECB:
            ret = ls_hal_aes_ecb_process(ctx->hal_ctx, src, dst, size);
            break;
        case AES_CBC:
            ret = ls_hal_aes_cbc_process(ctx->hal_ctx, src, dst, size);
            break;
        case AES_CTR:
            ret = ls_hal_aes_ctr_process(ctx->hal_ctx, src, dst, size);
            break;
        case AES_CFB8:
            ret = ls_hal_aes_cfb_process(ctx->hal_ctx, src, dst, size);
            break;
        case AES_CFB128:
            ret = ls_hal_aes_cfb128_process(ctx->hal_ctx, src, dst, size);
            break;
        default:
            CRYPTO_DBG_LOG("not support type(%d)\n", ctx->type);
            return ALI_CRYPTO_NOSUPPORT;
    }

    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hal aes(%d) process fail, 0x%x\n", ctx->type, ret);
    }

    return ret;
}

static ali_crypto_result _aes_final(const uint8_t *src, size_t src_size,
                                    uint8_t *dst, size_t *dst_size,
                                    sym_padding_t padding,
                                    void *   context)
{
    ali_crypto_result ret = ALI_CRYPTO_SUCCESS;
    api_aes_ctx_t *ctx = (api_aes_ctx_t *) context;
    uint32_t padding_len = 0;
    uint8_t *tmp_buf = NULL;
    size_t tmp_size = 0;

    if (padding == SYM_PKCS5_PAD) {
        if (ctx->is_enc) {
            padding_len = AES_BLOCK_SIZE - src_size % AES_BLOCK_SIZE;
            tmp_size = src_size + padding_len;
        } else {
            tmp_size = src_size;
        }
    } else if (padding == SYM_NOPAD) {
        tmp_size = src_size;
    } else {
        CRYPTO_DBG_LOG("padding type(%d) not supported\n", padding);
        return ALI_CRYPTO_NOSUPPORT;
    }

    if (ctx->is_enc && *dst_size < tmp_size) {
        CRYPTO_DBG_LOG("short buffer(%d) < %d\n", *dst_size, tmp_size);
        *dst_size = tmp_size;
        return ALI_CRYPTO_SHORT_BUFFER;
    }

    tmp_buf = ls_osa_malloc(tmp_size);
    if (NULL == tmp_buf) {
        CRYPTO_DBG_LOG("malloc(%d) failed\n", src_size + AES_BLOCK_SIZE);
        return ALI_CRYPTO_OUTOFMEM;
    }

    memcpy(tmp_buf, src, src_size);
    if (padding_len) {
        memset(tmp_buf + src_size, padding_len, padding_len);
    }

    switch(ctx->type) {
        case AES_ECB:
            ret = ls_hal_aes_ecb_process(ctx->hal_ctx, tmp_buf, tmp_buf, tmp_size);
            break;
        case AES_CBC:
            ret = ls_hal_aes_cbc_process(ctx->hal_ctx, tmp_buf, tmp_buf, tmp_size);
            break;
        case AES_CTR:
            ret = ls_hal_aes_ctr_process(ctx->hal_ctx, tmp_buf, tmp_buf, tmp_size);
            break;
        case AES_CFB8:
            ret = ls_hal_aes_cfb_process(ctx->hal_ctx, tmp_buf, tmp_buf, tmp_size);
            break;
        case AES_CFB128:
            ret = ls_hal_aes_cfb128_process(ctx->hal_ctx, tmp_buf, tmp_buf, tmp_size);
            break;
        default:
            CRYPTO_DBG_LOG("aes type(%d) not supported\n", ctx->type);
            goto _out;
    }

    if (ALI_CRYPTO_SUCCESS != ret) {
        CRYPTO_DBG_LOG("hal aes(%d) process fail, 0x%x\n", ctx->type, ret);
        goto _out;
    }

    /* unpadding for decrypt */
    if ( !ctx->is_enc && padding == SYM_PKCS5_PAD) {
        if (_get_pkcs_padding(tmp_buf, tmp_size, &tmp_size)) {
            CRYPTO_DBG_LOG("get pkcs padding fail\n");
            goto _out;
        }
    }

    if (*dst_size < tmp_size) {
        CRYPTO_DBG_LOG("short buffer(%d) < %d\n", *dst_size, tmp_size);
        ret = ALI_CRYPTO_SHORT_BUFFER;
        *dst_size = tmp_size;
        goto _out;
    }

    *dst_size = tmp_size;
    if (dst == NULL) {
        CRYPTO_DBG_LOG("NULL dst\n");
        ret = ALI_CRYPTO_INVALID_ARG;
        goto _out;
    }

    memcpy(dst, tmp_buf, tmp_size);

_out:
    if (tmp_buf != NULL) {
        ls_osa_free(tmp_buf);
    }

    return ret;
}

ali_crypto_result ali_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context)
{
    ali_crypto_result  ret = ALI_CRYPTO_SUCCESS;
    api_aes_ctx_t *ctx;

    if (context == NULL) {
        CRYPTO_DBG_LOG("invalid context\n");
        return ALI_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL || src_size == 0) {
        if (dst_size != NULL) {
            *dst_size = 0;
        }
        return ALI_CRYPTO_SUCCESS;
    }

    ctx = (api_aes_ctx_t *)context;
    ctx->hal_ctx = (char *)&(ctx->hal_ctx) + sizeof(ctx->hal_ctx);

    if (padding != SYM_NOPAD && padding != SYM_PKCS5_PAD) {
        CRYPTO_DBG_LOG("not support this padding, %d\n", padding);
        return ALI_CRYPTO_NOSUPPORT;
    }

    if ((src == NULL && src_size != 0) ||
        ((dst_size != NULL) && (dst == NULL && *dst_size != 0)) ||
        context == NULL) {
        CRYPTO_DBG_LOG("bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if (dst_size == NULL) {
        CRYPTO_DBG_LOG("invalid dst_size\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    if ((0 != *dst_size) && (dst == NULL)) {
        CRYPTO_DBG_LOG("NULL dst but non-zero dst_size(%d)\n", *dst_size);
        return ALI_CRYPTO_INVALID_ARG;
    }

    switch(ctx->type) {
        case AES_ECB:
        case AES_CBC:
        case AES_CTR:
            ret = _aes_final(src, src_size, dst, dst_size, padding, (void *)ctx);
            break;
        case AES_CFB8:
        case AES_CFB128:
            if (padding != SYM_NOPAD) {
                CRYPTO_DBG_LOG("AES CFB only supports NO_PAD\n");
                return ALI_CRYPTO_NOSUPPORT;
            }

            ret = _aes_final(src, src_size, dst, dst_size, SYM_NOPAD, (void *)ctx);
            break;
        default:
            CRYPTO_DBG_LOG("invalid type(%d)\n", ctx->type);
            return ALI_CRYPTO_NOSUPPORT;
    }
    
    if (ret != ALI_CRYPTO_SUCCESS) {
        CRYPTO_DBG_LOG("_aes_final type(%d) fail(%08x)\n", ctx->type, ret);
    }

    return ret;
}

ali_crypto_result ali_aes_reset(void *context)
{
    api_aes_ctx_t *ctx;
    
    if (context == NULL) {
        CRYPTO_DBG_LOG("bad input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    ctx = (api_aes_ctx_t *)context;
    memset(ctx->hal_ctx, 0, ctx->hal_size);

    return ALI_CRYPTO_SUCCESS;
}
