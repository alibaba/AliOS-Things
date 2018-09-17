/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_srv.h"
#include "tee_dbg.h"
#include "tst.h"

static TEE_Result _CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _OpenSessionEntryPoint(uint32_t  paramTypes,
                                         TEE_Param params[4],
                                         void **   sessionContext)
{
    return TEE_SUCCESS;
}

static void _CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result _InvokeCommandEntryPoint(void *    sessionContext,
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

    if (commandID == TEE_TST_TEST) {
        tee_dbg_print(INF, "tst test:\n");

        extern int32_t tee_tst_test(void);
        if (0 != tee_tst_test()) {
            tee_dbg_print(ERR, "tst test error!\n");
            return -1;
        }
        ret = TEE_SUCCESS;
    } else {
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    return ret;
}

TEE_SRV_DATA_START(_CreateEntryPoint, _DestroyEntryPoint,
                   _OpenSessionEntryPoint, _CloseSessionEntryPoint,
                   _InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", TST_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END
