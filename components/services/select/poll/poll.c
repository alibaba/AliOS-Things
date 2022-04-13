/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include "aos/kernel.h"
#include "aos/vfs.h"

#if defined(__ICCARM__) || defined(__CC_ARM)
    #include <sys/select.h>
#endif

#ifdef CONFIG_NO_TCPIP
struct poll_arg {
    aos_sem_t sem;
};

static void vfs_poll_notify(void *fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

static int wait_io(int maxfd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct poll_arg *parg, int timeout)
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
#include <network/network.h>
struct poll_arg {
    int efd;
};

extern int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
extern int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);
extern int lwip_shutdown(int s, int how);
extern int lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
extern int lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
extern int lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
extern int lwip_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
extern int lwip_close(int s);
extern int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);
extern int lwip_listen(int s, int backlog);
extern int lwip_recv(int s, void *mem, size_t len, int flags);
extern int lwip_read(int s, void *mem, size_t len);
extern int lwip_recvfrom(int s, void *mem, size_t len, int flags,
		                         struct sockaddr *from, socklen_t *fromlen);
extern int lwip_send(int s, const void *dataptr, size_t size, int flags);
extern int lwip_sendmsg(int s, const struct msghdr *message, int flags);
extern int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
		                       const struct sockaddr *to, socklen_t tolen);
extern int lwip_socket(int domain, int type, int protocol);
extern int lwip_write(int s, const void *dataptr, size_t size);
extern int lwip_writev(int s, const struct iovec *iov, int iovcnt);
extern int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
		                       struct timeval *timeout);
extern int lwip_ioctl(int s, long cmd, void *argp);
extern int lwip_fcntl(int s, int cmd, int val);
extern int lwip_eventfd(unsigned int initval, int flags);

static void vfs_poll_notify(void *fd, void *arg)
{
    struct poll_arg *parg = arg;
    uint64_t val = 1;
    lwip_write(parg->efd, &val, sizeof val);
}

static int init_parg(struct poll_arg *parg)
{
    int efd;
    efd = lwip_eventfd(0, 0);

    if (efd < 0) {
        errno = EINVAL;
        return -1;
    }

    parg->efd = efd;

    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    close(parg->efd);
}

static int wait_io(int maxfd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct poll_arg *parg, int timeout)
{
    int ret;
    struct timeval tv = {
        .tv_sec  = timeout / 1000,
        .tv_usec = (timeout % 1000) * 1000,
    };

    FD_SET(parg->efd, rfds);
    maxfd = parg->efd > maxfd ? parg->efd : maxfd;
    ret = lwip_select(maxfd + 1, rfds, wfds, efds, timeout >= 0 ? &tv : NULL);

    /* return socketfd event num only ,so we sub event fd num */
    if (ret > 0) {
        if (FD_ISSET(parg->efd, rfds)) {
            ret--;
        }
        if (FD_ISSET(parg->efd, wfds)) {
            errno = EINVAL;
            return -1;
        }
        if (FD_ISSET(parg->efd, efds)) {
            errno = EINVAL;
            return -1;
        }
    }
    return ret;
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
            if (pfd->fd > FD_SETSIZE - 1) {
                errno = EINVAL;
                return -1;
            }
            FD_SET(pfd->fd, rfds);
            maxfd = pfd->fd > maxfd ? pfd->fd : maxfd;
        } else {
            if (aos_do_pollfd(pfd->fd, true, vfs_poll_notify, pfd, parg) == -ENOENT) {
                errno = ENOENT;
                return -1;
            }
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
    fd_set wfds;
    fd_set efds;

    int ret = 0;
    int nset = 0;
    struct poll_arg parg;

    if (init_parg(&parg) < 0) {
        return -1;
    }

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    ret = pre_poll(fds, nfds, &rfds, &parg);

    if (ret < 0) {
        goto check_poll;
    }

    ret = wait_io(ret, &rfds, &wfds, &efds, &parg, timeout);

    if (ret > 0) {
        int i;

        for (i = 0; i < nfds; i++) {
            struct pollfd *pfd = &fds[i];

            if (FD_ISSET(pfd->fd, &rfds)) {
                pfd->revents |= POLLIN;
            }
            if (FD_ISSET(pfd->fd, &wfds)) {
                pfd->revents |= POLLOUT;
            }
            if (FD_ISSET(pfd->fd, &efds)) {
                pfd->revents |= POLLERR;
            }
        }
        nset += ret;
    }

check_poll:
    nset += post_poll(fds, nfds);

    deinit_parg(&parg);

    return ret < 0 ? -1 : nset;
}
