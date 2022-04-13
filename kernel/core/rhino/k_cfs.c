/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"

#if (RHINO_CONFIG_SCHED_CFS > 0)

#define NICE_0_LOAD 1024

#define MIN_CFS_PRI (RT_MAX_PRI + 1)

static struct rbt_root  cfs_root = RBT_ROOT;

#if 0
static const int prio_to_weight[40] = {
    /* -20 */     88761,     71755,     56483,     46273,     36291,
    /* -15 */     29154,     23254,     18705,     14949,     11916,
    /* -10 */      9548,      7620,      6100,      4904,      3906,
    /*  -5 */      3121,      2501,      1991,      1586,      1277,
    /*   0 */      1024,       820,       655,       526,       423,
    /*   5 */       335,       272,       215,       172,       137,
    /*  10 */       110,        87,        70,        56,        45,
    /*  15 */        36,        29,        23,        18,        15,
};
#endif

static const uint32_t prio_to_wmult[40] = {
    /* -20 */     48388,     59856,     76040,     92818,    118348,
    /* -15 */    147320,    184698,    229616,    287308,    360437,
    /* -10 */    449829,    563644,    704093,    875809,   1099582,
    /*  -5 */   1376151,   1717300,   2157191,   2708050,   3363326,
    /*   0 */   4194304,   5237765,   6557202,   8165337,  10153587,
    /*   5 */  12820798,  15790321,  19976592,  24970740,  31350126,
    /*  10 */  39045157,  49367440,  61356676,  76695844,  95443717,
    /*  15 */ 119304647, 148102320, 186737708, 238609294, 286331153,
};

static uint64_t __calc_delta(uint64_t delta_exec, uint32_t weight, uint32_t inv_weight)
{
    uint64_t fact;
    int shift = 32;

    /* hint to use a 32x32->64 mul */
    fact = (uint64_t)weight * inv_weight;

    while (fact >> 32) {
        fact >>= 1;
        shift--;
    }

    return (uint64_t)((delta_exec * fact) >> shift);
}

lr_timer_t calc_vruntime(lr_timer_t exec_time, uint8_t prio)
{
    uint8_t  index;

    if (prio == CFS_TASK_PRI) {
        return exec_time;
    }

    if ((prio <= RT_MAX_PRI) || (prio >= RHINO_IDLE_PRI)) {
        return exec_time;
    }

    index = prio - MIN_CFS_PRI;

    return __calc_delta(exec_time, NICE_0_LOAD, prio_to_wmult[index]);
}

/*
 * Insert @key into rbtree, On success, return 0, else return -1
 */
static void node_insert(struct rbt_root *root, cfs_node *node, lr_timer_t key)
{
    struct rbt_node **tmp = &(root->rbt_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
        cfs_node *my = rbt_entry(*tmp, cfs_node, rbt_node);
        parent = *tmp;
        if (key <= my->key) {
            tmp = &((*tmp)->rbt_left);
        } else if (key > my->key) {
            tmp = &((*tmp)->rbt_right);
        }
    }

    node->key = key;

    /* Add new node and rebalance tree. */
    rbt_link_node(&node->rbt_node, parent, tmp);
    rbt_insert_color(&node->rbt_node, root);
}

void cfs_node_insert(cfs_node *node, lr_timer_t key)
{
    node_insert(&cfs_root, node, key);
}

void cfs_node_del(cfs_node *node)
{
    rbt_erase(&node->rbt_node, &cfs_root);
}

lr_timer_t cfs_node_min_get(void)
{
    struct rbt_node *tmp;
    cfs_node        *node_cfs;

    tmp  = rbt_first(&cfs_root);
    if (tmp == NULL) {
        return 0;
    }

    node_cfs = rbt_entry(tmp, cfs_node, rbt_node);
    return node_cfs->key;
}

ktask_t *cfs_preferred_task_get(void)
{
    struct rbt_node *tmp;
    cfs_node        *node_cfs;
    ktask_t         *task;
#if (RHINO_CONFIG_CPU_NUM > 1)
    uint8_t  flag;
#endif

    tmp  = rbt_first(&cfs_root);
    if (tmp == NULL) {
        return NULL;
    }

    node_cfs = rbt_entry(tmp, cfs_node, rbt_node);
    task     = krhino_list_entry(node_cfs, ktask_t, node);

#if (RHINO_CONFIG_CPU_NUM > 1)
    flag = ((task->cur_exc == 0) && (task->cpu_binded == 0))
           || ((task->cur_exc == 0) && (task->cpu_binded == 1) && (task->cpu_num == cpu_cur_get()));

    if (flag > 0) {
        return task;
    }

    tmp = rbt_next(tmp);
    while (tmp) {
        node_cfs = rbt_entry(tmp, cfs_node, rbt_node);
        task     = krhino_list_entry(node_cfs, ktask_t, node);

        flag = ((task->cur_exc == 0) && (task->cpu_binded == 0))
               || ((task->cur_exc == 0) && (task->cpu_binded == 1) && (task->cpu_num == cpu_cur_get()));

        if (flag > 0) {
            return task;
        }

        tmp = rbt_next(tmp);
    }

    return NULL;
#else
    return task;
#endif
}
#endif

