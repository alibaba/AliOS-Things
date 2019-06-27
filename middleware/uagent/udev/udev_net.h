/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UDEV_NET_H
#define UDEV_NET_H

#include "stdint.h"

int udev_net_read(int fd, unsigned char *buffer, uint32_t len, uint32_t timeout_ms);
int udev_net_write(int fd, const unsigned char *buffer, uint32_t len, uint32_t timeout_ms);
int udev_net_disconnect(int fd);
int udev_net_connect(const char *host, uint16_t port);
int udev_send_packet(int fd, unsigned char *buf, int length, unsigned int timeout);

#endif /* UDEV_NET_H */