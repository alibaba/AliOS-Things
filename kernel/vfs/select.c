/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/network.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_inode.h>
#include <vfs.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <vfs_file.h>

#ifdef __ICCARM__
#include <sys/select.h>
#endif

#ifdef IO_NEED_TRAP
#include <vfs_trap.h>
#endif

extern aos_mutex_t g_vfs_mutex;

#if (AOS_CONFIG_VFS_POLL_SUPPORT>0)

#if !defined(WITH_LWIP) && !defined(WITH_SAL)&& defined(VCALL_RHINO)
#define NEED_WAIT_IO
#endif

#include <aos/network.h>

#ifdef CONFIG_NO_TCPIP

struct poll_arg {
    aos_sem_t sem;
};

static void setup_fd(int fd)
{
}

static void teardown_fd(int fd)
{
}

static void vfs_poll_notify(struct pollfd *fd, void *arg)
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
#elif defined(NEED_WAIT_IO)

#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

struct poll_arg {
    aos_sem_t sem;
};

static void setup_fd(int fd)
{
    int f = fcntl(fd, F_GETFL) | O_ASYNC;
    if (fcntl(fd, F_SETFL, f) < 0) {
        perror("fcntl setup");
    }
    if (fcntl(fd, F_SETOWN, gettid()) < 0) {
        perror("fcntl setown");
    }
}

static void teardown_fd(int fd)
{
    int f = fcntl(fd, F_GETFL) & ~O_ASYNC;
    if (fcntl(fd, F_SETFL, f) < 0) {
        perror("fcntl teardown");
    }
}

static int wait_io(int maxfd, fd_set *rfds, struct poll_arg *parg, int timeout)
{
    struct timeval tv = { 0 };
    int ret;
    fd_set saved_fds = *rfds;

    /* check if already data available */
    ret = select(maxfd + 1, rfds, NULL, NULL, &tv);
    if (ret > 0) {
        return ret;
    }

    timeout = timeout >= 0 ? timeout : AOS_WAIT_FOREVER;
    ret = aos_sem_wait(&parg->sem, timeout);
    if (ret != VFS_SUCCESS) {
        return 0;
    }

    *rfds = saved_fds;
    ret = select(maxfd + 1, rfds, NULL, NULL, &tv);
    return ret;
}

static void vfs_poll_notify(struct pollfd *fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

static void vfs_io_cb(int fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

void cpu_io_register(void (*f)(int, void *), void *arg);
void cpu_io_unregister(void (*f)(int, void *), void *arg);
static int init_parg(struct poll_arg *parg)
{
    cpu_io_register(vfs_io_cb, parg);
    aos_sem_new(&parg->sem,  0);
    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    aos_sem_free(&parg->sem);
    cpu_io_unregister(vfs_io_cb, parg);
}

#else
struct poll_arg {
    int efd;
};

static void vfs_poll_notify(struct pollfd *fd, void *arg)
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
        file_t  *f;
        struct pollfd *pfd = &fds[i];

        if (pfd->fd < AOS_CONFIG_VFS_FD_OFFSET) {
            setup_fd(pfd->fd);
            FD_SET(pfd->fd, rfds);
            maxfd = pfd->fd > maxfd ? pfd->fd : maxfd;
            continue;
        }

        f = get_file(pfd->fd);

        if (f == NULL) {
            return -1;
        }

        pfd = &fds[i];
        (f->node->ops.i_ops->poll)(f, true, vfs_poll_notify, pfd, parg);
    }

    return maxfd;
}

static int post_poll(struct pollfd *fds, int nfds)
{
    int j;
    int ret = 0;

    for (j = 0; j < nfds; j++) {
        file_t  *f;
        struct pollfd *pfd = &fds[j];

        if (pfd->fd < AOS_CONFIG_VFS_FD_OFFSET) {
            teardown_fd(pfd->fd);
            continue;
        }


        f = get_file(pfd->fd);

        if (f == NULL) {
            continue;
        }

        (f->node->ops.i_ops->poll)(f, false, NULL, NULL, NULL);

        if (pfd->revents) {
            ret ++;
        }
    }

    return ret;
}

int aos_poll(struct pollfd *fds, int nfds, int timeout)
{
    fd_set rfds;

    int ret = VFS_SUCCESS;
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
#endif

int aos_fcntl(int fd, int cmd, int val)
{
    if (fd < 0) {
        return -EINVAL;
    }

    if (fd < AOS_CONFIG_VFS_FD_OFFSET) {
        #ifdef IO_NEED_TRAP
            return trap_fcntl(fd, cmd, val);
        #else
            return -ENOENT;
        #endif
    }

    return 0;
}

int aos_ioctl_in_loop(int cmd, unsigned long arg)
{
    int      err;
    int      fd;

    for (fd = AOS_CONFIG_VFS_FD_OFFSET;
         fd < AOS_CONFIG_VFS_FD_OFFSET + AOS_CONFIG_VFS_DEV_NODES; fd++) {
        file_t  *f;
        inode_t *node;

        if ((err = aos_mutex_lock(&g_vfs_mutex, AOS_WAIT_FOREVER)) != 0) {
            return err;
        }

        f = get_file(fd);

        if (f == NULL) {
            aos_mutex_unlock(&g_vfs_mutex);
            return -ENOENT;
        }

        if ((err = aos_mutex_unlock(&g_vfs_mutex)) != 0) {
            return err;
        }

        node = f->node;

        if ((node->ops.i_ops->ioctl) != NULL) {
            err = (node->ops.i_ops->ioctl)(f, cmd, arg);

            if (err != VFS_SUCCESS) {
                return err;
            }
        }
    }

    return VFS_SUCCESS;
}
