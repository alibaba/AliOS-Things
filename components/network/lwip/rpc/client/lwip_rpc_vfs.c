/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "k_api.h"
#include "aos/kernel.h"
#include "sys/socket.h"
#include "sys/select.h"

#include "aos/vfs.h"

extern int vfs_st_socket(int domain, int type, int protocol);
extern int vfs_st_close(int st);
extern int vfs_st_accept(int st, struct sockaddr *addr, socklen_t *addrlen);
extern int vfs_st_bind(int st, const struct sockaddr *name, socklen_t namelen);
extern int vfs_st_connect(int st, const struct sockaddr *name, socklen_t namelen);
extern int vfs_st_listen(int st, int backlog);
extern int vfs_st_send(int st, const void *data, size_t size, int flags);
extern int vfs_st_recv(int st, void *mem, size_t len, int flags);
extern int vfs_st_getsockopt(int st, int level, int optname, void *optval, socklen_t *optlen);
extern int vfs_st_setsockopt(int st, int level, int optname, const void *optval, socklen_t optlen);
extern int vfs_st_sendto(int st, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
extern int vfs_st_recvfrom(int st, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
extern int vfs_st_fcntl(int st, int cmd, int val);
extern int vfs_st_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
extern int vfs_st_getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);
extern void vfs_st_freeaddrinfo(struct addrinfo *ai);
extern int vfs_st_shutdown(int s, int how);

static struct network_ops lwip_rpc_network_opts = {
    vfs_st_socket,
    vfs_st_accept,
    vfs_st_bind,
    vfs_st_shutdown,
    NULL,
    NULL,
    vfs_st_getsockopt,
    vfs_st_setsockopt,
    vfs_st_close,
    vfs_st_connect,
    vfs_st_listen,
    vfs_st_recv,
    vfs_st_recvfrom,
    vfs_st_send,
    NULL,
    vfs_st_sendto,
    NULL,
    NULL,
    NULL,
    vfs_st_fcntl,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    vfs_st_freeaddrinfo,
    vfs_st_getaddrinfo,
    vfs_st_select,
};

void ulwip_client_attach_vfs(void)
{
    if (0 != aos_register_network(NULL, &lwip_rpc_network_opts))
    {
        printf("[%s] register network failed\n", __FUNCTION__);
    }
}
