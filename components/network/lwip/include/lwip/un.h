/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef LWIP_HDR_UN_H
#define LWIP_HDR_UN_H

#include "lwip/sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_UDS
#define LWIP_UDS_SOCKET(fd)    ((fd) >= FD_UDS_SOCKET_OFFSET && (fd) < FD_UDS_SOCKET_OFFSET + FD_UDS_SOCKET_NUM)
#else
#define LWOP_UDS_SOCKET(fd)    (0)
#endif

#ifdef _SYS_UN_H
#include <sys/un.h>
#else
struct sockaddr_un {
    unsigned char sun_len;
    sa_family_t sun_family;
    char sun_path[108];
};
#endif

int uds_socket(int domain, int type, int protocol);
int uds_close(int sockfd);
int uds_listen(int sockfd, int backlog);
int uds_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int uds_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int uds_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t uds_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t uds_recv(int sockfd, void *buf, size_t len, int flags);
int uds_select(int nfds, fd_set *readfds, fd_set *writefds,
               fd_set *exceptfds, struct timeval *timeout);

/* operation flow of server
 * 1. socket
 * 2. bind
 * 3. listen
 * 4. accept
 * 5. select
 * 6. tx/rx
 */

/* operation flow of client
 * 1. socket
 * 2. bind (optional)
 * 3. connect
 * 4. tx/rx
 */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_AF_PACKET_H */
