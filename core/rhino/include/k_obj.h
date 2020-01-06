/**
 * @file k_obj.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_OBJ_H
#define K_OBJ_H

/** @addtogroup aos_rhino obj
 *  Internal object information
 *
 *  @{
 */


/**
 * Blocking strategy.
 * It determines which task can go first when they are waiting for one resource.(from 'pend' to 'ready')
 * BLK_POLICY_PRI is default.
 */
typedef enum {
    /**
     * High-priority tasks are easier to obtain resources, the same priority is in FIFO order.
     */
    BLK_POLICY_PRI = 0u,
    /**
     * Just in FIFO order.
     */
    BLK_POLICY_FIFO
} blk_policy_t;

/**
 * Reasons for the end of the blocking state
 */
typedef enum {
    BLK_FINISH = 0, /**< task blocking successfully */
    BLK_ABORT,      /**< task blocking is aborted */
    BLK_TIMEOUT,    /**< task blocking timeout */
    BLK_DEL,        /**< task blocking for a deleted 'obj' */
    BLK_INVALID
} blk_state_t;

/**
 * The objects types
 */
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

/**
 * Abstract model of the 'obj':
 * 'obj' is an abstraction of all types of objects that can cause task blocking
 * (that is, tasks waiting for the resource to enter the PEND state).
*/
typedef struct blk_obj {
    klist_t       blk_list;     /**< Manage blocked tasks */
    const name_t *name;
    blk_policy_t  blk_policy;
    kobj_type_t   obj_type;
#if (RHINO_CONFIG_USER_SPACE > 0)
    klist_t       obj_list;
#endif
#if (RHINO_CONFIG_TASK_DEL > 0)
    uint8_t       cancel;
#endif
} blk_obj_t;

/**
 * Records multiple types of objects created by the OS, each type forming a linked list
*/
typedef struct {
    klist_t task_head;
    klist_t mutex_head;

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

/** @} */

#endif /* K_OBJ_H */

