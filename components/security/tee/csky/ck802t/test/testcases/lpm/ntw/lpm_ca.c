/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_client_api.h"
#include "tee_dbg.h"
#include "teec_lpm_api.h"

typedef struct _lpm_args_t
{
    uint32_t mode;
    uint32_t gate;
    uint32_t irqid;
} lpm_args_t;

int tee_lpm_test()
{
    TEEC_Result ret = TEEC_SUCCESS;
    lpm_args_t  args;

    args.mode  = 0x0;
    args.gate  = 0x1;
    args.irqid = 0x2;

    ret = TEEC_lpm_to_security((void *)&args);

    return ret;
}
