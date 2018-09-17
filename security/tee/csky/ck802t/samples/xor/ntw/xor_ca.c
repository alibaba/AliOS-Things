/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_client_api.h"
#include "tee_dbg.h"
#include "xor.h"

static const TEEC_UUID _g_uuid = XOR_SRV_UUID;

/* c = a ^ b */
int tee_xor_test()
{
    unsigned int   a = 12345, b = 67890, c = 0;
    TEEC_Context   cntx;
    TEEC_Session   ss;
    TEEC_Operation op;
    TEEC_Result    ret = TEEC_SUCCESS;

    ret = TEEC_OpenSession(&ss, &_g_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "failed to call TEEC_OpenSession, 0x%08x\n", ret);
        goto cleanup2;
    }

    op.paramTypes        = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT,
                                     TEEC_VALUE_OUTPUT, TEEC_NONE);
    op.params[0].value.a = a;
    op.params[1].value.a = b;
    op.params[2].value.a = c;

    ret = TEEC_InvokeCommand(&ss, CMD_XOR, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "failed to call TEEC_InvokeCommand, 0x%08x\n", ret);
        goto cleanup3;
    }
    c = op.params[2].value.a;
    tee_dbg_print(INF, "a = %d; b = %d; c = a ^ b: %d\n", a, b, c);
    if (c != (a ^ b)) {
        ret = TEEC_ERROR_GENERIC;
        tee_dbg_print(ERR, "XOR failed!\n");
        goto cleanup3;
    }

cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:
    return ret;
}
