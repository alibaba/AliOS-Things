/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_LWIP_SYSCALL > 0)

#include <string.h>
#include "k_api.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip_syscall_arg.h"
#include "socket_fd.h"

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
    int ret;

    ktask_t                  *cur_task;
    lwip_close_syscall_arg_t *_arg = arg;

    cur_task = krhino_cur_task_get();
    ret = lwip_close(_arg->s);
    if (!ret) {
        socket_fd_dec_ref(cur_task->pid, _arg->s);
    }

    return ret;
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

    return lwip_recvfrom(_arg->s, _arg->mem, _arg->len, _arg->flags,
                         _arg->from, _arg->fromlen);
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

    return lwip_sendto(_arg->s, _arg->dataptr, _arg->size,
                       _arg->flags, _arg->to, _arg->tolen);
}

int sys_lwip_socket(void *arg)
{
    int ret;
    ktask_t *cur_task;
    lwip_socket_syscall_arg_t *_arg = arg;

    cur_task = krhino_cur_task_get();

    ret = lwip_socket(_arg->domain, _arg->type, _arg->protocol);
    if (ret >= 0) {
        socket_fd_inc_ref(cur_task->pid, ret);
    }

    return ret;
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

    return lwip_select(_arg->maxfdp1, _arg->readset, _arg->writeset,
                       _arg->exceptset, _arg->timeout);
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

int sys_lwip_gethostbyname_stub(void *arg)
{
    struct hostent *hostent;

    lwip_gethostbyname_syscall_arg_t *_arg = arg;

    hostent = lwip_gethostbyname(_arg->name);

    memcpy((void*)_arg->hostent_addr, (const void*)hostent->h_addr_list[0],
            sizeof(ip_addr_t));

    strncpy(_arg->hostname, hostent->h_name, DNS_MAX_NAME_LENGTH);
    _arg->hostname[DNS_MAX_NAME_LENGTH] = 0;

    _arg->aliases = NULL;

    return 0;
}

int sys_lwip_gethostbyname_r_stub(void *arg)
{
    lwip_gethostbyname_r_syscall_arg_t *_arg = arg;

    char *hostname;
    char *temp_buf;
    int   ret;

    struct helper {
        ip_addr_t *addr_list[2];
        ip_addr_t  addr;
        char      *aliases;
    } *h;

    temp_buf = (char*)krhino_mm_alloc(_arg->buflen);
    if (NULL == temp_buf) {
        return -1;
    }

    ret = lwip_gethostbyname_r(_arg->name, _arg->ret, temp_buf, _arg->buflen,
                                _arg->result, _arg->h_errnop);

    memcpy(_arg->buf, temp_buf, _arg->buflen);
    krhino_mm_free(temp_buf);

    h = (struct helper*)LWIP_MEM_ALIGN(_arg->buf);
    hostname = ((char*)h) + sizeof(struct helper);
    h->addr_list[0] = &h->addr;
    h->addr_list[1] = NULL;
    h->aliases = NULL;
    _arg->ret->h_name = hostname;
    _arg->ret->h_aliases = &h->aliases;
    _arg->ret->h_addrtype = AF_INET;
    _arg->ret->h_length = sizeof(ip_addr_t);
    _arg->ret->h_addr_list = (char**)&h->addr_list;

    *(_arg->result) = _arg->ret;

    return ret;
}

void sys_lwip_freeaddrinfo_stub(void *arg)
{
    return;
}

int sys_lwip_getaddrinfo_stub(void *arg)
{
    lwip_getaddrinfo_syscall_arg_t *_arg = arg;

    struct addrinfo *ai;
    int              ret;

    ret = lwip_getaddrinfo(_arg->nodename, _arg->servname, _arg->hints, &ai);

    memcpy((void*)_arg->ai, (const void*)ai, sizeof(struct addrinfo));
    _arg->ai->ai_next = NULL;

    memcpy((void*)_arg->sa, (const void*)ai->ai_addr, sizeof(struct sockaddr));

    lwip_freeaddrinfo(ai);

    return ret;
}

#endif /* RHINO_CONFIG_LWIP_SYSCALL */

