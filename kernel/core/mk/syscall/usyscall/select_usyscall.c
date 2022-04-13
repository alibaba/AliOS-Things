/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "syscall.h"
#include "syscall_no.h"
#include "select_syscall_arg.h"
#include <poll.h>

int aos_poll(struct pollfd *fds, int nfds, int timeout)
{
    aos_poll_syscall_arg_t arg;
    arg.fds = fds;
    arg.nfds = nfds;
    arg.timeout = timeout;

    return (int)SYSCALL(SYS_AOS_POLL, &arg);
}

int u_aos_select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
               struct timeval *timeout)
{
    aos_select_syscall_arg_t arg;
    arg.maxfd = maxfd;
    arg.readset = readset;
    arg.writeset = writeset;
    arg.exceptset = exceptset;
    arg.timeout = timeout;

    return (int)SYSCALL(SYS_AOS_SELECT, &arg);
}