/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "amp_network.h"
#include "amp_socket.h"
#include "amp_defines.h"
#include "app_mgr.h"
#include "ota_socket.h"

#define MOD_STR "OTA_SOCKET"
int ota_socket_check_conn(int sock)
{
    if (sock < 0) {
        amp_error(MOD_STR, "ota_socket_check_conn: invalid socket fd\n");
        return -1;
    }
    /*
        struct pollfd fd = { .fd = sock, .events = POLLOUT };
        int ret = 0;
        socklen_t len = 0;

        while (poll(&fd, 1, -1) == -1) {
            if (errno != EINTR ) {
                return -1;
            }
        }

        len = sizeof(ret);
        if (getsockopt (sock, SOL_SOCKET, SO_ERROR, &ret, &len) == -1 ||
            ret != 0) {
            return -1;
        }
    */
    return 0;
}

int ota_socket_connect(int port, char *host_addr)
{
    return HAL_TCP_Establish(host_addr, port);
}

int ota_socket_send(int socket, const char *buf, size_t len)
{
    return HAL_TCP_Write(socket, buf, len, TCP_SEND_TIMEOUT);
}

int ota_socket_recv(int socket, char *buf, size_t len)
{
    return HAL_TCP_Read(socket, buf, len, TCP_RECV_TIMEOUT);
}

void ota_socket_close(int socket)
{
    HAL_TCP_Destroy(socket);
}