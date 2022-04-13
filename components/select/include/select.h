/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_SELECT_H
#define AOS_SELECT_H

#include <poll.h>

#undef  POLLIN
#define POLLIN     0x001
#undef  POLLOUT
#define POLLOUT    0x004
#undef  POLLERR
#define POLLERR    0x008

int aos_poll(struct pollfd *fds, int nfds, int timeout);
int aos_select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);

#endif
