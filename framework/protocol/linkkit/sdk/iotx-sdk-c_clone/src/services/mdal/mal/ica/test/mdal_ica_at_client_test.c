/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <atparser.h>
#include "mdal_ica_at_client_test.h"
#include "mdal_mal_import.h"

#define TAG "mdal_ica_at_client"

int HAL_MDAL_MAL_ICA_Init()
{
/* At module initialization has done when mk3060 boot.
   If at module is not initialized when system is booting up,
   initialize at module here. */
#if 0
    int ret;
    ret = at.init(AT_RECV_PREFIX,
            AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER,
            1000);

    if(ret != 0)
    {
        return ret;
    }

    at.set_mode(ASYN);
    return 0;
#endif
}

int HAL_MDAL_MAL_ICA_InputCb(const char *prefix, const char *postfix, int maxlen,
                  oob_cb cb, void *arg)
{
    at.oob(prefix, postfix, maxlen, cb, arg);
    return 0;
}

int HAL_MDAL_MAL_ICA_Write(const char* at_cmd)
{
    return at.send_raw_no_rsp(at_cmd);
}
