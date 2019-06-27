/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "wrappers.h"

/*** TCP connection ***/
int udev_net_read(int fd, unsigned char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Read(fd, (char *)buffer, len, timeout_ms);
}

int udev_net_write(int fd, const unsigned char *buffer, uint32_t len, uint32_t timeout_ms)
{
    return HAL_TCP_Write(fd, (char *)buffer, len, timeout_ms);
}

int udev_net_disconnect(int fd)
{
    if (fd == (int)(-1)) {
        return -1;
    }

    HAL_TCP_Destroy(fd);
    fd = -1;
    return 0;
}

int udev_net_connect(const char *host, uint16_t port)
{
    return HAL_TCP_Establish(host, port);
}

/* send packet */
int udev_send_packet(int fd, unsigned char *buf, int length, unsigned int timeout)
{
    int rc = -1;
    int sent = 0;
    unsigned int now = aos_now_ms();
    unsigned int left_t = timeout;

    if (fd < 0 || !buf) {
        return -1;
    }

    /* TODO: conside timeout */
    while (sent < length) {
        left_t = timeout + 1 - (aos_now_ms() - now);
        rc = udev_net_write(fd, &buf[sent], length, left_t);
        if (rc < 0) { /* there was an error writing the data */
            break;
        }
        sent += rc;
    }

    return sent == length ? 0 : -1;
}