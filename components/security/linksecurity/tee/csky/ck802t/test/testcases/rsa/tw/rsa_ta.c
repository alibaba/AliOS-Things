/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_srv.h"
#include "tee_dbg.h"
#include "rsa_test.h"
#include "rsa.h"

static TEE_Result _RSA_Test_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _RSA_Test_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _RSA_Test_OpenSessionEntryPoint(uint32_t  paramTypes,
                                                  TEE_Param params[4],
                                                  void **   sessionContext)
{
    return TEE_SUCCESS;
}

static void _RSA_Test_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result _RSA_Test_InvokeCommandEntryPoint(void *    sessionContext,
                                                    uint32_t  commandID,
                                                    uint32_t  paramTypes,
                                                    TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;

    if (paramTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE,
                                      TEE_PARAM_TYPE_NONE,
                                      TEE_PARAM_TYPE_NONE)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (commandID == TEE_RSA_TEST) {

        /* Crypto Level Test */
        tee_dbg_print(INF, "rsa crypto test v1:\n");
        if (0 != rsa_test_v1()) {
            return TEE_ERROR_GENERIC;
        }
        tee_dbg_print(INF, "rsa crypto test v2:\n");
        if (0 != rsa_test_v2()) {
            return TEE_ERROR_GENERIC;
        }
        tee_dbg_print(INF, "rsa crypto test v3:\n");
        if (0 != rsa_test_v3()) {
            return TEE_ERROR_GENERIC;
        }
        tee_dbg_print(INF, "rsa crypto test v4:\n");
        if (0 != rsa_test_v4()) {
            return TEE_ERROR_GENERIC;
        }
        ret = TEE_SUCCESS;
    } else {
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    return ret;
}

TEE_SRV_DATA_START(_RSA_Test_CreateEntryPoint, _RSA_Test_DestroyEntryPoint,
                   _RSA_Test_OpenSessionEntryPoint,
                   _RSA_Test_CloseSessionEntryPoint,
                   _RSA_Test_InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", RSA_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END
