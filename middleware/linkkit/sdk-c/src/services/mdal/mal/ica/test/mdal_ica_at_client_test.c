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
    ret = at_init();

    if(ret != 0)
    {
        return ret;
    }

    return 0;
#endif
}

int HAL_MDAL_MAL_ICA_InputCb(const char *prefix, const char *postfix, int maxlen,
                             at_recv_cb cb, void *arg)
{
    at_register_callback(prefix, postfix, maxlen, cb, arg);
    return 0;
}

int HAL_MDAL_MAL_ICA_Write(const char* at_cmd)
{
    return at_send_no_reply(at_cmd, strlen(at_cmd), false);
}
