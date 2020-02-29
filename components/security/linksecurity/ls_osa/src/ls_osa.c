/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include "ls_osa.h"
#include "aos/kernel.h"

#ifdef CONFIG_NET_ENABLED
#include "network/network.h"
#endif

void ls_osa_print(const char *fmt, ...)
{
    va_list va_args;

    va_start(va_args, fmt);
    vprintf(fmt, va_args);
    va_end(va_args);
}

int ls_osa_snprintf(char *str, size_t size, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, size, fmt, args);
    va_end(args);

    return rc;
}

void *ls_osa_malloc(size_t size)
{
    return aos_malloc((unsigned int)size);
}

void *ls_osa_calloc(size_t nmemb, size_t size)
{
    return aos_calloc((unsigned int)nmemb, (unsigned int)size);
}

void ls_osa_free(void *ptr)
{
    aos_free(ptr);
}

void ls_osa_msleep(unsigned int msec)
{
    aos_msleep((int)msec);
}

long long ls_osa_get_time_ms(void)
{
    struct timeval tv;
    long long ret = 0;

    gettimeofday(&tv, NULL);
    ret = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return ret;
}

int ls_osa_mutex_create(void **mutex)
{
    if (mutex == NULL) {
        return -1;
    }
    *mutex = (aos_mutex_t *)aos_malloc(sizeof(aos_mutex_t));
    if (*mutex) {
        return -1;
    }

    return aos_mutex_new(*mutex);
}

void ls_osa_mutex_destroy(void *mutex)
{
    aos_mutex_free(mutex);
    if (mutex) {
        aos_free(mutex);
        mutex = NULL;
    }
}

int ls_osa_mutex_lock(void *mutex)
{
    return aos_mutex_lock(mutex, -1);
}

int ls_osa_mutex_unlock(void *mutex)
{
    return aos_mutex_unlock(mutex);
}

#ifdef CONFIG_NET_ENABLED
int ls_osa_net_connect(const char *host, const char *port, int type)
{
    int fd = -1;
    int ret = 0;
    struct addrinfo hints, *addr_list, *cur;

    /* do name resolution with both IPv6 and IPv4 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type == LS_NET_TYPE_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = type == LS_NET_TYPE_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if ((ret = getaddrinfo(host, port, &hints, &addr_list)) != 0) {
        ls_osa_print("getaddrinfo fail, errno: %d, %d\n", errno, ret);
        return -1;
    }

    /* try the sockaddrs until a connection succeeds */
    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        fd = (int)socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            continue;
        }

       do {
            if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
                goto _out;
            } else {
                if (errno == EINTR) {
                    continue;
                }

                break;
            }
        } while (1);

        close(fd);
        fd = -1;
    }

_out:
    freeaddrinfo(addr_list);

    return fd;
}

void ls_osa_net_disconnect(int fd)
{
    if (fd < 0) {
        return;
    }

    shutdown(fd, 2);
    close(fd);

    return;
}

int ls_osa_net_send(int fd, unsigned char *buf, size_t len, int *ret_orig)
{
    int ret;

    if (fd < 0 || ret_orig == NULL) {
        ls_osa_print("net_send: invalid args\n");
        return -1;
    }

    *ret_orig = 0;

    ret = (int)send(fd, buf, len, 0);
    if (ret < 0) {
        if (errno == EINTR) {
            *ret_orig = -1;
        }
    }

    return ret;
}

int ls_osa_net_recv(int fd, unsigned char *buf, size_t len, int timeout, int *ret_orig)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;

    if (fd < 0 || timeout < 0 || ret_orig == NULL) {
        ls_osa_print("net_recv: invalid args\n");
        return -1;
    }

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    ret = select(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);
    if (ret == 0) {
        /* timeout */
        *ret_orig = -2;
        return -1;
    }

    if (ret < 0) {
        if (errno == EINTR) {
            *ret_orig = -1;
        }

        return -1;
    }

    ret = (int)recv(fd, buf, len, 0);
    if (ret < 0) {
        if (errno == EINTR) {
            *ret_orig = -1;
        }

        return -1;
    }

    return ret;
}
#endif
