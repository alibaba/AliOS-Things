/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#include "ali_crypto.h"
#include "../libalicrypto/mbed/inc/mbed_crypto.h"
#include "tee_client_api.h"
#include "tee_crypto_client.h"

static const TEEC_UUID tee_crypt_uuid = CRYPTO_SRV_UUID;

ali_crypto_result ali_rand_gen(uint8_t *buf, size_t len)
{
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;

    if (buf == NULL || len == 0) {
        MBED_DBG_E("ali_rand_gen: invalid input args!\n");
        return ALI_CRYPTO_INVALID_ARG;
    }

    tee_uuid = tee_crypt_uuid;
    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "ali_aes_init: open ss err(0x%08x)\n", result);
    }

    op.paramTypes = TEEC_PARAM_TYPES(
            TEEC_MEMREF_TEMP_OUTPUT,
            TEEC_NONE,
            TEEC_NONE,
            TEEC_NONE);
    op.params[0].tmpref.buffer = buf;
    op.params[0].tmpref.size = len;

    result = TEEC_InvokeCommand(&ss, TEE_RAND_GEN, &op);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "ali_rand_gen: inv cmd err(%08x)\n", result);
    }

    TEEC_CloseSession(&ss);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result ali_seed(uint8_t *seed, size_t seed_len)
{
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;

    tee_uuid = tee_crypt_uuid;
    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "ali_seed: open ss err(0x%08x)\n", result);
    }

    op.paramTypes = TEEC_PARAM_TYPES(
            TEEC_MEMREF_TEMP_INOUT,
            TEEC_NONE,
            TEEC_NONE,
            TEEC_NONE);
    op.params[0].tmpref.buffer = seed;
    op.params[0].tmpref.size = seed_len;

    result = TEEC_InvokeCommand(&ss, TEE_SEED, &op);
    if (result != TEEC_SUCCESS) {
        PRINT_RET(ALI_CRYPTO_ERROR,
                "ali_seed: inv cmd err(%08x)\n", result);
    }

    TEEC_CloseSession(&ss);

    return ALI_CRYPTO_SUCCESS;
}
