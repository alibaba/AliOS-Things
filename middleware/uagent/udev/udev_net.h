/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _UDEV_NET_H_
#define _UDEV_NET_H_

#include "iot_import.h"

/*** TCP connection ***/
int udev_net_read(int fd, unsigned char *buffer, uint32_t len, uint32_t timeout_ms);

int udev_net_write(int fd, const unsigned char *buffer, uint32_t len, uint32_t timeout_ms);

int udev_net_disconnect(int fd);

int udev_net_connect(const char *host, uint16_t port);
/* send packet */
int udev_send_packet(int fd, unsigned char *buf, int length, unsigned int timeout);

#endif /* _UDEV_NET_H_ */
