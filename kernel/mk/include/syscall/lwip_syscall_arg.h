/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef LWIP_SYSCALL_ARG_H
#define LWIP_SYSCALL_ARG_H

#include "lwip/sockets.h"
#include "lwip/netdb.h"

typedef struct {
    int              s;
    struct sockaddr *addr;
    socklen_t       *addrlen;
} lwip_accept_syscall_art_t;

typedef struct {
    int                    s;
    const struct sockaddr *name;
    socklen_t              namelen;
} lwip_bind_syscall_arg_t;

typedef struct {
    int s;
    int how;
} lwip_shutdown_syscall_arg_t;

typedef struct {
    int              s;
    struct sockaddr *name;
    socklen_t       *namelen;
} lwip_getpeername_syscall_arg_t;

typedef struct {
    int              s;
    struct sockaddr *name;
    socklen_t       *namelen;
} lwip_getsockname_syscall_arg_t;

typedef struct {
    int        s;
    int        level;
    int        optname;
    void      *optval;
    socklen_t *optlen;
} lwip_getsockopt_syscall_arg_t;

typedef struct {
    int         s;
    int         level;
    int         optname;
    const void *optval;
    socklen_t   optlen;
} lwip_setsockopt_syscall_arg_t;

typedef struct {
    int s;
} lwip_close_syscall_arg_t;

typedef struct {
    int                    s;
    const struct sockaddr *name;
    socklen_t              namelen;
} lwip_connect_syscall_arg_t;

typedef struct {
    int s;
    int backlog;
} lwip_listen_syscall_arg_t;

typedef struct {
    int     s;
    void   *mem;
    size_t  len;
    int     flags;
} lwip_recv_syscall_arg_t;

typedef struct {
    int     s;
    void   *mem;
    size_t  len;
} lwip_read_syscall_arg_t;

typedef struct {
    int              s;
    void            *mem;
    size_t           len;
    int              flags;
    struct sockaddr *from;
    socklen_t       *fromlen;
} lwip_recvfrom_syscall_arg_t;

typedef struct {
    int         s;
    const void *dataptr;
    size_t      size;
    int         flags;
} lwip_send_syscall_arg_t;

typedef struct {
    int                  s;
    const struct msghdr *message;
    int                  flags;
} lwip_sendmsg_syscall_arg_t;

typedef struct {
    int                    s;
    const void            *dataptr;
    size_t                 size;
    int                    flags;
    const struct sockaddr *to;
    socklen_t              tolen;
} lwip_sendto_syscall_arg_t;

typedef struct {
    int domain;
    int type;
    int protocol;
} lwip_socket_syscall_arg_t;

typedef struct {
    int         s;
    const void *dataptr;
    size_t      size;
} lwip_write_syscall_arg_t;

typedef struct {
    int                 s;
    const struct iovec *iov;
    int                 iovcnt;
} lwip_writev_syscall_arg_t;

typedef struct {
    int             maxfdp1;
    fd_set         *readset;
    fd_set         *writeset;
    fd_set         *exceptset;
    struct timeval *timeout;
} lwip_select_syscall_arg_t;

typedef struct {
    int   s;
    long  cmd;
    void *argp;
} lwip_ioctl_syscall_arg_t;

typedef struct {
    int s;
    int cmd;
    int val;
} lwip_fcntl_syscall_arg_t;

typedef struct {
    unsigned int initval;
    int          flags;
} lwip_eventfd_syscall_arg_t;

#if LWIP_PACKET
typedef struct {
    int s;
    int rcvevent;
    int sendevent;
    int errevent;
} lwip_try_wakeup_syscall_arg_t;
#endif

typedef struct {
    const char *name;
    char       *aliases;
    ip_addr_t  *hostent_addr;
    char       *hostname;
} lwip_gethostbyname_syscall_arg_t;

typedef struct {
    const char      *name;
    struct hostent  *ret;
    char            *buf;
    size_t           buflen;
    struct hostent **result;
    int             *h_errnop;
} lwip_gethostbyname_r_syscall_arg_t;

typedef struct {
    struct addrinfo *ai;
} lwip_freeaddrinfo_syscall_arg_t;

typedef struct {
    const char             *nodename;
    const char             *servname;
    const struct addrinfo  *hints;
    struct addrinfo        *ai;
    struct sockaddr        *sa;
} lwip_getaddrinfo_syscall_arg_t;

#endif /* LWIP_SYSCALL_ARG_H */

