/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "ali_crypto.h"
#include "ali_crypto_test.h"

#define TEST_KEY_SIZE           (32)
#define TEST_DATA_SIZE          (141)
#define TEST_ECB_SIZE           (96)
#define TEST_CBC_SIZE           (96)
#define TEST_CTR_SIZE           (141)
#define TEST_CTS_SIZE           (141)
#define TEST_XTS_SIZE           (141)

static uint8_t _g_aes_key[TEST_KEY_SIZE] = {
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x0f,
   0x0f, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09,
   0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01
};

static uint8_t _g_aes_iv[AES_IV_SIZE] = {
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

static uint8_t _g_test_data[TEST_DATA_SIZE] = {
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x01,
   0x02, 0x03, 0x04, 0x05, 0x13
};

/* openssl calculated results */
static uint8_t _g_ecb_128_enc[TEST_ECB_SIZE] = {
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7,
    0x55, 0x4c, 0x93, 0x50, 0x16, 0x35, 0xaa, 0xdb,
    0xad, 0xf8, 0x8b, 0xc6, 0xd3, 0x6e, 0x57, 0xf7
};
static uint8_t _g_ecb_192_enc[TEST_ECB_SIZE] = {
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a,
    0x7c, 0x73, 0x26, 0x78, 0x4d, 0x71, 0xaf, 0xaf,
    0x2b, 0x1a, 0xae, 0x70, 0xe3, 0x1f, 0x6f, 0x4a
};
static uint8_t _g_ecb_256_enc[TEST_ECB_SIZE] = {
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53,
    0x34, 0xad, 0x17, 0xeb, 0x50, 0xfb, 0x58, 0xda,
    0x3b, 0x60, 0x18, 0x18, 0x0f, 0x70, 0x13, 0x53
};

static int _aes_ecb_encrypt_decrypt(uint32_t idx, bool is_enc,
                      size_t key_len, uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t dst_size;

    /* for gcov coverage */
    result = ali_aes_init(AES_ECB, is_enc,
                 NULL, NULL, key_len, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_aes_init(AES_ECB, is_enc,
                 _g_aes_key, NULL, 0, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_aes_init((aes_type_t)-1, is_enc,
                 _g_aes_key, NULL, key_len, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    result = ali_aes_init(AES_ECB, is_enc,
                 _g_aes_key, NULL, key_len, NULL, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "aes_ecb init fail(%08x)\n", result);
    }


    if ((idx & 0x1) != 0x0) {
        /* for gcov coverage */
        result = ali_aes_process(src, dst, 16, NULL);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
        /* for gcov coverage */
        result = ali_aes_process(NULL, dst, 16, aes_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
        /* for gcov coverage */
        result = ali_aes_process(src, dst, 17, aes_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }

        result = ali_aes_process(src, dst, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ecb process 1th fail(%08x)", result);
        }
        result = ali_aes_process(src + 16, dst + 16, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ecb process 2th fail(%08x)", result);
        }
        result = ali_aes_process(src + 32, dst + 32, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ecb process 3th fail(%08x)", result);
        }

        dst_size = TEST_ECB_SIZE - 48;

        /* for gcov coverage */
        result = ali_aes_finish(
                     src + 48, TEST_ECB_SIZE - 48,
                     dst + 48, &dst_size, SYM_NOPAD, NULL);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }

        result = ali_aes_finish(
                     src + 48, TEST_ECB_SIZE - 48,
                     dst + 48, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "finish fail(%08x)", result);
        }
    } else {
        dst_size = TEST_ECB_SIZE;
        result = ali_aes_finish(
                     src, TEST_ECB_SIZE,
                     dst, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_ECB_SIZE) {
            PRINT_RET(-1, "finish fail(%08x)", result);
        }
    }

    return 0;
}

static int _aes_cbc_encrypt_decrypt(uint32_t idx, bool is_enc,
                      size_t key_len, uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t dst_size;

    /* for gcov coverage */
    result = ali_aes_init(AES_CBC, is_enc,
                 _g_aes_key, NULL, key_len, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    result = ali_aes_init(AES_CBC, is_enc,
                 _g_aes_key, NULL, key_len, _g_aes_iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "aes_cbc init fail(%08x)\n", result);
    }

    if ((idx & 0x1) != 0x0) {
        /* for gcov coverage */
        result = ali_aes_process(src, dst, 17, aes_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            return -1;
        }
        result = ali_aes_process(src, dst, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc process 1th fail(%08x)", result);
        }
        result = ali_aes_process(src + 16, dst + 16, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc process 2th fail(%08x)", result);
        }
        result = ali_aes_process(src + 32, dst + 32, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc process 3th fail(%08x)", result);
        }

        dst_size = TEST_CBC_SIZE - 48;
        result = ali_aes_finish(
                     src + 48, TEST_CBC_SIZE - 48,
                     dst + 48, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_cbc finish fail(%08x)", result);
        }
    } else {
        dst_size = TEST_CBC_SIZE;
        result = ali_aes_finish(
                     src, TEST_CBC_SIZE,
                     dst, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CBC_SIZE) {
            PRINT_RET(-1, "aes_cbc finish fail(%08x)", result);
        }
    }

    return 0;
}

static int _aes_ctr_encrypt_decrypt(uint32_t idx, bool is_enc,
                      size_t key_len, uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t dst_size;

    /* for gcov coverage */
    result = ali_aes_init(AES_CTR, is_enc,
                 _g_aes_key, NULL, key_len, NULL, aes_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    result = ali_aes_init(AES_CTR, is_enc,
                 _g_aes_key, NULL, key_len, _g_aes_iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "aes_ctr init fail(%08x)\n", result);
    }

    if ((idx & 0x1) != 0x0) {
        result = ali_aes_process(src, dst, 13, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ctr process 1th fail(%08x)", result);
        }
        result = ali_aes_process(src + 13, dst + 13, 27, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ctr process 2th fail(%08x)", result);
        }
        result = ali_aes_process(src + 40, dst + 40, 8, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ctr process 3th fail(%08x)", result);
        }

        dst_size = TEST_CTR_SIZE - 48;
        result = ali_aes_finish(
                     src + 48, TEST_CTR_SIZE - 48,
                     dst + 48, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "aes_ctr finish fail(%08x)", result);
        }
    } else {
        dst_size = TEST_CTR_SIZE;
        result = ali_aes_finish(
                     src, TEST_CTR_SIZE,
                     dst, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CTR_SIZE) {
            PRINT_RET(-1, "aes_ctr finish fail(%08x)", result);
        }
    }

    return 0;
}

#if 0
static int _aes_cts_encrypt_decrypt(uint32_t idx, bool is_enc,
                      size_t key_len, uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t dst_size;

    result = ali_aes_init(AES_CTS, is_enc,
                 _g_aes_key, NULL, key_len, _g_aes_iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        CRYPT_ERR("aes_cts init fail(%08x)\n", result);
        return -1;
    }

    if ((idx & 0x1) != 0x0) {
        result = ali_aes_process(src, dst, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_cts process 1th fail(%08x)", result);
            return -1;
        }
        result = ali_aes_process(src + 16, dst + 16, 32, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_cts process 2th fail(%08x)", result);
            return -1;
        }
        result = ali_aes_process(src + 48, dst + 48, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_cts process 3th fail(%08x)", result);
            return -1;
        }

        dst_size = TEST_CTS_SIZE - 64;
        result = ali_aes_finish(
                     src + 64, TEST_CTS_SIZE - 64,
                     dst + 64, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_cts finish fail(%08x)", result);
            return -1;
        }
    } else {
        dst_size = TEST_CTS_SIZE;
        result = ali_aes_finish(
                     src, TEST_CTS_SIZE,
                     dst, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_CTS_SIZE) {
            CRYPT_ERR("aes_cts finish fail(%08x)", result);
            return -1;
        }
    }

    return 0;
}

static int _aes_xts_encrypt_decrypt(uint32_t idx, bool is_enc,
                      size_t key_len, uint8_t *src, uint8_t *dst, void *aes_ctx)
{
    ali_crypto_result result;
    size_t dst_size;

    result = ali_aes_init(AES_XTS, is_enc,
                 _g_aes_key, _g_aes_key, key_len, _g_aes_iv, aes_ctx);
    if (result != ALI_CRYPTO_SUCCESS) {
        CRYPT_ERR("aes_xts init fail(%08x)\n", result);
        return -1;
    }

    if ((idx & 0x1) != 0x0) {
        result = ali_aes_process(src, dst, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_xts process 1th fail(%08x)", result);
            return -1;
        }
        result = ali_aes_process(src + 16, dst + 16, 32, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_xts process 2th fail(%08x)", result);
            return -1;
        }
        result = ali_aes_process(src + 48, dst + 48, 16, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_xts process 3th fail(%08x)", result);
            return -1;
        }

        dst_size = TEST_XTS_SIZE - 64;
        result = ali_aes_finish(
                     src + 64, TEST_XTS_SIZE - 64,
                     dst + 64, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("aes_xts finish fail(%08x)", result);
            return -1;
        }
    } else {
        dst_size = TEST_XTS_SIZE;
        result = ali_aes_finish(
                     src, TEST_XTS_SIZE,
                     dst, &dst_size, SYM_NOPAD, aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS || dst_size != TEST_XTS_SIZE) {
            CRYPT_ERR("aes_xts finish fail(%08x)", result);
            return -1;
        }
    }

    return 0;
}
#endif

static int _ali_crypto_aes_ecb_test(void)
{
    int ret;
    ali_crypto_result result;
    size_t i, key_len;
    void *aes_ctx = NULL;
    size_t aes_ctx_size;
    aes_type_t type = AES_ECB;
    uint8_t *ecb_enc_exp;
    uint8_t plaintext[TEST_ECB_SIZE];
    uint8_t ciphertext[TEST_ECB_SIZE];

    /* for gcov coverage */
    result = ali_aes_init(AES_ECB, true,
                 NULL, NULL, 16, NULL, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_aes_reset(NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_aes_copy_context(NULL, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    for (i = 0; i < 3; i++) {
        if (i == 0) {
            key_len = 16;
            ecb_enc_exp = _g_ecb_128_enc;
        } else if (i == 1) {
            key_len = 24;
            ecb_enc_exp = _g_ecb_192_enc;
        } else {
            key_len = 32;
            ecb_enc_exp = _g_ecb_256_enc;
        }

        result = ali_aes_get_ctx_size(type, &aes_ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "get ctx size fail(%08x)\n", result);
        }

        aes_ctx = CRYPT_MALLOC(aes_ctx_size);
        if (aes_ctx == NULL) {
            GO_RET(-1, "kmalloc(%d) fail\n", (int)aes_ctx_size);
        }

        /* encrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_ecb_encrypt_decrypt(i, true,
                      key_len, _g_test_data, ciphertext, aes_ctx);
        if (ret < 0) {
            GO_RET(-1, "ecb encrypt fail!\n");
        }

        /* decrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_ecb_encrypt_decrypt(i, false,
                       key_len, ciphertext, plaintext, aes_ctx);
        if (ret < 0) {
            GO_RET(-1, "ecb decrypt fail!\n");
        }

        /* check result */
        if(CRYPT_MEMCMP(ciphertext, ecb_enc_exp, TEST_ECB_SIZE) ||
            CRYPT_MEMCMP(plaintext, _g_test_data, TEST_ECB_SIZE)) {
            ali_crypto_print_data("plaintext", plaintext, TEST_ECB_SIZE);
            ali_crypto_print_data("ciphertext", ciphertext, TEST_ECB_SIZE);
            GO_RET(-1, "AES_ECB_%d test fail!\n", (int)key_len << 3);
        } else {
            CRYPT_INF("AES_ECB_%d test success!\n", (int)key_len << 3);
        }

        result = ali_aes_copy_context(aes_ctx, aes_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            GO_RET(-1, "ali_aes_copy_context not expect!\n");
        }

        result = ali_aes_reset(aes_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(-1, "ali_aes_reset fail %d!\n", result);
        }
        CRYPT_FREE(aes_ctx);
        aes_ctx = NULL;
    }

    return 0;

_OUT:
    if (aes_ctx) {
        CRYPT_FREE(aes_ctx);
    }

    return -1;
}

static int _ali_crypto_aes_cbc_test(void)
{
    int ret;
    ali_crypto_result result;
    size_t i, key_len;
    void *aes_ctx = NULL;
    size_t aes_ctx_size;
    aes_type_t type = AES_CBC;
    uint8_t plaintext[TEST_CBC_SIZE];
    uint8_t ciphertext[TEST_CBC_SIZE];

    for (i = 0; i < 3; i++) {
        if (i == 0) {
            key_len = 16;
        } else if (i == 1) {
            key_len = 24;
        } else {
            key_len = 32;
        }

        result = ali_aes_get_ctx_size(type, &aes_ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(-1, "get ctx size fail(%08x)\n", result);
        }

        aes_ctx = CRYPT_MALLOC(aes_ctx_size);
        if (aes_ctx == NULL) {
            GO_RET(-1, "malloc(%d) fail\n", (int)aes_ctx_size);
        }

        /* encrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_cbc_encrypt_decrypt(i, true,
                      key_len, _g_test_data, ciphertext, aes_ctx);
        if (ret < 0) {
            GO_RET(-1, "cbc encrypt fail!\n");
        }

        /* decrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_cbc_encrypt_decrypt(i, false,
                       key_len, ciphertext, plaintext, aes_ctx);
        if (ret < 0) {
            GO_RET(-1, "cbc decrypt fail!\n");
        }

        /* check result */
        if(CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CBC_SIZE)) {
            ali_crypto_print_data("plaintext", plaintext, TEST_CBC_SIZE);
            ali_crypto_print_data("ciphertext", ciphertext, TEST_CBC_SIZE);
            GO_RET(-1, "AES_CBC_%d test fail!\n", (int)key_len << 3);
        } else {
            CRYPT_INF("AES_CBC_%d test success!\n", (int)key_len << 3);
        }

        CRYPT_FREE(aes_ctx);
        aes_ctx = NULL;
    }

    return 0;

_OUT:
    if (aes_ctx) {
        CRYPT_FREE(aes_ctx);
    }

    return -1;
}

static int _ali_crypto_aes_ctr_test(void)
{
    int ret;
    ali_crypto_result result;
    size_t i, key_len;
    void *aes_ctx = NULL;
    size_t aes_ctx_size;
    aes_type_t type = AES_CTR;
    uint8_t plaintext[TEST_CTR_SIZE];
    uint8_t ciphertext[TEST_CTR_SIZE];

    for (i = 0; i < 3; i++) {
        if (i == 0) {
            key_len = 16;
        } else if (i == 1) {
            key_len = 24;
        } else {
            key_len = 32;
        }

        result = ali_aes_get_ctx_size(type, &aes_ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(-1, "get ctx size fail(%08x)\n", result);
        }

        aes_ctx = CRYPT_MALLOC(aes_ctx_size);
        if (aes_ctx == NULL) {
            GO_RET(-1, "malloc(%d) fail\n", (int)aes_ctx_size);
        }

        /* encrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_ctr_encrypt_decrypt(i, true,
                      key_len, _g_test_data, ciphertext, aes_ctx);
        if (ret < 0) {
            GO_RET(-1, "ctr encrypt fail!\n");
        }

        /* decrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_ctr_encrypt_decrypt(i, false,
                       key_len, ciphertext, plaintext, aes_ctx);
        if (ret < 0) {
            GO_RET(-1, "ctr decrypt fail!\n");
        }

        /* check result */
        if(CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CTR_SIZE)) {
            ali_crypto_print_data("plaintext", plaintext, TEST_CTR_SIZE);
            ali_crypto_print_data("ciphertext", ciphertext, TEST_CTR_SIZE);
            GO_RET(-1, "AES_CTR_%d test fail!\n", (int)key_len << 3);
        } else {
            CRYPT_INF("AES_CTR_%d test success!\n", (int)key_len << 3);
        }

        CRYPT_FREE(aes_ctx);
        aes_ctx = NULL;
    }

    return 0;

_OUT:
    if (aes_ctx) {
        CRYPT_FREE(aes_ctx);
    }

    return -1;
}

#if 0
static int _ali_crypto_aes_cts_test(void)
{
    int ret;
    ali_crypto_result result;
    size_t i, key_len;
    void *aes_ctx = NULL;
    size_t aes_ctx_size;
    aes_type_t type = AES_CTS;
    uint8_t plaintext[TEST_CTS_SIZE];
    uint8_t ciphertext[TEST_CTS_SIZE];

    for (i = 0; i < 3; i++) {
        if (i == 0) {
            key_len = 16;
        } else if (i == 1) {
            key_len = 24;
        } else {
            key_len = 32;
        }

        result = ali_aes_get_ctx_size(type, &aes_ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("get ctx size fail(%08x)\n", result);
            goto _OUT;
        }

        aes_ctx = CRYPT_MALLOC(aes_ctx_size);
        if (aes_ctx == NULL) {
            CRYPT_ERR("kmalloc(%d) fail\n", aes_ctx_size);
            goto _OUT;
        }

        /* encrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_cts_encrypt_decrypt(i, true,
                      key_len, _g_test_data, ciphertext, aes_ctx);
        if (ret < 0) {
            CRYPT_ERR("cts encrypt fail!\n");
            goto _OUT;
        }

        /* decrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_cts_encrypt_decrypt(i, false,
                       key_len, ciphertext, plaintext, aes_ctx);
        if (ret < 0) {
            CRYPT_ERR("cts decrypt fail!\n");
            goto _OUT;
        }

        /* check result */
        if(CRYPT_MEMCMP(plaintext, _g_test_data, TEST_CTS_SIZE)) {
            CRYPT_INF("AES_CTS_%d test fail!\n", key_len << 3);
            ali_crypto_print_data("plaintext", plaintext, TEST_CTS_SIZE);
            ali_crypto_print_data("ciphertext", ciphertext, TEST_CTS_SIZE);
            goto _OUT;
        } else {
            CRYPT_INF("AES_CTS_%d test success!\n", key_len << 3);
        }

        CRYPT_FREE(aes_ctx);
        aes_ctx = NULL;
    }

    return 0;

_OUT:
    if (aes_ctx) {
        CRYPT_FREE(aes_ctx);
    }

    return -1;
}

static int _ali_crypto_aes_xts_test(void)
{
    int ret;
    ali_crypto_result result;
    size_t i, key_len;
    void *aes_ctx = NULL;
    size_t aes_ctx_size;
    aes_type_t type = AES_XTS;
    uint8_t plaintext[TEST_XTS_SIZE];
    uint8_t ciphertext[TEST_XTS_SIZE];

    for (i = 0; i < 3; i++) {
        if (i == 0) {
            key_len = 16;
        } else if (i == 1) {
            key_len = 24;
        } else {
            key_len = 32;
        }

        result = ali_aes_get_ctx_size(type, &aes_ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            CRYPT_ERR("get ctx size fail(%08x)\n", result);
            goto _OUT;
        }

        aes_ctx = CRYPT_MALLOC(aes_ctx_size);
        if (aes_ctx == NULL) {
            CRYPT_ERR("kmalloc(%d) fail\n", aes_ctx_size);
            goto _OUT;
        }

        /* encrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_xts_encrypt_decrypt(i, true,
                      key_len, _g_test_data, ciphertext, aes_ctx);
        if (ret < 0) {
            CRYPT_ERR("xts encrypt fail!\n");
            goto _OUT;
        }

        /* decrypt */
        CRYPT_MEMSET(aes_ctx, 0, aes_ctx_size);
        ret = _aes_xts_encrypt_decrypt(i, false,
                       key_len, ciphertext, plaintext, aes_ctx);
        if (ret < 0) {
            CRYPT_ERR("xts decrypt fail!\n");
            goto _OUT;
        }

        /* check result */
        if(CRYPT_MEMCMP(plaintext, _g_test_data, TEST_XTS_SIZE)) {
            CRYPT_INF("AES_XTS_%d test fail!\n", key_len << 3);
            ali_crypto_print_data("plaintext", plaintext, TEST_XTS_SIZE);
            ali_crypto_print_data("ciphertext", ciphertext, TEST_XTS_SIZE);
            goto _OUT;
        } else {
            CRYPT_INF("AES_XTS_%d test success!\n", key_len << 3);
        }

        CRYPT_FREE(aes_ctx);
        aes_ctx = NULL;
    }

    return 0;

_OUT:
    if (aes_ctx) {
        CRYPT_FREE(aes_ctx);
    }

    return -1;
}
#endif

int ali_crypto_aes_test(void)
{
    int ret;

    /* for gcov coverage */
    ret = ali_aes_get_ctx_size(AES_ECB, NULL);
    if (ret == ALI_CRYPTO_SUCCESS) {
        return ALI_CRYPTO_ERROR;
    }

    CRYPT_INF("Test aes ecb:\n");
    ret = _ali_crypto_aes_ecb_test();
    if (ret < 0) {
        return ret;
    }

    CRYPT_INF("Test aes cbc:\n");
    ret = _ali_crypto_aes_cbc_test();
    if (ret < 0) {
        return ret;
    }

    CRYPT_INF("Test aes ctr:\n");
    ret = _ali_crypto_aes_ctr_test();
    if (ret < 0) {
        return ret;
    }

#if 0
    CRYPT_INF("Test aes cts:\n");
    ret = _ali_crypto_aes_cts_test();
    if (ret < 0) {
        return ret;
    }

    CRYPT_INF("Test aes xts:\n");
    ret = _ali_crypto_aes_xts_test();
    if (ret < 0) {
        return ret;
    }
#endif

    return 0;
}
