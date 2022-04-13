/**
 * @file rbtree_wrapper.h
 * rbtree_wrapper.h API header file.
 *
 * @version   V1.0
 * @date      2020-05-14
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef ___RBTREE_WRAPPER_H__
#define ___RBTREE_WRAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <aos/kernel.h>
#include <epoll_inner.h>
#include <rbtree.h>

epoll_item_t *rbr_find(struct rbt_root *root, int fd);

int rbt_insert(struct rbt_root *root, epoll_item_t *item);

int rbt_delete(struct rbt_root *root, int fd);

#ifdef __cplusplus
}
#endif

#endif