/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AOS_MP_SYSCALL_H
#define AOS_MP_SYSCALL_H

#include "aos/vfs.h"

typedef struct {
    struct pollfd *fds;
    int nfds;
    int timeout;
} aos_poll_syscall_arg_t;

typedef struct {
    int maxfd;
    fd_set *readset;
    fd_set *writeset;
    fd_set *exceptset;
    struct timeval *timeout;
} aos_select_syscall_arg_t;

#endif /* AOS_MP_SYSCALL_H */

