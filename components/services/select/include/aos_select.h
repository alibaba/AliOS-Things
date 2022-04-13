
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_SELECT_H
#define AOS_SELECT_H

#include <poll.h>

int aos_poll(struct pollfd *fds, int nfds, int timeout);
int aos_select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);

#endif
