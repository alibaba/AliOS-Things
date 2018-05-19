#ifndef _SOCKETS_MT_H_
#define _SOCKETS_MT_H_

#ifdef SOCKETS_MT

int lwip_mt_init(void);
int lwip_socket_mt(int domain, int type, int protocol);
int lwip_bind_mt(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_connect_mt(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_listen_mt(int s, int backlog);
int lwip_accept_mt(int s, struct sockaddr *addr, socklen_t *addrlen);
int lwip_getpeername_mt(int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getsockname_mt(int s, struct sockaddr *name, socklen_t *namelen);
int lwip_setsockopt_mt(int s, int level, int optname, const void *optval, socklen_t optlen);
int lwip_getsockopt_mt(int s, int level, int optname, void *optval, socklen_t *optlen);
int lwip_ioctl_mt(int s, long cmd, void *argp);
int lwip_sendto_mt(int s, const void *data, size_t size, int flags,
            const struct sockaddr *to, socklen_t tolen);
int lwip_send_mt(int s, const void *data, size_t size, int flags);
int lwip_recvfrom_mt(int s, void *mem, size_t len, int flags,
            struct sockaddr *from, socklen_t *fromlen);
int lwip_recv_mt(int s, void *mem, size_t len, int flags);
int lwip_read_mt(int s, void *mem, size_t len);
int lwip_write_mt(int s, const void *data, size_t size);
int lwip_shutdown_mt(int s, int how);
int lwip_close_mt(int s);
int lwip_select_mt(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
int lwip_fcntl_mt(int s, int cmd, int val);

#ifdef SOCKETS_TCP_TRACE
int lwip_trace_tcp(int s, int cmd, void *arg);
#endif


#if LWIP_COMPAT_SOCKETS

#ifdef SOCKETS_MT_DBUG

#define accept(a,b,c)         lwip_accept_mt(a,b,c); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define bind(a,b,c)           lwip_bind_mt(a,b,c); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define shutdown(a,b)         lwip_shutdown_mt(a,b); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define closesocket(s)        lwip_close_mt(s); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define connect(a,b,c)        lwip_connect_mt(a,b,c); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define getsockname(a,b,c)    lwip_getsockname_mt(a,b,c); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define getpeername(a,b,c)    lwip_getpeername_mt(a,b,c); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define setsockopt(a,b,c,d,e) lwip_setsockopt_mt(a,b,c,d,e); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define getsockopt(a,b,c,d,e) lwip_getsockopt_mt(a,b,c,d,e); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define listen(a,b)           lwip_listen_mt(a,b); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define recv(a,b,c,d)         lwip_recv_mt(a,b,c,d); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define recvfrom(a,b,c,d,e,f) lwip_recvfrom_mt(a,b,c,d,e,f); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define send(a,b,c,d)         lwip_send_mt(a,b,c,d); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define sendto(a,b,c,d,e,f)   lwip_sendto_mt(a,b,c,d,e,f); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define socket(a,b,c)         lwip_socket_mt(a,b,c); os_printf("%s %d\n", __FUNCTION__, __LINE__);
#define select(a,b,c,d,e)     lwip_select_mt(a,b,c,d,e)
#define ioctlsocket(a,b,c)    lwip_ioctl_mt(a,b,c) ; os_printf("%s %d\n", __FUNCTION__, __LINE__);

#if LWIP_POSIX_SOCKETS_IO_NAMES
#define read(a,b,c)           lwip_read_mt(a,b,c)
#define write(a,b,c)          lwip_write_mt(a,b,c)
#define close(s)              lwip_close_mt(s)
#define fcntl(a,b,c)          lwip_fcntl_mt(a,b,c)
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */

#else /* SOCKETS_MT_DBUG */

#define accept(a,b,c)         lwip_accept_mt(a,b,c)
#define bind(a,b,c)           lwip_bind_mt(a,b,c)
#define shutdown(a,b)         lwip_shutdown_mt(a,b)
#define closesocket(s)        lwip_close_mt(s)
#define connect(a,b,c)        lwip_connect_mt(a,b,c)
#define getsockname(a,b,c)    lwip_getsockname_mt(a,b,c)
#define getpeername(a,b,c)    lwip_getpeername_mt(a,b,c)
#define setsockopt(a,b,c,d,e) lwip_setsockopt_mt(a,b,c,d,e)
#define getsockopt(a,b,c,d,e) lwip_getsockopt_mt(a,b,c,d,e)
#define listen(a,b)           lwip_listen_mt(a,b)
#define recv(a,b,c,d)         lwip_recv_mt(a,b,c,d)
#define recvfrom(a,b,c,d,e,f) lwip_recvfrom_mt(a,b,c,d,e,f)
#define send(a,b,c,d)         lwip_send_mt(a,b,c,d)
#define sendto(a,b,c,d,e,f)   lwip_sendto_mt(a,b,c,d,e,f)
#define socket(a,b,c)         lwip_socket_mt(a,b,c)
#define select(a,b,c,d,e)     lwip_select_mt(a,b,c,d,e)
#define ioctlsocket(a,b,c)    lwip_ioctl_mt(a,b,c)

#if LWIP_POSIX_SOCKETS_IO_NAMES
#define read(a,b,c)           lwip_read_mt(a,b,c)
#define write(a,b,c)          lwip_write_mt(a,b,c)
#define close(s)              lwip_close_mt(s)
#define fcntl(a,b,c)          lwip_fcntl_mt(a,b,c)
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */


#endif /* SOCKETS_MT_DBUG */

#endif /* LWIP_COMPAT_SOCKETS */

#endif /* SOCKETS_MT */

#endif
