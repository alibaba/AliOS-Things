/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "sys/types.h"
#include "syscall.h"
#include "epoll_syscall_arg.h"
#include "syscall_no.h"

int aos_epoll_create(int size)
{
    epoll_create_syscall_arg_t arg;
    arg.size = 0;
    return (int)SYSCALL(SYS_EPOLL_CREATE1, &arg);
}

int aos_epoll_create1(int size)
{
    epoll_create_syscall_arg_t arg;
    arg.size = size;
    return (int)SYSCALL(SYS_EPOLL_CREATE1, &arg);
}

int aos_epoll_ctl(int epid, int op, int sockid, struct epoll_event *event)
{
    epoll_ctl_syscall_arg_t arg;

    arg.epid = epid;
    arg.op = op;
    arg.sockid = sockid;
    arg.event = event;
    return (int)SYSCALL(SYS_EPOLL_CTL, &arg);
}

int aos_epoll_wait(int epid, struct epoll_event *events, int maxevents, int timeout)
{
    epoll_wait_syscall_arg_t arg;

    arg.epid = epid;
    arg.events = events;
    arg.maxevents = maxevents;
    arg.timeout = timeout;
    return (int)SYSCALL(SYS_EPOLL_WAIT, &arg);
}
