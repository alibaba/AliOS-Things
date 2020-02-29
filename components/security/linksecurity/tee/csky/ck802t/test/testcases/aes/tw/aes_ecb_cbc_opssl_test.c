/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aes_test.h"

int32_t aes_ecb_opssl_test_v2(void)
{
    tee_crypto_result ret;
    int32_t           ctx_sz;
    uint32_t          ctx[TEST_AES_CTX_BUF_SZ >> 2];
    uint8_t           dst[DATA_SIZE];

    ret = tee_aes_get_ctx_size(AES_ECB, (size_t *)&ctx_sz);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_get_ctx_size: 0x%x\n", ret);
        return -1;
    }

    if (ctx_sz > TEST_AES_CTX_BUF_SZ) {
        tee_dbg_print(ERR, "Err: ctx size > TEST_AES_CTX_BUF_SZ\n");
        return -1;
    }

    /* key128 */
    memset(ctx, 0, ctx_sz);
    ret = tee_aes_init(AES_ECB, true, Key128, NULL, 16, NULL, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_init: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_process(SrcData, dst, DATA_SIZE, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_finish: 0x%x\n", ret);
        return -1;
    }
    dump_code("key128 Data", dst, DATA_SIZE);
    if (0 != memcmp(dst, DstData_Key128_Opssl_ECB_Out, DATA_SIZE)) {
        tee_dbg_print(ERR, "key128 Data Cmp ERR!\n");
        return -1;
    } else {
        tee_dbg_print(INF, "PASS-----\n");
    }

    return 0;
}

int32_t aes_cbc_opssl_test_v3(void)
{
    int32_t  ret, ctx_sz;
    uint32_t ctx[TEST_AES_CTX_BUF_SZ >> 2];
    uint8_t  dst[DATA_SIZE];

    ret = tee_aes_get_ctx_size(AES_ECB, (size_t *)&ctx_sz);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_get_ctx_size: 0x%x\n", ret);
        return -1;
    }

    if (ctx_sz > TEST_AES_CTX_BUF_SZ) {
        tee_dbg_print(ERR, "Err: ctx size > TEST_AES_CTX_BUF_SZ\n");
        return -1;
    }

    /* key128 */
    memset(ctx, 0, ctx_sz);
    ret = tee_aes_init(AES_CBC, true, Key128, NULL, 16, IV, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_init: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_process(SrcData, dst, DATA_SIZE, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_finish: 0x%x\n", ret);
        return -1;
    }
    dump_code("key128 Data", dst, DATA_SIZE);
    if (0 != memcmp(dst, DstData_Key128_Opssl_CBC_Out, DATA_SIZE)) {
        tee_dbg_print(ERR, "Err: key128 Data Cmp ERR!\n");
        return -1;
    } else {
        tee_dbg_print(INF, "PASS-----\n");
    }

    return 0;
}
