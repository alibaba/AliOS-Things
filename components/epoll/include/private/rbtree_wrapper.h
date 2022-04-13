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
#include <private/epoll_inner.h>
#include <k_rbtree.h>

epoll_item_t *rbr_find(struct k_rbtree_root_t *root, int fd);

int rbt_insert(struct k_rbtree_root_t *root, epoll_item_t *item);

int rbt_delete(struct k_rbtree_root_t *root, int fd);

#ifdef __cplusplus
}
#endif

#endif