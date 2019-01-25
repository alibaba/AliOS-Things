/**
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 */

#include "sal_crypto.h"
#include "ali_crypto.h"

#ifdef ALI_CRYPTO_SM4
/* pkcs5 only support 8 bytes block size
 * pcks7 support 8,16 bytes block size, so in sm4 pkcs5 equal pkcs7 */
/*
 * output:  buf
 * output_len: block size
 * data_len: cur_data_len */
static void _add_pkcs_padding(unsigned char *output, size_t output_len,
        size_t data_len)
{
    size_t padding_len = output_len - data_len;
    unsigned char i;

    for(i = 0; i < padding_len; i++) {
        output[data_len + i] = (unsigned char)padding_len;
    }
}

static int _get_pkcs_padding(unsigned char *input, size_t input_len,
        size_t *data_len)
{
    size_t i, pad_idx;
    unsigned char padding_len, bad = 0;

    if(NULL == input || NULL == data_len) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    padding_len = input[input_len - 1];
    *data_len = input_len - padding_len;
    /* Avoid logical || since it results in a branch */
    bad |= padding_len > input_len;
    bad |= padding_len == 0;

    /* The number of bytes checked must be independent of padding_len,
     * so pick input_len, which is usually 8 or 16 (one block) */
    pad_idx = input_len - padding_len;
    for(i = 0; i < input_len; i++) {
        bad |= (input[i] ^ padding_len) * (i >= pad_idx);
    }
    return (ALI_CRYPTO_INVALID_PADDING * (bad != 0));
}


static ali_crypto_result _ali_sm4_ecb_final(const uint8_t *src, size_t src_size,
                                            uint8_t *dst, size_t *dst_size,
                                            sym_padding_t padding,
                                            sm4_ctx_t *ctx)
{
    int ret = ALI_CRYPTO_SUCCESS;
    uint8_t block[SM4_BLOCK_SIZE];
    int mode;
    size_t round = 0;
    size_t data_len;
    
    if (ctx == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ecb_final: invalid context!\n");
    }

    if (!(padding == SYM_NOPAD || padding == SYM_PKCS5_PAD)) {
        /* not support zero padding */
        PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                "ecb_final: only support no-padding and pkcs5/7!\n");
    }

    if (padding == SYM_NOPAD) {
        if (src == NULL || src_size == 0) {
            if (dst_size != NULL) {
                *dst_size = 0;
            }
            return ALI_CRYPTO_SUCCESS;
        }
    } else if (padding == SYM_PKCS5_PAD){
        /* pkcs5 finish must have input data */
        if (src == NULL || src_size == 0) {
            if (dst_size != NULL) {
                *dst_size = 0;
            }
            return ALI_CRYPTO_INVALID_ARG;
        }
    }

    if (dst_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ecb_final: invalid arg!\n");
    }

    if ((*dst_size != 0) && (dst == NULL)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ecb_final: invalid arg!\n");
    }

    if (ctx->is_enc) {
        mode = ALI_ALGO_SM4_ENCRYPT;
    } else {
        mode = ALI_ALGO_SM4_DECRYPT;
    }

    if (padding == SYM_NOPAD) {
        if (src_size % SM4_BLOCK_SIZE != 0) {
            PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                "ecb_final: no pad invalid size(%d vs %d)\n",
                (int)src_size, (int)*dst_size);
        }
        if (src_size > *dst_size) {
            *dst_size = src_size;
            PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                "ecb_final: no pad short buffer\n");
        }
    } else if (padding == SYM_PKCS5_PAD) {
        if (ctx->is_enc) {
            if ((src_size + (SM4_BLOCK_SIZE - src_size % SM4_BLOCK_SIZE)) > *dst_size) {
                *dst_size = src_size + (SM4_BLOCK_SIZE - src_size % SM4_BLOCK_SIZE);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                        "ecb_final: enc pkcs short buffer(%d vs %d)\n",
                        (int)src_size, (int)*dst_size);
            }
        } else {
            if (src_size % SM4_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_INVALID_PADDING,
                    "ecb_final: cipher size is not block align(%d)\n", (int)src_size);
            }
            if ((src_size - SM4_BLOCK_SIZE) > *dst_size) {
                ret = ali_algo_sm4_crypt_ecb(&(ctx->ctx), mode,
                                             src + (src_size - SM4_BLOCK_SIZE),
                                             block);
                if (ret != 0) {
                    PRINT_RET(ALI_CRYPTO_ERROR,
                            "ecb_final: ali_algo_sm4_crypt_ecb fail(%d)\n", ret);
                }

                ret = _get_pkcs_padding(block, SM4_BLOCK_SIZE, &data_len);
                if (ret != 0) {
                    PRINT_RET(ALI_CRYPTO_ERROR, "get pkcs padding fail(0x%08x)\n", ret);
                }
                *dst_size = src_size - (SM4_BLOCK_SIZE - data_len);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                    "ecb_final: dec pkcs short buffer(%d vs %d)\n",
                    (int)src_size, (int)*dst_size);
            }
        }
    }

    if (ALI_ALGO_SM4_ENCRYPT == mode) {
        /* encrypt */
        size_t cur_len;
        round = 0;
        while (round < (src_size / SM4_BLOCK_SIZE)) {
            ret = ali_algo_sm4_crypt_ecb(&(ctx->ctx), mode,
                                             src + round * SM4_BLOCK_SIZE,
                                             dst + round * SM4_BLOCK_SIZE);
            if (ret != 0) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "ecb_final: ali_algo_sm4_crypt_ecb fail(%d)\n", ret);
            }
            round++;
        }

        cur_len = round * SM4_BLOCK_SIZE;
        if (padding == SYM_NOPAD) {
            if (src_size != cur_len) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "ecb_final: src size not block align(%u)\n", cur_len);
            }

            *dst_size = cur_len;
        } else if (padding == SYM_PKCS5_PAD) {
            osa_memcpy(block, src + cur_len, src_size - cur_len);
            _add_pkcs_padding(block, SM4_BLOCK_SIZE, src_size - cur_len);
            ret = ali_algo_sm4_crypt_ecb(&(ctx->ctx), mode,
                                     block, dst + cur_len);

            if (ret != 0) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "ecb_final: ali_algo_sm4_crypt_ecb fail(%d)\n",
                         ret);
	    }

            *dst_size = cur_len + SM4_BLOCK_SIZE;
        }
    } else {
        /* decrypt */
        uint8_t *tmp_dst = osa_malloc(src_size);

        if (tmp_dst == NULL) {
            PRINT_RET(ALI_CRYPTO_OUTOFMEM, "ecb_final: out of memory\n");
        }
        round = 0;
        while (round < (src_size / SM4_BLOCK_SIZE)) {
            ret = ali_algo_sm4_crypt_ecb(&(ctx->ctx), mode,
                                             src + round * SM4_BLOCK_SIZE,
                                             tmp_dst + round * SM4_BLOCK_SIZE);
            if (ret != 0) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "ecb_final: ali_algo_sm4_crypt_ecb fail(%d)\n", ret);
            }
            round++;
        }
        if (padding == SYM_NOPAD) {
            if (src_size > *dst_size) {
                *dst_size = src_size;
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                        "ecb_final: dec no pad short buffer(src size %u vs dst size %u)\n",
                        src_size, *dst_size);
            }

            osa_memcpy(dst, tmp_dst, src_size);
            *dst_size = src_size;
        } else if (padding == SYM_PKCS5_PAD) {
            ret = _get_pkcs_padding(tmp_dst + (round - 1) * SM4_BLOCK_SIZE,
                    SM4_BLOCK_SIZE, &data_len);
            if (ret != 0) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "ecb_final: get pkcs padding fail(0x%08x)\n", ret);
            }
            if (*dst_size < src_size - (SM4_BLOCK_SIZE - data_len)) {
                osa_free(tmp_dst);
                *dst_size = src_size - (SM4_BLOCK_SIZE - data_len);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                        "ecb_final: dec pkcs short buffer\n");
            }

            *dst_size = src_size - (SM4_BLOCK_SIZE - data_len);

            if (*dst_size > 0){
                osa_memcpy(dst, tmp_dst , *dst_size);
            }
        }
        osa_free(tmp_dst);
        tmp_dst = NULL;
    }
    return ((ali_crypto_result)ret);
}

#if defined(ALI_ALGO_CIPHER_MODE_CBC)
static ali_crypto_result _ali_sm4_cbc_final(const uint8_t *src, size_t src_size,
                                            uint8_t *dst, size_t *dst_size,
                                            sym_padding_t padding,
                                            sm4_ctx_t *ctx)
{
    int ret = ALI_CRYPTO_SUCCESS;
    int mode;
    size_t data_len;
    uint8_t *tmp_dst = NULL;

    if (ctx == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "cbc_final: invalid context!\n");
    }

    if (!(padding == SYM_NOPAD || padding == SYM_PKCS5_PAD)) {
        PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                "ecb_final: only support no-padding and pkcs5/7!\n");
    }

    if (padding == SYM_NOPAD) {
        if (src == NULL || src_size == 0) {
            if (dst_size != NULL) {
                *dst_size = 0;
            }
            return ALI_CRYPTO_SUCCESS;
        }
    } else {
        /* pkcs5 finish must have input data */
        if (src == NULL || src_size == 0) {
            if (dst_size != NULL) {
                *dst_size = 0;
            }
            return ALI_CRYPTO_INVALID_ARG;
        }
    }

    if (dst_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "cbc_final: invalid arg!\n");
    }

    if ((*dst_size != 0) && (dst == NULL)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "cbc_final: invalid arg!\n");
    }
    if (ctx->is_enc) {
        mode = ALI_ALGO_SM4_ENCRYPT;
    } else {
        mode = ALI_ALGO_SM4_DECRYPT;
    }

    if (padding == SYM_NOPAD) {
        if (src_size % SM4_BLOCK_SIZE != 0) {
            PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                    "cbc_final: no pad invalid size(%d vs %d)\n",
                    (int)src_size, (int)*dst_size);
        }
        if (src_size > *dst_size){
            *dst_size = src_size;
            PRINT_RET(ALI_CRYPTO_SHORT_BUFFER, "cbc_final: short buffer\n");
        }
    } else {
        if (ctx->is_enc) {
            if ((src_size + (SM4_BLOCK_SIZE - src_size % SM4_BLOCK_SIZE)) > *dst_size) {
                *dst_size = src_size + (SM4_BLOCK_SIZE - src_size % SM4_BLOCK_SIZE);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                        "ecb_final: enc pkcs5 short buffer(%d vs %d)\n",
                        (int)src_size, (int)*dst_size);
            }
        } else {
            if (src_size % SM4_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_INVALID_PADDING,
                    "cbc_final: cipher size is not block align(%d)\n", (int)src_size);
            }
            if ((src_size - SM4_BLOCK_SIZE) > *dst_size) {
                tmp_dst = osa_malloc(src_size);
                if (tmp_dst == NULL) {
                    PRINT_RET(ALI_CRYPTO_OUTOFMEM, "cbc_final: out of memory\n");
                }
                ret = ali_algo_sm4_crypt_cbc(&(ctx->ctx), mode, src_size,
                          (unsigned char *)ctx->iv, src, tmp_dst);
                if (ret != 0) {
                    osa_free(tmp_dst);
                    PRINT_RET(ALI_CRYPTO_ERROR,
                            "cbc_final: ali_algo_sm4_crypt_cbc fail(%d)\n", ret);
                }

                ret = _get_pkcs_padding(tmp_dst + src_size - SM4_BLOCK_SIZE,
                        SM4_BLOCK_SIZE, &data_len);
                if (ret != 0) {
                    osa_free(tmp_dst);
                    PRINT_RET(ALI_CRYPTO_ERROR,
                            "cbc_final: get pkcs padding fail(0x%08x)\n", ret);
                }
                *dst_size = src_size - (SM4_BLOCK_SIZE - data_len);

                osa_free(tmp_dst);
                tmp_dst = NULL;
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                    "cbc_final: dec pkcs short buffer(%d vs %d)\n",
                    (int)src_size, (int)*dst_size);
            }
        }
    }

    if (ALI_ALGO_SM4_ENCRYPT == mode) {
        /* encrypt, short buffer will be blocked above */
        size_t cur_len;
        uint8_t block[SM4_BLOCK_SIZE];

        cur_len = src_size & (~(SM4_BLOCK_SIZE - 1));
        ret = ali_algo_sm4_crypt_cbc(&(ctx->ctx), mode,
                cur_len, (unsigned char *)ctx->iv,
                (const unsigned char *)src, (unsigned char *)dst);
        if (ret != 0) {
            PRINT_RET(ALI_CRYPTO_ERROR,
                    "cbc_final: ali_algo_sm4_crypt_cbc fail(%d)\n", ret);
        }

        if (padding == SYM_PKCS5_PAD) {
            osa_memcpy(block, src + cur_len, src_size - cur_len);
            _add_pkcs_padding(block, SM4_BLOCK_SIZE, src_size - cur_len);
            ret = ali_algo_sm4_crypt_cbc(&(ctx->ctx), mode,
                SM4_BLOCK_SIZE, (unsigned char *)ctx->iv,
                (const unsigned char *)block,
                (unsigned char *)(dst + cur_len));
            if (ret != 0) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "cbc_final: ali_algo_sm4_crypt_cbc fail(%d)\n", ret);
            }
            *dst_size = cur_len + SM4_BLOCK_SIZE;
        }
    } else {
        /* dencrypt */
        if (padding == SYM_NOPAD) {
            ret = ali_algo_sm4_crypt_cbc(&(ctx->ctx), mode,
                src_size, (unsigned char *)ctx->iv,
                (const unsigned char *)src,
                (unsigned char *)dst);
            if (ret != 0) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "cbc_final: ali_algo_sm4_crypt_cbc fail(%d)\n", ret);
            }

            *dst_size = src_size;
        } else {
            /* avoid dst size is not enougth */
            tmp_dst = osa_malloc(src_size);
            if (tmp_dst == NULL) {
                PRINT_RET(ALI_CRYPTO_ERROR, "cbc_final: out of memory\n");
            }
            ret = ali_algo_sm4_crypt_cbc(&(ctx->ctx), mode,
                    src_size, (unsigned char *)ctx->iv,
                    (const unsigned char *)src, (unsigned char *)tmp_dst);
            if (ret != 0) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "cbc_final: ali_algo_sm4_crypt_cbc fail(%d)\n", ret);
            }

            ret = _get_pkcs_padding(tmp_dst + src_size - SM4_BLOCK_SIZE,
                    SM4_BLOCK_SIZE, &data_len);
            if (ret != 0) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                        "cbc_final: get pkcs padding fail(0x%08x)\n", ret);
            }
            if (*dst_size < src_size - (SM4_BLOCK_SIZE - data_len)) {
                osa_free(tmp_dst);
                *dst_size = src_size - (SM4_BLOCK_SIZE - data_len);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                        "cbc_final: dec pkcs short buffer\n");
            }
            osa_memcpy(dst, tmp_dst, src_size - (SM4_BLOCK_SIZE - data_len));
            *dst_size = src_size - (SM4_BLOCK_SIZE - data_len);
            osa_free(tmp_dst);
        }
    }
    return (ali_crypto_result)ret;
}
#endif /* ALI_ALGO_CIPHER_MODE_CBC */

ali_crypto_result sal_sm4_get_ctx_size(sm4_type_t type, size_t *size)
{
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sm4_get_ctx_size: bad input!\n");
    }
    switch(type) {
        case SM4_ECB:
#if defined(ALI_ALGO_CIPHER_MODE_CBC)
        case SM4_CBC:
#endif
            break;
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                    "sal_sm4_get_ctx_size: invalid sm4 type(%d)\n", type);
    }
    *size = sizeof(sm4_ctx_t);
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_sm4_init(sm4_type_t type, bool is_enc,
                      const uint8_t *key1, const uint8_t *key2,
                      size_t keybytes, const uint8_t *iv, void *context)
{
    int ret = ALI_CRYPTO_SUCCESS;
    sm4_ctx_t *sm4_ctx;

    (void)key2;
    if (key1 == NULL || context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                    "sal_sm4_init: bad input args!\n");
    }
    if (keybytes != 16) {
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                    "sal_sm4_init: bad key lenth(%d)\n", (int)keybytes);
    }
    sm4_ctx = (sm4_ctx_t *)context;
    if ((IS_VALID_CTX_MAGIC(sm4_ctx->magic) &&
        sm4_ctx->status != CRYPTO_STATUS_FINISHED) &&
        sm4_ctx->status != CRYPTO_STATUS_CLEAN) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE,
                    "sal_sm4_init: bad status(%d)\n", (int)sm4_ctx->status);
    }
    switch(type) {
        case SM4_ECB:
            break;
#if defined(ALI_ALGO_CIPHER_MODE_CBC)
        case SM4_CBC: {
            if (iv == NULL) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                    "sal_sm4_init: cbc iv is null\n");
            }

            osa_memcpy(sm4_ctx->iv, iv, 16);
            break;
        }
#endif /* ALI_ALGO_CIPHER_MODE_CBC */
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                    "sal_sm4_init: invalid sm4 type(%d)\n", type);
    }
    ali_algo_sm4_init(&(sm4_ctx->ctx));
    sm4_ctx->is_enc = is_enc;

    ret = ali_algo_sm4_setkey(&(sm4_ctx->ctx), key1, keybytes * 8);

    if (ret != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                    "sal_sm4_init: start mode(%d) fail(%d)\n", type, ret);
    }
    sm4_ctx->offset = 0;
    sm4_ctx->type = type;
    sm4_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(sm4_ctx->magic);
    return ALI_CRYPTO_SUCCESS;
}


ali_crypto_result sal_sm4_process(const uint8_t *src, uint8_t *dst,
                                  size_t size, void *context)
{
    int ret = ALI_CRYPTO_SUCCESS;
    sm4_ctx_t *sm4_ctx;
    int mode;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "sal_sm4_process: bad ctx!\n");
    }
    if (src == NULL || dst == NULL || size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm4_process: bad args!\n");
    }
    sm4_ctx = (sm4_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(sm4_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "sal_sm4_process: bad magic!\n");
    }
    if ((sm4_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (sm4_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE, "sal_sm4_update: bad status(%d)\n",
                (int)sm4_ctx->status);
    }
    if (sm4_ctx->is_enc) {
        mode = ALI_ALGO_SM4_ENCRYPT;
    } else {
        mode = ALI_ALGO_SM4_DECRYPT;
    }
    switch(sm4_ctx->type) {
        /* FIXME, limitation, size must be block size aigned */
        case SM4_ECB: {
            size_t cur_len = 0;
            if (size % SM4_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                        "sal_sm4_process: invalid size(%d)\n", (int)size);
            }
            while (cur_len < size) {
                ret = ali_algo_sm4_crypt_ecb(&(sm4_ctx->ctx), mode,
                                         src + cur_len, dst + cur_len);
                if (ret != 0) {
                    PRINT_RET(ALI_CRYPTO_ERROR, "ali_algo_sm4_crypt_ecb fail(%d)\n",
                             ret);
                }
                cur_len += SM4_BLOCK_SIZE;
            }
            break;
        }
#if defined(ALI_ALGO_CIPHER_MODE_CBC)
        case SM4_CBC: {
            if (size % SM4_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                    "sal_sm4_process: invalid size(%d)\n", (int)size);
            }

            ret = ali_algo_sm4_crypt_cbc(&(sm4_ctx->ctx), mode,
                    size, (unsigned char *)sm4_ctx->iv,
                    (const unsigned char *)src, (unsigned char *)dst);
            break;
        }
#endif /* ALI_ALGO_CIPHER_MODE_CBC */
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                "sal_sm4_process: invalid type(%d)\n", sm4_ctx->type);
    }

    if (ret != ALI_CRYPTO_SUCCESS) {
        if (sm4_ctx->is_enc) {
            ALI_CRYPTO_DBG_E("sal_sm4_process: encrypt(%d) fail!\n", sm4_ctx->type);
        } else {
            ALI_CRYPTO_DBG_E("sal_sm4_process: decrypt(%d) fail!\n", sm4_ctx->type);
        }
        return ALI_CRYPTO_ERROR;
    }

    sm4_ctx->status = CRYPTO_STATUS_PROCESSING;

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_sm4_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context)
{
    ali_crypto_result ret;
    sm4_ctx_t *sm4_ctx;

    if ((src == NULL && src_size != 0) ||
            ((dst_size != NULL) && (dst == NULL && *dst_size != 0))
            || context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm4_finish: bad input args!\n");
    }

    sm4_ctx = (sm4_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(sm4_ctx->magic)) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "sal_sm4_finish: bad magic!\n");
    }

    if ((sm4_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (sm4_ctx->status != CRYPTO_STATUS_PROCESSING)) {
        PRINT_RET(ALI_CRYPTO_ERR_STATE,
                "sal_sm4_finish: bad status(%d)\n", (int)sm4_ctx->status);
    }
    switch(sm4_ctx->type) {
        case SM4_ECB: {
            ret = _ali_sm4_ecb_final(
                      src, src_size, dst, dst_size,
                      padding, sm4_ctx);
            break;
        }
#if defined(ALI_ALGO_CIPHER_MODE_CBC)
        case SM4_CBC: {
            ret = _ali_sm4_cbc_final(
                      src, src_size, dst, dst_size,
                      padding, sm4_ctx);
            break;
        }
#endif /* ALI_ALGO_CIPHER_MODE_CBC */
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                    "sal_sm4_finish: invalid sm4 type(%d)\n", sm4_ctx->type);
    }
    if (ret != ALI_CRYPTO_SUCCESS) {
        ali_algo_sm4_free(&(sm4_ctx->ctx));
        PRINT_RET(ret, "sal_sm4_process: sm4 type(%d) final fail(%08x)\n",
                sm4_ctx->type, ret);
    }
    CLEAN_CTX_MAGIC(sm4_ctx->magic);
    sm4_ctx->status = CRYPTO_STATUS_FINISHED;
    sm4_ctx->offset = 0;

    ali_algo_sm4_free(&(sm4_ctx->ctx));
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_sm4_reset(void *context)
{
    sm4_ctx_t *sm4_ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm4_reset: bad input args!\n");
    }
    sm4_ctx = (sm4_ctx_t *)context;
    osa_memset(sm4_ctx, 0, sizeof(sm4_ctx_t));
    return ALI_CRYPTO_SUCCESS;
}
#endif
