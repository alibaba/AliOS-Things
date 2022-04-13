/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <sys/select.h>

extern int aos_select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                         struct timeval *timeout)
{
    return aos_select(nfds, readfds, writefds, errorfds, timeout);
}
