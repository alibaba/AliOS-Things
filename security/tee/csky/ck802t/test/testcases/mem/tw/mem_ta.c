/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_tos.h"
#include "tee_srv.h"
#include "mem.h"

static TEE_Result CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void DestroyEntryPoint(void)
{
    return;
}

static TEE_Result OpenSessionEntryPoint(
        uint32_t paramTypes,
        TEE_Param params[4],
        void **sessionContext)
{
    return TEE_SUCCESS;
}

static void CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result InvokeCommandEntryPoint(
        void *sessionContext,
        uint32_t commandID,
        uint32_t paramTypes,
        TEE_Param params[4])
{
    switch (commandID) {
        case TEE_CMD_TMP_MEM:
        case TEE_CMD_REGS_SHM:
        case TEE_CMD_ALOC_SHM: {
            if (paramTypes != TEE_PARAM_TYPES(
                                  TEE_PARAM_TYPE_MEMREF_INPUT,
                                  TEE_PARAM_TYPE_MEMREF_OUTPUT,
                                  TEE_PARAM_TYPE_NONE,
                                  TEE_PARAM_TYPE_NONE))
                return TEE_ERROR_BAD_PARAMETERS;

            memmove(params[1].memref.buffer,
                        params[0].memref.buffer, params[0].memref.size);

            break;
        }

        default:
            return TEE_ERROR_BAD_PARAMETERS;
    }

    return TEE_SUCCESS;
}

TEE_SRV_DATA_START(CreateEntryPoint,
        DestroyEntryPoint,
        OpenSessionEntryPoint,
        CloseSessionEntryPoint,
        InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", MEM_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END
