/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "aos/kernel.h"
#include <network/network.h>
#include <aos/errno.h>
#include "ulog/ulog.h"

#include "qxwz_socket.h"
#include "qxwz_types.h"
#include "ulocation_qxwz_common.h"

static soc_cb_t g_soc_funtion;
static qxwz_s32_t g_soc;
static qxwz_s8_t g_soc_buf[SOCKET_BUFFER] = {0};
static qxwz_s32_t g_soc_len = 0;

static uint32_t time_left(uint32_t t_end, uint32_t t_now)
{
    uint32_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

int qxwz_tcp_establish(const char *host, uint16_t port)
{
    struct addrinfo  hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur          = NULL;
    int              fd           = 0;
    int              rc           = -1;
    char             service[6];

    memset(&hints, 0, sizeof(hints));

    LOGD("uLocation-qxwz", "establish tcp connection with server(host=%s port=%u)", host, port);

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%u", port);

    if ((rc = getaddrinfo(host, service, &hints, &addrInfoList)) != 0) {
        LOGE("uLocation-qxwz", "getaddrinfo error\n");
        return -1;
    }
    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {
        if (cur->ai_family != AF_INET) {
            LOGE("uLocation-qxwz", "socket type error\n");
            rc = -1;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            rc = fd;
            break;
        }

        close(fd);
        LOGE("uLocation-qxwz", "connect error\n");
        rc = -1;
    }

    if (-1 == rc) {
        LOGE("uLocation-qxwz", "fail to establish tcp\n");
    } else {
        LOGD("uLocation-qxwz", "success to establish tcp, fd=%d\n", rc);
    }
    freeaddrinfo(addrInfoList);

    return rc;
}


int32_t qxwz_tcp_destroy(uintptr_t fd)
{
    int rc;

    rc = shutdown((int)fd, 2);
    if (0 != rc) {
        LOGE("uLocation-qxwz", "shutdown error\n");
        return -1;
    }

    rc = close((int)fd);
    if (0 != rc) {
        LOGE("uLocation-qxwz", "closesocket error\n");
        return -1;
    }

    return 0;
}


int32_t qxwz_tcp_write(uintptr_t fd, const char *buf, uint32_t len,
                      uint32_t timeout_ms)
{
    int      ret, err_code;
    uint32_t len_sent;
    uint32_t t_end, t_left;
    fd_set   sets;

    t_end    = aos_now_ms() + timeout_ms;
    len_sent = 0;
    err_code = 0;
    ret      = 1;

    do {
        t_left = time_left(t_end, aos_now_ms());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            timeout.tv_sec  = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;
            ret = select(fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(fd, &sets)) {
                    LOGD("uLocation-qxwz", "Should NOT arrive\n");
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                break;
            } else {
                if (EINTR == errno) {
                    LOGD("uLocation-qxwz", "EINTR be caught\n");
                    continue;
                }

                err_code = -1;
                LOGE("uLocation-qxwz", "select-write fail\n");
                break;
            }
        }

        if (ret > 0) {
            ret = send(fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                LOGD("uLocation-qxwz", "No data be sent\n");
            } else {
                if (EINTR == errno) {
                    LOGD("uLocation-qxwz", "EINTR be caught\n");
                    continue;
                }

                err_code = -1;
                LOGE("uLocation-qxwz", "send fail\n");
                break;
            }
        }
    } while ((len_sent < len) &&
             (time_left(t_end, aos_now_ms()) > 0));

    return err_code == 0 ? len_sent : err_code;
}

int32_t qxwz_tcp_read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    int            ret, err_code;
    uint32_t       len_recv;
    uint32_t       t_end, t_left;
    fd_set         sets;
    struct timeval timeout;

    t_end    = aos_now_ms() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    do {
        t_left = time_left(t_end, aos_now_ms());
        if (0 == t_left) {
            break;
        }
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeout.tv_sec  = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0) {
            ret = recv(fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (0 == ret) {
                LOGE("uLocation-qxwz", "connection is closed\n");
                err_code = -1;
                break;
            } else {
                if (EINTR == errno) {
                    continue;
                }
                LOGE("uLocation-qxwz", "send fail\n");
                err_code = -2;
                break;
            }
        } else if (0 == ret) {
            break;
        } else {
            if (EINTR == errno) {
                continue;
            }
            LOGE("uLocation-qxwz", "select-recv fail errno=%d\n",errno);
            err_code = -2;
            break;
        }
    } while ((len_recv < len));

    if (len_recv != 0){
        g_soc_funtion.cb_recv(fd, buf, len_recv);
    }

    return (0 != len_recv) ? len_recv : err_code;
}

qxwz_s32_t qxwz_soc_close(qxwz_s32_t soc)
{
    if (soc != g_soc){
        LOGE("uLocation-qxwz", "socket fd error!\n");
        return -1;
    }
    return qxwz_tcp_destroy(soc);
}

qxwz_s32_t qxwz_soc_recv_handler(void)
{
    qxwz_s32_t soc = g_soc;
    memset(g_soc_buf, 0, sizeof(g_soc_buf));
    g_soc_len = sizeof(g_soc_buf);
    return qxwz_tcp_read(soc, g_soc_buf, g_soc_len, SOCKET_TIMEOUT);
}

qxwz_s32_t qxwz_soc_send(qxwz_s32_t soc, const qxwz_s8_t *buf, qxwz_s32_t len)
{
    if (soc != g_soc){
        LOGE("uLocation-qxwz", "socket fd error!\n");
        return -1;
    }
    if (buf == NULL || len ==0){
        LOGE("uLocation-qxwz", "send buffer is empty!\n");
        return -1;
    }
    return qxwz_tcp_write(soc, buf, len, SOCKET_TIMEOUT);
}

qxwz_s32_t qxwz_soc_connect(qxwz_s32_t soc, host_info_t *host, soc_cb_t *cbs)
{
    qxwz_s32_t ret;

    if (soc != g_soc){
        LOGE("uLocation-qxwz", "socket fd error!\n");
        return -1;
    }
    if (host == NULL || cbs == NULL){
        LOGE("uLocation-qxwz", "host and cbs is NULL\n");
        return -1;
    }
    g_soc_funtion = *cbs;
    ret = qxwz_tcp_establish(host->hostName, host->port);
    if (ret != 0)
    {
        LOGE("uLocation-qxwz", "tcp establish error!\n");
        return -1;
    }
}

qxwz_s32_t qxwz_soc_create(qxwz_u32_t nwk_id)
{
    qxwz_s32_t fd;
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        LOGE("uLocation-qxwz", "create socket error\n");
        return -1;
    }
    g_soc = fd;

    return fd;
}

qxwz_s32_t qxwz_printf(const char *fmt, ...)
{
    char buf[QXWZ_PRINT_BUFFER] = {0};

    va_list val;
    va_start(val, fmt);
    vsnprintf(buf, QXWZ_PRINT_BUFFER, fmt, val);
    va_end(val);

    printf("%s", buf);
    return 0;
}
