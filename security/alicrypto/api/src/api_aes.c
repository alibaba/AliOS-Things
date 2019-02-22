/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "sal_crypto.h"
#include "ali_crypto.h"

#if (CONFIG_HAL_AES_ECB || CONFIG_HAL_AES_CBC || CONFIG_HAL_AES_CTR || CONFIG_HAL_AES_CFB)
#include "ls_hal_crypt.h"
#endif

/* pkcs5 only support 8 bytes block size
 * pcks7 support 8,16 bytes block size, so in aes pkcs5 equal pkcs7 */
/*
 * output:  buf
 * output_len: block size
 * data_len: cur_data_len */

typedef struct {
    aes_type_t        type;
    union{
#if (CONFIG_HAL_AES_ECB || CONFIG_HAL_AES_CBC || CONFIG_HAL_AES_CTR || CONFIG_HAL_AES_CFB)
        hal_aes_ctx_t     hal_ctx;
#endif
        aes_ctx_t         sal_ctx;
    };
} api_aes_ctx_t;

#if (CONFIG_HAL_AES_ECB || CONFIG_SAL_AES_ECB || CONFIG_HAL_AES_CBC || CONFIG_SAL_AES_CBC)
static void _add_pkcs_padding(unsigned char *output, size_t output_len,
                              size_t data_len)
{
    size_t        padding_len = output_len - data_len;
    unsigned char i;

    for (i = 0; i < padding_len; i++) {
        output[data_len + i] = (unsigned char)padding_len;
    }
}

static int _get_pkcs_padding(unsigned char *input, size_t input_len,
                             size_t *data_len)
{
    size_t        i, pad_idx;
    unsigned char padding_len, bad = 0;

    if (NULL == input || NULL == data_len) {
        return ALI_CRYPTO_INVALID_ARG;
    }

    padding_len = input[input_len - 1];
    *data_len   = input_len - padding_len;

    /* Avoid logical || since it results in a branch */
    bad |= padding_len > input_len;
    bad |= padding_len == 0;

    /* The number of bytes checked must be independent of padding_len,
     * so pick input_len, which is usually 8 or 16 (one block) */
    pad_idx = input_len - padding_len;
    for (i = 0; i < input_len; i++) {
        bad |= (input[i] ^ padding_len) * (i >= pad_idx);
    }

    return (ALI_CRYPTO_INVALID_PADDING * (bad != 0));
}
#endif

ali_crypto_result ali_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    ali_crypto_result result = 0;

    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input!\n");
    }
    switch(type){
#if (CONFIG_HAL_AES_ECB || CONFIG_SAL_AES_ECB)
        case AES_ECB:
            *size = sizeof(api_aes_ctx_t);
            break;
#endif
#if (CONFIG_HAL_AES_CBC || CONFIG_SAL_AES_CBC)
        case AES_CBC:
            *size = sizeof(api_aes_ctx_t);
            break;
#endif
#if (CONFIG_HAL_AES_CTR || CONFIG_SAL_AES_CTR)
        case AES_CTR:
            *size = sizeof(api_aes_ctx_t);
            break;
#endif
#if (CONFIG_HAL_AES_CFB || CONFIG_SAL_AES_CFB)
        case AES_CFB8:
            *size = sizeof(api_aes_ctx_t);
            break;
#endif
#if (CONFIG_SAL_AES_CFB)
        case AES_CFB128:
            *size = sizeof(api_aes_ctx_t);
            break;
#endif
        default:
            PRINT_RET(-1, "ali_aes_init: invalid type(%d)\n", type);
    }
    return result;
}

ali_crypto_result ali_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv,
                               void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    api_aes_ctx_t *ctx;

    if (key1 == NULL || context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    if (keybytes != 16 && keybytes != 24 && keybytes != 32) {
        PRINT_RET(ALI_CRYPTO_LENGTH_ERR, "bad key lenth(%d)\n", (int)keybytes);
    }
    ctx = (api_aes_ctx_t *) context;
    ctx->type = type;
    switch(type){
#if (CONFIG_HAL_AES_ECB || CONFIG_SAL_AES_ECB)
        case AES_ECB:
#if CONFIG_HAL_AES_ECB
            result = ls_hal_aes_ecb_init(&(ctx->hal_ctx), is_enc, key1, keybytes);
#else
            result = sal_aes_init(type, is_enc, key1, key2, keybytes, iv, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CBC || CONFIG_SAL_AES_CBC)
        case AES_CBC:
#if CONFIG_HAL_AES_CBC
            result = ls_hal_aes_cbc_init(&(ctx->hal_ctx), is_enc, key1, keybytes, (uint8_t *) iv);
#else
            result = sal_aes_init(type, is_enc, key1, key2, keybytes, iv, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CTR || CONFIG_SAL_AES_CTR)
        case AES_CTR:
#if CONFIG_HAL_AES_CTR
            result = ls_hal_aes_ctr_init(&(ctx->hal_ctx), is_enc, key1, keybytes, (uint8_t *) iv);
#else
            result = sal_aes_init(type, is_enc, key1, key2, keybytes, iv, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CFB || CONFIG_SAL_AES_CFB)
        case AES_CFB8:
#if CONFIG_HAL_AES_CFB
            result = ls_hal_aes_cfb_init(&(ctx->hal_ctx), is_enc, key1, keybytes, (uint8_t *) iv);
#else
            result = sal_aes_init(type, is_enc, key1, key2, keybytes, iv, &(ctx->sal_ctx));
#endif
            break;
#endif 
#if (CONFIG_SAL_AES_CFB)
        case AES_CFB128:
            result = sal_aes_init(type, is_enc, key1, key2, keybytes, iv, &(ctx->sal_ctx));
            break;
#endif
        default:
            PRINT_RET(-1, "ali_aes_init: invalid type(%d)\n", type);
    }
    return result;
}

ali_crypto_result ali_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    aes_type_t type;
    api_aes_ctx_t *ctx;

    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "bad ctx!\n");
    }

    if (src == NULL || dst == NULL || size == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad args!\n");
    }

    ctx = (api_aes_ctx_t *) context;
    type = ctx->type;
    switch(type){
#if (CONFIG_HAL_AES_ECB || CONFIG_SAL_AES_ECB)
        case AES_ECB:
#if CONFIG_HAL_AES_ECB
            result = ls_hal_aes_ecb_process(&(ctx->hal_ctx), src, dst, size);
#else
            result = sal_aes_process(src, dst, size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CBC || CONFIG_SAL_AES_CBC)
        case AES_CBC:
#if CONFIG_HAL_AES_CBC
            result = ls_hal_aes_cbc_process(&(ctx->hal_ctx), src, dst, size);
#else
            result = sal_aes_process(src, dst, size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CTR || CONFIG_SAL_AES_CTR)
        case AES_CTR:
#if CONFIG_HAL_AES_CTR
            result = ls_hal_aes_ctr_process(&(ctx->hal_ctx), src, dst, size);
#else
            result = sal_aes_process(src, dst, size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CFB || CONFIG_SAL_AES_CFB)
        case AES_CFB8:
#if CONFIG_HAL_AES_CFB
            result = ls_hal_aes_cfb_process(&(ctx->hal_ctx), src, dst, size);
#else
            result = sal_aes_process(src, dst, size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_SAL_AES_CFB)
        case AES_CFB128:
            result = sal_aes_process(src, dst, size, &(ctx->sal_ctx));
            break;
#endif
        default:
            PRINT_RET(-1, "ali_aes_process: invalid type(%d)\n", type);
    }
    return result;
}

#if (CONFIG_HAL_AES_ECB || CONFIG_SAL_AES_ECB)
static ali_crypto_result _aes_ecb_final(const uint8_t *src, size_t src_size,
                                            uint8_t *dst, size_t *dst_size,
                                            sym_padding_t padding,
                                            void *   context)
{
    int            ret = ALI_CRYPTO_SUCCESS;
    uint8_t        block[AES_BLOCK_SIZE];
    uint32_t       mode;
    size_t         round = 0;
    size_t         data_len;
#if CONFIG_HAL_AES_ECB
    hal_aes_ctx_t  *ctx = (hal_aes_ctx_t *)context;
#else
    aes_ctx_t  *ctx = (aes_ctx_t *)context;
#endif

    if (context == NULL) {
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
    } else if (padding == SYM_PKCS5_PAD) {
        /* pkcs5 finish must have input data */
        if (NULL == src || 0 == src_size) {
            if (dst_size != NULL) {
                *dst_size = 0;
            }
            return ALI_CRYPTO_INVALID_ARG;
        }
    }

    if (dst_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ecb_final: invalid arg!\n");
    }

    if ((0 != *dst_size) && (dst == NULL)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ecb_final: invalid arg!\n");
    }
#if CONFIG_HAL_AES_ECB
    mode = ctx->mode;
#else
    mode = ctx->is_enc;
#endif
    if (padding == SYM_NOPAD) {
        if (src_size % AES_BLOCK_SIZE != 0) {
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
        if (mode) {
            if ((src_size + (AES_BLOCK_SIZE - src_size % AES_BLOCK_SIZE)) >
                *dst_size) {
                *dst_size =
                  src_size + (AES_BLOCK_SIZE - src_size % AES_BLOCK_SIZE);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "ecb_final: enc pkcs short buffer(%d vs %d)\n",
                          (int)src_size, (int)*dst_size);
            }
        } else {
            if (src_size % AES_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_INVALID_PADDING,
                          "ecb_final: cipher size is not block align(%d)\n",
                          (int)src_size);
            }
            if ((src_size - AES_BLOCK_SIZE) > *dst_size) {
#if CONFIG_HAL_AES_ECB
                ret = ls_hal_aes_ecb_process(ctx, src + (src_size - AES_BLOCK_SIZE), block, AES_BLOCK_SIZE);
#else
                ret = sal_aes_process(src + (src_size - AES_BLOCK_SIZE), block, AES_BLOCK_SIZE, ctx);
#endif
                if (0 != ret) {
                    PRINT_RET(ALI_CRYPTO_ERROR,
                              "ecb_final: aes_crypt_ecb fail(%d)\n",
                              ret);
                }

                ret = _get_pkcs_padding(block, AES_BLOCK_SIZE, &data_len);
                if (0 != ret) {
                    PRINT_RET(ALI_CRYPTO_ERROR,
                              "get pkcs padding fail(0x%08x)\n", ret);
                }
                *dst_size = src_size - (AES_BLOCK_SIZE - data_len);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "ecb_final: dec pkcs short buffer(%d vs %d)\n",
                          (int)src_size, (int)*dst_size);
            }
        }
    }

    if (ALI_ALGO_AES_ENCRYPT == mode) {
        /* encrypt */
        size_t cur_len;
        round = 0;
        while (round < (src_size / AES_BLOCK_SIZE)) {
#if CONFIG_HAL_AES_ECB
            ret = ls_hal_aes_ecb_process(ctx, src + round * AES_BLOCK_SIZE, dst + round * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
#else
            ret = sal_aes_process(src + round * AES_BLOCK_SIZE, dst + round * AES_BLOCK_SIZE, AES_BLOCK_SIZE, ctx);
#endif
            if (0 != ret) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "ecb_final: aes_crypt_ecb fail(%d)\n", ret);
            }
            round++;
        }

        cur_len = round * AES_BLOCK_SIZE;
        if (padding == SYM_NOPAD) {
            if (src_size != cur_len) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "ecb_final: src size not block align(%d)\n", cur_len);
            }

            *dst_size = cur_len;
        } else if (padding == SYM_PKCS5_PAD) {
            osa_memcpy(block, src + cur_len, src_size - cur_len);
            _add_pkcs_padding(block, AES_BLOCK_SIZE, src_size - cur_len);
#if CONFIG_HAL_AES_ECB
            ret = ls_hal_aes_ecb_process(ctx, block, dst+cur_len, AES_BLOCK_SIZE);
#else
            ret = sal_aes_process(block, dst+cur_len, AES_BLOCK_SIZE, ctx);
#endif
            if (0 != ret) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "ecb_final: aes_crypt_ecb fail(%d)\n", ret);
            }

            *dst_size = cur_len + AES_BLOCK_SIZE;
        }
    } else {
        /* dencrypt */
        uint8_t *tmp_dst = osa_malloc(src_size);

        if (NULL == tmp_dst) {
            PRINT_RET(ALI_CRYPTO_OUTOFMEM, "ecb_final: out of memory\n");
        }
        round = 0;
        while (round < (src_size / AES_BLOCK_SIZE)) {
#if CONFIG_HAL_AES_ECB
            ret = ls_hal_aes_ecb_process(ctx, 
                                        src + round * AES_BLOCK_SIZE,
                                        tmp_dst + round * AES_BLOCK_SIZE,
                                        AES_BLOCK_SIZE);
#else
            ret = sal_aes_process(src + round * AES_BLOCK_SIZE,
                                        tmp_dst + round * AES_BLOCK_SIZE,
                                        AES_BLOCK_SIZE, ctx);
#endif
            if (0 != ret) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "ecb_final: aes_crypt_ecb fail(%d)\n", ret);
            }
            round++;
        }

        if (padding == SYM_NOPAD) {
            if (src_size > *dst_size) {
                *dst_size = src_size;
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "ecb_final: dec no pad short buffer(src size %d vs "
                          "dst size %d)\n",
                          src_size, *dst_size);
            }

            osa_memcpy(dst, tmp_dst, src_size);
            *dst_size = src_size;
        } else if (padding == SYM_PKCS5_PAD) {
            ret = _get_pkcs_padding(tmp_dst + (round - 1) * AES_BLOCK_SIZE,
                                    AES_BLOCK_SIZE, &data_len);
            if (0 != ret) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "ecb_final: get pkcs padding fail(0x%08x)\n", ret);
            }
            if (*dst_size < src_size - (AES_BLOCK_SIZE - data_len)) {
                osa_free(tmp_dst);
                *dst_size = src_size - (AES_BLOCK_SIZE - data_len);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "ecb_final: dec pkcs short buffer\n");
            }

            *dst_size = src_size - (AES_BLOCK_SIZE - data_len);

            if(*dst_size > 0){
                osa_memcpy(dst, tmp_dst, *dst_size);
            }
        }
        osa_free(tmp_dst);
        tmp_dst = NULL;
    }
    return (ali_crypto_result)ret;
}
#endif

#if (CONFIG_HAL_AES_CBC || CONFIG_SAL_AES_CBC)
static ali_crypto_result _aes_cbc_final(const uint8_t *src, size_t src_size,
                                            uint8_t *dst, size_t *dst_size,
                                            sym_padding_t padding,
                                            void *   context)
{
    int           ret = ALI_CRYPTO_SUCCESS;
    int           mode;
    size_t        data_len;
    uint8_t       *tmp_dst = NULL;
#if CONFIG_HAL_AES_CBC
    hal_aes_ctx_t  *ctx = (hal_aes_ctx_t *)context;
#else
    aes_ctx_t  *ctx = (aes_ctx_t *)context;
#endif

    if (context == NULL) {
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
        if (NULL == src || 0 == src_size) {
            if (dst_size != NULL) {
                *dst_size = 0;
            }
            return ALI_CRYPTO_INVALID_ARG;
        }
    }

    if (dst_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "cbc_final: invalid arg!\n");
    }

    if ((0 != *dst_size) && (dst == NULL)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "cbc_final: invalid arg!\n");
    }
#if CONFIG_HAL_AES_CBC
    mode = ctx->mode;
#else
    mode = ctx->is_enc;
#endif
    if (padding == SYM_NOPAD) {
        if (src_size % AES_BLOCK_SIZE != 0) {
            PRINT_RET(ALI_CRYPTO_LENGTH_ERR,
                      "cbc_final: no pad invalid size(%d vs %d)\n",
                      (int)src_size, (int)*dst_size);
        }
        if (src_size > *dst_size) {
            *dst_size = src_size;
            PRINT_RET(ALI_CRYPTO_SHORT_BUFFER, "cbc_final: short buffer\n");
        } else {
            *dst_size = src_size;
        }
    } else {
        if (mode) {
            if ((src_size + (AES_BLOCK_SIZE - src_size % AES_BLOCK_SIZE)) >
                *dst_size) {
                *dst_size =
                  src_size + (AES_BLOCK_SIZE - src_size % AES_BLOCK_SIZE);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "ecb_final: enc pkcs5 short buffer(%d vs %d)\n",
                          (int)src_size, (int)*dst_size);
            }
        } else {
            if (src_size % AES_BLOCK_SIZE != 0) {
                PRINT_RET(ALI_CRYPTO_INVALID_PADDING,
                          "cbc_final: cipher size is not block align(%d)\n",
                          (int)src_size);
            }
            if ((src_size - AES_BLOCK_SIZE) > *dst_size) {
                tmp_dst = osa_malloc(src_size);
                if (NULL == tmp_dst) {
                    PRINT_RET(ALI_CRYPTO_OUTOFMEM,
                              "cbc_final: out of memory\n");
                }
#if CONFIG_HAL_AES_CBC
                ret = ls_hal_aes_cbc_process(ctx, src, tmp_dst, src_size);
#else
                ret = sal_aes_process(src, tmp_dst, src_size, ctx);
#endif
                if (0 != ret) {
                    osa_free(tmp_dst);
                    PRINT_RET(ALI_CRYPTO_ERROR,
                              "cbc_final: aes_crypt_cbc fail(%d)\n",
                              ret);
                }

                ret = _get_pkcs_padding(tmp_dst + src_size - AES_BLOCK_SIZE,
                                        AES_BLOCK_SIZE, &data_len);
                if (0 != ret) {
                    osa_free(tmp_dst);
                    PRINT_RET(ALI_CRYPTO_ERROR,
                              "cbc_final: get pkcs padding fail(0x%08x)\n",
                              ret);
                }
                *dst_size = src_size - (AES_BLOCK_SIZE - data_len);

                osa_free(tmp_dst);
                tmp_dst = NULL;
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "cbc_final: dec pkcs short buffer(%d vs %d)\n",
                          (int)src_size, (int)*dst_size);
            }
        }
    }

    if (ALI_ALGO_AES_ENCRYPT == mode) {
        /* encrypt, short buffer will be blocked above */
        size_t  cur_len;
        uint8_t block[AES_BLOCK_SIZE];

        cur_len = src_size & (~(AES_BLOCK_SIZE - 1));
#if CONFIG_HAL_AES_CBC
        ret = ls_hal_aes_cbc_process(ctx, src, dst, cur_len);
#else
        ret = sal_aes_process(src, dst, cur_len, ctx);
#endif
        if (0 != ret) {
            PRINT_RET(ALI_CRYPTO_ERROR,
                      "cbc_final: aes_crypt_cbc fail(%d)\n", ret);
        }

        if (padding == SYM_PKCS5_PAD) {
            osa_memcpy(block, src + cur_len, src_size - cur_len);
            _add_pkcs_padding(block, AES_BLOCK_SIZE, src_size - cur_len);
#if CONFIG_HAL_AES_CBC
            ret = ls_hal_aes_cbc_process(ctx, block, dst + cur_len, AES_BLOCK_SIZE);
#else
            ret = sal_aes_process(block, dst + cur_len, AES_BLOCK_SIZE, ctx);
#endif
            if (0 != ret) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: aes_crypt_cbc fail(%d)\n", ret);
            }
            *dst_size = cur_len + AES_BLOCK_SIZE;
        }
    } else {
        /* dencrypt */
        if (padding == SYM_NOPAD) {
#if CONFIG_HAL_AES_CBC
            ret = ls_hal_aes_cbc_process(ctx, src, dst, src_size);
#else
            ret = sal_aes_process(src, dst, src_size, ctx);
#endif
            if (0 != ret) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: aes_crypt_cbc fail(%d)\n", ret);
            }

            *dst_size = src_size;
        } else if (padding == SYM_PKCS5_PAD) {
            /* avoid dst size is not enougth */
            tmp_dst = osa_malloc(src_size);
            if (NULL == tmp_dst) {
                PRINT_RET(ALI_CRYPTO_ERROR, "cbc_final: out of memory\n");
            }
#if CONFIG_HAL_AES_CBC
            ret = ls_hal_aes_cbc_process(ctx, src, tmp_dst, src_size);
#else
            ret = sal_aes_process(src, tmp_dst, src_size, ctx);
#endif
            if (0 != ret) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: aes_crypt_cbc fail(%d)\n", ret);
            }

            ret = _get_pkcs_padding(tmp_dst + src_size - AES_BLOCK_SIZE,
                                    AES_BLOCK_SIZE, &data_len);
            if (0 != ret) {
                osa_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: get pkcs padding fail(0x%08x)\n", ret);
            }
            if (*dst_size < src_size - (AES_BLOCK_SIZE - data_len)) {
                osa_free(tmp_dst);
                *dst_size = src_size - (AES_BLOCK_SIZE - data_len);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "cbc_final: dec pkcs short buffer\n");
            }
            osa_memcpy(dst, tmp_dst, src_size - (AES_BLOCK_SIZE - data_len));
            *dst_size = src_size - (AES_BLOCK_SIZE - data_len);
            osa_free(tmp_dst);
        }
    }
    return (ali_crypto_result)ret;
}
#endif

#if (CONFIG_HAL_AES_CTR || CONFIG_SAL_AES_CTR)
static ali_crypto_result _aes_ctr_final(const uint8_t *src, size_t src_size,
                                            uint8_t *dst, size_t *dst_size,
                                            void *context)
{
    int           ret = ALI_CRYPTO_SUCCESS;
#if CONFIG_HAL_AES_ECB
    hal_aes_ctx_t  *ctx = (hal_aes_ctx_t *)context;
#else
    aes_ctx_t  *ctx = (aes_ctx_t *)context;
#endif
    
    if (ctx == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "ctr_final: invalid context!\n");
    }

    if (src == NULL || src_size == 0) {
        if (dst_size != NULL) {
            *dst_size = 0;
        }
        return ALI_CRYPTO_SUCCESS;
    }

    if (dst_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ctr_final: invalid arg!\n");
    }

    if (src_size > *dst_size) {
        *dst_size = src_size;
        PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                  "ctr_final: short buffer(%d vs %d)\n", (int)src_size,
                  (int)*dst_size);
    }
#if CONFIG_HAL_AES_CTR
    ret = ls_hal_aes_ctr_process(ctx, src, dst, src_size);
#else    
    ret = sal_aes_process(src, dst, src_size, ctx);
#endif
    *dst_size = src_size;

    return (ali_crypto_result)ret;
}
#endif

#if (CONFIG_HAL_AES_CFB || CONFIG_SAL_AES_CFB)
static ali_crypto_result _aes_cfb_final(const uint8_t *src, size_t src_size,
                                            uint8_t *dst, size_t *dst_size,
                                            void *context)
{
    int ret = ALI_CRYPTO_SUCCESS;
#if CONFIG_HAL_AES_ECB
    hal_aes_ctx_t  *ctx = (hal_aes_ctx_t *)context;
#else
    aes_ctx_t  *ctx = (aes_ctx_t *)context;
#endif

    if (ctx == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_CONTEXT, "cfb_final: invalid context!\n");
    }

    if (src == NULL || src_size == 0) {
        if (dst_size != NULL) {
            *dst_size = 0;
        }
        return ALI_CRYPTO_SUCCESS;
    }

    if (dst_size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "cfb_final: invalid arg!\n");
    }

    if (src_size > *dst_size) {
        *dst_size = src_size;
        PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                  "cfb_final: short buffer(%d vs %d)\n", (int)src_size,
                  (int)*dst_size);
    }

#if CONFIG_HAL_AES_CFB
    ret = ls_hal_aes_cfb_process(ctx, src, dst, src_size);
#else
    ret = sal_aes_process(src, dst, src_size, ctx);
#endif
    *dst_size = src_size;

    return (ali_crypto_result)ret;
}
#endif

ali_crypto_result ali_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    aes_type_t type;
    api_aes_ctx_t *ctx;

    if ((src == NULL && src_size != 0) ||
        ((dst_size != NULL) && (dst == NULL && *dst_size != 0)) ||
        context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    ctx = (api_aes_ctx_t *) context;
    type = ctx->type;
    switch(type){
#if (CONFIG_HAL_AES_ECB || CONFIG_SAL_AES_ECB)
        case AES_ECB:
#if CONFIG_HAL_AES_ECB
            result = _aes_ecb_final(src, src_size, dst, dst_size, padding, &(ctx->hal_ctx));
#else
            result = _aes_ecb_final(src, src_size, dst, dst_size, padding, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CBC || CONFIG_SAL_AES_CBC)
        case AES_CBC:
#if CONFIG_HAL_AES_CBC
            result = _aes_cbc_final(src, src_size, dst, dst_size, padding, &(ctx->hal_ctx));
#else
            result = _aes_cbc_final(src, src_size, dst, dst_size, padding, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CTR || CONFIG_SAL_AES_CTR)
        case AES_CTR:
#if CONFIG_HAL_AES_CTR
            result = _aes_ctr_final(src, src_size, dst, dst_size, &(ctx->hal_ctx));
#else
            result = _aes_ctr_final(src, src_size, dst, dst_size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CFB || CONFIG_SAL_AES_CFB)
        case AES_CFB8:
#if CONFIG_HAL_AES_CFB
            result = _aes_cfb_final(src, src_size, dst, dst_size, &(ctx->hal_ctx));
#else
            result = _aes_cfb_final(src, src_size, dst, dst_size, &(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_SAL_AES_CFB)
        case AES_CFB128:
            result = _aes_cfb_final(src, src_size, dst, dst_size, &(ctx->sal_ctx));
            break;
#endif
        default:
            PRINT_RET(-1, "ali_aes_finish: invalid type(%d)\n", type);
    }
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(result, "ali_aes_finish: aes type(%d) final fail(%08x)\n",
                  type, result);
    }
    return result;
}

ali_crypto_result ali_aes_reset(void *context)
{
    ali_crypto_result  result = ALI_CRYPTO_SUCCESS;
    aes_type_t type;
    api_aes_ctx_t *ctx;
    
    if (context == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "bad input args!\n");
    }

    ctx = (api_aes_ctx_t *) context;
    type = ctx->type;
    switch(type){
#if (CONFIG_HAL_AES_ECB || CONFIG_SAL_AES_ECB)
        case AES_ECB:
#if CONFIG_HAL_AES_ECB
            osa_memset(&(ctx->hal_ctx), 0, sizeof(ctx->hal_ctx));
#else
            result = sal_aes_reset(&(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CBC || CONFIG_SAL_AES_CBC)
        case AES_CBC:
#if CONFIG_HAL_AES_CBC
            osa_memset(&(ctx->hal_ctx), 0, sizeof(ctx->hal_ctx));
#else
            result = sal_aes_reset(&(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CTR || CONFIG_SAL_AES_CTR)
        case AES_CTR:
#if CONFIG_HAL_AES_CTR
            osa_memset(&(ctx->hal_ctx), 0, sizeof(ctx->hal_ctx));
#else
            result = sal_aes_reset(&(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_HAL_AES_CFB || CONFIG_SAL_AES_CFB)
        case AES_CFB8:
#if CONFIG_HAL_AES_CFB
            osa_memset(&(ctx->hal_ctx), 0, sizeof(ctx->hal_ctx));
#else
            result = sal_aes_reset(&(ctx->sal_ctx));
#endif
            break;
#endif
#if (CONFIG_SAL_AES_CFB)
        case AES_CFB128:
            result = sal_aes_reset(&(ctx->sal_ctx));
            break;
#endif
        default:
            PRINT_RET(-1, "ali_aes_reset: invalid type(%d)\n", type);
    }
    return result;
}


