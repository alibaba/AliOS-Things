/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_srv.h"
#include "tee_types.h"
#include "tee_dbg.h"

#include "tee_tos.h"
#include "tee_crypto.h"
#include "tee_aes_cmd.h"
#include "tee_aes.h"

#define AES_CONTEXT_SIZE    120

static TEE_Result _AES_Test_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _AES_Test_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _AES_Test_OpenSessionEntryPoint(
        uint32_t paramTypes,
        TEE_Param params[4],
        void **sessionContext)
{
    return TEE_SUCCESS;
}

static void _AES_Test_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result _AES_Test_InvokeCommandEntryPoint(
        void *sessionContext,
        uint32_t commandID,
        uint32_t paramTypes,
        TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;
    uint8_t *key;
    uint8_t *iv;
    uint8_t *in;
    uint8_t *out;
    uint32_t len;
    int32_t ctx_sz;
    uint8_t ctx[AES_CONTEXT_SIZE];
    bool    enc;

    if (commandID == TEE_AES_CBC_ENC) {
        enc = true;
    } else if (commandID == TEE_AES_CBC_DEC) {
        enc = false;
    } else {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (paramTypes != TEE_PARAM_TYPES(
            TEE_PARAM_TYPE_MEMREF_INPUT,
            TEE_PARAM_TYPE_MEMREF_INPUT,
            TEE_PARAM_TYPE_MEMREF_INPUT,
            TEE_PARAM_TYPE_MEMREF_OUTPUT)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    key = params[0].memref.buffer;
    iv  = params[1].memref.buffer;
    in  = params[2].memref.buffer;
    out = params[3].memref.buffer;
    if (params[2].memref.size != params[3].memref.size) {
        return TEE_ERROR_BAD_PARAMETERS;
    } else {
        len = params[2].memref.size;
    }

    ret = tee_aes_get_ctx_size(AES_CBC, (size_t *)&ctx_sz);
    if (ret != TEE_CRYPTO_SUCCESS) {
        return -1;
    }

    if (ctx_sz > AES_CONTEXT_SIZE) {
        return -1;
    }
    memset(ctx, 0, ctx_sz);
    uart_print_string("aes init");
    ret = tee_aes_init(AES_CBC, enc, key, NULL, AES_BLOCK_SIZE, iv, ctx);
    if (ret != TEE_CRYPTO_SUCCESS) {
        return -1;
    }

    ret = tee_aes_process(in, out, len, ctx);
    if (ret != TEE_CRYPTO_SUCCESS) {
        return -1;
    }

    ret = tee_aes_finish(NULL, 0, NULL, NULL, SYM_NOPAD, ctx);
    if (ret != TEE_CRYPTO_SUCCESS) {
        return -1;
    }

    /* update iv */
    if (enc) {
        if (((len % AES_BLOCK_SIZE) == 0) && (len != 0)) {
            memcpy(iv, &out[len - AES_BLOCK_SIZE], AES_BLOCK_SIZE);
        }
    } else {
        if (((len % AES_BLOCK_SIZE) == 0) && (len != 0)) {
            memcpy(iv, &in[len - AES_BLOCK_SIZE], AES_BLOCK_SIZE);
        }
    }
    return ret;
}

TEE_SRV_DATA_START(_AES_Test_CreateEntryPoint,
        _AES_Test_DestroyEntryPoint,
        _AES_Test_OpenSessionEntryPoint,
        _AES_Test_CloseSessionEntryPoint,
        _AES_Test_InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", AES_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END
