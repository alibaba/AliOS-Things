/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <lwip/netdb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct network {
    int fd;
    struct sockaddr_in address;
    int (*net_connect)(struct network *n, char *addr, int port, int net_type);
    int (*net_read)(struct network *, unsigned char *, int, int);
    int (*net_write)(struct network *, unsigned char *, int, int);
    void (*net_disconncet)(struct network *n);
} network_t;

void rws_net_init(network_t *n);

#ifdef __cplusplus
}
#endif
#endif