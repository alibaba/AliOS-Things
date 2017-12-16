/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "mem.h"
#include "tee_dbg.h"
#include "tee_client_api.h"

#define TST_DATA_LEN        (0x80)
#define SHM_INPUT_OFFSET    (0x0)
#define SHM_OUTPUT_OFFSET   (0x80)
#define SHM_LEN             (0x100)

static const TEEC_UUID tee_srv_mem = MEM_SRV_UUID;

#ifdef MEM_DBG
static void _dump_data(uint8_t *name, uint8_t *data, uint32_t len)
{
    uint32_t i;

    TEE_ASSERT(data && len);

    for (i = 0; i < (len - len % 8); i += 8) {
        tee_dbg_print(INF, "%s: %02x%02x %02x%02x %02x%02x %02x%02x\n",
                          name, data[i+0], data[i+1], data[i+2], data[i+3],
                          data[i+4], data[i+5], data[i+6], data[i+7]);
    }
    while(i < len) {
        tee_dbg_print(INF, "%s: %02x\n", name, data[i++]);
    }

    return;
}
#endif

int tee_tmp_mem_test()
{
    uint8_t input[TST_DATA_LEN];
    uint8_t output[TST_DATA_LEN];
    TEEC_Session     session;
    TEEC_Operation   operation;
    TEEC_Result      result = TEEC_SUCCESS;

    result = TEEC_OpenSession(&session, &tee_srv_mem, NULL);
    if (result != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "tmp_mem: open ss fail(%08x)\n", result);
        goto cleanup2;
    }

    memset(input, 0xa, TST_DATA_LEN);
    memset(output, 0xf, TST_DATA_LEN);
    if (!memcmp(input, output, TST_DATA_LEN)) {
        TEE_ASSERT(0);
    }

    operation.paramTypes = TEEC_PARAM_TYPES(
                               TEEC_MEMREF_TEMP_INPUT,
                               TEEC_MEMREF_TEMP_OUTPUT,
                               TEEC_NONE,
                               TEEC_NONE);
    operation.params[0].tmpref.buffer = input;
    operation.params[0].tmpref.size = TST_DATA_LEN;
    operation.params[1].tmpref.buffer = output;
    operation.params[1].tmpref.size = TST_DATA_LEN;

    result = TEEC_InvokeCommand(&session, TEE_CMD_TMP_MEM, &operation);
    if (result != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "tmp_mem: inv cmd fail(%08x)\n", result);
        goto cleanup3;
    }

#ifdef MEM_DBG
    _dump_data((uint8_t *)"output", output, TST_DATA_LEN);
#endif

    if (memcmp(input, operation.params[1].tmpref.buffer, TST_DATA_LEN)) {
        tee_dbg_print(ERR, "tmp_mem: test fail!\n");
        result = TEEC_ERROR_GENERIC;
    } else {
        tee_dbg_print(INF, "tmp_mem: test success!\n");
        result = TEEC_SUCCESS;
    }

cleanup3:
    TEEC_CloseSession(&session);
cleanup2:

    if (result != TEEC_SUCCESS) {
        return -1;
    }

    return 0;
}

int tee_regs_shm_test()
{
    uint8_t shm_buf[SHM_LEN];
    TEEC_Session      session;
    TEEC_SharedMemory sharedMem;
    TEEC_Operation    operation;
    TEEC_Result       result = TEEC_SUCCESS;

    result = TEEC_OpenSession(&session, &tee_srv_mem, NULL);
    if (result != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "regs_shm: open ss fail(%08x)\n", result);
        goto cleanup2;
    }

    memset(shm_buf, 0xa, TST_DATA_LEN);
    memset(shm_buf + TST_DATA_LEN, 0xf, TST_DATA_LEN);
    if (!memcmp(shm_buf, shm_buf + TST_DATA_LEN, TST_DATA_LEN)) {
        TEE_ASSERT(0);
    }

    sharedMem.buffer = (void *)shm_buf;
    sharedMem.size   = SHM_LEN;
    sharedMem.flags  = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    result = TEEC_RegisterSharedMemory(&sharedMem);
    if (TEEC_SUCCESS != result) {
        tee_dbg_print(ERR,"regs_shm, regs shm fail(%08x)\n", result);
        goto cleanup3;
    }

    operation.paramTypes = TEEC_PARAM_TYPES(
                               TEEC_MEMREF_PARTIAL_INPUT,
                               TEEC_MEMREF_PARTIAL_OUTPUT,
                               TEEC_NONE,
                               TEEC_NONE);
    operation.params[0].memref.parent = &sharedMem;
    operation.params[0].memref.offset = SHM_INPUT_OFFSET;
    operation.params[0].memref.size   = TST_DATA_LEN;
    operation.params[1].memref.parent = &sharedMem;
    operation.params[1].memref.offset = SHM_OUTPUT_OFFSET;
    operation.params[1].memref.size   = TST_DATA_LEN;

    result = TEEC_InvokeCommand(&session, TEE_CMD_REGS_SHM, &operation);
    if (result != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "regs_shm: inv cmd fail(%08x)\n", result);
        goto cleanup4;
    }

#ifdef MEM_DBG
    _dump_data((uint8_t *)"regs_shm", sharedMem.buffer, SHM_LEN);
#endif

    if (memcmp(shm_buf, shm_buf + TST_DATA_LEN, TST_DATA_LEN)) {
        tee_dbg_print(ERR, "regs_shm: test fail!\n");
        result = TEEC_ERROR_GENERIC;
    } else {
        tee_dbg_print(INF, "regs_shm: test success!\n");
        result = TEEC_SUCCESS;
    }

cleanup4:
    TEEC_ReleaseSharedMemory(&sharedMem);
cleanup3:
    TEEC_CloseSession(&session);
cleanup2:

    if (result != TEEC_SUCCESS) {
        return -1;
    }

    return 0;
}

int tee_alloc_shm_test()
{
    TEEC_Session      session;
    TEEC_SharedMemory sharedMem;
    TEEC_Operation    operation;
    TEEC_Result       result = TEEC_SUCCESS;

    result = TEEC_OpenSession(&session, &tee_srv_mem, NULL);
    if (result != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "alloc_shm: open ss fail(%08x)\n", result);
        goto cleanup2;
    }

    sharedMem.size   = SHM_LEN;
    sharedMem.flags  = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    result = TEEC_AllocateSharedMemory(&sharedMem);
    if (TEEC_SUCCESS != result) {
        tee_dbg_print(ERR, "alloc_shm, alloc shm fail(%08x)\n", result);
        goto cleanup3;
    }

    memset(sharedMem.buffer, 0xa, TST_DATA_LEN);
    memset((uint8_t *)sharedMem.buffer + TST_DATA_LEN, 0xf, TST_DATA_LEN);
    if (!memcmp(sharedMem.buffer,
            (uint8_t *)sharedMem.buffer + TST_DATA_LEN, TST_DATA_LEN)) {
        TEE_ASSERT(0);
    }

    operation.paramTypes = TEEC_PARAM_TYPES(
                               TEEC_MEMREF_PARTIAL_INPUT,
                               TEEC_MEMREF_PARTIAL_OUTPUT,
                               TEEC_NONE,
                               TEEC_NONE);
    operation.params[0].memref.parent = &sharedMem;
    operation.params[0].memref.offset   = SHM_INPUT_OFFSET;
    operation.params[0].memref.size = TST_DATA_LEN;
    operation.params[1].memref.parent = &sharedMem;
    operation.params[1].memref.offset   = SHM_OUTPUT_OFFSET;
    operation.params[1].memref.size = TST_DATA_LEN;

    result = TEEC_InvokeCommand(&session, TEE_CMD_ALOC_SHM, &operation);
    if (result != TEEC_SUCCESS) {
        tee_dbg_print(ERR, "alloc_shm: inv cmd fail(%08x)\n", result);
        goto cleanup4;
    }

#ifdef MEM_DBG
    _dump_data((uint8_t *)"alloc_shm", sharedMem.buffer, SHM_LEN);
#endif

    if (memcmp(sharedMem.buffer,
               (uint8_t *)sharedMem.buffer + TST_DATA_LEN, TST_DATA_LEN)) {
        tee_dbg_print(ERR, "alloc_shm: test fail!\n");
        result = TEEC_ERROR_GENERIC;
    } else {
        tee_dbg_print(INF, "alloc_shm: test success!\n");
        result = TEEC_SUCCESS;
    }

cleanup4:
    TEEC_ReleaseSharedMemory(&sharedMem);
cleanup3:
    TEEC_CloseSession(&session);
cleanup2:

    if (result != TEEC_SUCCESS) {
        return -1;
    }

    return 0;
}
