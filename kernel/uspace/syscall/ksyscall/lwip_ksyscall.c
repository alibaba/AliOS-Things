/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>

#if (CONFIG_LWIP_SYSCALL > 0)

#include <lwip/sockets.h>
#include <lwip_syscall_arg.h>

int sys_lwip_accept_stub(void *arg)
{
    lwip_accept_syscall_art_t *_arg = arg;

    return lwip_accept(_arg->s, _arg->addr, _arg->addrlen);
}

int sys_lwip_bind_stub(void *arg)
{
    lwip_bind_syscall_arg_t *_arg = arg;

    return lwip_bind(_arg->s, _arg->name, _arg->namelen);
}

int sys_lwip_shutdown_stub(void *arg)
{
    lwip_shutdown_syscall_arg_t *_arg = arg;

    return lwip_shutdown(_arg->s, _arg->how);
}

int sys_lwip_getpeername_stub (void *arg)
{
    lwip_getpeername_syscall_arg_t *_arg = arg;

    return lwip_getpeername(_arg->s, _arg->name, _arg->namelen);
}

int sys_lwip_getsockname_stub (void *arg)
{
    lwip_getsockname_syscall_arg_t *_arg = arg;

    return lwip_getsockname(_arg->s, _arg->name, _arg->namelen);
}

int sys_lwip_getsockopt_stub (void *arg)
{
    lwip_getsockopt_syscall_arg_t *_arg = arg;

    return lwip_getsockopt(_arg->s, _arg->level, _arg->optname,
                           _arg->optval, _arg->optlen);
}

int sys_lwip_setsockopt_stub (void *arg)
{
    lwip_setsockopt_syscall_arg_t *_arg = arg;

    return lwip_setsockopt(_arg->s, _arg->level, _arg->optname,
                           _arg->optval, _arg->optlen);
}

int sys_lwip_close_stub(void *arg)
{
    lwip_close_syscall_arg_t *_arg = arg;

    return lwip_close(_arg->s);
}

int sys_lwip_connect_stub(void *arg)
{
    lwip_connect_syscall_arg_t *_arg = arg;

    return lwip_connect(_arg->s, _arg->name, _arg->namelen);
}

int sys_lwip_listen_stub(void *arg)
{
    lwip_listen_syscall_arg_t *_arg = arg;

    return lwip_listen(_arg->s, _arg->backlog);
}

int sys_lwip_recv_stub(void *arg)
{
    lwip_recv_syscall_arg_t *_arg = arg;

    return lwip_recv(_arg->s, _arg->mem, _arg->len, _arg->flags);
}

int sys_lwip_read_stub(void *arg)
{
    lwip_read_syscall_arg_t *_arg = arg;

    return lwip_read(_arg->s, _arg->mem, _arg->len);
}

int sys_lwip_recvfrom_stub(void *arg)
{
    lwip_recvfrom_syscall_arg_t *_arg = arg;

    return lwip_recvfrom(_arg->s, _arg->mem, _arg->len, _arg->flags, _arg->from, _arg->fromlen);
}

int sys_lwip_send_stub(void *arg)
{
    lwip_send_syscall_arg_t *_arg = arg;

    return lwip_send(_arg->s, _arg->dataptr, _arg->size, _arg->flags);
}

int sys_lwip_sendmsg_stub(void *arg)
{
    lwip_sendmsg_syscall_arg_t *_arg = arg;

    return lwip_sendmsg(_arg->s, _arg->message, _arg->flags);
}

int sys_lwip_sendto_stub(void *arg)
{
    lwip_sendto_syscall_arg_t *_arg = arg;

    return lwip_sendto(_arg->s, _arg->dataptr, _arg->size, _arg->flags, _arg->to, _arg->tolen);
}

int sys_lwip_socket(void *arg)
{
    lwip_socket_syscall_arg_t *_arg = arg;

    return lwip_socket(_arg->domain, _arg->type, _arg->protocol);
}

int sys_lwip_write_stub(void *arg)
{
    lwip_write_syscall_arg_t *_arg = arg;

    return lwip_write(_arg->s, _arg->dataptr, _arg->size);
}

int sys_lwip_writev_stub(void *arg)
{
    lwip_writev_syscall_arg_t *_arg = arg;

    return lwip_writev(_arg->s, _arg->iov, _arg->iovcnt);
}

int sys_lwip_select_stub(void *arg)
{
    lwip_select_syscall_arg_t *_arg = arg;

    return lwip_select(_arg->maxfdp1, _arg->readset, _arg->writeset, _arg->exceptset, _arg->timeout);
}

int sys_lwip_ioctl_stub(void *arg)
{
    lwip_ioctl_syscall_arg_t *_arg = arg;

    return lwip_ioctl(_arg->s, _arg->cmd, _arg->argp);
}

int sys_lwip_fcntl_stub(void *arg)
{
    lwip_fcntl_syscall_arg_t *_arg = arg;

    return lwip_fcntl(_arg->s, _arg->cmd, _arg->val);
}

int sys_lwip_eventfd_stub(void *arg)
{
    lwip_eventfd_syscall_arg_t *_arg = arg;

    return lwip_eventfd(_arg->initval, _arg->flags);
}

#if LWIP_PACKET
int sys_lwip_try_wakeup_stub(void *arg)
{
    lwip_try_wakeup_syscall_arg_t *_arg = arg;

    return lwip_try_wakeup(_arg->s, _arg->rcvevent, _arg->sendevent, _arg->errevent);
}
#endif

#endif / *CONFIG_LWIP_SYSCALL */

