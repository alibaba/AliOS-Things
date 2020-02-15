/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "aos/vfs.h"

#include "aos/yloop.h"

#include <network/network.h>

#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#if defined(__ICCARM__) || defined(__CC_ARM)
#include <sys/select.h>
#endif

#ifdef IO_NEED_TRAP
#include <vfs_trap.h>
#endif

#ifdef CONFIG_NO_LWIP

struct poll_arg {
    aos_sem_t sem;
};

static void setup_fd(int fd)
{
}

static void teardown_fd(int fd)
{
}

static void vfs_poll_notify(void *fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

static int wait_io(int maxfd, fd_set *rfds, struct poll_arg *parg, int timeout)
{
    timeout = timeout >= 0 ? timeout : AOS_WAIT_FOREVER;
    aos_sem_wait(&parg->sem, timeout);
    return 0;
}

static int init_parg(struct poll_arg *parg)
{
    aos_sem_new(&parg->sem,  0);
    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    aos_sem_free(&parg->sem);
}
#else
struct poll_arg {
    int efd;
};

static void vfs_poll_notify(void *fd, void *arg)
{
    struct poll_arg *parg = arg;
    uint64_t val = 1;
    write(parg->efd, &val, sizeof val);
}

static void setup_fd(int fd)
{
}

static void teardown_fd(int fd)
{
}

static int init_parg(struct poll_arg *parg)
{
    int efd;
    efd = eventfd(0, 0);

    if (efd < 0) {
        return -1;
    }

    parg->efd = efd;

    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    close(parg->efd);
}

static int wait_io(int maxfd, fd_set *rfds, struct poll_arg *parg, int timeout)
{
    struct timeval tv = {
        .tv_sec  = timeout / 1024,
        .tv_usec = (timeout % 1024) * 1024,
    };

    FD_SET(parg->efd, rfds);
    maxfd = parg->efd > maxfd ? parg->efd : maxfd;
    return select(maxfd + 1, rfds, NULL, NULL, timeout >= 0 ? &tv : NULL);
}
#endif

static int pre_poll(struct pollfd *fds, int nfds, fd_set *rfds, void *parg)
{
    int i;
    int maxfd = 0;

    for (i = 0; i < nfds; i++) {
        struct pollfd *pfd = &fds[i];

        pfd->revents = 0;
    }

    for (i = 0; i < nfds; i++) {
        struct pollfd *pfd = &fds[i];

        if (pfd->fd < aos_vfs_fd_offset_get()) {
            setup_fd(pfd->fd);
            FD_SET(pfd->fd, rfds);
            maxfd = pfd->fd > maxfd ? pfd->fd : maxfd;
            continue;
        }

        if (aos_do_pollfd(pfd->fd, true, vfs_poll_notify, pfd, parg) == -ENOENT) {
            return -1;
        }
    }

    return maxfd;
}

static int post_poll(struct pollfd *fds, int nfds)
{
    int j;
    int ret = 0;

    for (j = 0; j < nfds; j++) {
        struct pollfd *pfd = &fds[j];

        if (pfd->fd < aos_vfs_fd_offset_get()) {
            teardown_fd(pfd->fd);
            continue;
        }

        if (aos_do_pollfd(pfd->fd, false, NULL, NULL, NULL) == -ENOENT) {
            continue;
        }

        if (pfd->revents) {
            ret ++;
        }
    }

    return ret;
}

int aos_poll(struct pollfd *fds, int nfds, int timeout)
{
    fd_set rfds;

    int ret = 0;
    int nset = 0;
    struct poll_arg parg;

    if (init_parg(&parg) < 0) {
        return -1;
    }

    FD_ZERO(&rfds);
    ret = pre_poll(fds, nfds, &rfds, &parg);

    if (ret < 0) {
        goto check_poll;
    }

    ret = wait_io(ret, &rfds, &parg, timeout);

    if (ret >= 0) {
        int i;

        for (i = 0; i < nfds; i++) {
            struct pollfd *pfd = fds + i;

            if (FD_ISSET(pfd->fd, &rfds)) {
                pfd->revents |= POLLIN;
            }
        }

        nset += ret;
    }

check_poll:
    nset += post_poll(fds, nfds);

    deinit_parg(&parg);

    return ret < 0 ? 0 : nset;
}

int aos_fcntl(int fd, int cmd, int val)
{
    if (fd < 0) {
        return -EINVAL;
    }

    if (fd < aos_vfs_fd_offset_get()) {
#ifdef IO_NEED_TRAP
        return trap_fcntl(fd, cmd, val);
#else
        return -ENOENT;
#endif
    }

    return 0;
}
