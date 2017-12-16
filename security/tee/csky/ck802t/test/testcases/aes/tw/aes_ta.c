/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_srv.h"
#include "aes_test.h"
#include "aes.h"

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

    if (paramTypes != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (commandID == TEE_AES_TEST) {
#if 0
        tee_dbg_print(INF, "aes api level test:\n");
        if(0 != AES_API_Test()) {
            return TEE_ERROR_GENERIC;
        }
#endif
        tee_dbg_print(INF, "aes ecb_cbc self test:\n");
        if(0 != aes_ecb_cbc_test_v1()) {
            return TEE_ERROR_GENERIC;
        }

        tee_dbg_print(INF, "aes ecb openssl test:\n");
        if(0 != aes_ecb_opssl_test_v2()) {
            return TEE_ERROR_GENERIC;
        }
        tee_dbg_print(INF, "aes cbc openssl test:\n");
        if(0 != aes_cbc_opssl_test_v3()) {
            return TEE_ERROR_GENERIC;
        }

#if 0
        tee_dbg_print(INF, "aes multi context test:\n");
        if(0 != aes_multi_ctx_test_v4()) {
            return TEE_ERROR_GENERIC;
        }
#endif
        ret = TEE_SUCCESS;
    } else {
        ret = TEE_ERROR_BAD_PARAMETERS;
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
