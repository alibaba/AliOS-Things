/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <network/network.h>
#include "ulog/ulog.h"
#include "netmgr.h"
#include "http_client_api.h"

bool httpc_running = false;
httpc_handle_t httpc_handle = 0;

static int httpc_recv_fun(httpc_handle_t httpc, uint8_t *body_buf, uint16_t body_len, int8_t err)
{
    LOG("http session %x recv %d bytes data,err %d \n", httpc, body_len, err);
    return 0;
}

static void httpc_delayed_action(void *arg)
{
    HTTPC_METHOD_TYPE_T method = (HTTPC_METHOD_TYPE_T)(*(HTTPC_METHOD_TYPE_T *)arg);

    if (httpc_handle == 0) {
        httpc_running = false;
        return;
    }

    LOG("http session %x method %d at %d\n", httpc_handle, method, (uint32_t)aos_now_ms());
printf("%s,%d\r\n",__func__,__LINE__);
    switch (method) {
        case GET:
            httpc_send_request(httpc_handle, GET, NULL, NULL, 0);
            break;
        default:
            break;
    }

    if (httpc_running == true) {
        aos_post_delayed_action(100, httpc_delayed_action, (void *)(long)method);
    }
}

static void httpc_cmd_handle(char *buf, int blen, int argc, char **argv)
{
    const char *type = argc > 1? argv[1]: "";
    HTTPC_METHOD_TYPE_T method;
    httpc_connection_t settings;
    int socket;

    if (strncmp(type, "stop", strlen("stop")) == 0) {
        httpc_running = false;
        return;
    }

    if (httpc_running == false) {
        socket = socket(AF_INET, SOCK_STREAM, 0);
        if (socket < 0) {
            LOG("alloc socket fd fail\n");
            return;
        }
        memset(&settings, 0, sizeof(settings));
        settings.socket = socket;
        settings.recv_fn = httpc_recv_fun;
        httpc_handle = httpc_init(&settings);
        if (httpc_handle == 0) {
            LOG("http session init fail\n");
            return;
        }
    }

    if (strncmp(type, "GET", strlen("GET")) == 0) {
        method = GET;
        httpc_running = true;
    }

    if (httpc_running == true) {
        aos_post_delayed_action(100, httpc_delayed_action, (void *)(long)method);
    }
}

static struct cli_command httpc_cmd = {
    .name = "httpc",
    .help = "httpc GET | stop",
    .function = httpc_cmd_handle
};

int application_start(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);
    netmgr_init();
    netmgr_start(false);
    aos_cli_register_command(&httpc_cmd);
    http_client_intialize();
    aos_loop_run();
}
