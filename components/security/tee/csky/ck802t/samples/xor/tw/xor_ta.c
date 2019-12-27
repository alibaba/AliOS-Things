/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_tos.h"
#include "tee_srv.h"
#include "xor.h"

static TEE_Result _xor_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _xor_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _xor_OpenSessionEntryPoint(uint32_t  paramTypes,
                                             TEE_Param params[4],
                                             void **   sessionContext)
{
    return TEE_SUCCESS;
}

static void _xor_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result _xor_InvokeCommandEntryPoint(void *    sessionContext,
                                               uint32_t  commandID,
                                               uint32_t  paramTypes,
                                               TEE_Param params[4])
{
    if (paramTypes !=
        TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_VALUE_INPUT,
                        TEE_PARAM_TYPE_VALUE_OUTPUT, TEE_PARAM_TYPE_NONE)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (CMD_XOR != commandID) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    params[2].value.a = params[0].value.a ^ params[1].value.a;

    return TEE_SUCCESS;
}

TEE_SRV_DATA_START(_xor_CreateEntryPoint, _xor_DestroyEntryPoint,
                   _xor_OpenSessionEntryPoint, _xor_CloseSessionEntryPoint,
                   _xor_InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", XOR_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END
