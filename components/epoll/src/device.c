/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include <aos/vfs.h>
#include "epoll.h"
#include "private/epoll_inner.h"
#include "private/rbtree_wrapper.h"

#define   EVENT_FLAG   0x01

static inline uint16_t epoll2poll(uint32_t in)
{
    return (uint16_t)(in & 0xffff);
}

static inline uint32_t poll2epoll(uint16_t in)
{
    return (uint32_t)(in & 0xffff);
}

typedef void (*epoll_event_cb_t)(uint32_t revent, void *data);

#if CONFIG_EPOLL_LWIP
    extern int lwip_set_epoll_callback(int fd, epoll_event_cb_t cb, void *data);
#endif

void lwip_epoll_notify(uint32_t revent, void *arg)
{
    epoll_dev_t *pdev = NULL;
    epoll_item_t *item = (epoll_item_t *)arg;

    if (arg == NULL) {
        return;
    }

    pdev = item->epoll;
    if (pdev == NULL) {
        return;
    }

    aos_mutex_lock(&pdev->evt_mutex, AOS_WAIT_FOREVER);
    item->revents = revent;
    if ((revent & (~EP_PRIVATE_BITS)) & item->event.events) {
        if (item->rdy == 0) {
            dlist_add(&item->link_node, &pdev->ready_list);
            item->rdy = 1;
        }
        aos_event_set(&pdev->event, EVENT_FLAG, AOS_EVENT_OR_CLEAR);
    } else { /*clean events*/
        if (item->rdy == 1) {
            dlist_del(&item->link_node);
            item->rdy = 0;
        }
    }
    aos_mutex_unlock(&pdev->evt_mutex);
}

static void vfs_epoll_notify(void *data, void *arg)
{
    struct pollfd *pfd = (struct pollfd *)data;
    epoll_dev_t *pdev = NULL;
    epoll_item_t *item = (epoll_item_t *)arg;

    if (arg == NULL || data == NULL) {
        return;
    }

    pdev = item->epoll;
    if (pdev == NULL) {
        return;
    }

    aos_mutex_lock(&pdev->evt_mutex, AOS_WAIT_FOREVER);
    if (poll2epoll(pfd->revents) & item->event.events) {
        item->revents = poll2epoll(pfd->revents);
        if (item->rdy == 0) {
            dlist_add(&item->link_node, &pdev->ready_list);
            item->rdy = 1;
        }
        aos_event_set(&pdev->event, EVENT_FLAG, AOS_EVENT_OR_CLEAR);
    }
    aos_mutex_unlock(&pdev->evt_mutex);
}

static int epoll_add_event_cb(epoll_item_t *item)
{
    int ret = 0;
    /* register lwip cb */
    if (item->fd < aos_vfs_fd_offset_get()) {
#if CONFIG_EPOLL_LWIP
        ret = lwip_set_epoll_callback(item->fd, lwip_epoll_notify, item);
        if (ret < 0) {
            ret = -EBADF;
        }
#endif
    } else { /*register vfs cb */
        ret = aos_do_pollfd(item->fd, true, vfs_epoll_notify, &item->compat_evt, item);
        if (ret < 0) {
            if (ret == -1) {
                ret = -EBADF;
            }
        }
    }
    return ret;
}

static int epoll_delete_event_cb(int fd)
{
    int ret = 0;

    if (fd < aos_vfs_fd_offset_get()) { // register lwip cb
#if CONFIG_EPOLL_LWIP
        ret = lwip_set_epoll_callback(fd, NULL, NULL);
        if (ret < 0) {
            errno = EBADF;
            return -1;
        }
#endif
    } else {  /* unregister vfs cb */
        ret = aos_do_pollfd(fd, false, NULL, NULL, NULL);
        if (ret < 0) {
            if (ret < -1) {
                errno = -ret;
            }
            return -1;
        }
    }
    return ret;
}

static int epoll_open(inode_t *node, file_t *file)
{
    int ret = -1;
    epoll_dev_t *pdev = malloc(sizeof(epoll_dev_t));
    if (pdev == NULL) {
        printf("epoll malloc failed\r\n");
        errno = ENOMEM;
        return -1;
    }
    memset(pdev, 0, sizeof(epoll_dev_t));

    ret = aos_mutex_new(&pdev->rbt_mutex);
    if (ret < 0) {
        goto err;
    }
    ret = aos_mutex_new(&pdev->evt_mutex);
    if (ret < 0) {
        goto err;
    }
    ret = aos_event_new(&pdev->event, 0);

    if (ret < 0) {
        goto err;
    }

    pdev->rb_tree = RBT_ROOT;
    dlist_init(&pdev->ready_list);

    file->f_arg = pdev;
    return 0;
err:
    printf("epoll open failed\r\n");
    errno = ENOMEM;
    aos_mutex_free(&pdev->rbt_mutex);
    aos_mutex_free(&pdev->evt_mutex);
    aos_event_free(&pdev->event);

    file->f_arg = NULL;
    free(pdev);
    return -1;
}


static int epoll_close(file_t *file)
{
    epoll_dev_t *pdev = file->f_arg;
    epoll_item_t *node, *rb_tmp;
    dlist_t *tmp;

    if (pdev == NULL) {
        return 0;
    }

    aos_mutex_lock(&pdev->rbt_mutex, AOS_WAIT_FOREVER);
    dlist_for_each_entry_safe(&pdev->ready_list, tmp, node, epoll_item_t, link_node) {
        dlist_del(&node->link_node);
    }

    k_rbtree_postorder_for_each_entry_safe(node, rb_tmp, &pdev->rb_tree, tree_node) {
        epoll_delete_event_cb(node->fd);
        k_rbtree_erase(&node->tree_node, &pdev->rb_tree);
        free(node);
    }

    aos_mutex_unlock(&pdev->rbt_mutex);

    aos_mutex_free(&pdev->rbt_mutex);
    aos_mutex_free(&pdev->evt_mutex);
    aos_event_free(&pdev->event);

    free(pdev);
    file->f_arg = NULL;
    return 0;
}

static int epoll_set_item(epoll_item_t *item, epoll_dev_t *dev, epoll_ctl_data_t *data)
{
    if (data->event == NULL) {
        return -1;
    }
    item->fd = data->fd;
    item->rdy = 0;
    memcpy(&item->event, data->event, sizeof(struct epoll_event));
    item->epoll = dev;
    /*compat to vfs poll*/
    item->compat_evt.fd = data->fd;
    item->compat_evt.events = (uint16_t) data->event->events;
    return 0;
}
static int epoll_control(epoll_dev_t *pdev, epoll_ctl_data_t *data)
{
    int ret = -1;
    epoll_item_t *item = NULL;
    aos_mutex_lock(&pdev->rbt_mutex, AOS_WAIT_FOREVER);

    switch (data->op) {
        case EPOLL_CTL_ADD: {
            item = rbr_find(&pdev->rb_tree, data->fd);
            if (item != NULL) {
                printf("epoll:fd:%d already added\r\n", data->fd);
                errno = EEXIST;
                break;
            }
            item = malloc(sizeof(epoll_item_t));
            if (item == NULL) {
                errno = ENOMEM;
                break;
            }
            memset(item, 0, sizeof(epoll_item_t));
            if (epoll_set_item(item, pdev, data) < 0) {
                errno = EINVAL;
                free(item);
                break;
            }
            ret = rbt_insert(&pdev->rb_tree, item);
            if (ret < 0) {
                errno = EINVAL;
                free(item);
                break;
            }

            ret = epoll_add_event_cb(item);
            if (ret < 0) {
                rbt_delete(&pdev->rb_tree, data->fd);
                errno = -ret;
                printf("add cb failed \r\n");
                ret = -1;
            }
        }
        break;
        case EPOLL_CTL_DEL: {
            epoll_item_t *node;
            dlist_t *tmp;
            epoll_delete_event_cb(data->fd);

            dlist_for_each_entry_safe(&pdev->ready_list, tmp, node, epoll_item_t, link_node) {
                if (node->fd == data->fd) {
                    dlist_del(&node->link_node);
                }
            }
            ret = rbt_delete(&pdev->rb_tree, data->fd);
            if (ret < 0) {
                errno = ENOENT;
            }
        }
        break;
        case EPOLL_CTL_MOD: {
            if (data->event == NULL) {
                errno = EINVAL;
                break;
            }
            item = rbr_find(&pdev->rb_tree, data->fd);
            if (item == NULL) {
                errno = ENOENT;
                printf("epoll:fd:%d does not exist\r\n", data->fd);
                break;
            }
            aos_mutex_lock(&pdev->evt_mutex, AOS_WAIT_FOREVER);
            memcpy(&item->event, data->event, sizeof(struct epoll_event));
            aos_mutex_unlock(&pdev->evt_mutex);
            ret = 0;
        }
        break;
    }
    aos_mutex_unlock(&pdev->rbt_mutex);
    return ret;
}

static int epoll_waitting(epoll_dev_t *pdev, epoll_wait_data_t *data)
{
    epoll_item_t *node;
    dlist_t *tmp;
    int i = 0;
    int ret = 0;
    uint32_t real_flag = 0;
    aos_mutex_lock(&pdev->rbt_mutex, AOS_WAIT_FOREVER);
    dlist_for_each_entry_safe(&pdev->ready_list, tmp, node, epoll_item_t, link_node) {


        if (node->fd < aos_vfs_fd_offset_get()) {

        } else {
            aos_mutex_lock(&pdev->evt_mutex, AOS_WAIT_FOREVER);
            node->revents = 0;
            aos_mutex_unlock(&pdev->evt_mutex);
            ret = aos_do_pollfd(node->fd, true, vfs_epoll_notify, &node->compat_evt, node);
            if (ret < 0) {
                printf("aos_do_pollfd err, node->fd = %d,ret = %d\r\n", node->fd, ret);
                if (ret < -1) {
                    errno = -ret;
                }
                ret = -1;
                break;
            }
        }

        aos_mutex_lock(&pdev->evt_mutex, AOS_WAIT_FOREVER);
        if (!(node->event.events & node->revents)) {
            dlist_del(&node->link_node);
            node->revents = 0;
            node->rdy = 0;
            aos_mutex_unlock(&pdev->evt_mutex);
            continue;
        }
        data->events[i].events = node->event.events & node->revents;
        memcpy(&data->events[i].data, &node->event.data, sizeof(data->events[i].data));

        if ((node->event.events & EPOLLET) != 0) {
            dlist_del(&node->link_node);
            node->revents = 0;
            node->rdy = 0;
        }

        if ((node->event.events & EPOLLONESHOT) != 0) {
            node->event.events &= EP_PRIVATE_BITS;
        }
        aos_mutex_unlock(&pdev->evt_mutex);
        if (++i == data->maxevents) {
            break;
        }
    }
    aos_mutex_unlock(&pdev->rbt_mutex);
    if (i > 0) {
        return i;
    }

    aos_event_get(&pdev->event, EVENT_FLAG, AOS_EVENT_OR_CLEAR, &real_flag, data->timeout);

    aos_mutex_lock(&pdev->rbt_mutex, AOS_WAIT_FOREVER);
    dlist_for_each_entry_safe(&pdev->ready_list, tmp, node, epoll_item_t, link_node) {
        if (node->fd < aos_vfs_fd_offset_get()) {

        } else {
            ret = aos_do_pollfd(node->fd, true, vfs_epoll_notify, &node->compat_evt, node);
            if (ret < 0) {
                printf("aos_do_pollfd err, node->fd = %d,ret = %d\r\n", node->fd, ret);
                if (ret < -1) {
                    errno = -ret;
                }
                ret = -1;
                break;
            }
        }
        aos_mutex_lock(&pdev->evt_mutex, AOS_WAIT_FOREVER);

        if (!(node->event.events & node->revents)) {
            dlist_del(&node->link_node);
            node->revents = 0;
            node->rdy = 0;
            aos_mutex_unlock(&pdev->evt_mutex);
            continue;
        }
        data->events[i].events = node->event.events & node->revents;
        memcpy(&data->events[i].data, &node->event.data, sizeof(data->events[i].data));

        if ((node->event.events & EPOLLET) != 0) {
            dlist_del(&node->link_node);
            node->revents = 0;
            node->rdy = 0;
        }

        if ((node->event.events & EPOLLONESHOT) != 0) {
            node->event.events &= EP_PRIVATE_BITS;
        }
        aos_mutex_unlock(&pdev->evt_mutex);
        if (++i == data->maxevents) {
            break;
        }
    }

    aos_mutex_unlock(&pdev->rbt_mutex);
    if (i > 0) {
        return i;
    }
    return ret;
}

static int epoll_ioctl(file_t *f, int cmd, unsigned long arg)
{
    epoll_dev_t *pdev = f->f_arg;
    if (pdev == NULL || arg == 0) {
        errno = EINVAL;
        return -1;
    }

    if (EPOLL_VFS_CTL == cmd) {
        return epoll_control(pdev, (epoll_ctl_data_t *)arg);
    } else if (EPOLL_VFS_WAIT == cmd) {
        return epoll_waitting(pdev, (epoll_wait_data_t *)arg);
    }
    errno = EINVAL;
    return -1;
}

ssize_t epoll_write(file_t *f, const void *buf, size_t len)
{
    printf("epoll:not support!\r\n");
    return 0;
}

static ssize_t epoll_read(file_t *f, void *buf, size_t len)
{
    printf("epoll:not support!\r\n");
    return 0;
}

static int epoll_poll(file_t *f, int setup, poll_notify_t notify,
                      void *fd, void *opa)
{
    printf("epoll:not support!\r\n");
    return 0;
}

static file_ops_t epoll_fops = {
    .open = epoll_open,
    .read = epoll_read,
    .write = epoll_write,
    .close = epoll_close,
    .poll = epoll_poll,
    .ioctl = epoll_ioctl,
};

int vfs_device_epoll_init(void)
{
    int ret;
    static uint8_t inited = 0;
    if (inited == 1) {
        return 0;
    }

    ret = aos_register_driver("/dev/epoll", &epoll_fops, NULL);
    if (ret < 0) {
        if (ret < -1) {
            errno = -ret;
        }
        return -1;
    }

    inited = 1;
    return 0;
}
