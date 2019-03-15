/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <aos/kernel.h>
#include <network/network.h>
#include "http_wrapper.h"

httpc_wrapper_recv_fn_t g_recv_fn;

struct hostent *httpc_wrapper_gethostbyname(const char *name)
{
    return gethostbyname(name);
}

int httpc_wrapper_ssl_connect(int socket, const struct sockaddr *name, socklen_t namelen)
{
    return -1;
}

int httpc_wrapper_connect(int socket, const struct sockaddr *name, socklen_t namelen)
{
    return connect(socket, name, namelen);
}

int httpc_wrapper_ssl_send(int socket, const void *data, uint16_t size, int flags)
{
    return -1;
}

int httpc_wrapper_send(int socket, const void *data, uint16_t size, int flags)
{
    return send(socket, data, size, flags);
}

#define CONFIG_HTTPC_RX_BUF_SIZE 1500
static void httpc_recv_thread(void *arg)
{
    httpc_t *httpc_sessions = (httpc_t *)arg;
    fd_set sets;
    uint8_t index = 0;
    int max_fd = -1;
    int ret;
    uint8_t buf[CONFIG_HTTPC_RX_BUF_SIZE];

     // TODO: support multiple http sessions
    while (1) {
        FD_ZERO(&sets);
        for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
            if (httpc_sessions[index].socket < 0) {
                continue;
            }
            if (max_fd < httpc_sessions[index].socket) {
                max_fd = httpc_sessions[index].socket;
            }
            FD_SET(httpc_sessions[index].socket, &sets);
        }
        if (max_fd == -1) {
            aos_msleep(1);
            continue;
        }
        ret = select(max_fd + 1, &sets, NULL, NULL, NULL);
        if (ret > 0) {
            for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
                if (httpc_sessions[index].socket < 0) {
                    continue;
                }
                if (FD_ISSET(httpc_sessions[index].socket, &sets)) {
                    if ((httpc_sessions[index].flags & HTTP_CLIENT_FLAG_SECURE) ==
                        HTTP_CLIENT_FLAG_SECURE) {
                        // TODO: add mbedtls support
                    } else {
                        ret = recv(httpc_sessions[index].socket, buf, CONFIG_HTTPC_RX_BUF_SIZE, 0);
                    }
                    if (ret > 0) {
                        g_recv_fn(&httpc_sessions[index], buf, (int32_t)ret);
                    }
                }
            }
        }
    }
}

int httpc_wrapper_register_recv(httpc_t *httpc, httpc_wrapper_recv_fn_t recv_fn)
{
    g_recv_fn = recv_fn;
    aos_task_new("httpc_recv", httpc_recv_thread, httpc, 4096);
    return HTTPC_SUCCESS;
}

void http_log(const char *fmt, ...)
{
    va_list args;

    printf("[httpc][%06d] ", (unsigned)aos_now_ms());
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\r\n");
}
