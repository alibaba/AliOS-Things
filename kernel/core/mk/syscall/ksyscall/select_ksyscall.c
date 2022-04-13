/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "select_syscall_arg.h"
#include "aos/kernel.h"

extern int aos_poll(struct pollfd *fds, int nfds, int timeout);
extern int aos_select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                      struct timeval *timeout);

int sys_aos_poll_stub(void *arg)
{

    aos_poll_syscall_arg_t *_arg = arg;
    return aos_poll(_arg->fds, _arg->nfds, _arg->timeout);
}

int sys_aos_select_stub(void *arg)
{
    aos_select_syscall_arg_t *_arg = arg;
    return aos_select(_arg->maxfd, _arg->readset, _arg->writeset, _arg->exceptset, _arg->timeout);
}