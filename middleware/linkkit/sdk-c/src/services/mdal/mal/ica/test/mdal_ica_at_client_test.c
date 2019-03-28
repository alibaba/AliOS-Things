/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <atparser.h>
#include "mdal_ica_at_client_test.h"
#include "mdal_mal_import.h"

#define TAG "mdal_ica_at_client"
extern int at_dev_fd;

int HAL_MDAL_MAL_ICA_Init()
{
/* At module initialization has done when mk3060 boot.
   If at module is not initialized when system is booting up,
   initialize at module here. */
}

int HAL_MDAL_MAL_ICA_InputCb(const char *prefix, const char *postfix,
                             char buf, int buf_size, at_recv_cb cb, void *arg)
{
    at_register_callback(at_dev_fd, prefix, postfix, buf, buf_size, cb, arg);
    return 0;
}

int HAL_MDAL_MAL_ICA_Write(const char* at_cmd)
{
    return at_send_no_reply(at_dev_fd, at_cmd, strlen(at_cmd), false);
}
