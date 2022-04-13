/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_OTA_SOCKET_H
#define BE_OTA_SOCKET_H

#include <stddef.h>
#include <stdint.h>

#define TCP_SEND_TIMEOUT    3000
#define TCP_RECV_TIMEOUT    3000

int ota_socket_connect(int port, char *host_addr);

int ota_socket_send(int socket, const char *buf, size_t len);

int ota_socket_recv(int socket, char *buf, size_t len);

void ota_socket_close(int socket);

int ota_socket_check_conn(int sock);
#endif /* BE_OTA_SOCKET_H */
