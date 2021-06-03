/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include "aos/kernel.h"
#include "aos/vfs.h"
#include "aos/list.h"
#include "sys/socket.h"
#include <select.h>


#define WRITE_VAL 1

#ifndef CONFIG_AOS_LWIP
#define CONFIG_NO_TCPIP
#endif

typedef struct poll_node {
    struct pollfd pfd;
    dlist_t next;
} poll_node_t;

#ifdef CONFIG_NO_TCPIP

struct poll_arg {
    aos_sem_t sem;
};

static void vfs_poll_notify(void *fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}


static int init_parg(struct poll_arg *parg)
{
    aos_sem_new(&parg->sem,  0);
    return 0;
}

static int wait_io(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                   struct poll_arg *parg, struct timeval *timeout)
{
    uint64_t ms = timeout ? (timeout->tv_sec * 1000 + timeout->tv_usec / 1000) : AOS_WAIT_FOREVER;
    aos_sem_wait(&parg->sem, ms);
    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    aos_sem_free(&parg->sem);
}
#else
#include <sys/socket.h>

extern int lwip_write(int s, const void *dataptr, size_t size);
extern int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                       struct timeval *timeout);
extern int lwip_eventfd(unsigned int initval, int flags);
extern int lwip_close(int s);

struct poll_arg {
    int efd;
};

static void vfs_poll_notify(void *fd, void *arg)
{
    struct poll_arg *parg = arg;
    uint64_t val = WRITE_VAL;
    net_write(parg->efd, &val, sizeof val);
}

static int init_parg(struct poll_arg *parg)
{
    int efd;
    efd = net_eventfd(0, 0);
    if (efd < 0) {
        errno = EINVAL;
        return -1;
    }

    parg->efd = efd;

    return 0;
}

static int wait_io(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                   struct poll_arg *parg, struct timeval *timeout)
{
    int ret = 0;
    fd_set *real_rset = readset;
    fd_set bk_rset;
    if (readset == NULL) {
        real_rset = &bk_rset;
        memset(real_rset, 0, sizeof(fd_set));
    }
    FD_SET(parg->efd, real_rset);
    maxfd = parg->efd > maxfd - 1 ? (parg->efd + 1) : maxfd;

    ret = net_select(maxfd, real_rset, writeset, exceptset, timeout);
    if (ret > 0) {
        if (FD_ISSET(parg->efd, real_rset)) {
            /*mask eventfd event to user*/
            ret--;
        }
    }

    FD_CLR(parg->efd, real_rset);
    return ret;
}

static void deinit_parg(struct poll_arg *parg)
{
    net_close(parg->efd);
}
#endif
static int pre_select(int fd, dlist_t *list, struct poll_arg *parg, fd_set *readset, fd_set *writeset,
                      fd_set *exceptset)
{
    poll_node_t *node;
    struct pollfd *pfd = NULL;

    if (readset != NULL && FD_ISSET(fd, readset)) {
        if (fd < aos_vfs_fd_offset_get()) {
            return 0;
        }
        node = aos_malloc(sizeof(poll_node_t));
        if (node == NULL) {
            errno = ENOMEM;
            return -1;
        }
        memset(node, 0, sizeof(poll_node_t));

        pfd = &node->pfd;
        pfd->fd = fd;
        pfd->events = POLLIN;
        dlist_add(&node->next, list);
        /* delete from socket select events */
        FD_CLR(fd, readset);
        if (aos_do_pollfd(fd, true, vfs_poll_notify, pfd, parg) == -ENOENT) {
            return -1;
        }
        /* read\write\except event add one time only, so continue here */
        return 0;
    }

    if (writeset != NULL && FD_ISSET(fd, writeset)) {
        poll_node_t *node;
        if (fd < aos_vfs_fd_offset_get()) {
            return 0;
        }
        node = aos_malloc(sizeof(poll_node_t));
        if (node == NULL) {
            errno = ENOMEM;
            return -1;
        }
        memset(node, 0, sizeof(poll_node_t));

        pfd = &node->pfd;
        pfd->fd = fd;
        pfd->events = POLLOUT;
        dlist_add(&node->next, list);
        FD_CLR(fd, writeset);
        if (aos_do_pollfd(fd, true, vfs_poll_notify, pfd, parg) == -ENOENT) {
            return -1;
        }
        return 0;
    }

    if (exceptset != NULL && FD_ISSET(fd, exceptset)) {
        poll_node_t *node;
        if (fd < aos_vfs_fd_offset_get()) {
            return 0;
        }
        node = aos_malloc(sizeof(poll_node_t));
        if (node == NULL) {
            errno = ENOMEM;
            return -1;
        }
        memset(node, 0, sizeof(poll_node_t));

        pfd = &node->pfd;
        pfd->fd = fd;
        pfd->events = POLLERR;
        dlist_add(&node->next, list);
        FD_CLR(fd, exceptset);
        if (aos_do_pollfd(fd, true, vfs_poll_notify, pfd, parg) == -ENOENT) {
            errno = ENOENT;
            return -1;
        }
        return 0;
    }
    return 0;
}

static int post_select(dlist_t *list, fd_set *readset, fd_set *writeset, fd_set *exceptset)
{
    int ret = 0;
    dlist_t  *temp;
    poll_node_t *node;

    dlist_for_each_entry_safe(list, temp, node, poll_node_t, next) {

        struct pollfd *pfd = &node->pfd;

        if (pfd->fd < aos_vfs_fd_offset_get()) {
            continue;
        }
        /* unregister vfs event */
        if (aos_do_pollfd(pfd->fd, false, NULL, NULL, NULL) == -ENOENT) {
            continue;
        }

        /* change poll event to select event */
        if ((pfd->revents & POLLIN) && (readset != NULL)) {
            FD_SET(pfd->fd, readset);
            ret ++;
        }

        if ((pfd->revents & POLLOUT) && (writeset != NULL)) {
            FD_SET(pfd->fd, writeset);
            ret ++;
        }

        if ((pfd->revents & POLLERR) && (exceptset != NULL)) {
            FD_SET(pfd->fd, exceptset);
            ret ++;
        }
    }
    return ret;
}

int aos_select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
               struct timeval *timeout)
{
    int i;
    int ret = -1;
    int nset = 0;
    struct poll_arg parg;
    dlist_t  *temp;
    poll_node_t *node;
    dlist_t vfs_list;

    dlist_init(&vfs_list);

    if (init_parg(&parg) < 0) {
        goto err;
    }

    for (i = 0; i < maxfd; i++) {
        if (pre_select(i, &vfs_list, &parg, readset, writeset, exceptset) < 0) {
            deinit_parg(&parg);
            goto err;
        }
    }

    ret = wait_io(maxfd, readset, writeset, exceptset,  &parg, timeout);

    nset = ret + post_select(&vfs_list, readset, writeset, exceptset);

    deinit_parg(&parg);
err:
    dlist_for_each_entry_safe(&vfs_list, temp, node, poll_node_t, next) {
        dlist_del(&node->next);
        aos_free(node);
    }

    return ret < 0 ? -1 : nset;
}
