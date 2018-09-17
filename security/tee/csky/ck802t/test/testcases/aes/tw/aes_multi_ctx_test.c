/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aes_test.h"

int32_t aes_multi_ctx_test_v4(void)
{
    tee_crypto_result ret;
    int32_t           ctx_sz;
    uint8_t           ctx1[TEST_AES_CTX_BUF_SZ];
    uint8_t           ctx2[TEST_AES_CTX_BUF_SZ];
    uint8_t           dst1[DATA_SIZE];
    uint8_t           dst2[DATA_SIZE];

    ret = tee_aes_get_ctx_size(AES_CBC, (size_t *)&ctx_sz);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_get_ctx_size: 0x%x\n", ret);
        return -1;
    }

    if (ctx_sz > TEST_AES_CTX_BUF_SZ) {
        tee_dbg_print(ERR, "Err: ctx size > TEST_AES_CTX_BUF_SZ\n");
        return -1;
    }

    /* key128 */
    memset(ctx1, 0, ctx_sz);
    memset(ctx2, 0, ctx_sz);

    /* thread1: init key128 */
    ret = tee_aes_init(AES_CBC, true, Key128, NULL, 16, IV, ctx1);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_init: 0x%x\n", ret);
        return -1;
    }

    /* thread2: init key256 */
    ret = tee_aes_init(AES_ECB, true, Key256, NULL, 32, NULL, ctx2);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_init: 0x%x\n", ret);
        return -1;
    }

    /* thread1: process1 */
    ret = tee_aes_process(SrcData, dst1, (DATA_SIZE >> 1), ctx1);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    /* thread2: process1 */
    ret = tee_aes_process(SrcData, dst2, (DATA_SIZE >> 1), ctx2);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    /* thread1: process2 */
    ret = tee_aes_process(SrcData + (DATA_SIZE >> 1), dst1 + (DATA_SIZE >> 1),
                          (DATA_SIZE >> 1), ctx1);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    /* thread2: process2 */
    ret = tee_aes_process(SrcData + (DATA_SIZE >> 1), dst2 + (DATA_SIZE >> 1),
                          (DATA_SIZE >> 1), ctx2);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_process: 0x%x\n", ret);
        return -1;
    }

    /* thread1: finish */
    ret = tee_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, ctx1);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_finish: 0x%x\n", ret);
        return -1;
    }


    /* thread2: finish */
    ret = tee_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, ctx2);
    if (TEE_CRYPTO_SUCCESS != ret) {
        tee_dbg_print(ERR, "Err: tee_aes_finish: 0x%x\n", ret);
        return -1;
    }


    dump_code("Thread1 Result", dst1, DATA_SIZE);
    dump_code("Thread2 Result", dst2, DATA_SIZE);
    if (0 != memcmp(DstData_Key128_Opssl_CBC_Out, dst1, DATA_SIZE)) {
        tee_dbg_print(ERR, "Thread1 Result Err!\n");
        return -1;
    } else {
        tee_dbg_print(ERR, "Thread1 Pass!\n");
    }

    if (0 != memcmp(DstData_Key256_Opssl_ECB_Out, dst2, DATA_SIZE)) {
        tee_dbg_print(ERR, "Thread2 Result Err!\n");
        return -1;
    } else {
        tee_dbg_print(ERR, "Thread2 Pass!\n");
    }

    return 0;
}
