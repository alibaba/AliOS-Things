/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#include "ali_crypto.h"
#include "ali_crypto_test.h"

#define TEST_KEY_SIZE (16)
#define TEST_DATA_SIZE (141)

static uint8_t test_key[TEST_KEY_SIZE] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                           0x07, 0x08, 0x08, 0x07, 0x06, 0x05,
                                           0x04, 0x03, 0x02, 0x01 };

static uint8_t _g_test_data[TEST_DATA_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x0a, 0x0b, 0x0c, 0x0d,
    0x0e, 0x0f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x13
};

/* openssl calculated result */
static uint8_t hmac_md5[MD5_HASH_SIZE]   = { 0x20, 0xc5, 0xc6, 0xa7, 0x17, 0x6f,
                                           0x27, 0xfe, 0x7a, 0x1d, 0x7e, 0x85,
                                           0x5b, 0x5c, 0xa8, 0xc4 };
static uint8_t hmac_sha1[SHA1_HASH_SIZE] = { 0xe5, 0xdf, 0x48, 0xfe, 0x08,
                                             0x91, 0x37, 0xa2, 0x55, 0x95,
                                             0xbc, 0xf3, 0x76, 0x06, 0x92,
                                             0x1e, 0x54, 0x98, 0xe0, 0x4b };
static uint8_t hmac_sha224[SHA224_HASH_SIZE] = {
    0x1c, 0x47, 0x04, 0x45, 0xcd, 0xee, 0x00, 0x9a, 0x46, 0x66,
    0x2e, 0x1e, 0xb7, 0x04, 0xc9, 0x8f, 0xd5, 0xbb, 0x90, 0x38,
    0xbb, 0x93, 0x9a, 0x08, 0x47, 0xe7, 0x17, 0xca
};
static uint8_t hmac_sha256[SHA256_HASH_SIZE] = {
    0xd5, 0xce, 0x2b, 0x95, 0xa3, 0xea, 0x70, 0x69, 0x6a, 0x29, 0xbf,
    0xe7, 0x9b, 0xa2, 0xc9, 0x18, 0x27, 0x4d, 0x3f, 0xd7, 0xae, 0xe7,
    0x81, 0x88, 0x2a, 0xe7, 0x19, 0x68, 0x47, 0x07, 0xa3, 0xb3
};
static uint8_t hmac_sha384[SHA384_HASH_SIZE] = {
    0x26, 0x10, 0x72, 0x0d, 0xf1, 0x70, 0x03, 0x40, 0x65, 0x4c, 0x94, 0xf5,
    0x45, 0xbc, 0xbc, 0xcc, 0xd4, 0x17, 0xf5, 0x70, 0x81, 0xda, 0x91, 0x99,
    0xe0, 0xca, 0x7a, 0x8c, 0x9c, 0x15, 0x5b, 0x22, 0xe8, 0xaa, 0x1c, 0xcf,
    0xef, 0xe4, 0x6e, 0xf2, 0xfb, 0xdb, 0x6a, 0xf2, 0x22, 0xae, 0x70, 0x78
};
static uint8_t hmac_sha512[SHA512_HASH_SIZE] = {
    0x66, 0x43, 0xba, 0xfc, 0x6f, 0x9e, 0xa3, 0xf8, 0xbf, 0x3d, 0x46,
    0x46, 0x26, 0xfb, 0x8f, 0xa4, 0x04, 0x4c, 0x8a, 0x07, 0xfa, 0xac,
    0x1d, 0x16, 0x33, 0xe6, 0xbd, 0x65, 0x01, 0xe2, 0x44, 0x83, 0x45,
    0x78, 0x25, 0xbc, 0x42, 0x4b, 0x25, 0x85, 0xe0, 0x2a, 0xb4, 0xff,
    0x6b, 0x92, 0x0c, 0x50, 0xdb, 0x0c, 0x00, 0x6e, 0x4d, 0xd5, 0x5c,
    0xcc, 0x4e, 0x9f, 0xba, 0x3f, 0xfd, 0x81, 0x3f, 0x0b
};

int ali_crypto_hmac_test(void)
{
    ali_crypto_result result;
    hash_type_t       type;
    void *            hmac_ctx = NULL;
    uint32_t          hmac_ctx_size;
    uint8_t           md[MAX_HASH_SIZE];

    /* for gcov coverage */
    result = ali_hmac_get_ctx_size(MD5, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_get_ctx_size(HASH_NONE, (size_t *)(&hmac_ctx_size));
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_init(MD5, test_key, TEST_KEY_SIZE, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_init(HASH_NONE, test_key, TEST_KEY_SIZE, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_update(_g_test_data, 13, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_final(md, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_digest(MD5, NULL, TEST_KEY_SIZE, _g_test_data,
                             TEST_DATA_SIZE, md);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_reset(NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }
    result = ali_hmac_copy_context(NULL, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    for (type = SHA1; type <= MD5; type++) {
        if (type == SHA512 || type == SHA384) {
            CRYPT_INF("hmac not support hash 384 512\n");
            continue;
        }
        result = ali_hmac_get_ctx_size(type, (size_t *)(&hmac_ctx_size));
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "get ctx size fail(%08x)\n", result);
        }

        hmac_ctx = CRYPT_MALLOC(hmac_ctx_size);
        if (hmac_ctx == NULL) {
            GO_RET(ALI_CRYPTO_OUTOFMEM, "kmalloc(%08x) fail\n",
                   (int)hmac_ctx_size);
        }
        CRYPT_MEMSET(hmac_ctx, 0, hmac_ctx_size);

        {
            result = ali_hmac_init(type, test_key, TEST_KEY_SIZE, hmac_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                GO_RET(result, "init fail(%08x)", result);
            }

            /* for gcov coverage */
            result = ali_hmac_update(NULL, 13, hmac_ctx);
            if (result == ALI_CRYPTO_SUCCESS) {
                goto _OUT;
            }

            result = ali_hmac_update(_g_test_data, 13, hmac_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                GO_RET(result, "update 1th fail(%08x)", result);
            }
            result = ali_hmac_update(_g_test_data + 13, 63, hmac_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                GO_RET(result, "update 2th fail(%08x)", result);
            }
            result = ali_hmac_update(_g_test_data + 13 + 63, 65, hmac_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                GO_RET(result, "update 3th fail(%08x)", result);
            }

            result = ali_hmac_final(md, hmac_ctx);
            if (result != ALI_CRYPTO_SUCCESS) {
                GO_RET(result, "final fail(%08x)", result);
            }

            /* for gcov coverage */
            result = ali_hmac_digest(HASH_NONE, test_key, TEST_KEY_SIZE,
                                     _g_test_data, TEST_DATA_SIZE, md);
            if (result == ALI_CRYPTO_SUCCESS) {
                goto _OUT;
            }

            result = ali_hmac_digest(type, test_key, TEST_KEY_SIZE,
                                     _g_test_data, TEST_DATA_SIZE, md);
            if (result != ALI_CRYPTO_SUCCESS) {
                GO_RET(result, "digest fail(%08x)", result);
            }
        }

        ali_hmac_copy_context(hmac_ctx, hmac_ctx);
        ali_hmac_reset(hmac_ctx);
        CRYPT_FREE(hmac_ctx);
        hmac_ctx = NULL;

        if (type == SHA1) {
            if (CRYPT_MEMCMP(md, hmac_sha1, SHA1_HASH_SIZE)) {
                ali_crypto_print_data("hmac-sha1", md, SHA1_HASH_SIZE);
                GO_RET(-1, "HMAC-SHA1 test fail!\n");
            } else {
                CRYPT_INF("HMAC-SHA1 test success!\n");
            }
        } else if (type == SHA224) {
            if (CRYPT_MEMCMP(md, hmac_sha224, SHA224_HASH_SIZE)) {
                ali_crypto_print_data("hmac-sha224", md, SHA224_HASH_SIZE);
                GO_RET(-1, "HMAC-SHA224 test fail!\n");
            } else {
                CRYPT_INF("HMAC-SHA224 test success!\n");
            }
        } else if (type == SHA256) {
            if (CRYPT_MEMCMP(md, hmac_sha256, SHA256_HASH_SIZE)) {
                ali_crypto_print_data("hmac-sha256", md, SHA256_HASH_SIZE);
                GO_RET(-1, "HMAC-SHA256 test fail!\n");
            } else {
                CRYPT_INF("HMAC-SHA256 test success!\n");
            }
            /*       } else if (type == SHA384) {
                       if(CRYPT_MEMCMP(md, hmac_sha384, SHA384_HASH_SIZE)) {
                           ali_crypto_print_data("hmac-sha384", md,
               SHA384_HASH_SIZE); GO_RET(-1, "HMAC-SHA384 test fail!\n"); } else
               { CRYPT_INF("HMAC-SHA384 test success!\n");
                       }
                   } else if (type == SHA512) {
                       if(CRYPT_MEMCMP(md, hmac_sha512, SHA512_HASH_SIZE)) {
                           ali_crypto_print_data("hmac-sha512", md,
               SHA512_HASH_SIZE); GO_RET(-1, "HMAC-SHA512 test fail!\n"); } else
               { CRYPT_INF("HMAC-SHA512 test success!\n");
                       }
            */
        } else if (type == MD5) {
            if (CRYPT_MEMCMP(md, hmac_md5, MD5_HASH_SIZE)) {
                ali_crypto_print_data("hmac-md5", md, MD5_HASH_SIZE);
                GO_RET(-1, "HMAC-MD5 test fail!\n");
            } else {
                CRYPT_INF("HMAC-MD5 test success!\n");
            }
        }
    }

    return 0;

_OUT:
    if (hmac_ctx) {
        CRYPT_FREE(hmac_ctx);
    }

    return -1;
}
