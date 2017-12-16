/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */
#include "tee_client_api.h"
#include "tee_dbg.h"
#include "tee_aes_cmd.h"

static const TEEC_UUID tee_aes_uuid = AES_SRV_UUID;

int32_t tee_aes_cbc_encrypt(const uint8_t *key,
                            const uint8_t *iv,
                            int32_t input_len,
                            const uint8_t *input,
                            uint8_t *output)
{
    TEEC_Session     ss;
    TEEC_Operation   op;
    TEEC_Result      ret = TEEC_SUCCESS;

    if ((key == NULL) || (iv == NULL) || (input == NULL) || (output == NULL) || (input_len <= 0)) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    ret = TEEC_OpenSession(&ss, &tee_aes_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to open ss\n", ret);
        goto cleanup2;
    }

    op.params[0].tmpref.buffer = (uint8_t *)key;
    op.params[0].tmpref.size   = AES_KEY_SIZE;
    op.params[1].tmpref.buffer = (uint8_t *)iv;
    op.params[1].tmpref.size   = AES_IV_SIZE;
    op.params[2].tmpref.buffer = (uint8_t *)input;
    op.params[2].tmpref.size   = input_len;
    op.params[3].tmpref.buffer = output;
    op.params[3].tmpref.size   = input_len;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_OUTPUT);

    ret = TEEC_InvokeCommand(&ss, TEE_AES_CBC_ENC, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_AES_CBC_ENC);
        goto cleanup3;
    } else {
        tee_dbg_print(INF, "tee aes cbc enc success\n");
    }
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}

int32_t tee_aes_cbc_decrypt(const uint8_t *key,
                            const uint8_t *iv,
                            int32_t input_len,
                            const uint8_t *input,
                            uint8_t *output)
{
    TEEC_Session     ss;
    TEEC_Operation   op;
    TEEC_Result      ret = TEEC_SUCCESS;

    ret = TEEC_OpenSession(&ss, &tee_aes_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to open ss\n", ret);
        goto cleanup2;
    }

    op.params[0].tmpref.buffer = (uint8_t *)key;
    op.params[0].tmpref.size   = AES_KEY_SIZE;
    op.params[1].tmpref.buffer = (uint8_t *)iv;
    op.params[1].tmpref.size   = AES_IV_SIZE;
    op.params[2].tmpref.buffer = (uint8_t *)input;
    op.params[2].tmpref.size   = input_len;
    op.params[3].tmpref.buffer = output;
    op.params[3].tmpref.size   = input_len;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_OUTPUT);

    ret = TEEC_InvokeCommand(&ss, TEE_AES_CBC_DEC, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_AES_CBC_DEC);
        goto cleanup3;
    } else {
        tee_dbg_print(INF, "tee aes cbc dec success\n");
    }
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}
