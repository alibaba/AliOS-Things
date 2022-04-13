/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef EPOLL_KSYSCALL_H
#define EPOLL_KSYSCALL_H

#include "epoll_syscall_arg.h"

int sys_epoll_create1_stub(epoll_create_syscall_arg_t *arg);

int sys_epoll_ctl_stub(epoll_ctl_syscall_arg_t *arg);

int sys_epoll_wait_stub(epoll_wait_syscall_arg_t *arg);

#endif /* EPOLL_KSYSCALL_H */

