/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_client_api.h"
#include "tee_dbg.h"
#include "rng.h"

static const TEEC_UUID tee_random_uuid = RANDOM_SRV_UUID;

int tee_random_test()
{
    TEEC_Session     ss;
    TEEC_Operation   op;
    TEEC_Result      ret = TEEC_SUCCESS;
    uint32_t          random_data[4];

    ret = TEEC_OpenSession(&ss, &tee_random_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to open ss\n", ret);
        goto cleanup2;
    }


    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INPUT,
                                     TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = (uint8_t *)random_data;
    op.params[0].tmpref.size   = 16;
    op.params[1].value.a       = 0x55667788;
    op.params[1].value.b       = 0x0;

    ret = TEEC_InvokeCommand(&ss, TEE_RANDOM_TEST, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_RANDOM_TEST);
        goto cleanup3;
    } else {
        tee_dbg_print(INF, "tee random test success\n");
    }
    tee_dbg_print(INF, "ca get: %x %x %x %x\n", random_data[0], random_data[1], random_data[2], random_data[3]);
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}
