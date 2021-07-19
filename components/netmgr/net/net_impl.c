/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <sys/socket.h>
#include <netdb.h>

#ifdef __cplusplus
extern "C" {
#endif

int net_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
#ifdef WITH_LWIP
	return lwip_accept(s, addr, addrlen);
#else
    return -1;
#endif
}

int net_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
#ifdef WITH_LWIP
	return lwip_bind(s, name, namelen);
#else
    return -1;
#endif
}

int net_shutdown(int s, int how)
{
#ifdef WITH_LWIP
	return lwip_shutdown(s, how);
#else
    return -1;
#endif
}

int net_getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
#ifdef WITH_LWIP
	return lwip_getpeername(s, name, namelen);
#else
    return -1;
#endif
}

int net_getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
#ifdef WITH_LWIP
	return lwip_getsockname(s, name, namelen);
#else
    return -1;
#endif
}

int net_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
#ifdef WITH_LWIP
	return lwip_getsockopt(s, level, optname, optval, optlen);
#else
    return -1;
#endif
}

int net_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
{
#ifdef WITH_LWIP
	return lwip_setsockopt(s, level, optname, optval, optlen);
#else
    return -1;
#endif
}

int net_close(int s)
{
#ifdef WITH_LWIP
	return lwip_close(s);
#else
    return -1;
#endif
}

int net_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
#ifdef WITH_LWIP
	return lwip_connect(s, name, namelen);
#else
    return -1;
#endif
}

int net_listen(int s, int backlog)
{
#ifdef WITH_LWIP
	return lwip_listen(s, backlog);
#else
    return -1;
#endif
}

int net_recv(int s, void *mem, size_t len, int flags)
{
#ifdef WITH_LWIP
	return lwip_recv(s, mem, len, flags);
#else
    return -1;
#endif
}

int net_read(int s, void *mem, size_t len)
{
#ifdef WITH_LWIP
	return lwip_read(s, mem, len);
#else
    return -1;
#endif
}

int net_recvfrom(int s, void *mem, size_t len, int flags,struct sockaddr *from, socklen_t *fromlen)
{
#ifdef WITH_LWIP
	return lwip_recvfrom(s, mem, len, flags, from, fromlen);
#else
    return -1;
#endif
}

int net_send(int s, const void *dataptr, size_t size, int flags)
{
#ifdef WITH_LWIP
	return lwip_send(s, dataptr, size, flags);
#else
    return -1;
#endif
}

int net_sendmsg(int s, const struct msghdr *message, int flags)
{
#ifdef WITH_LWIP
	return lwip_sendmsg(s, message, flags);
#else
    return -1;
#endif
}

int net_sendto(int s, const void *dataptr, size_t size, int flags,	const struct sockaddr *to, socklen_t tolen)
{
#ifdef WITH_LWIP
	return lwip_sendto(s, dataptr, size, flags, to, tolen);
#else
    return -1;
#endif
}

int net_socket(int domain, int type, int protocol)
{
#ifdef WITH_LWIP
	return lwip_socket(domain, type, protocol);
#else
    return -1;
#endif
}

int net_write(int s, const void *data, size_t size)
{
#ifdef WITH_LWIP
	return lwip_write(s, data, size);
#else
    return -1;
#endif
}

int net_writev(int s, const struct iovec *iov, int iovcnt)
{
#ifdef WITH_LWIP
	return lwip_writev(s, iov, iovcnt);
#else
    return -1;
#endif
}

int net_select(int maxfdp1, fd_set *readset, fd_set *writeset,
               fd_set *exceptset, struct timeval *timeout)
{
#ifdef WITH_LWIP
	return lwip_select(maxfdp1, readset, writeset,
               exceptset, timeout);
#else
    return -1;
#endif
}

int net_ioctl(int s, long cmd, void *argp) {
#ifdef WITH_LWIP
	return lwip_ioctl(s, cmd, argp);
#else
    return -1;
#endif
}

int net_fcntl(int s, int cmd, int val) {

#ifdef WITH_LWIP
	return lwip_fcntl(s, cmd, val);
#else
    return -1;
#endif
}

int net_eventfd(unsigned int initval, int flags)
{
#ifdef WITH_LWIP
    return lwip_eventfd(initval, flags);
#else
    return -1;
#endif
}

struct hostent *net_gethostbyname(const char *name)
{
#ifdef WITH_LWIP
    return lwip_gethostbyname(name);
#else
    return NULL;
#endif
}

int net_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                size_t buflen, struct hostent **result, int *h_errnop)
{
#ifdef WITH_LWIP
    return lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop);
#else
    return -1;
#endif
}

void net_freeaddrinfo(struct addrinfo *ai)
{
#ifdef WITH_LWIP
    lwip_freeaddrinfo(ai);
#endif
}

int net_getaddrinfo(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res)
{
#ifdef WITH_LWIP
    return lwip_getaddrinfo(nodename, servname, hints, res);
#else
    return -1;
#endif
}

#ifdef __cplusplus
}
#endif

