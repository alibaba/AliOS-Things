/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "aes_test.h"

int32_t aes_ecb_cbc_test_v1(void)
{
    tee_crypto_result ret;
    int32_t ctx_sz;
    uint8_t ctx[TEST_AES_CTX_BUF_SZ];
    uint8_t dst[DATA_SIZE];
    uint8_t dst2[DATA_SIZE];

    ret = tee_aes_get_ctx_size(AES_ECB, (size_t *)&ctx_sz);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_get_ctx_size: 0x%x\n", ret);
        return -1;
    }

    if (ctx_sz > TEST_AES_CTX_BUF_SZ) {
        tee_dbg_print(ERR, "Err: ctx size > TEST_AES_CTX_BUF_SZ\n");
        return -1;
    }

    //dump_code("src", SrcData, DATA_SIZE);

    /**************************************************************************/
    memset(ctx, 0, ctx_sz);
    memset(dst, 0, DATA_SIZE);
    memset(dst2, 0, DATA_SIZE);
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

    //dump_code("aes-ecb enc", dst, DATA_SIZE);

    memset(ctx, 0, ctx_sz);
    ret = tee_aes_init(AES_ECB, false, Key128, NULL, 16, NULL, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_init: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_process(dst, dst2, DATA_SIZE, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_finish: 0x%x\n", ret);
        return -1;
    }

    //dump_code("aes-ecb enc-dec", dst2, DATA_SIZE);

    if (0 != memcmp(dst2, SrcData, DATA_SIZE)) {
        tee_dbg_print(ERR, "Data cmp ERR!\n");
        return -1;
    }


    /**************************************************************************/
    memset(ctx, 0, ctx_sz);
    memset(dst, 0, DATA_SIZE);
    memset(dst2, 0, DATA_SIZE);
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

    //dump_code("aes-cbc enc", dst, DATA_SIZE);

    memset(ctx, 0, ctx_sz);
    ret = tee_aes_init(AES_CBC, false, Key128, NULL, 16, IV, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_init: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_process(dst, dst2, DATA_SIZE, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    ret = tee_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, ctx);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_finish: 0x%x\n", ret);
        return -1;
    }

    //dump_code("aes-cbc enc-dec", dst2, DATA_SIZE);

    if (0 != memcmp(dst2, SrcData, DATA_SIZE)) {
        tee_dbg_print(ERR, "Err: Data cmp ERR!\n");
        return -1;
    }

    tee_dbg_print(INF, "PASS-----\n");

    return 0;
}
