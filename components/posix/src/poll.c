/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <poll.h>

extern int aos_poll(struct pollfd *fds, int nfds, int timeout);
int poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
    return aos_poll(fds, nfds, timeout);
}
