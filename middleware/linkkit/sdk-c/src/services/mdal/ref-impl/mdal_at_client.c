/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <atparser.h>
#include "mdal_at_client.h"

#define TAG "mdal_at_client"

int mdal_at_client_init()
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

int mdal_at_client_oob(const char *prefix, const char *postfix, int maxlen,
                  oob_cb cb, void *arg)
{
    at.oob(prefix, postfix, maxlen, cb, arg);
    return 0;
}

int mdal_at_client_write(const char* at_cmd)
{
    return at.send_raw_no_rsp(at_cmd);
}
