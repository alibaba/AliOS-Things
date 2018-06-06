/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_client_api.h"
#include "tee_rsa.h"
#include "tee_id2_cmd.h"

static const TEEC_UUID id2_uuid = ID2_SRV_UUID;

int tee_RSA_generate_key(uint8_t ID, uint8_t type)
{
    ID2_DBG_I("tee_RSA_generate_key: not support!\n");

    return -1;
}

int tee_RSA_get_pub_key(uint8_t ID, uint8_t *pub, uint32_t *in_len)
{
    ID2_DBG_I("tee_RSA_get_pub_key: not support!\n");

    return -1;
}

int tee_RSA_sign(uint8_t ID, const uint8_t *in, uint32_t in_len,
                 uint8_t *sign, uint32_t *sign_len, uint8_t type)
{
    int ret = 0;
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;
    TEEC_SharedMemory shm;

    if (in == NULL || in_len <= 0 ||
        sign == NULL || sign_len == NULL) {
        ID2_DBG_E("tee_RSA_sign: invalid input args!\n");
        return -1;
    }

    if (ID == KEY_ID_0) {
        tee_uuid = id2_uuid;
    } else {
        ID2_DBG_E("tee_RSA_sign: not support this ID(%d)\n", ID);
        return -1;
    }

    if (*sign_len < SIGN_LEN) {
        ID2_DBG_E("tee_RSA_sign: short buffer!\n");
        *sign_len = SIGN_LEN;
        return -1;
    }

    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_RSA_sign: open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup2;
    }

    shm.size  = in_len + *sign_len;
    shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    result = TEEC_AllocateSharedMemory(&shm);
    if (TEEC_SUCCESS != result) {
        ID2_DBG_E("tee_RSA_sign: alloc shm err(%08x)\n", result);
        ret = -1;
        goto cleanup3;
    }

    memcpy(shm.buffer, (uint8_t *)in, in_len);
    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_VALUE_INPUT,
                        TEEC_MEMREF_PARTIAL_INPUT,
                        TEEC_MEMREF_PARTIAL_OUTPUT,
                        TEEC_NONE);
    op.params[0].value.a = type;
    op.params[1].memref.parent = &shm;
    op.params[1].memref.offset = 0;
    op.params[1].memref.size = in_len;
    op.params[2].memref.parent = &shm;
    op.params[2].memref.offset = in_len;
    op.params[2].memref.size = *sign_len;

    result = TEEC_InvokeCommand(&ss, TEE_ID2_RSA_SIGN, &op);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_RSA_sign: inv cmd err(%08x)\n", result);
        ret = -1;
        goto cleanup4;
    }

    *sign_len = op.params[2].memref.size;
    memcpy(sign, (uint8_t *)shm.buffer + in_len, *sign_len);

cleanup4:
    TEEC_ReleaseSharedMemory(&shm);
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:
    return ret;
}

int tee_RSA_verify(uint8_t ID, const uint8_t *in, uint32_t in_len,
                   uint8_t *sign, uint32_t sign_len, uint8_t type)
{
    int ret = 0;
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;
    TEEC_SharedMemory shm;

    if (in == NULL || in_len <= 0 ||
        sign == NULL || sign_len <= 0) {
        ID2_DBG_E("tee_RSA_verify: invalid input args!\n");
        return -1;
    }

    if (ID == KEY_ID_0) {
        tee_uuid = id2_uuid;
    } else {
        ID2_DBG_E("tee_RSA_verify: not support this ID(%d)\n", ID);
        return -1;
    }

    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_RSA_verify: open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup2;
    }

    shm.size  = in_len + sign_len;
    shm.flags = TEEC_MEM_INPUT;
    result = TEEC_AllocateSharedMemory(&shm);
    if (TEEC_SUCCESS != result) {
        ID2_DBG_E("tee_rsa_sign: alloc shm err(%08x)\n", result);
        ret = -1;
        goto cleanup3;
    }

    memcpy(shm.buffer, (uint8_t *)in, in_len);
    memcpy((uint8_t *)shm.buffer + in_len, sign, sign_len);
    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_VALUE_INPUT,
                        TEEC_MEMREF_PARTIAL_INPUT,
                        TEEC_MEMREF_PARTIAL_INPUT,
                        TEEC_NONE);
    op.params[0].value.a = type;
    op.params[1].memref.parent = &shm;
    op.params[1].memref.offset = 0;
    op.params[1].memref.size = in_len;
    op.params[2].memref.parent = &shm;
    op.params[2].memref.offset = in_len;
    op.params[2].memref.size = sign_len;

    result = TEEC_InvokeCommand(&ss, TEE_ID2_RSA_VERIFY, &op);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_RSA_verify: inv cmd err(%08x)\n", result);
        ret = -1;
        goto cleanup4;
    }

cleanup4:
    TEEC_ReleaseSharedMemory(&shm);
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}


int tee_RSA_public_encrypt(uint8_t ID, const uint8_t *in, uint32_t in_len,
                           uint8_t *out, uint32_t *out_len, uint8_t padding)
{
    int ret = 0;
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;
    TEEC_SharedMemory shm;

    if (in == NULL || in_len <= 0 ||
        out == NULL || out_len == NULL) {
        ID2_DBG_E("tee_RSA_public_encrypt: invalid input args!\n");
        return -1;
    }

    if (ID == KEY_ID_0) {
        tee_uuid = id2_uuid;
    } else {
        ID2_DBG_E("tee_RSA_public_encrypt: not support this ID(%d)\n", ID);
        return -1;
    }

    if (*out_len < SIGN_LEN) {
        *out_len = SIGN_LEN;
        ID2_DBG_E("tee_rsa_public_encrypt: short buffer!\n");
        return -1;
    }


    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_rsa_public_encrypt: open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup2;
    }

    shm.size  = in_len + *out_len;
    shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    result = TEEC_AllocateSharedMemory(&shm);
    if (TEEC_SUCCESS != result) {
        ID2_DBG_E("tee_rsa_public_encrypt: alloc shm err(%08x)\n", result);
        ret = -1;
        goto cleanup3;
    }

    memcpy(shm.buffer, (uint8_t *)in, in_len);
    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_VALUE_INPUT,
                        TEEC_MEMREF_PARTIAL_INPUT,
                        TEEC_MEMREF_PARTIAL_OUTPUT,
                        TEEC_NONE);
    op.params[0].value.a = padding;
    op.params[1].memref.parent = &shm;
    op.params[1].memref.offset = 0;
    op.params[1].memref.size = in_len;
    op.params[2].memref.parent = &shm;
    op.params[2].memref.offset = in_len;
    op.params[2].memref.size = *out_len;

    result = TEEC_InvokeCommand(&ss, TEE_ID2_RSA_PUBK_ENC, &op);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_rsa_public_encrypt: inv cmd err(%08x)\n", result);
        ret = -1;
        goto cleanup4;
    }

    *out_len = op.params[2].memref.size;
    memcpy(out, (uint8_t *)shm.buffer + in_len, *out_len);

cleanup4:
    TEEC_ReleaseSharedMemory(&shm);
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}

int tee_RSA_private_decrypt(uint8_t ID, uint8_t *in, uint32_t in_len,
                            uint8_t *out, uint32_t *out_len, uint8_t padding)
{
    int ret = 0;
    TEEC_UUID tee_uuid;
    TEEC_Result result;
    TEEC_Session ss;
    TEEC_Operation op;
    TEEC_SharedMemory shm;

    if (in == NULL || in_len <= 0 ||
        out == NULL || out_len == NULL) {
        ID2_DBG_E("tee_RSA_private_decrypt: invalid input args!\n");
        return -1;
    }

    if (ID == KEY_ID_0) {
        tee_uuid = id2_uuid;
    } else {
        ID2_DBG_E("tee_RSA_private_decrypt: not support this ID(%d)\n", ID);
        return -1;
    }


    result = TEEC_OpenSession(&ss, &tee_uuid, NULL);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_RSA_private_decrypt: open ss err(%08x)\n", result);
        ret = -1;
        goto cleanup2;
    }

    shm.size  = in_len + *out_len;
    shm.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    result = TEEC_AllocateSharedMemory(&shm);
    if (TEEC_SUCCESS != result) {
        ID2_DBG_E("tee_RSA_private_decrypt: alloc shm err(%08x)\n", result);
        ret = -1;
        goto cleanup3;
    }

    memcpy(shm.buffer, in, in_len);
    op.paramTypes = TEEC_PARAM_TYPES(
                        TEEC_VALUE_INPUT,
                        TEEC_MEMREF_PARTIAL_INPUT,
                        TEEC_MEMREF_PARTIAL_OUTPUT,
                        TEEC_NONE);
    op.params[0].value.a = padding;
    op.params[1].memref.parent = &shm;
    op.params[1].memref.offset = 0;
    op.params[1].memref.size = in_len;
    op.params[2].memref.parent = &shm;
    op.params[2].memref.offset = in_len;
    op.params[2].memref.size = *out_len;

    result = TEEC_InvokeCommand(&ss, TEE_ID2_RSA_PRVK_DEC, &op);
    if (result != TEEC_SUCCESS) {
        ID2_DBG_E("tee_RSA_private_decrypt: inv cmd err(%08x)\n", result);
        ret = -1;
        goto cleanup4;
    }

    *out_len = op.params[2].memref.size;
    memcpy(out, (uint8_t *)shm.buffer + in_len, *out_len);

cleanup4:
    TEEC_ReleaseSharedMemory(&shm);
cleanup3:
    TEEC_CloseSession(&ss);
cleanup2:

    return ret;
}

