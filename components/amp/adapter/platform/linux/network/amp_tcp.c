/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include "amp_defines.h"
#define MOD_STR "TCP"

static uint64_t _linux_get_time_ms(void)
{
    struct timeval tv = { 0 };
    uint64_t time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return time_ms;
}

static uint64_t _linux_time_left(uint64_t t_end, uint64_t t_now)
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
    struct addrinfo hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur = NULL;
    int fd = 0;
    int rc = 0;
    char service[6];
    uint8_t dns_retry = 0;

    memset(&hints, 0, sizeof(hints));

    amp_debug(MOD_STR, "establish tcp connection with server(host='%s', port=[%u])\n", host, port);

    hints.ai_family = AF_INET; /* only IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%u", port);

    while (dns_retry++ < 8) {
        rc = getaddrinfo(host, service, &hints, &addrInfoList);
        if (rc != 0) {
#if defined(_PLATFORM_IS_LINUX_)
            if (rc == EAI_AGAIN) {
                int ret = 0;
                ret = res_init();
                amp_debug(MOD_STR, "getaddrinfo res_init, ret is %d, errno is %d\n", ret, errno);
            }
#endif
            amp_debug(MOD_STR, "getaddrinfo error[%d], res: %s, host: %s, port: %s\n", dns_retry, gai_strerror(rc), host, service);
            sleep(1);
            continue;
        } else {
            break;
        }
    }

    if (rc != 0) {
        amp_debug(MOD_STR, "getaddrinfo error(%d), host = '%s', port = [%d]\n", rc, host, port);
        return (uintptr_t)(-1);
    }

    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {
        if (cur->ai_family != AF_INET) {
            amp_error(MOD_STR, "socket type error\n");
            rc = -1;
            continue;
        }

        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            amp_error(MOD_STR, "create socket error\n");
            rc = -1;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            rc = fd;
            break;
        }

        close(fd);
        amp_error(MOD_STR, "connect error\n");
        rc = -1;
    }

    if (-1 == rc) {
        amp_error(MOD_STR, "fail to establish tcp\n");
    } else {
        amp_debug(MOD_STR, "success to establish tcp, fd=%d\n", rc);
    }
    freeaddrinfo(addrInfoList);

    return (uintptr_t)rc;
}

int HAL_TCP_Destroy(uintptr_t fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = shutdown((int) fd, 2);
    if (0 != rc) {
        amp_error(MOD_STR, "shutdown error\n");
        return -1;
    }

    rc = close((int) fd);
    if (0 != rc) {
        amp_error(MOD_STR, "closesocket error\n");
        return -1;
    }

    return 0;
}

int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, tcp_fd;
    uint32_t len_sent;
    uint64_t t_end, t_left;
    fd_set sets;
    int net_err = 0;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    if (fd >= FD_SETSIZE) {
        return -1;
    }
    tcp_fd = (int)fd;

    do {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(tcp_fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = select(tcp_fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(tcp_fd, &sets)) {
                    amp_debug(MOD_STR, "Should NOT arrive\n");
                    /* If timeout in next loop, it will not sent any data */
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                amp_warn(MOD_STR, "select-write timeout %d\n", tcp_fd);
                break;
            } else {
                if (EINTR == errno) {
                    amp_debug(MOD_STR, "EINTR be caught\n");
                    continue;
                }

                amp_debug(MOD_STR, "select-write fail, ret = select() = %d\n", ret);
                net_err = 1;
                break;
            }
        }

        if (ret > 0) {
            ret = send(tcp_fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                amp_debug(MOD_STR, "No data be sent\n");
            } else {
                if (EINTR == errno) {
                    amp_debug(MOD_STR, "EINTR be caught\n");
                    continue;
                }

                amp_debug(MOD_STR, "send fail, ret = send() = %d\n", ret);
                net_err = 1;
                break;
            }
        }
    } while (!net_err && (len_sent < len) && (_linux_time_left(t_end, _linux_get_time_ms()) > 0));

    if (net_err) {
        return -1;
    } else {
        return len_sent;
    }
}

int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, err_code, tcp_fd;
    uint32_t len_recv;
    uint64_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    if (fd >= FD_SETSIZE) {
        return -1;
    }
    tcp_fd = (int)fd;

    do {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());
        if (0 == t_left) {
            break;
        }
        FD_ZERO(&sets);
        FD_SET(tcp_fd, &sets);

        timeout.tv_sec = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(tcp_fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0) {
            ret = recv(tcp_fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (0 == ret) {
                amp_debug(MOD_STR, "connection is closed\n");
                err_code = -1;
                break;
            } else {
                if (EINTR == errno) {
                    continue;
                }
                amp_debug(MOD_STR, "recv fail\n");
                err_code = -2;
                break;
            }
        } else if (0 == ret) {
            break;
        } else {
            if (EINTR == errno) {
                continue;
            }
            amp_debug(MOD_STR, "select-recv fail\n");
            err_code = -2;
            break;
        }
    } while ((len_recv < len));

    /* priority to return data bytes if any data be received from TCP connection. */
    /* It will get error code on next calling */
    return (0 != len_recv) ? len_recv : err_code;
}
