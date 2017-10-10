/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_SOCKET_H_
#define OTA_SOCKET_H_

#include <stdint.h>
#include <aos/aos.h>

int ota_socket_connect(int port, char *host_addr);

int ota_socket_send(int socket, const  char *buf, size_t len);

int ota_socket_recv(int socket,  char *buf, size_t len);

void ota_socket_close(int socket);

int ota_socket_check_conn(int sock);
#endif /* OTA_SOCKET_H_ */
