/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_OBJ_H
#define K_OBJ_H

typedef enum {
    BLK_POLICY_PRI = 0u,
    BLK_POLICY_FIFO
} blk_policy_t;

typedef enum {
    BLK_FINISH = 0,
    BLK_ABORT,
    BLK_TIMEOUT,
    BLK_DEL,
    BLK_INVALID
} blk_state_t;

typedef enum {
    RHINO_OBJ_TYPE_NONE = 0,
    RHINO_SEM_OBJ_TYPE,
    RHINO_MUTEX_OBJ_TYPE,
    RHINO_QUEUE_OBJ_TYPE,
    RHINO_BUF_QUEUE_OBJ_TYPE,
    RHINO_TIMER_OBJ_TYPE,
    RHINO_EVENT_OBJ_TYPE,
    RHINO_MM_OBJ_TYPE
} kobj_type_t;

typedef struct blk_obj {
    klist_t          blk_list;
    const name_t    *name;
    blk_policy_t     blk_policy;
    kobj_type_t      obj_type;
} blk_obj_t;

typedef struct {
    klist_t task_head;
    klist_t mutex_head;

#if (RHINO_CONFIG_MM_BLK > 0)
    klist_t mblkpool_head;
#endif

#if (RHINO_CONFIG_SEM > 0)
    klist_t sem_head;
#endif

#if (RHINO_CONFIG_QUEUE > 0)
    klist_t queue_head;
#endif

#if (RHINO_CONFIG_EVENT_FLAG > 0)
    klist_t event_head;
#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)
    klist_t buf_queue_head;
#endif
} kobj_list_t;

#endif /* K_OBJ_H */

