/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/kernel.h"
#include "fs/vfs_types.h"
#include "fs/vfs_api.h"
#include "network_syscall_arg.h"
#include "socketfd.h"

int sys_network_accept_stub(void *arg)
{
    network_accept_syscall_art_t *_arg = arg;

    return vfs_accept(_arg->path, _arg->s, _arg->addr, _arg->addrlen);
}

int sys_network_bind_stub(void *arg)
{
    network_bind_syscall_arg_t *_arg = arg;

    return vfs_bind(_arg->s, _arg->name, _arg->namelen);
}

int sys_network_shutdown_stub(void *arg)
{
    network_shutdown_syscall_arg_t *_arg = arg;

    return vfs_shutdown(_arg->s, _arg->how);
}

int sys_network_getpeername_stub(void *arg)
{
    network_getpeername_syscall_arg_t *_arg = arg;

    return vfs_getpeername(_arg->s, _arg->name, _arg->namelen);
}

int sys_network_getsockname_stub(void *arg)
{
    network_getsockname_syscall_arg_t *_arg = arg;

    return vfs_getsockname(_arg->s, _arg->name, _arg->namelen);
}

int sys_network_getsockopt_stub(void *arg)
{
    network_getsockopt_syscall_arg_t *_arg = arg;

    return vfs_getsockopt(_arg->s, _arg->level, _arg->optname,
                           _arg->optval, _arg->optlen);
}

int sys_network_setsockopt_stub(void *arg)
{
    network_setsockopt_syscall_arg_t *_arg = arg;

    return vfs_setsockopt(_arg->s, _arg->level, _arg->optname,
                           _arg->optval, _arg->optlen);
}

extern int socketfd_deref(int pid, int fd);
int sys_network_close_stub(void *arg)
{
    int ret;

    ktask_t                  *cur_task;
    network_close_syscall_arg_t *_arg = arg;

    cur_task = krhino_cur_task_get();
    ret = vfs_close(_arg->s);
    if (!ret) {
        socketfd_deref(cur_task->pid, _arg->s);
    }

    return ret;
}

int sys_network_connect_stub(void *arg)
{
    network_connect_syscall_arg_t *_arg = arg;

    return vfs_connect(_arg->s, _arg->name, _arg->namelen);
}

int sys_network_listen_stub(void *arg)
{
    network_listen_syscall_arg_t *_arg = arg;

    return vfs_listen(_arg->s, _arg->backlog);
}

int sys_network_recv_stub(void *arg)
{
    network_recv_syscall_arg_t *_arg = arg;

    return vfs_recv(_arg->s, _arg->mem, _arg->len, _arg->flags);
}

int sys_network_read_stub(void *arg)
{
    network_read_syscall_arg_t *_arg = arg;

    return vfs_read(_arg->s, _arg->mem, _arg->len);
}

int sys_network_recvfrom_stub(void *arg)
{
    network_recvfrom_syscall_arg_t *_arg = arg;

    return vfs_recvfrom(_arg->s, _arg->mem, _arg->len, _arg->flags,
                         _arg->from, _arg->fromlen);
}

int sys_network_send_stub(void *arg)
{
    network_send_syscall_arg_t *_arg = arg;

    return vfs_send(_arg->s, _arg->dataptr, _arg->size, _arg->flags);
}

int sys_network_sendmsg_stub(void *arg)
{
    network_sendmsg_syscall_arg_t *_arg = arg;

    return vfs_sendmsg(_arg->s, _arg->message, _arg->flags);
}

int sys_network_sendto_stub(void *arg)
{
    network_sendto_syscall_arg_t *_arg = arg;

    return vfs_sendto(_arg->s, _arg->dataptr, _arg->size,
                       _arg->flags, _arg->to, _arg->tolen);
}

extern int socketfd_ref(int pid, int fd);
int sys_network_socket(void *arg)
{
    int ret;
    ktask_t *cur_task;
    network_socket_syscall_arg_t *_arg = arg;

    cur_task = krhino_cur_task_get();

    ret = vfs_socket(_arg->path, _arg->domain, _arg->type, _arg->protocol);
    if (ret >= 0) {
        socketfd_ref(cur_task->pid, ret);
    }

    return ret;
}

int sys_network_write_stub(void *arg)
{
    network_write_syscall_arg_t *_arg = arg;

    return vfs_write(_arg->s, _arg->dataptr, _arg->size);
}

int sys_network_writev_stub(void *arg)
{
    network_writev_syscall_arg_t *_arg = arg;

    return vfs_writev(_arg->s, _arg->iov, _arg->iovcnt);
}

int sys_network_select_stub(void *arg)
{
    // TODO
    return -1;
}

int sys_network_ioctl_stub(void *arg)
{
    network_ioctl_syscall_arg_t *_arg = arg;

    return vfs_ioctl(_arg->s, _arg->cmd, _arg->arg);
}

int sys_network_fcntl_stub(void *arg)
{
    network_fcntl_syscall_arg_t *_arg = arg;

    return vfs_fcntl(_arg->s, _arg->cmd, _arg->val);
}

int sys_network_eventfd_stub(void *arg)
{
    network_eventfd_syscall_arg_t *_arg = arg;

    return vfs_eventfd(_arg->initval, _arg->flags);
}

int sys_network_try_wakeup_stub(void *arg)
{
    network_try_wakeup_syscall_arg_t *_arg = arg;

    return vfs_try_wakeup(_arg->s, _arg->rcvevent, _arg->sendevent, _arg->errevent);
}

int sys_network_gethostbyname_stub(void *arg)
{
    // TODO
    return -1;
}

int sys_network_gethostbyname_r_stub(void *arg)
{
    // TODO
    return -1;
}

void sys_network_freeaddrinfo_stub(void *arg)
{
    // TODO
    return;
}

int sys_network_getaddrinfo_stub(void *arg)
{
    // TODO
    return -1;
}
