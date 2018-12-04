/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <arch_syscall.h>
#include <lwip_syscall_arg.h>
#include <syscall_no.h>

int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    lwip_accept_syscall_art_t _arg;

    _arg.s       = s;
    _arg.addr    = addr;
    _arg.addrlen = addrlen;

    return (int)SYSCALL(SYS_LWIP_ACCEPT, (void*)&_arg);
}

int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    lwip_bind_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_LWIP_BIND, (void*)&_arg);
}

int lwip_shutdown(int s, int how)
{
    lwip_shutdown_syscall_arg_t _arg;

    _arg.s   = s;
    _arg.how = how;

    return (int)SYSCALL(SYS_LWIP_SHUTDOWN, (void*)&_arg);
}

int lwip_getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
    lwip_getpeername_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_LWIP_GETPEERNAME, (void*)&_arg);
}

int lwip_getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
    lwip_getsockname_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_LWIP_GETSOCKNAME, (void*)&_arg);
}

int lwip_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
    lwip_getsockopt_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.level   = level;
    _arg.optname = optname;
    _arg.optval  = optval;
    _arg.optlen  = optlen;

    return (int)SYSCALL(SYS_LWIP_GETSOCKOPT, (void*)&_arg);
}

int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
{
    lwip_setsockopt_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.level   = level;
    _arg.optname = optname;
    _arg.optval  = optval;
    _arg.optlen  = optlen;

    return (int)SYSCALL(SYS_LWIP_SETSOCKET, (void*)&_arg);
}

int lwip_close(int s)
{
    lwip_close_syscall_arg_t _arg;

    _arg.s = s;

    return (int)SYSCALL(SYS_LWIP_CLOSE, (void*)&_arg);
}

int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    lwip_connect_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.name    = name;
    _arg.namelen = namelen;

    return (int)SYSCALL(SYS_LWIP_CONNECT, (void*)&_arg);
}

int lwip_listen(int s, int backlog)
{
    lwip_listen_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.backlog = backlog;

    return (int)SYSCALL(SYS_LWIP_LISTEN, (void*)&_arg);
}

int lwip_recv(int s, void *mem, size_t len, int flags)
{
    lwip_recv_syscall_arg_t _arg;

    _arg.s     = s;
    _arg.mem   = mem;
    _arg.len   = len;
    _arg.flags = flags;

    return (int)SYSCALL(SYS_LWIP_RECV, (void*)&_arg);
}

int lwip_read(int s, void *mem, size_t len)
{
    lwip_read_syscall_arg_t _arg;

    _arg.s   = s;
    _arg.mem = mem;
    _arg.len = len;

    return (int)SYSCALL(SYS_LWIP_READ, (void*)&_arg);
}

int lwip_recvfrom(int s, void *mem, size_t len, int flags,
                  struct sockaddr *from, socklen_t *fromlen)
{
    lwip_recvfrom_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.mem     = mem;
    _arg.len     = len;
    _arg.flags   = flags;
    _arg.from    = from;
    _arg.fromlen = fromlen;

    return (int)SYSCALL(SYS_LWIP_RECVFROM, (void*)&_arg);
}

int lwip_send(int s, const void *dataptr, size_t size, int flags)
{
    lwip_send_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.dataptr = dataptr;
    _arg.size    = size;
    _arg.flags   = flags;

    return (int)SYSCALL(SYS_LWIP_SEND, (void*)&_arg);
}

int lwip_sendmsg(int s, const struct msghdr *message, int flags)
{
    lwip_sendmsg_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.message = message;
    _arg.flags   = flags;

    return (int)SYSCALL(SYS_LWIP_SENDMSG, (void*)&_arg);
}

int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen)
{
    lwip_sendto_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.dataptr = dataptr;
    _arg.size    = size;
    _arg.flags   = flags;
    _arg.to      = to;
    _arg.tolen   = tolen;

    return (int)SYSCALL(SYS_LWIP_SENDTO, (void*)&_arg);
}

int lwip_socket(int domain, int type, int protocol)
{
    lwip_socket_syscall_arg_t _arg;

    _arg.domain   = domain;
    _arg.type     = type;
    _arg.protocol = protocol;

    return (int)SYSCALL(SYS_LWIP_SOCKET, (void*)&_arg);
}

int lwip_write(int s, const void *dataptr, size_t size)
{
    lwip_write_syscall_arg_t _arg;

    _arg.s       = s;
    _arg.dataptr = dataptr;
    _arg.size    = size;

    return (int)SYSCALL(SYS_LWIP_WRITE, (void*)&_arg);
}

int lwip_writev(int s, const struct iovec *iov, int iovcnt)
{
    lwip_writev_syscall_arg_t _arg;

    _arg.s      = s;
    _arg.iov    = iov;
    _arg.iovcnt = iovcnt;

    return (int)SYSCALL(SYS_LWIP_WRITEV, (void*)&_arg);
}

int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout)
{
    lwip_select_syscall_arg_t _arg;

    _arg.maxfdp1   = maxfdp1;
    _arg.readset   = readset;
    _arg.writeset  = writeset;
    _arg.exceptset = exceptset;
    _arg.timeout   = timeout;

    return (int)SYSCALL(SYS_LWIP_SELECT, (void*)&_arg);
}

int lwip_ioctl(int s, long cmd, void *argp)
{
    lwip_ioctl_syscall_arg_t _arg;

    _arg.s    = s;
    _arg.cmd  = cmd;
    _arg.argp = argp;

    return (int)SYSCALL(SYS_LWIP_IOCTL, (void*)&_arg);
}

int lwip_fcntl(int s, int cmd, int val)
{
    lwip_fcntl_syscall_arg_t _arg;

    _arg.s   = s;
    _arg.cmd = cmd;
    _arg.val = val;

    return (int)SYSCALL(SYS_LWIP_FCNTL, (void*)&_arg);
}

int lwip_eventfd(unsigned int initval, int flags)
{
    lwip_eventfd_syscall_arg_t _arg;

    _arg.initval = initval;
    _arg.flags   = flags;

    return (int)SYSCALL(SYS_LWIP_EVENTFD, (void*)&_arg);
}

#if LWIP_PACKET
int lwip_try_wakeup(int s, int rcvevent, int sendevent, int errevent)
{
    lwip_try_wakeup_syscall_arg_t _arg;

    _arg.s         = s;
    _arg.rcvevent  = rcvevent;
    _arg.sendevent = sendevent;
    _arg.errevent  = errevent;

    return (int)SYSCALL(SYS_LWIP_TRY_WAKEUP, (void*)&_arg);
}
#endif

