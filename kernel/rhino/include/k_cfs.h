/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CFS_H
#define K_CFS_H

#include "k_rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_rhino cfs
 *  Completely Fair Scheduler.
 *
 *  @{
 */

typedef struct cfs_node_s {
    struct k_rbtree_node_t rbt_node;  /* rbttree node */
    lr_timer_t      key;       /* key */
} cfs_node;

void cfs_node_insert(cfs_node *node, lr_timer_t key);
void cfs_node_del(cfs_node *node);
lr_timer_t cfs_node_min_get(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* K_CFS_H */

