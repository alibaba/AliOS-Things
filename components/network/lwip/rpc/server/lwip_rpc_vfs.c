/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "k_api.h"
#include "aos/kernel.h"
#include "aos/vfs.h"

extern int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
extern int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);
extern int lwip_shutdown(int s, int how);
extern int lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
extern int lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
extern int lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
extern int lwip_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
extern int lwip_close(int s);
extern int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);
extern int lwip_listen(int s, int backlog);
extern int lwip_recv(int s, void *mem, size_t len, int flags);
extern int lwip_read(int s, void *mem, size_t len);
extern int lwip_recvfrom(int s, void *mem, size_t len, int flags,
                         struct sockaddr *from, socklen_t *fromlen);
extern int lwip_send(int s, const void *dataptr, size_t size, int flags);
extern int lwip_sendmsg(int s, const struct msghdr *message, int flags);
extern int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
                       const struct sockaddr *to, socklen_t tolen);
extern int lwip_socket(int domain, int type, int protocol);
extern int lwip_write(int s, const void *dataptr, size_t size);
extern int lwip_writev(int s, const struct iovec *iov, int iovcnt);
extern int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                       struct timeval *timeout);
extern int lwip_ioctl(int s, long cmd, void *argp);
extern int lwip_fcntl(int s, int cmd, int val);
extern int lwip_eventfd(unsigned int initval, int flags);

extern struct hostent *lwip_gethostbyname(const char *name);
extern int lwip_getaddrinfo(const char *__restrict, const char *__restrict, const struct addrinfo *__restrict, struct addrinfo **__restrict);
extern void lwip_freeaddrinfo(struct addrinfo *);
extern int lwip_try_wakeup(int s, int rcvevent, int sendevent, int errevent);
extern int lwip_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                                size_t buflen, struct hostent **result, int *h_errnop);

typedef int32_t (*func_vfs_read_t)(int32_t fd, char *buf, uint32_t len);
typedef int32_t (*func_vfs_write_t)(int32_t fd, const char *buf, uint32_t len);
typedef int32_t (*func_vfs_ioctl_t)(int32_t fd, int32_t cmd, uint32_t arg);

static struct network_ops lwip_network_opts = {
    lwip_socket,      //int32_t (*socket)(int32_t domain, int32_t type, int32_t protocol);
    lwip_accept,      //int32_t (*accept)(int32_t fd, struct sockaddr *addr, socklen_t *addrlen);
    lwip_bind,        //int32_t (*bind)(int32_t fd, const struct sockaddr *name, socklen_t namelen);
    lwip_shutdown,    //int32_t (*shutdown)(int32_t fd, int32_t how);
    lwip_getpeername, //int32_t (*getpeername)(int32_t fd, struct sockaddr *name, socklen_t *namelen);
    lwip_getsockname, //int32_t (*getsockname)(int32_t fd, struct sockaddr *name, socklen_t *namelen);
    lwip_getsockopt,  //int32_t (*getsockopt)(int32_t fd, int32_t level, int32_t optname, void *optval, socklen_t *optlen);
    lwip_setsockopt,  // int32_t (*setsockopt)(int32_t fd, int32_t level, int32_t optname, const void *optval, socklen_t optlen);
    lwip_close,       //  int32_t (*close)(int32_t fd);
    lwip_connect,     //int32_t (*connect)(int32_t fd, const struct sockaddr *name, socklen_t namelen);
    lwip_listen,      //int32_t (*listen)(int32_t fd, int32_t backlog);
    lwip_recv,        //int32_t (*recv)(int32_t fd, void *mem, size_t len, int32_t flags);
    lwip_recvfrom,    //int32_t (*recvfrom)(int32_t fd, void *mem, size_t len, int32_t flags, struct sockaddr *from, socklen_t *fromlen);
    lwip_send,        //int32_t (*send)(int32_t fd, const void *dataptr, size_t size, int32_t flags);
    lwip_sendmsg,     //int32_t (*sendmsg)(int32_t fd, const struct msghdr *message, int32_t flags);
    lwip_sendto,      //int32_t (*sendto)(int32_t fd, const void *dataptr, size_t size, int32_t flags, const struct sockaddr *to, socklen_t tolen);
    (func_vfs_read_t)lwip_read,        //int32_t (*read)(int32_t fd, char *buf, uint32_t len);
    (func_vfs_write_t)lwip_write,       //int32_t (*write)(int32_t fd, const char *buf, uint32_t len);
    (func_vfs_ioctl_t)lwip_ioctl,       //int32_t (*ioctl)(int32_t fd, int32_t cmd, uint32_t arg);
    lwip_fcntl,       //int32_t (*fcntl)(int32_t fd, int32_t cmd, int32_t val);
    lwip_writev,      //int32_t (*writev)(int32_t fd, const struct iovec *iov, int32_t iovcnt);
    lwip_eventfd,     //int32_t (*eventfd)(uint32_t initval, int32_t flags);
#if LWIP_PACKET
    lwip_try_wakeup, // int32_t (*try_wakeup)(int32_t fd, int32_t rcvevent, int32_t sendevent, int32_t errevent);
#else
    NULL,
#endif
    lwip_gethostbyname,   //  struct hostent* (*gethostbyname)(const char *name);
    lwip_gethostbyname_r, //int (*gethostbyname_r)(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop);
    lwip_freeaddrinfo,    // void (*freeaddrinfo)(struct addrinfo *ai);
    lwip_getaddrinfo,
    lwip_select,
};

void ulwip_server_attach_vfs(void)
{
    if (0 != aos_register_network(&lwip_network_opts, NULL))
    {
        printf("[%s] register network failed\n", __FUNCTION__);
    }
}
