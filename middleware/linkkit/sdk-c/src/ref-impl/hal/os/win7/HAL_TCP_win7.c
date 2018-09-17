/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>

#ifdef _MSC_BUILD
#include <Winbase.h>
#pragma comment(lib,"ws2_32")
#endif

#include "iot_import.h"
#include "iotx_hal_internal.h"

static uint64_t time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}


uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
    uintptr_t sockfd;
    WSADATA wsaData;
    struct hostent *hp;
    struct sockaddr_in addrServer;

    WSAStartup(0x202, &wsaData);

    hal_info("host : %s, port : %u\n", host, port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);   /* socket */
    hp = gethostbyname(host);


    memset(&addrServer, 0, sizeof(addrServer));
    memcpy(&(addrServer.sin_addr), hp->h_addr, hp->h_length);

    hal_info("ip = %u.%u.%u.%u",
                         addrServer.sin_addr.S_un.S_un_b.s_b1,
                         addrServer.sin_addr.S_un.S_un_b.s_b2,
                         addrServer.sin_addr.S_un.S_un_b.s_b3,
                         addrServer.sin_addr.S_un.S_un_b.s_b4);

    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons((unsigned short)1883);

    hal_info("connecting to %s", host);
    if (connect(sockfd, (struct sockaddr *)&addrServer, sizeof(struct sockaddr))) {
        hal_err("connect failed!\n");
        return -1;
    }

    hal_info("connect successfully!\n");

    return sockfd;
}


int32_t HAL_TCP_Destroy(uintptr_t fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = shutdown((int) fd, 2);
    if (0 != rc) {
        hal_err("shutdown error");
        return -1;
    }

    rc = closesocket((int) fd);
    if (0 != rc) {
        hal_err("closesocket error");
        return -1;
    }

    rc = WSACleanup();
    if (0 != rc) {
        hal_err("WSACleanup error");
        return -1;
    }

    return 0;
}


int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, err_code;
    uint32_t len_sent;
    uint64_t t_end, t_left;
    fd_set sets;

    t_end = GetTickCount() + timeout_ms;
    len_sent = 0;
    err_code = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    do {
        t_left = time_left(t_end, GetTickCount());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = select(0, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(fd, &sets)) {
                    /* TODO */
                    /* not this fd, continue; */
                    /* If timeout in next loop, it will not sent any data */
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                hal_err("select-write timeout");
                break;
            } else {
                if (WSAEINTR == WSAGetLastError()) {
                    continue;
                }
                hal_err("select-write fail");
                err_code = -1;
                break;
            }
        }

        if (ret > 0) {
            ret = send(fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                hal_err("No any data be sent");
            } else {
                /* socket error occur */
                hal_err("send fail");
                err_code = -1;
                break;
            }
        }
    } while ((len_sent < len) && (time_left(t_end, GetTickCount()) > 0));

    /* Priority to return data bytes if any data be sent to TCP connection. */
    /* It will get error code on next calling */
    return (0 == len_sent) ? err_code : len_sent;
}


int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, err_code;
    uint32_t len_recv;
    uint64_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;

    t_end = GetTickCount() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    do {
        t_left = time_left(t_end, GetTickCount());

        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeout.tv_sec = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(0, &sets, NULL, NULL, &timeout);
        if (ret > 0) {
            ret = recv(fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (0 == ret) {
                hal_err("connection is closed");
                err_code = -1;
                break;
            } else {
                if (WSAEINTR == WSAGetLastError()) {
                    continue;
                }
                hal_err("recv fail");
                err_code = -2;
                break;
            }
        } else if (0 == ret) {
            break;
        } else {
            if (WSAEINTR == WSAGetLastError()) {
                continue;
            }
            hal_err("select-read fail");
            err_code = -2;
            break;
        }
    } while ((len_recv < len) && (time_left(t_end, GetTickCount()) > 0));

    /* priority to return data bytes if any data be received from TCP connection. */
    /* It will get error code on next calling */
    return (0 != len_recv) ? len_recv : err_code;
}
