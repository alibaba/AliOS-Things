/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_client_api.h"
#include "tee_dbg.h"
#include "tee_random_cmd.h"

static const TEEC_UUID tee_random_uuid = RANDOM_SRV_UUID;

int tee_random_seed(uint32_t seed)
{
    TEEC_Session     ss;
    TEEC_Operation   op;
    TEEC_Result      ret = TEEC_SUCCESS;

    ret = TEEC_OpenSession(&ss, &tee_random_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to open ss\n", ret);
        goto cleanup2;
    }

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE,
                                     TEEC_NONE);
    if (seed == 0) {
        op.params[0].value.a    = 0x1ab48fd5;
        op.params[0].value.b    = 0xbe836b2a;
    } else {
        op.params[0].value.a    = seed * 0x1ab48fd5;
        op.params[0].value.b    = seed * 0xbe836b2a;
    }

    ret = TEEC_InvokeCommand(&ss, TEE_RANDOM_SEED_SET, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_RANDOM_SEED_SET);
        goto cleanup3;
    } else {
        tee_dbg_print(INF, "tee random seed success\n");
    }
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}

int tee_random_generate(uint8_t *buf, size_t len)
{
    TEEC_Session     ss;
    TEEC_Operation   op;
    TEEC_Result      ret = TEEC_SUCCESS;

    ret = TEEC_OpenSession(&ss, &tee_random_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to open ss\n", ret);
        goto cleanup2;
    }

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT,
                                     TEEC_NONE,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = buf;
    op.params[0].tmpref.size   = len;

    ret = TEEC_InvokeCommand(&ss, TEE_RANDOM_DATA_GET, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_RANDOM_DATA_GET);
        goto cleanup3;
    } else {
        tee_dbg_print(INF, "tee random generate success\n");
    }
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}
