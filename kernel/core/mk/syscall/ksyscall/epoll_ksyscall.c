
#include <string.h>
#include "epoll/epoll.h"
#include "epoll_syscall_arg.h"

int sys_epoll_create1_stub(epoll_create_syscall_arg_t *arg)
{
    return aos_epoll_create1(arg->size);
}

int sys_epoll_ctl_stub(epoll_ctl_syscall_arg_t *arg)
{
    return aos_epoll_ctl(arg->epid, arg->op, arg->sockid, arg->event);
}

int sys_epoll_wait_stub(epoll_wait_syscall_arg_t *arg)
{
    return aos_epoll_wait(arg->epid, arg->events, arg->maxevents, arg->timeout);
}


