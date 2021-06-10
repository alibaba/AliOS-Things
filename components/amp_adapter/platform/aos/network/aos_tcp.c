/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "aos/kernel.h"
#include "aos_tcp.h"
#include "aos_socket.h"
#include "aos_system.h"

#define PLATFORM_LOG_D(format, ...)                                \
    do {                                                                   \
        aos_printf("D: %d %s() | " format "\n", __LINE__, __FUNCTION__, \
               ##__VA_ARGS__);                                             \
    } while (0);

#define PLATFORM_LOG_E(format, ...)                                \
    do {                                                                   \
        aos_printf("E: %d %s() | " format "\n", __LINE__, __FUNCTION__, \
               ##__VA_ARGS__);                                             \
    } while (0);

#ifndef CONFIG_NO_TCPIP

static uint64_t aliot_platform_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

int aos_tcp_establish(const char *host, unsigned int port)
{
    struct addrinfo  hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur          = NULL;
    int              fd           = 0;
    int              rc           = -1;
    char             service[6];

    memset(&hints, 0, sizeof(hints));

    PLATFORM_LOG_D(
                "establish tcp connection with server(host=%s port=%u)", host, port);

    hints.ai_family   = AF_INET; // only IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%u", port);

    if ((rc = getaddrinfo(host, service, &hints, &addrInfoList)) != 0) {
        PLATFORM_LOG_E("getaddrinfo error: %d", rc);
        return (uintptr_t)-1;
    }
    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {

        if (cur->ai_family != AF_INET) {
            PLATFORM_LOG_E("socket type error");
            rc = -1;
            continue;
        }

        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            PLATFORM_LOG_E("create socket error");
            rc = -1;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            rc = fd;
            break;
        }

        close(fd);
        PLATFORM_LOG_E("connect error");
        rc = -1;
    }

    if (-1 == rc) {
        PLATFORM_LOG_D("fail to establish tcp");
    } else {
        PLATFORM_LOG_D("success to establish tcp, fd=%d", rc);
    }
    freeaddrinfo(addrInfoList);

    return rc;
}

int aos_tcp_destroy(unsigned int fd)
{
    int rc;

    // Shutdown both send and receive operations.
    rc = shutdown((int)fd, 2);
    if (0 != rc) {
        PLATFORM_LOG_E("shutdown error");
        return -1;
    }

    rc = close((int)fd);
    if (0 != rc) {
        PLATFORM_LOG_E("closesocket error");
        return -1;
    }

    return 0;
}


int aos_tcp_write(unsigned int fd, const char *buf, unsigned int len, unsigned int timeout_ms)
{
    int      ret, err_code;
    uint32_t len_sent;
    uint64_t t_end, t_left;
    fd_set   sets;

    if (fd >= FD_SETSIZE) {
        return -1;
    }
    t_end    = aos_now_ms() + timeout_ms;
    len_sent = 0;
    err_code = 0;
    ret      = 1; // send one time if timeout_ms is value 0

    do {
        t_left = aliot_platform_time_left(t_end, aos_now_ms());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            timeout.tv_sec  = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;
            ret = select(fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(fd, &sets)) {
                    PLATFORM_LOG_D("Should NOT arrive");
                    // If timeout in next loop, it will not sent any data
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                // PLATFORM_LOG_D("select-write timeout %lu", fd);
                break;
            } else {
                if (EINTR == errno) {
                    PLATFORM_LOG_D("EINTR be caught");
                    continue;
                }

                err_code = -1;
                PLATFORM_LOG_E("select-write fail");
                break;
            }
        }

        if (ret > 0) {
            ret = send(fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                PLATFORM_LOG_D("No data be sent");
            } else {
                if (EINTR == errno) {
                    PLATFORM_LOG_D("EINTR be caught");
                    continue;
                }

                err_code = -1;
                PLATFORM_LOG_E("send fail");
                break;
            }
        }
    } while ((len_sent < len) &&
             (aliot_platform_time_left(t_end, aos_now_ms()) > 0));

    return err_code == 0 ? len_sent : err_code;
}

int aos_tcp_read(unsigned int fd, char *buf, unsigned int len, unsigned int timeout_ms)
{
   int res = 0;
    int32_t recv_bytes = 0;
    ssize_t recv_res = 0;
    uint64_t timestart_ms = 0, timenow_ms = 0, timeselect_ms = 0;
    fd_set recv_sets;
    struct timeval timestart, timenow, timeselect;

    FD_ZERO(&recv_sets);
    FD_SET(fd, &recv_sets);

    /* Start Time */
    gettimeofday(&timestart, NULL);
    timestart_ms = timestart.tv_sec * 1000 + timestart.tv_usec / 1000;
    timenow_ms = timestart_ms;

    do {
        gettimeofday(&timenow, NULL);
        timenow_ms = timenow.tv_sec * 1000 + timenow.tv_usec / 1000;

        if (timenow_ms - timestart_ms >= timenow_ms ||
            timeout_ms - (timenow_ms - timestart_ms) > timeout_ms) {
            break;
        }

        timeselect_ms = timeout_ms - (timenow_ms - timestart_ms);
        timeselect.tv_sec = timeselect_ms / 1000;
        timeselect.tv_usec = timeselect_ms % 1000 * 1000;

        res = select(fd + 1, &recv_sets, NULL, NULL, &timeselect);
        if (res == 0) {
            continue;
        } else if (res < 0) {
            aos_printf("aos_tcp_read, errno: %d\n", errno);
            return -1;
        } else {
            if (FD_ISSET(fd, &recv_sets)) {
                recv_res = recv(fd, buf + recv_bytes, len - recv_bytes, 0);
                if (recv_res == 0) {
                    aos_printf("aos_tcp_read, nwk connection closed\n");
                    break;
                } else if (recv_res < 0) {
                    aos_printf("aos_tcp_read, errno: %d\n", errno);
                    if (errno == EINTR) {
                        continue;
                    }
                    return -1;
                } else {
                    recv_bytes += recv_res;
                    if (recv_bytes == len) {
                        break;
                    }
                }
            }
        }
    } while (((timenow_ms - timestart_ms) < timeout_ms) && (recv_bytes < len));

    return recv_bytes;
}
#else
uintptr_t aos_tcp_establish(_IN_ const char *host, _IN_ uint16_t port)
{
    return 0;
}
int32_t aos_tcp_destroy(uintptr_t fd)
{
    return 0;
}
int32_t aos_tcp_write(uintptr_t fd, const char *buf, uint32_t len,
                      uint32_t timeout_ms)
{
    return 0;
}
int32_t aos_tcp_read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    return 0;
}
#endif
