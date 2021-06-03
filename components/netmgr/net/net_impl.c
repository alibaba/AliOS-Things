/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <sys/socket.h>
#include <netdb.h>

#ifdef __cplusplus
extern "C" {
#endif

static int g_sock_type = SOCK_TYPE_LWIP;
static int get_sock_type(int s) {
    if((s >= LWIP_SOCKET_OFFSET) && (s < SAL_SOCKET_OFFSET)) {
        return SOCK_TYPE_LWIP;
    } else if((s >= SAL_SOCKET_OFFSET) && (s < LWIP_EVENT_OFFSET)) {
        return SOCK_TYPE_SAL;
    } else {
        return SOCK_TYPE_UNKNOWN;
    }
}

int set_sock_type(sock_type_t type) {
    if((type == SOCK_TYPE_LWIP)
        || (type == SOCK_TYPE_SAL)) {
        g_sock_type = type;
        return 0;
    } else {
        return -1;
    }
}

static int g_event_type = EVENT_TYPE_LWIP;
static int get_event_type(int s) {
    if((s >= LWIP_EVENT_OFFSET) && (s < SAL_EVENT_OFFSET)) {
        return EVENT_TYPE_LWIP;
    } else if((s >= SAL_EVENT_OFFSET) && (s < (SAL_EVENT_OFFSET+FD_AOS_NUM_EVENTS))) {
        return EVENT_TYPE_SAL;
    } else {
        return EVENT_TYPE_UNKNOWN;
    }
}

int set_event_type(event_type_t type) {
    if((type == EVENT_TYPE_LWIP)
        || (type == EVENT_TYPE_SAL)) {
        g_event_type = type;
        return 0;
    } else {
        return -1;
    }
}

int net_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_accept(s, addr, addrlen);
    }
    else
#endif
#ifdef WITH_SAL
    if( type == SOCK_TYPE_SAL) {
        return sal_accept(s, addr, addrlen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_bind(s, name, namelen);
    }
    else
#endif
#ifdef WITH_SAL
    if( type == SOCK_TYPE_SAL) {
	    return sal_bind(s, name, namelen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_shutdown(int s, int how)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_shutdown(s, how);
    }
    else
#endif
#ifdef WITH_SAL
    if( type == SOCK_TYPE_SAL) {
	    return sal_shutdown(s, how);
    }
    else
#endif
    {
        return -1;
    }
}

int net_getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_getpeername(s, name, namelen);
    }else
#endif
    {
        return -1;
    }
}

int net_getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_getsockname(s, name, namelen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_getsockopt(s, level, optname, optval, optlen);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_getsockopt(s, level, optname, optval, optlen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_setsockopt(s, level, optname, optval, optlen);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_setsockopt(s, level, optname, optval, optlen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_close(int s)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_close(s);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_close(s);
    }
    else
#endif
    {
        return -1;
    }
}

int net_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_connect(s, name, namelen);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_connect(s, name, namelen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_listen(int s, int backlog)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_listen(s, backlog);
    }
    else
#endif
    {
        return -1;
    }
}

int net_recv(int s, void *mem, size_t len, int flags)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_recv(s, mem, len, flags);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_recv(s, mem, len, flags);
    }
    else
#endif
    {
        return -1;
    }
}

int net_read(int s, void *mem, size_t len)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_read(s, mem, len);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_read(s, mem, len);
    }
    else
#endif
    {
        return -1;
    }

}

int net_recvfrom(int s, void *mem, size_t len, int flags,struct sockaddr *from, socklen_t *fromlen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_recvfrom(s, mem, len, flags, from, fromlen);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_recvfrom(s, mem, len, flags, from, fromlen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_send(int s, const void *dataptr, size_t size, int flags)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_send(s, dataptr, size, flags);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_send(s, dataptr, size, flags);
    }else
#endif
    {
        return -1;
    }
}

int net_sendmsg(int s, const struct msghdr *message, int flags)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_sendmsg(s, message, flags);
    }
    else
#endif
    {
        return -1;
    }
}

int net_sendto(int s, const void *dataptr, size_t size, int flags,	const struct sockaddr *to, socklen_t tolen)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_sendto(s, dataptr, size, flags, to, tolen);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_sendto(s, dataptr, size, flags, to, tolen);
    }
    else
#endif
    {
        return -1;
    }
}

int net_socket(int domain, int type, int protocol)
{
#ifdef WITH_LWIP
    if(g_sock_type == SOCK_TYPE_LWIP) {
	    return lwip_socket(domain, type, protocol);
    }
    else
#endif
#ifdef WITH_SAL
    if {
	    return sal_socket(domain, type, protocol);
    }
    else
#endif
    {
        return -1;
    }
}

int net_write(int s, const void *data, size_t size)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_write(s, data, size);
    }
    else
#endif
#ifdef WITH_SAL
    if(type == SOCK_TYPE_SAL) {
	    return sal_write(s, data, size);
    }
    else
#endif
    {
        return -1;
    }
}

int net_writev(int s, const struct iovec *iov, int iovcnt)
{
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_writev(s, iov, iovcnt);
    }
    else
#endif
    {
        return -1;
    }
}

int net_select(int maxfdp1, fd_set *readset, fd_set *writeset,
               fd_set *exceptset, struct timeval *timeout)
{
    int sock_type = get_sock_type(maxfdp1);
    int event_type = get_event_type(maxfdp1);

#ifdef WITH_LWIP
    if((sock_type == SOCK_TYPE_LWIP)
        || (event_type == EVENT_TYPE_LWIP)) {
	    return lwip_select(maxfdp1, readset, writeset,
               exceptset, timeout);
    }
    else
#endif
#ifdef WITH_SAL
    if((sock_type == SOCK_TYPE_SAL)
        || (event_type == EVENT_TYPE_SAL)) {
	    return sal_select(maxfdp1, readset, writeset,
               exceptset, timeout);
    }
    else
#endif
    {
        return -1;
    }

}

int net_ioctl(int s, long cmd, void *argp) {
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_ioctl(s, cmd, argp);
    }
    else
#endif
    {
        return -1;
    }
}
#if 0

int fcntl(int s, int cmd, int val) {
    int type = get_sock_type(s);

#ifdef WITH_LWIP
    if(type == SOCK_TYPE_LWIP) {
	    return lwip_fcntl(s, iov, iovcnt);
    }
    else
#endif
    {
        return -1;
    }
}
#endif

int net_eventfd(unsigned int initval, int flags)
{
    int ret;

#ifdef WITH_LWIP
    if(g_event_type == EVENT_TYPE_LWIP) {
	    ret = lwip_eventfd(initval, flags);
    }
    else
#endif
#ifdef WITH_SAL
    if(g_event_type == EVENT_TYPE_SAL) {
	    ret = sal_eventfd(initval, flags);
    }
    else
#endif
    {
        ret = -1;
    }
    return ret;
}

struct hostent *net_gethostbyname(const char *name)
{
#ifdef WITH_LWIP
    if(g_sock_type == SOCK_TYPE_LWIP) {
        return lwip_gethostbyname(name);
    }
    else
#endif
#ifdef WITH_SAL
    if(g_sock_type == SOCK_TYPE_SAL) {
	    return sal_gethostbyname(name);
    }
    else
#endif
    {
        return NULL;
    }
}

int net_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                size_t buflen, struct hostent **result, int *h_errnop) {
    int lwip_ret = -1;

#ifdef WITH_LWIP
    lwip_ret = lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop);
#endif
    return lwip_ret;
}

void net_freeaddrinfo(struct addrinfo *ai)
{
#ifdef WITH_LWIP
    if(g_sock_type == SOCK_TYPE_LWIP) {
        lwip_freeaddrinfo(ai);
    }
    else
#endif
#ifdef WITH_SAL
    if (g_sock_type == SOCK_TYPE_SAL) {
        sal_freeaddrinfo(ai);
    }
    else
#endif
    {
        //do nothings
    }
}

int net_getaddrinfo(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res)
{
#ifdef WITH_LWIP
    if(g_sock_type == SOCK_TYPE_LWIP) {
        return lwip_getaddrinfo(nodename, servname, hints, res);
    }
    else
#endif
#ifdef WITH_SAL
    if (g_sock_type == SOCK_TYPE_SAL) {
        return sal_getaddrinfo(nodename, servname, hints, res);
    }
    else
#endif
    {
        return -1;
    }
}

#ifdef __cplusplus
}
#endif

