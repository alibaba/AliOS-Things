/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_client_api.h"
#include "tee_id2.h"
#include "tee_id2_cmd.h"

static const TEEC_UUID tee_id2_uuid = ID2_SRV_UUID;

int tee_get_ID2(uint8_t *id2, uint32_t *len)
{
    int ret = 0;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;

    TEE_ASSERT(id2 || len);

    if (id2 == NULL) {
        *len = ID_LEN;
        return 0;
    }

    result = TEEC_OpenSession(&ss, &tee_id2_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_get_ID2: open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup2;
    }

    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_MEMREF_TEMP_OUTPUT,
                        TEEC_NONE,
                        TEEC_NONE,
                        TEEC_NONE);
    op.params[0].tmpref.buffer = id2;
    op.params[0].tmpref.size = ID_LEN;
    result = TEEC_InvokeCommand(&ss, TEE_ID2_GET_ID, &op);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_get_ID2: inv cmd err(%08x)\n", result);
        ret = -1;
        goto cleanup3;
    }

    if (len) {
        *len = ID_LEN;
    }

cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}
