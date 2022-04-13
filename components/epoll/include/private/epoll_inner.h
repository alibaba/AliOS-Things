/**
 * @file epoll_inner.h
 * epoll_inner.h API header file.
 *
 * @version   V1.0
 * @date      2020-04-26
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef ___EPOLL_INNNER_H__
#define ___EPOLL_INNNER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
#include <aos/kernel.h>
#include <k_rbtree.h>
#include <aos/list.h>
#include <epoll.h>

#define EPOLL_VFS_CTL             10
#define EPOLL_VFS_WAIT            11

#define EP_PRIVATE_BITS (EPOLLWAKEUP | EPOLLONESHOT | EPOLLET | EPOLLEXCLUSIVE)

typedef struct eventpoll {
    struct k_rbtree_root_t rb_tree;
    int             rbcnt;
    dlist_t         ready_list;
    int             rdnum;
    int             waiting;
    aos_hdl_t     rbt_mutex;
    aos_hdl_t     evt_mutex;
    aos_hdl_t     event;

} epoll_dev_t;


struct pollfd {
    int   fd;
    short events;
    short revents;
};

typedef struct epoll_item {
    struct k_rbtree_node_t    tree_node;
    dlist_t            link_node;
    int                rdy; //exist in list
    int                fd;
    struct epoll_event event;
    epoll_dev_t        *epoll;
    struct pollfd      compat_evt;//compat to vfs poll
    uint32_t           revents;
} epoll_item_t;

typedef struct {
    int                 fd;
    int                 op;
    struct epoll_event  *event;
} epoll_ctl_data_t;

typedef struct {
    int                 maxevents;
    int                 timeout;
    struct epoll_event  *events;
} epoll_wait_data_t;

int vfs_device_epoll_init(void);
#ifdef __cplusplus
}
#endif

#endif