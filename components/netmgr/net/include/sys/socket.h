/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef WITH_LWIP
#include "lwip/opt.h"
#include "lwip/sockets.h"
#endif
#ifdef WITH_SAL
#include "sal_sockets.h"
#endif
#if defined(__ICCARM__) || defined(__CC_ARM)
#include <sys/errno.h>
#else
#include <errno.h>
#endif

/************************************************************************************************
 *                                                                                              *
 *standard io offset    af packet fd offset       socket event fd offset                        *
 *   |    socket fd offset      |   sal socket fd offset    |       sal event offset fd            *
 *   |         |                |             |             |                |                  *
 *   |=========|================|=============|=============|================|=============== | *
 *    reserved    socket fd       af_packet fd sal socket fd socket event fd   sal  event fd    *
 *                                                                           *
 *****************************************************************************/

/* reserved socketd for posix standard file descriptors */
#define POSIX_EVB_SOCKET_RESERVED_START 0
#define POSIX_EVB_SOCKET_RESERVED_END 2
#define LWIP_PACKET_NUM_SOCKETS  4
#define SAL_NUM_SOCKETS     4
#define FD_AOS_SOCKET_OFFSET (LWIP_SOCKET_OFFSET+(POSIX_EVB_SOCKET_RESERVED_END-POSIX_EVB_SOCKET_RESERVED_START+1))
#define LWIP_NUM_SOCKETS (MEMP_NUM_NETCONN-(POSIX_EVB_SOCKET_RESERVED_END-POSIX_EVB_SOCKET_RESERVED_START+1))
#define LWIP_PACKET_SOCKET_OFFSET (FD_AOS_SOCKET_OFFSET + LWIP_NUM_SOCKETS)
#define SAL_SOCKET_OFFSET (LWIP_PACKET_SOCKET_OFFSET + LWIP_PACKET_NUM_SOCKETS)
#define LWIP_EVENT_OFFSET (SAL_SOCKET_OFFSET + SAL_NUM_SOCKETS)
#define LWIP_NUM_EVENTS   MEMP_NUM_NETCONN
#define SAL_EVENT_OFFSET (SAL_SOCKET_OFFSET + LWIP_NUM_EVENTS)
#define FD_AOS_NUM_SOCKETS (LWIP_NUM_SOCKETS + LWIP_PACKET_NUM_SOCKETS + SAL_NUM_SOCKETS)
#define FD_AOS_NUM_EVENTS  4
#define FD_AOS_EVENT_OFFSET (FD_AOS_SOCKET_OFFSET + LWIP_NUM_SOCKETS + LWIP_PACKET_NUM_SOCKETS + SAL_NUM_SOCKETS)

typedef enum {
    SOCK_TYPE_LWIP,
    SOCK_TYPE_SAL,
    SOCK_TYPE_UNKNOWN,
    SOCK_TYPE_MAX
} sock_type_t;

typedef enum {
    EVENT_TYPE_LWIP,
    EVENT_TYPE_SAL,
    EVENT_TYPE_UNKNOWN,
    EVENT_TYPE_MAX
} event_type_t;

#if LWIP_COMPAT_SOCKETS == 2
/* This helps code parsers/code completion by not having the COMPAT functions as defines */
#define lwip_accept       accept
#define lwip_bind         bind
#define lwip_shutdown     shutdown
#define lwip_getpeername  getpeername
#define lwip_getsockname  getsockname
#define lwip_setsockopt   setsockopt
#define lwip_getsockopt   getsockopt
#define lwip_close        closesocket
#define lwip_connect      connect
#define lwip_listen       listen
#define lwip_recv         recv
#define lwip_recvfrom     recvfrom
#define lwip_send         send
#define lwip_sendmsg      sendmsg
#define lwip_sendto       sendto
#define lwip_socket       socket
// #define lwip_select       select
#define lwip_select2      select2
#define lwip_ioctlsocket  ioctl

#ifndef POSIX_DEVICE_IO_NEED
#if LWIP_POSIX_SOCKETS_IO_NAMES
#define lwip_read         read
#define lwip_write        write
#define lwip_writev       writev
#define lwip_eventfd      eventfd
#undef lwip_close
#define lwip_close        close
#define closesocket(s)    close(s)
#define lwip_fcntl        fcntl
#define lwip_ioctl        ioctl
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */
#endif /* POSIX_DEVICE_IO_NEED */
#endif /* LWIP_COMPAT_SOCKETS == 2 */

int net_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int net_bind(int s, const struct sockaddr *name, socklen_t namelen);
int net_shutdown(int s, int how);
int net_getpeername (int s, struct sockaddr *name, socklen_t *namelen);
int net_getsockname (int s, struct sockaddr *name, socklen_t *namelen);
int net_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
int net_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
int net_close(int s);
int net_connect(int s, const struct sockaddr *name, socklen_t namelen);
int net_listen(int s, int backlog);
int net_recv(int s, void *mem, size_t len, int flags);
int net_read(int s, void *mem, size_t len);
int net_recvfrom(int s, void *mem, size_t len, int flags,
    struct sockaddr *from, socklen_t *fromlen);
int net_send(int s, const void *dataptr, size_t size, int flags);
int net_sendmsg(int s, const struct msghdr *message, int flags);
int net_sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen);
int net_socket(int domain, int type, int protocol);
int net_write(int s, const void *data, size_t size);
int net_writev(int s, const struct iovec *iov, int iovcnt);
int net_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);
int net_ioctl(int s, long cmd, void *argp);
//int fcntl(int s, int cmd, int val);
int net_eventfd(unsigned int initval, int flags);

#if LWIP_COMPAT_SOCKETS
#if LWIP_COMPAT_SOCKETS != 2
#ifdef POSIX_DEVICE_IO_NEED
static inline int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	return net_accept(s, addr, addrlen);
}

static inline int bind(int s, const struct sockaddr *name, socklen_t namelen)
{
	return net_bind(s, name, namelen);
}

static inline int shutdown(int s, int how)
{
	return net_shutdown(s, how);
}

static inline int getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
	return net_getpeername(s, name, namelen);
}
static inline int getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
	return net_getsockname(s, name, namelen);
}

static inline int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
	return net_getsockopt(s, level, optname, optval, optlen);
}

static inline int setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
{
	return net_setsockopt(s, level, optname, optval, optlen);
}

static inline int connect(int s, const struct sockaddr *name, socklen_t namelen)
{
	return net_connect(s, name, namelen);
}

static inline int listen(int s, int backlog)
{
	return net_listen(s, backlog);
}

static inline int recv(int s, void *mem, size_t len, int flags)
{
	return net_recv(s, mem, len, flags);
}

static inline int recvfrom(int s, void *mem, size_t len, int flags,struct sockaddr *from, socklen_t *fromlen)
{
	return net_recvfrom(s, mem, len, flags, from, fromlen);
}

static inline int send(int s, const void *dataptr, size_t size, int flags)
{
	return net_send(s, dataptr, size, flags);
}

static inline int sendmsg(int s, const struct msghdr *message, int flags)
{
	return net_sendmsg(s, message, flags);
}

static inline int sendto(int s, const void *dataptr, size_t size, int flags,	const struct sockaddr *to, socklen_t tolen)
{
	return net_sendto(s, dataptr, size, flags, to, tolen);
}

static inline int socket(int domain, int type, int protocol)
{
	return net_socket(domain, type, protocol);
}

static inline int writev(int s, const struct iovec *iov, int iovcnt)
{
	return net_writev(s, iov, iovcnt);
}

static inline int eventfd(unsigned int initval, int flags)
{
	return net_eventfd(initval, flags);
}

/** @ingroup socket */
#define select(maxfdp1,readset,writeset,exceptset,timeout)     net_select(maxfdp1,readset,writeset,exceptset,timeout)
/** @ingroup socket */
#define ioctlsocket(s,cmd,argp)                   net_ioctl(s,cmd,argp)
/** @ingroup socket */
/* This is a fix for newlibc does not define HAVE_FCNTL */
extern int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg);
#define fcntl(s,cmd,val)                          _fcntl_r(0, s,cmd,val)

#else
/** @ingroup socket */
#define accept(s,addr,addrlen)                    net_accept(s,addr,addrlen)
/** @ingroup socket */
#define bind(s,name,namelen)                      net_bind(s,name,namelen)
/** @ingroup socket */
#define shutdown(s,how)                           net_shutdown(s,how)
/** @ingroup socket */
#define getpeername(s,name,namelen)               net_getpeername(s,name,namelen)
/** @ingroup socket */
#define getsockname(s,name,namelen)               net_getsockname(s,name,namelen)
/** @ingroup socket */
#define setsockopt(s,level,optname,opval,optlen)  net_setsockopt(s,level,optname,opval,optlen)
/** @ingroup socket */
#define getsockopt(s,level,optname,opval,optlen)  net_getsockopt(s,level,optname,opval,optlen)
/** @ingroup socket */
#define closesocket(s)                            net_close(s)
/** @ingroup socket */
#define connect(s,name,namelen)                   net_connect(s,name,namelen)
/** @ingroup socket */
#define listen(s,backlog)                         net_listen(s,backlog)
/** @ingroup socket */
#define recv(s,mem,len,flags)                     net_recv(s,mem,len,flags)
/** @ingroup socket */
#define recvfrom(s,mem,len,flags,from,fromlen)    net_recvfrom(s,mem,len,flags,from,fromlen)
/** @ingroup socket */
#define send(s,dataptr,size,flags)                net_send(s,dataptr,size,flags)
/** @ingroup socket */
#define sendmsg(s,message,flags)                  net_sendmsg(s,message,flags)
/** @ingroup socket */
#define sendto(s,dataptr,size,flags,to,tolen)     net_sendto(s,dataptr,size,flags,to,tolen)
/** @ingroup socket */
#define socket(domain,type,protocol)              net_socket(domain,type,protocol)
/** @ingroup socket */
#define select(maxfdp1,readset,writeset,exceptset,timeout)     net_select(maxfdp1,readset,writeset,exceptset,timeout)
/** @ingroup socket */
#define ioctlsocket(s,cmd,argp)                   net_ioctl(s,cmd,argp)

#if LWIP_POSIX_SOCKETS_IO_NAMES
/** @ingroup socket */
#define read(s,mem,len)                           net_read(s,mem,len)
/** @ingroup socket */
#define write(s,dataptr,len)                      net_write(s,dataptr,len)
/** @ingroup socket */
/** @ingroup socket */
//#define ioctl(s,cmd,argp)                         net_ioctl(s,cmd,argp)
#define eventfd(v, f)                             net_eventfd(v, f)
#define writev(s,iov,iovcnt)                      net_writev(s,iov,iovcnt)
/** @ingroup socket */
#define close(s)                                  net_close(s)
/** @ingroup socket */
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */
#endif /* POSIX_DEVICE_IO_NEED */
#endif /* LWIP_COMPAT_SOCKETS != 2 */
#endif /* LWIP_COMPAT_SOCKETS */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SOCKET_H */

