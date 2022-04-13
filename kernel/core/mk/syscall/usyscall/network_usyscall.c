/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "stdlib.h"
#include "string.h"
#include "syscall.h"
#include "network_syscall_arg.h"
#include "syscall_no.h"

int network_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    network_accept_syscall_art_t _arg;

    _arg.s       = s;
    _arg.addr    = addr;
    _arg.addrlen = addrlen;

    return (int)SYSCALL(SYS_NETWORK_ACCEPT, (void *)&_arg);
}

int network_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    network_bind_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_NETWORK_BIND, (void *)&_arg);
}

int network_shutdown(int s, int how)
{
    network_shutdown_syscall_arg_t _arg;

    _arg.s   = s;
    _arg.how = how;

    return (int)SYSCALL(SYS_NETWORK_SHUTDOWN, (void *)&_arg);
}

int network_getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
    network_getpeername_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_NETWORK_GETPEERNAME, (void *)&_arg);
}

int network_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
    network_getsockname_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_NETWORK_GETSOCKNAME, (void *)&_arg);
}

int network_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
    network_getsockopt_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.level   = level;
    _arg.optname = optname;
    _arg.optval  = optval;
    _arg.optlen  = optlen;

    return (int)SYSCALL(SYS_NETWORK_GETSOCKOPT, (void *)&_arg);
}

int network_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
    network_setsockopt_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.level   = level;
    _arg.optname = optname;
    _arg.optval  = optval;
    _arg.optlen  = optlen;

    return (int)SYSCALL(SYS_NETWORK_SETSOCKET, (void *)&_arg);
}

int network_close(int s)
{
    network_close_syscall_arg_t _arg;

    _arg.s = s;

    return (int)SYSCALL(SYS_NETWORK_CLOSE, (void *)&_arg);
}

int network_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    network_connect_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_NETWORK_CONNECT, (void *)&_arg);
}

int network_listen(int s, int backlog)
{
    network_listen_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.backlog = backlog;

    return (int)SYSCALL(SYS_NETWORK_LISTEN, (void *)&_arg);
}

int network_recv(int s, void *mem, size_t len, int flags)
{
    network_recv_syscall_arg_t _arg;

    _arg.s     = s;
    _arg.mem   = mem;
    _arg.len   = len;
    _arg.flags = flags;

    return (int)SYSCALL(SYS_NETWORK_RECV, (void *)&_arg);
}

int network_read(int s, void *mem, size_t len)
{
    network_read_syscall_arg_t _arg;

    _arg.s   = s;
    _arg.mem = mem;
    _arg.len = len;

    return (int)SYSCALL(SYS_NETWORK_READ, (void *)&_arg);
}

int network_recvfrom(int s, void *mem, size_t len, int flags,
                  struct sockaddr *from, socklen_t *fromlen)
{
    network_recvfrom_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.mem     = mem;
    _arg.len     = len;
    _arg.flags   = flags;
    _arg.from    = from;
    _arg.fromlen = fromlen;

    return (int)SYSCALL(SYS_NETWORK_RECVFROM, (void *)&_arg);
}

int network_send(int s, const void *dataptr, size_t size, int flags)
{
    network_send_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.dataptr = dataptr;
    _arg.size    = size;
    _arg.flags   = flags;

    return (int)SYSCALL(SYS_NETWORK_SEND, (void *)&_arg);
}

int network_sendmsg(int s, const struct msghdr *message, int flags)
{
    network_sendmsg_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.message = message;
    _arg.flags   = flags;

    return (int)SYSCALL(SYS_NETWORK_SENDMSG, (void *)&_arg);
}

int network_sendto(int s, const void *dataptr, size_t size, int flags,
                const struct sockaddr *to, socklen_t tolen)
{
    network_sendto_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.dataptr = dataptr;
    _arg.size    = size;
    _arg.flags   = flags;
    _arg.to      = to;
    _arg.tolen   = tolen;

    return (int)SYSCALL(SYS_NETWORK_SENDTO, (void *)&_arg);
}

int network_socket(int domain, int type, int protocol)
{
    network_socket_syscall_arg_t _arg;

    _arg.domain   = domain;
    _arg.type     = type;
    _arg.protocol = protocol;

    return (int)SYSCALL(SYS_NETWORK_SOCKET, (void *)&_arg);
}

int network_write(int s, const void *dataptr, size_t size)
{
    network_write_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.dataptr = dataptr;
    _arg.size    = size;

    return (int)SYSCALL(SYS_NETWORK_WRITE, (void *)&_arg);
}

int network_writev(int s, const struct iovec *iov, int iovcnt)
{
    network_writev_syscall_arg_t _arg;

    _arg.s      = s;
    _arg.iov    = iov;
    _arg.iovcnt = iovcnt;

    return (int)SYSCALL(SYS_NETWORK_WRITEV, (void *)&_arg);
}

int network_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout)
{
    network_select_syscall_arg_t _arg;

    _arg.maxfdp1   = maxfdp1;
    _arg.readset   = readset;
    _arg.writeset  = writeset;
    _arg.exceptset = exceptset;
    _arg.timeout   = timeout;

    return (int)SYSCALL(SYS_NETWORK_SELECT, (void *)&_arg);
}

int network_ioctl(int s, long cmd, unsigned long arg)
{
    network_ioctl_syscall_arg_t _arg;

    _arg.s    = s;
    _arg.cmd  = cmd;
    _arg.arg = arg;

    return (int)SYSCALL(SYS_NETWORK_IOCTL, (void *)&_arg);
}

int network_fcntl(int s, int cmd, int val)
{
    network_fcntl_syscall_arg_t _arg;

    _arg.s   = s;
    _arg.cmd = cmd;
    _arg.val = val;

    return (int)SYSCALL(SYS_NETWORK_FCNTL, (void *)&_arg);
}

int network_eventfd(unsigned int initval, int flags)
{
    network_eventfd_syscall_arg_t _arg;

    _arg.initval = initval;
    _arg.flags   = flags;

    return (int)SYSCALL(SYS_NETWORK_EVENTFD, (void *)&_arg);
}

int network_try_wakeup(int s, int rcvevent, int sendevent, int errevent)
{
    network_try_wakeup_syscall_arg_t _arg;

    _arg.s         = s;
    _arg.rcvevent  = rcvevent;
    _arg.sendevent = sendevent;
    _arg.errevent  = errevent;

    return (int)SYSCALL(SYS_NETWORK_TRY_WAKEUP, (void *)&_arg);
}

struct hostent *network_gethostbyname(const char *name)
{
    static struct hostent s_hostent;

    static char *s_aliases;

    static ip_addr_t  s_hostent_addr;
    static ip_addr_t *s_phostent_addr[2];

    char s_hostname[DNS_MAX_NAME_LENGTH + 1];

    network_gethostbyname_syscall_arg_t _arg;

    _arg.name         = name;
    _arg.aliases      = s_aliases;
    _arg.hostent_addr = &s_hostent_addr;
    _arg.hostname     = s_hostname;

    SYSCALL(SYS_NETWORK_GETHOSTBYNAME, (void *)&_arg);

    s_phostent_addr[0] = &s_hostent_addr;
    s_phostent_addr[1] = NULL;

    s_hostent.h_name      = s_hostname;
    s_hostent.h_aliases   = &s_aliases;
    s_hostent.h_addrtype  = AF_INET;
    s_hostent.h_length    = sizeof(ip_addr_t);
    s_hostent.h_addr_list = (char **)&s_phostent_addr;

    return &s_hostent;
}

int network_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                         size_t buflen, struct hostent **result, int *h_errnop)
{
    network_gethostbyname_r_syscall_arg_t _arg;

    _arg.name     = name;
    _arg.ret      = ret;
    _arg.buf      = buf;
    _arg.buflen   = buflen;
    _arg.result   = result;
    _arg.h_errnop = h_errnop;

    return (int)SYSCALL(SYS_NETWORK_GETHOSTBYNAME_R, (void *)&_arg);
}

void network_freeaddrinfo(struct addrinfo *ai)
{
    char   *name;
    struct  sockaddr *sa;

    if (ai == NULL) {
        return;
    }

    name = ai->ai_canonname;
    if (name != NULL) {
        free(name);
    }

    sa = ai->ai_addr;
    if (sa != NULL) {
        free(sa);
    }

    free(ai);
}

int network_getaddrinfo(const char *nodename, const char *servname,
                     const struct addrinfo *hints, struct addrinfo **res)
{
    struct addrinfo *ai;
    struct sockaddr *sa;

    char *name;
    int   namelen;

    int ret;

    network_getaddrinfo_syscall_arg_t _arg;

    namelen = strlen(nodename);
    name = (char *)malloc(namelen + 1);
    if (name == NULL) {
        return -1001;
    }

    ai = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    if (ai == NULL) {
        ret = -1002;
        goto free_name;
    }

    sa = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    if (sa == NULL) {
        ret = -1003;
        goto free_ai;
    }

    _arg.nodename = nodename;
    _arg.servname = servname;
    _arg.hints    = hints;
    _arg.ai       = ai;
    _arg.sa       = sa;

    ret = (int)SYSCALL(SYS_NETWORK_GETADDRINOF, (void *)&_arg);
    if (ret != 0) {
        goto free_sa;
    }

    memcpy(name, nodename, namelen);
    name[namelen]    = 0;
    ai->ai_canonname = name;
    ai->ai_addr      = sa;
    *res             = ai;

    return ret;

free_sa:
    free(sa);
free_ai:
    free(ai);
free_name:
    free(name);
    *res = NULL;

    return ret;
}

