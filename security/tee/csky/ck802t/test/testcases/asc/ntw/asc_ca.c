/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_client_api.h"
#include "tee_dbg.h"
#include "asc.h"

#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))

static const TEEC_UUID tee_asc_uuid = ASC_SRV_UUID;

int tee_asc_drv_test()
{
    uint8_t temp[ASC_TEST_SRAM_SIZE];
    uint8_t data[ASC_TEST_SRAM_SIZE];
    uint32_t test_addr = 0;
    void * addr = NULL;
    uint32_t test_size = ASC_TEST_SRAM_SIZE;
    TEEC_Session     ss;
    TEEC_Operation   op;
    TEEC_Result      ret = TEEC_SUCCESS;

    ret = TEEC_OpenSession(&ss, &tee_asc_uuid, NULL);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to open ss\n", ret);
        goto cleanup2;
    }

    addr = kmalloc(ASC_TEST_SRAM_SIZE + 64, 0);
    if (0 == addr) {
        tee_dbg_print(ERR, "out of memory\n");
        goto cleanup3;
    }

    /* save test memory to temp data */
    test_addr = ROUNDUP((uint32_t)addr, 64);
    memcpy(temp, (void *)test_addr, test_size);

    memset(data, 0xa, test_size);
    memset((void *)test_addr, 0x0, test_size);
    TEE_ASSERT(memcmp((void *)test_addr, data, test_size));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
                               TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = test_addr;
    op.params[0].value.b = test_size;

    /* set non-write perm in tw */
    ret = TEEC_InvokeCommand(&ss, TEE_SET_PERM, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_SET_PERM);
        goto cleanup3;
    }

    memcpy((void *)test_addr, data, test_size);
    if (memcmp((void *)test_addr, data, test_size)) {
        tee_dbg_print(INF, "set mem perm test success\n");
    } else {
        tee_dbg_print(INF, "set mem perm test fail\n");
    }

    /* reset this memory perm */
    ret = TEEC_InvokeCommand(&ss, TEE_CLR_PERM, &op);
    if (ret != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "fail(%08x) to inv cmd(%d)\n", ret, TEE_CLR_PERM);
        goto cleanup3;
    }

    /* restore test memory */
    memcpy((void *)test_addr, temp, test_size);

cleanup3:
    TEEC_CloseSession(&ss);
    if (addr) {
        kfree((void *)addr);
    }
cleanup2:

    return ret;
}
