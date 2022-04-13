/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef NETWORK_USYSCALL_H
#define NETWORK_USYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>

int network_accept(int s, struct sockaddr *addr, socklen_t *addrlen);

int network_bind(int s, const struct sockaddr *name, socklen_t namelen);

int network_shutdown(int s, int how);

int network_getpeername(int s, struct sockaddr *name, socklen_t *namelen);

int network_getsockname(int s, struct sockaddr *name, socklen_t *namelen);

int network_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);

int network_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);

int network_close(int s);

int network_connect(int s, const struct sockaddr *name, socklen_t namelen);

int network_listen(int s, int backlog);

int network_recv(int s, void *mem, size_t len, int flags);

int network_read(int s, void *mem, size_t len);

int network_recvfrom(int s, void *mem, size_t len, int flags,
                  struct sockaddr *from, socklen_t *fromlen);

int network_send(int s, const void *dataptr, size_t size, int flags);

int network_sendmsg(int s, const struct msghdr *message, int flags);

int network_sendto(int s, const void *dataptr, size_t size, int flags,
                const struct sockaddr *to, socklen_t tolen);

int network_socket(int domain, int type, int protocol);

int network_write(int s, const void *dataptr, size_t size);

int network_writev(int s, const struct iovec *iov, int iovcnt);

int network_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);

int network_ioctl(int s, long cmd, unsigned long arg);

int network_fcntl(int s, int cmd, int val);

int network_eventfd(unsigned int initval, int flags);

int network_try_wakeup(int s, int rcvevent, int sendevent, int errevent);

struct hostent *network_gethostbyname(const char *name);

int network_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                         size_t buflen, struct hostent **result, int *h_errnop);

void network_freeaddrinfo(struct addrinfo *ai);

int network_getaddrinfo(const char *nodename, const char *servname,
                     const struct addrinfo *hints, struct addrinfo **res);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_USYSCALL_H */