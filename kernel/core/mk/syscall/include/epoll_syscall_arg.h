/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef EPOLL_SYSCALL_H
#define EPOLL_SYSCALL_H

#include "epoll/epoll.h"

typedef struct {
    int size;
} epoll_create_syscall_arg_t;

typedef struct {
    int epid;
    int op;
    int sockid;
    struct epoll_event *event;
} epoll_ctl_syscall_arg_t;


typedef struct {
    int epid;
    struct epoll_event *events;
    int maxevents;
    int timeout;
} epoll_wait_syscall_arg_t;

#endif /* EPOLL_SYSCALL_H */

