/**
 * @file k_cfs.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_CFS_H
#define K_CFS_H

#include "rbtree_augmented.h"
#include "rbtree.h"

/** @addtogroup aos_rhino cfs
 *  Completely Fair Scheduler.
 *
 *  @{
 */

typedef struct cfs_node_s {
    struct rbt_node rbt_node;  /* rbttree node */
    lr_timer_t      key;       /* key */
} cfs_node;

void cfs_node_insert(cfs_node *node, lr_timer_t key);
void cfs_node_del(cfs_node *node);
lr_timer_t cfs_node_min_get(void);

/** @} */

#endif /* K_CFS_H */

