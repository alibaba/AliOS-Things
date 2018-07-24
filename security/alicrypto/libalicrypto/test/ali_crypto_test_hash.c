/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include "ali_crypto.h"
#include "ali_crypto_test.h"

#define TEST_DATA_SIZE (141)

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
static uint8_t hash_md5[MD5_HASH_SIZE]   = { 0x95, 0x79, 0xa2, 0x46, 0x8e, 0xbc,
                                           0x5b, 0xd6, 0x45, 0x57, 0xbb, 0x4f,
                                           0xaf, 0xae, 0x5a, 0x05 };
static int8_t  hash_sha1[SHA1_HASH_SIZE] = { 0x54, 0x1d, 0x6f, 0x6e, 0x46,
                                            0x7e, 0xfe, 0x1d, 0xa8, 0x66,
                                            0x06, 0x34, 0xb0, 0x21, 0x3d,
                                            0x65, 0xb8, 0xa4, 0x02, 0xca };
static uint8_t hash_sha224[SHA224_HASH_SIZE] = {
    0x36, 0x94, 0x36, 0xf9, 0xd4, 0xe9, 0xbe, 0x59, 0xbb, 0x59,
    0x5c, 0x73, 0x4a, 0xf0, 0xe8, 0x52, 0x48, 0x09, 0x84, 0x42,
    0xec, 0x80, 0xdb, 0x86, 0x5a, 0x51, 0x44, 0x3a
};
static uint8_t hash_sha256[SHA256_HASH_SIZE] = {
    0x3b, 0x7f, 0x52, 0xae, 0x5b, 0xe8, 0x09, 0x19, 0x02, 0x1a, 0x83,
    0x8d, 0xcc, 0xc6, 0x01, 0xc3, 0x76, 0x41, 0x22, 0x64, 0x4b, 0x1c,
    0x35, 0xa2, 0x9d, 0xd3, 0xc5, 0x76, 0x36, 0xd7, 0xda, 0x5f
};
static uint8_t hash_sha384[SHA384_HASH_SIZE] = {
    0x21, 0xc7, 0x05, 0xb3, 0x37, 0x66, 0xf3, 0xb5, 0x0d, 0x51, 0xf5, 0x0f,
    0x91, 0xfc, 0xa1, 0xcf, 0x78, 0x35, 0x82, 0x77, 0xfd, 0x2c, 0x31, 0xbb,
    0x8a, 0x26, 0x6f, 0x2a, 0x82, 0x52, 0x1a, 0x70, 0xfd, 0xfc, 0xa2, 0xb7,
    0xee, 0x7f, 0xb5, 0xfd, 0x9e, 0x20, 0x36, 0x91, 0xc6, 0xd6, 0x54, 0xa0
};
static uint8_t hash_sha512[SHA512_HASH_SIZE] = {
    0x9e, 0xca, 0x2a, 0x96, 0x01, 0x48, 0x0f, 0xa2, 0x6b, 0x99, 0x27,
    0x1a, 0x7f, 0x72, 0xe3, 0xa4, 0xee, 0x2f, 0x08, 0x92, 0x2e, 0xdb,
    0xf7, 0x19, 0xd8, 0xcd, 0xcb, 0xfc, 0x8b, 0x56, 0x8c, 0x04, 0xfb,
    0xb3, 0x69, 0xdf, 0x26, 0xfb, 0x0b, 0x9f, 0xbe, 0x1d, 0x42, 0xbd,
    0x39, 0x87, 0x52, 0x16, 0x42, 0xac, 0x62, 0x57, 0x94, 0x59, 0xa4,
    0xce, 0x8d, 0x69, 0x78, 0xb7, 0xf8, 0x95, 0xb8, 0x78
};

int ali_crypto_hash_test(void)
{
    ali_crypto_result result;
    hash_type_t       type;
    void *            hash_ctx = NULL;
    size_t            hash_ctx_size;
    uint8_t           hash[MAX_HASH_SIZE];
    uint8_t           hash_all[MAX_HASH_SIZE];

    /* for gcov coverage */
    result = ali_hash_get_ctx_size(HASH_NONE, &hash_ctx_size);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_get_ctx_size(HASH_NONE, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_init(HASH_NONE, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_init(HASH_NONE, hash_ctx);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_update(_g_test_data, 13, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_update(NULL, 13, (void *)-1);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_final(hash, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_final(hash, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_reset(NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    /* for gcov coverage */
    result = ali_hash_copy_context(NULL, NULL);
    if (result == ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    for (type = SHA1; type <= MD5; type++) {
        if (type == SHA512 || type == SHA384) {
            CRYPT_INF("hash not support hash 384 512\n");
            continue;
        }
        result = ali_hash_get_ctx_size(type, &hash_ctx_size);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "get ctx size fail(%08x)\n", result);
        }

        hash_ctx = CRYPT_MALLOC(hash_ctx_size);
        if (hash_ctx == NULL) {
            GO_RET(result, "malloc(%d) fail\n", (int)hash_ctx_size);
        }
        CRYPT_MEMSET(hash_ctx, 0, hash_ctx_size);

        result = ali_hash_init(type, hash_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "init fail(%08x)", result);
        }

        result = ali_hash_update(_g_test_data, 13, hash_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "update 1th fail(%08x)", result);
        }
        result = ali_hash_update(_g_test_data + 13, 63, hash_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "update 2th fail(%08x)", result);
        }
        result = ali_hash_update(_g_test_data + 13 + 63, 65, hash_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "update 3th fail(%08x)", result);
        }

        result = ali_hash_final(hash, hash_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "final fail(%08x)", result);
        }

        result = ali_hash_digest(type, _g_test_data, TEST_DATA_SIZE, hash_all);
        if (result != ALI_CRYPTO_SUCCESS) {
            GO_RET(result, "digest fail(%08x)", result);
        }

        /* for gcov coverage */
        result = ali_hash_copy_context(hash_ctx, hash_ctx);
        if (result == ALI_CRYPTO_SUCCESS) {
            result = ALI_CRYPTO_ERROR;
            goto _OUT;
        }

        result = ali_hash_reset(hash_ctx);
        if (result != ALI_CRYPTO_SUCCESS) {
            result = ALI_CRYPTO_ERROR;
            goto _OUT;
        }
        CRYPT_FREE(hash_ctx);
        hash_ctx = NULL;

        if (type == SHA1) {
            if (CRYPT_MEMCMP(hash, hash_sha1, SHA1_HASH_SIZE) ||
                CRYPT_MEMCMP(hash_all, hash_sha1, SHA1_HASH_SIZE)) {
                ali_crypto_print_data("sha1", hash, SHA1_HASH_SIZE);
                GO_RET(-1, "SHA1 test fail!");
            } else {
                CRYPT_INF("SHA1 test success!\n");
            }
        } else if (type == SHA224) {
            if (CRYPT_MEMCMP(hash, hash_sha224, SHA224_HASH_SIZE) ||
                CRYPT_MEMCMP(hash_all, hash_sha224, SHA224_HASH_SIZE)) {
                ali_crypto_print_data("sha224", hash, SHA224_HASH_SIZE);
                GO_RET(-1, "SHA224 test fail!\n");
            } else {
                CRYPT_INF("SHA224 test success!\n");
            }
        } else if (type == SHA256) {
            if (CRYPT_MEMCMP(hash, hash_sha256, SHA256_HASH_SIZE) ||
                CRYPT_MEMCMP(hash_all, hash_sha256, SHA256_HASH_SIZE)) {
                ali_crypto_print_data("sha256", hash, SHA256_HASH_SIZE);
                GO_RET(-1, "SHA256 test fail!\n");
            } else {
                CRYPT_INF("SHA256 test success!\n");
            }
            /*        } else if (type == SHA384) {
                        if(CRYPT_MEMCMP(hash, hash_sha384, SHA384_HASH_SIZE) ||
                                CRYPT_MEMCMP(hash_all, hash_sha384,
               SHA384_HASH_SIZE)) { ali_crypto_print_data("sha384", hash,
               SHA384_HASH_SIZE); GO_RET(-1, "SHA384 test fail!\n"); } else {
                            CRYPT_INF("SHA384 test success!\n");
                        }
                    } else if (type == SHA512) {
                        if(CRYPT_MEMCMP(hash, hash_sha512, SHA512_HASH_SIZE) ||
                                CRYPT_MEMCMP(hash_all, hash_sha512,
               SHA512_HASH_SIZE)) { ali_crypto_print_data("sha512", hash,
               SHA512_HASH_SIZE); ali_crypto_print_data("sha512", hash_all,
               SHA512_HASH_SIZE); GO_RET(-1, "SHA512 test fail!\n"); } else {
                            CRYPT_INF("SHA512 test success!\n");
                        }
            */
        } else if (type == MD5) {
            if (CRYPT_MEMCMP(hash, hash_md5, MD5_HASH_SIZE) ||
                CRYPT_MEMCMP(hash_all, hash_md5, MD5_HASH_SIZE)) {
                ali_crypto_print_data("md5", hash, MD5_HASH_SIZE);
                GO_RET(-1, "md5 test fail!\n");
            } else {
                CRYPT_INF("md5 test success!\n");
            }
        }
    }

    return 0;

_OUT:
    if (hash_ctx) {
        CRYPT_FREE(hash_ctx);
    }

    return -1;
}
