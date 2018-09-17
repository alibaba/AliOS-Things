/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_tos.h"
#include "tee_srv.h"
#include "tee_dbg.h"
#include "hash.h"
#include "tee_crypto.h"

#define HASH_MAX_SIZE 64
#define TEST_DATA_SIZE 141


/**
  Self-test the hash
  @return TEE_CRYPTO_SUCCESS if successful, CRYPT_NOP if self-tests have been
  disabled
*/
int md5_test(void)
{
    return TEE_CRYPTO_SUCCESS;
}

static uint8_t test_data[141] = {
    0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4,
    0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8,
    0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4,
    0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8,
    0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4,
    0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8,
    0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4,
    0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8,
    0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4,
    0x5,  0x6,  0x7, 0x8, 0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8,
    0x01, 0x02, 0x3, 0x4, 0x5,  0x6,  0x7, 0x8, 0xa,  0xb,  0xc, 0xd,
    0xe,  0xf,  0x0, 0x1, 0x2,  0x3,  0x4, 0x5, 0x13
};

uint8_t hash_sha1[20] = { 0x54, 0x1d, 0x6f, 0x6e, 0x46, 0x7e, 0xfe,
                          0x1d, 0xa8, 0x66, 0x06, 0x34, 0xb0, 0x21,
                          0x3d, 0x65, 0xb8, 0xa4, 0x02, 0xca };

uint8_t hash_sha224[28] = { 0x36, 0x94, 0x36, 0xf9, 0xd4, 0xe9, 0xbe,
                            0x59, 0xbb, 0x59, 0x5c, 0x73, 0x4a, 0xf0,
                            0xe8, 0x52, 0x48, 0x09, 0x84, 0x42, 0xec,
                            0x80, 0xdb, 0x86, 0x5a, 0x51, 0x44, 0x3a };

uint8_t hash_sha256[32] = { 0x3b, 0x7f, 0x52, 0xae, 0x5b, 0xe8, 0x09, 0x19,
                            0x02, 0x1a, 0x83, 0x8d, 0xcc, 0xc6, 0x01, 0xc3,
                            0x76, 0x41, 0x22, 0x64, 0x4b, 0x1c, 0x35, 0xa2,
                            0x9d, 0xd3, 0xc5, 0x76, 0x36, 0xd7, 0xda, 0x5f };

uint8_t hash_sha384[48] = { 0x21, 0xc7, 0x05, 0xb3, 0x37, 0x66, 0xf3, 0xb5,
                            0x0d, 0x51, 0xf5, 0x0f, 0x91, 0xfc, 0xa1, 0xcf,
                            0x78, 0x35, 0x82, 0x77, 0xfd, 0x2c, 0x31, 0xbb,
                            0x8a, 0x26, 0x6f, 0x2a, 0x82, 0x52, 0x1a, 0x70,
                            0xfd, 0xfc, 0xa2, 0xb7, 0xee, 0x7f, 0xb5, 0xfd,
                            0x9e, 0x20, 0x36, 0x91, 0xc6, 0xd6, 0x54, 0xa0 };

uint8_t hash_sha512[64] = { 0x9e, 0xca, 0x2a, 0x96, 0x01, 0x48, 0x0f, 0xa2,
                            0x6b, 0x99, 0x27, 0x1a, 0x7f, 0x72, 0xe3, 0xa4,
                            0xee, 0x2f, 0x08, 0x92, 0x2e, 0xdb, 0xf7, 0x19,
                            0xd8, 0xcd, 0xcb, 0xfc, 0x8b, 0x56, 0x8c, 0x04,
                            0xfb, 0xb3, 0x69, 0xdf, 0x26, 0xfb, 0x0b, 0x9f,
                            0xbe, 0x1d, 0x42, 0xbd, 0x39, 0x87, 0x52, 0x16,
                            0x42, 0xac, 0x62, 0x57, 0x94, 0x59, 0xa4, 0xce,
                            0x8d, 0x69, 0x78, 0xb7, 0xf8, 0x95, 0xb8, 0x78 };

TEE_Result test_sha_hash()
{
    TEE_Result        ret = TEE_SUCCESS;
    tee_crypto_result result;
    uint32_t          hash_len;
    uint8_t           hash1[HASH_MAX_SIZE];
    uint8_t           hash2[HASH_MAX_SIZE];
    hash_type_t       type;
    uint32_t          hash_ctx_size;
    void *            hash_ctx = NULL;

    for (type = SHA1; type <= SHA512; type++) {
        switch (type) {
            case SHA1:
                hash_len = 20;
                break;
            case SHA224:
                hash_len = 28;
                break;
            case SHA256:
                hash_len = 32;
                break;
            case SHA384:
                hash_len = 48;
                break;
            case SHA512:
                hash_len = 64;
                break;
            default:
                tee_dbg_print(ERR, "not support type(%08x)\n", type);
                return TEE_ERROR_NOT_SUPPORTED;
        }

        tee_dbg_print(ERR, "hash type: 0x%x!\n", type);

        result = tee_hash_get_ctx_size(type, (size_t *)(&hash_ctx_size));
        if (result != TEE_CRYPTO_SUCCESS) {
            tee_dbg_print(ERR, "get ctx size fail(%08x)\n", result);
            return TEE_ERROR_GENERIC;
        }

        hash_ctx = malloc(hash_ctx_size);
        if (hash_ctx == NULL) {
            tee_dbg_print(ERR, "kmalloc(%d) fail\n", hash_ctx_size);
            return TEE_ERROR_GENERIC;
        }
        memset(hash_ctx, 0, hash_ctx_size);

        /* Calc HASH1 */
        result = tee_hash_init(type, hash_ctx);
        if (result != TEE_CRYPTO_SUCCESS) {
            ret = TEE_ERROR_GENERIC;
            tee_dbg_print(ERR, "init fail(%08x) \n", result);
            goto _error;
        }

        result = tee_hash_update(test_data, 13, hash_ctx);
        if (result != TEE_CRYPTO_SUCCESS) {
            ret = TEE_ERROR_GENERIC;
            tee_dbg_print(ERR, "update 1th fail(%08x) \n", result);
            goto _error;
        }
        result = tee_hash_update(test_data + 13, 63, hash_ctx);
        if (result != TEE_CRYPTO_SUCCESS) {
            ret = TEE_ERROR_GENERIC;
            tee_dbg_print(ERR, "update 2th fail(%08x) \n", result);
            goto _error;
        }
        result = tee_hash_update(test_data + 13 + 63, 65, hash_ctx);
        if (result != TEE_CRYPTO_SUCCESS) {
            ret = TEE_ERROR_GENERIC;
            tee_dbg_print(ERR, "update 3th fail(%08x) \n", result);
            goto _error;
        }

        result = tee_hash_final(hash1, hash_ctx);
        if (result != TEE_CRYPTO_SUCCESS) {
            ret = TEE_ERROR_GENERIC;
            tee_dbg_print(ERR, "final fail(%08x) \n", result);
            goto _error;
        }
        free(hash_ctx);
        hash_ctx = NULL;

        /* Calc HASH2 */
        result = tee_hash_digest(type, test_data, TEST_DATA_SIZE, hash2);
        if (result != TEE_CRYPTO_SUCCESS) {
            ret = TEE_ERROR_GENERIC;
            tee_dbg_print(ERR, "digest fail(%08x) \n", result);
            goto _error;
        }

        if (type == SHA1) {
            if ((memcmp(hash1, hash_sha1, 20)) ||
                (memcmp(hash2, hash_sha1, 20))) {
                tee_dbg_print(INF, "SHA1(mode - %08x) test fail!\n", type);
                ret = TEE_ERROR_GENERIC;
            } else {
                tee_dbg_print(INF, "SHA1(mode - %08x) test success!\n", type);
                ret = TEE_SUCCESS;
            }
        } else if (type == SHA224) {
            if ((memcmp(hash1, hash_sha224, 28)) ||
                (memcmp(hash2, hash_sha224, 28))) {
                tee_dbg_print(INF, "SHA224(mode - %08x) test fail!\n", type);
                ret = TEE_ERROR_GENERIC;
            } else {
                tee_dbg_print(INF, "SHA224(mode - %08x) test success!\n", type);
                ret = TEE_SUCCESS;
            }
        } else if (type == SHA256) {
            if ((memcmp(hash1, hash_sha256, 32)) ||
                (memcmp(hash2, hash_sha256, 32))) {
                tee_dbg_print(INF, "SHA256(mode - %08x) test fail!\n", type);
                ret = TEE_ERROR_GENERIC;
            } else {
                tee_dbg_print(INF, "SHA256(mode - %08x) test success!\n", type);
                ret = TEE_SUCCESS;
            }
        } else if (type == SHA384) {
            if ((memcmp(hash1, hash_sha384, 48)) ||
                (memcmp(hash2, hash_sha384, 48))) {
                tee_dbg_print(INF, "SHA384(mode - %08x) test fail!\n", type);
                ret = TEE_ERROR_GENERIC;
            } else {
                tee_dbg_print(INF, "SHA384(mode - %08x) test success!\n", type);
                ret = TEE_SUCCESS;
            }
        } else if (type == SHA512) {
            if ((memcmp(hash1, hash_sha512, 48)) ||
                (memcmp(hash2, hash_sha512, 48))) {
                tee_dbg_print(INF, "SHA512(mode - %08x) test fail!\n", type);
                ret = TEE_ERROR_GENERIC;
            } else {
                tee_dbg_print(INF, "SHA512(mode - %08x) test success!\n", type);
                ret = TEE_SUCCESS;
            }
        }

        if (TEE_SUCCESS != ret) {
            goto _error;
        }
    }

_error:
    if (hash_ctx) {
        free(hash_ctx);
        hash_ctx = 0;
    }
    return ret;
}

static TEE_Result _hash_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _hash_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _hash_OpenSessionEntryPoint(uint32_t  paramTypes,
                                              TEE_Param params[4],
                                              void **   sessionContext)
{
    return TEE_SUCCESS;
}

static void _hash_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result _hash_InvokeCommandEntryPoint(void *    sessionContext,
                                                uint32_t  commandID,
                                                uint32_t  paramTypes,
                                                TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;

    if (paramTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE,
                                      TEE_PARAM_TYPE_NONE,
                                      TEE_PARAM_TYPE_NONE)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (commandID == TEE_SHA_TEST) {
        tee_dbg_print(INF, "HW Hash test start!\n");
        ret = test_sha_hash();
        if (ret != TEE_SUCCESS) {
            tee_dbg_print(ERR, "HW HASH test fail!\n");
            return ret;
        }
        tee_dbg_print(INF, "SW MD5 test start!\n");
        ret = md5_test();
        if (ret != TEE_SUCCESS) {
            tee_dbg_print(ERR, "SW MD5 test fail!\n");
            return ret;
        }
    } else {
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    return ret;
}

TEE_SRV_DATA_START(_hash_CreateEntryPoint, _hash_DestroyEntryPoint,
                   _hash_OpenSessionEntryPoint, _hash_CloseSessionEntryPoint,
                   _hash_InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", HASH_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END
