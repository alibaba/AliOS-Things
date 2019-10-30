#include "mbed_crypto.h"
#include "ali_crypto.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* pkcs5 only support 8 bytes block size
 * pcks7 support 8,16 bytes block size, so in aes pkcs5 equal pkcs7 */
/*
 * output:  buf
 * output_len: block size
 * data_len: cur_data_len */
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

static ali_crypto_result _ali_aes_cbc_final(const uint8_t *src, size_t src_size,
                                            uint8_t *dst, size_t *dst_size,
                                            sym_padding_t padding,
                                            aes_ctx_t    *ctx)
{
    int      ret;
    int      mode;
    size_t   data_len;
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
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "cbc_final: invalid arg!\n");
    }

    if ((0 != *dst_size) && (dst == NULL)) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "cbc_final: invalid arg!\n");
    }
    if (ctx->is_enc) {
        mode = MBEDTLS_AES_ENCRYPT;
    } else {
        mode = MBEDTLS_AES_DECRYPT;
    }

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
    } else if (padding == SYM_PKCS5_PAD) {
        if (ctx->is_enc) {
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
                tmp_dst = OSA_malloc(src_size);
                if (NULL == tmp_dst) {
                    PRINT_RET(ALI_CRYPTO_OUTOFMEM,
                              "cbc_final: out of memory\n");
                }
                ret =
                  mbedtls_aes_crypt_cbc(&(ctx->ctx), mode, src_size,
                                        (unsigned char *)ctx->iv, src, tmp_dst);
                if (0 != ret) {
                    OSA_free(tmp_dst);
                    PRINT_RET(ALI_CRYPTO_ERROR,
                              "cbc_final: mbedtls_aes_crypt_cbc fail(%d)\n",
                              ret);
                }

                ret = _get_pkcs_padding(tmp_dst + src_size - AES_BLOCK_SIZE,
                                        AES_BLOCK_SIZE, &data_len);
                if (0 != ret) {
                    OSA_free(tmp_dst);
                    PRINT_RET(ALI_CRYPTO_ERROR,
                              "cbc_final: get pkcs padding fail(0x%08x)\n",
                              ret);
                }
                *dst_size = src_size - (AES_BLOCK_SIZE - data_len);

                OSA_free(tmp_dst);
                tmp_dst = NULL;
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "cbc_final: dec pkcs short buffer(%d vs %d)\n",
                          (int)src_size, (int)*dst_size);
            }
        }
    }

    if (MBEDTLS_AES_ENCRYPT == mode) {
        /* encrypt, short buffer will be blocked above */
        size_t  cur_len;
        uint8_t block[AES_BLOCK_SIZE];

        cur_len = src_size & (~(AES_BLOCK_SIZE - 1));
        ret     = mbedtls_aes_crypt_cbc(
          &(ctx->ctx), mode, cur_len, (unsigned char *)ctx->iv,
          (const unsigned char *)src, (unsigned char *)dst);
        if (0 != ret) {
            PRINT_RET(ALI_CRYPTO_ERROR,
                      "cbc_final: mbedtls_aes_crypt_cbc fail(%d)\n", ret);
        }

        if (padding == SYM_PKCS5_PAD) {
            OSA_memcpy(block, src + cur_len, src_size - cur_len);
            _add_pkcs_padding(block, AES_BLOCK_SIZE, src_size - cur_len);
            ret = mbedtls_aes_crypt_cbc(
              &(ctx->ctx), mode, AES_BLOCK_SIZE, (unsigned char *)ctx->iv,
              (const unsigned char *)block, (unsigned char *)(dst + cur_len));
            if (0 != ret) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: mbedtls_aes_crypt_cbc fail(%d)\n", ret);
            }
            *dst_size = cur_len + AES_BLOCK_SIZE;
        }
    } else {
        /* dencrypt */
        if (padding == SYM_NOPAD) {
            ret = mbedtls_aes_crypt_cbc(
              &(ctx->ctx), mode, src_size, (unsigned char *)ctx->iv,
              (const unsigned char *)src, (unsigned char *)dst);
            if (0 != ret) {
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: mbedtls_aes_crypt_cbc fail(%d)\n", ret);
            }

            *dst_size = src_size;
        } else if (padding == SYM_PKCS5_PAD) {
            /* avoid dst size is not enougth */
            tmp_dst = OSA_malloc(src_size);
            if (NULL == tmp_dst) {
                PRINT_RET(ALI_CRYPTO_ERROR, "cbc_final: out of memory\n");
            }
            ret = mbedtls_aes_crypt_cbc(
              &(ctx->ctx), mode, src_size, (unsigned char *)ctx->iv,
              (const unsigned char *)src, (unsigned char *)tmp_dst);
            if (0 != ret) {
                OSA_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: mbedtls_aes_crypt_cbc fail(%d)\n", ret);
            }

            ret = _get_pkcs_padding(tmp_dst + src_size - AES_BLOCK_SIZE,
                                    AES_BLOCK_SIZE, &data_len);
            if (0 != ret) {
                OSA_free(tmp_dst);
                PRINT_RET(ALI_CRYPTO_ERROR,
                          "cbc_final: get pkcs padding fail(0x%08x)\n", ret);
            }
            if (*dst_size < src_size - (AES_BLOCK_SIZE - data_len)) {
                OSA_free(tmp_dst);
                *dst_size = src_size - (AES_BLOCK_SIZE - data_len);
                PRINT_RET(ALI_CRYPTO_SHORT_BUFFER,
                          "cbc_final: dec pkcs short buffer\n");
            }
            OSA_memcpy(dst, tmp_dst, src_size - (AES_BLOCK_SIZE - data_len));
            *dst_size = src_size - (AES_BLOCK_SIZE - data_len);
            OSA_free(tmp_dst);
        }
    }

    return (ali_crypto_result)ret;
}

ali_crypto_result breeze_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "aes_get_ctx_size: bad input!\n");
    }

    *size = sizeof(aes_ctx_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result breeze_aes_init(aes_type_t type, bool is_enc,
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
        case AES_CBC: {
            if (iv == NULL) {
                PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                          "ali_aes_init: cbc iv is null\n");
            }

            OSA_memcpy(aes_ctx->iv, iv, 16);
            break;
        }
        default:
            PRINT_RET(ALI_CRYPTO_INVALID_TYPE,
                      "ali_aes_init: invalid aes type(%d)\n", type);
    }

    mbedtls_aes_init(&(aes_ctx->ctx));
    aes_ctx->is_enc = is_enc;

    if (aes_ctx->is_enc) {
        ret = mbedtls_aes_setkey_enc(&(aes_ctx->ctx), key1, keybytes * 8);
    } else {
        ret = mbedtls_aes_setkey_dec(&(aes_ctx->ctx), key1, keybytes * 8);
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

ali_crypto_result breeze_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context)
{
    ali_crypto_result ret;
    aes_ctx_t        *aes_ctx;

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
        case AES_CBC: {
            ret = _ali_aes_cbc_final(src, src_size, dst, dst_size, padding,
                                     aes_ctx);
            break;
        }
        default:
            PRINT_RET(ALI_CRYPTO_NOSUPPORT,
                      "ali_aes_finish: invalid aes type(%d)\n", aes_ctx->type);
    }

    if (ret != ALI_CRYPTO_SUCCESS) {
        mbedtls_aes_free(&(aes_ctx->ctx));
        PRINT_RET(ret, "ali_aes_finish: aes type(%d) final fail(%08x)\n",
                  aes_ctx->type, ret);
    }

    CLEAN_CTX_MAGIC(aes_ctx->magic);
    aes_ctx->status = CRYPTO_STATUS_FINISHED;
    aes_ctx->offset = 0;

    mbedtls_aes_free(&(aes_ctx->ctx));
    return ALI_CRYPTO_SUCCESS;
}
