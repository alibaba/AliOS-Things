/*!
 * @file sal_sockets.h
 *
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_SOCKET_H_
#define _SAL_SOCKET_H_

#include <stddef.h> //!< for size_t
#include <sys/time.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SAL socket APIs, which will be mapped to standard socket APIs.
 * So generally these APIs are not likely to be used directly;
 * instead, standard socket APIs are used.
 */
int sal_select(int maxfdp1, fd_set *readset, fd_set *writeset,
               fd_set *exceptset, struct timeval *timeout);

int sal_socket(int domain, int type, int protocol);

int sal_write(int s, const void *data, size_t size);

int sal_connect(int s, const struct sockaddr *name, socklen_t namelen);

int sal_bind(int s, const struct sockaddr *name, socklen_t namelen);

int sal_eventfd(unsigned int initval, int flags);

int sal_setsockopt(int s, int level, int optname,
                   const void *optval, socklen_t optlen);

int sal_getsockopt(int s, int level, int optname,
                   void *optval, socklen_t *optlen);

struct hostent *sal_gethostbyname(const char *name);

int sal_close(int s);

int sal_init(void);

int sal_add_dev(char* driver_name, void* data);

int sal_sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);

int sal_send(int s, const void *data, size_t size, int flags);

int sal_recvfrom(int s, void *mem, size_t len, int flags,
                 struct sockaddr *from, socklen_t *fromlen);

int sal_recv(int s, void *mem, size_t len, int flags);

int sal_read(int s, void *mem, size_t len);

void sal_freeaddrinfo(struct addrinfo *ai);

int sal_getaddrinfo(const char *nodename, const char *servname,
                    const struct addrinfo *hints, struct addrinfo **res);

int sal_shutdown(int s, int how);

int sal_fcntl(int s, int cmd, int val);

#if 0
/**
 * Standard socket APIs, mapped from SAL socket APIs.
 * Please refer to socket mannual for detailed description of these APIs.
 */
#define select(maxfdp1,readset,writeset,exceptset,timeout) \
    sal_select(maxfdp1,readset,writeset,exceptset,timeout)

#define write(s,data,size) \
    sal_write(s,data,size)

#define socket(domain,type,protocol) \
    sal_socket(domain,type,protocol)

#define connect(s,name,namelen) \
    sal_connect(s,name,namelen)

#define bind(s,name,namelen) \
    sal_bind(s,name,namelen)

#define shutdown(s,how) \
    sal_shutdown(s,how)

#define eventfd(initval,flags) \
    sal_eventfd(initval,flags)

#define setsockopt(s,level,optname,optval,optlen) \
    sal_setsockopt(s,level,optname,optval,optlen)

#define getsockopt(s,level,optname,optval,optlen) \
    sal_getsockopt(s,level,optname,optval,optlen)

#define gethostbyname(name) \
    sal_gethostbyname(name)

#define close(s) \
    sal_close(s)

#define sendto(s,dataptr,size,flags,to,tolen) \
    sal_sendto(s,dataptr,size,flags,to,tolen)

#define recvfrom(s,mem,len,flags,from,fromlen) \
    sal_recvfrom(s,mem,len,flags,from,fromlen)

#define send(s,data,size,flags) \
    sal_send(s,data,size,flags)

#define recv(s,data,size,flags) \
    sal_recv(s,data,size,flags)

#define read(s,data,size) \
    sal_read(s,data,size)

#define freeaddrinfo(addrinfo) sal_freeaddrinfo(addrinfo)

#define getaddrinfo(nodname, servname, hints, res) \
    sal_getaddrinfo(nodname, servname, hints, res)

#define fcntl(s,cmd,val)  sal_fcntl(s,cmd,val)

#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0)
#endif
/** @} */ /* end of group aos_sal */

#ifdef __cplusplus
}
#endif

#endif
