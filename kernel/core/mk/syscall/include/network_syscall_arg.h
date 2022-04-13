/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef NETWORK_SYSCALL_ARG_H
#define NETWORK_SYSCALL_ARG_H

#include "sys/socket.h"
#include "sys/select.h"
#include "netdb.h"

#ifndef DNS_MAX_NAME_LENGTH
#define DNS_MAX_NAME_LENGTH             256
#endif

typedef unsigned int mem_ptr_t;

#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT 1
#endif

#define NETWORK_MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_ALIGNMENT-1)))

typedef struct {
    const char *path;
    int              s;
    struct sockaddr *addr;
    socklen_t       *addrlen;
} network_accept_syscall_art_t;

typedef struct {
    int                    s;
    const struct sockaddr *name;
    socklen_t              namelen;
} network_bind_syscall_arg_t;

typedef struct {
    int s;
    int how;
} network_shutdown_syscall_arg_t;

typedef struct {
    int              s;
    struct sockaddr *name;
    socklen_t       *namelen;
} network_getpeername_syscall_arg_t;

typedef struct {
    int              s;
    struct sockaddr *name;
    socklen_t       *namelen;
} network_getsockname_syscall_arg_t;

typedef struct {
    int        s;
    int        level;
    int        optname;
    void      *optval;
    socklen_t *optlen;
} network_getsockopt_syscall_arg_t;

typedef struct {
    int         s;
    int         level;
    int         optname;
    const void *optval;
    socklen_t   optlen;
} network_setsockopt_syscall_arg_t;

typedef struct {
    int s;
} network_close_syscall_arg_t;

typedef struct {
    int                    s;
    const struct sockaddr *name;
    socklen_t              namelen;
} network_connect_syscall_arg_t;

typedef struct {
    int s;
    int backlog;
} network_listen_syscall_arg_t;

typedef struct {
    int     s;
    void   *mem;
    size_t  len;
    int     flags;
} network_recv_syscall_arg_t;

typedef struct {
    int     s;
    void   *mem;
    size_t  len;
} network_read_syscall_arg_t;

typedef struct {
    int              s;
    void            *mem;
    size_t           len;
    int              flags;
    struct sockaddr *from;
    socklen_t       *fromlen;
} network_recvfrom_syscall_arg_t;

typedef struct {
    int         s;
    const void *dataptr;
    size_t      size;
    int         flags;
} network_send_syscall_arg_t;

typedef struct {
    int                  s;
    const struct msghdr *message;
    int                  flags;
} network_sendmsg_syscall_arg_t;

typedef struct {
    int                    s;
    const void            *dataptr;
    size_t                 size;
    int                    flags;
    const struct sockaddr *to;
    socklen_t              tolen;
} network_sendto_syscall_arg_t;

typedef struct {
    const char *path;
    int domain;
    int type;
    int protocol;
} network_socket_syscall_arg_t;

typedef struct {
    int         s;
    const void *dataptr;
    size_t      size;
} network_write_syscall_arg_t;

typedef struct {
    int                 s;
    const struct iovec *iov;
    int                 iovcnt;
} network_writev_syscall_arg_t;

typedef struct {
    int             maxfdp1;
    fd_set         *readset;
    fd_set         *writeset;
    fd_set         *exceptset;
    struct timeval *timeout;
} network_select_syscall_arg_t;

typedef struct {
    int   s;
    long  cmd;
    unsigned long arg;
} network_ioctl_syscall_arg_t;

typedef struct {
    int s;
    int cmd;
    int val;
} network_fcntl_syscall_arg_t;

typedef struct {
    unsigned int initval;
    int          flags;
} network_eventfd_syscall_arg_t;

typedef struct {
    int s;
    int rcvevent;
    int sendevent;
    int errevent;
} network_try_wakeup_syscall_arg_t;

typedef struct ip4_addr {
    unsigned int addr;
} ip4_addr_t;

typedef struct ip6_addr {
    unsigned int addr[4];
} ip6_addr_t;

typedef struct _ip_addr {
    union {
        ip6_addr_t ip6;
        ip4_addr_t ip4;
    } u_addr;
    /** @ref sal_ip_addr_type */
    unsigned char type;
} ip_addr_t;

typedef struct {
    const char *name;
    char       *aliases;
    ip_addr_t  *hostent_addr;
    char       *hostname;
} network_gethostbyname_syscall_arg_t;

typedef struct {
    const char      *name;
    struct hostent  *ret;
    char            *buf;
    size_t           buflen;
    struct hostent **result;
    int             *h_errnop;
} network_gethostbyname_r_syscall_arg_t;

typedef struct {
    struct addrinfo *ai;
} network_freeaddrinfo_syscall_arg_t;

typedef struct {
    const char             *nodename;
    const char             *servname;
    const struct addrinfo  *hints;
    struct addrinfo        *ai;
    struct sockaddr        *sa;
} network_getaddrinfo_syscall_arg_t;

#endif /* NETWORK_SYSCALL_ARG_H */

