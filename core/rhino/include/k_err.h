/**
 * @file k_err.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_ERR_H
#define K_ERR_H

/** @addtogroup aos_rhino err
 *  System error code, and error hook.
 *
 *  @{
 */

/* Define basic errno for rhino modules */
typedef enum
{
    RHINO_SUCCESS = 0u,
    RHINO_SYS_FATAL_ERR,
    RHINO_SYS_SP_ERR,
    RHINO_RUNNING,
    RHINO_STOPPED,
    RHINO_INV_PARAM,
    RHINO_NULL_PTR,
    RHINO_INV_ALIGN,
    RHINO_KOBJ_TYPE_ERR,
    RHINO_KOBJ_DEL_ERR,
    RHINO_KOBJ_DOCKER_EXIST,
    RHINO_KOBJ_BLK,
    RHINO_KOBJ_SET_FULL,
    RHINO_NOTIFY_FUNC_EXIST,

    RHINO_MM_POOL_SIZE_ERR = 100u,
    RHINO_MM_ALLOC_SIZE_ERR,
    RHINO_MM_FREE_ADDR_ERR,
    RHINO_MM_CORRUPT_ERR,
    RHINO_DYN_MEM_PROC_ERR,
    RHINO_NO_MEM,
    RHINO_RINGBUF_FULL,
    RHINO_RINGBUF_EMPTY,

    RHINO_SCHED_DISABLE = 200u,
    RHINO_SCHED_ALREADY_ENABLED,
    RHINO_SCHED_LOCK_COUNT_OVF,
    RHINO_INV_SCHED_WAY,

    RHINO_TASK_INV_STACK_SIZE = 300u,
    RHINO_TASK_NOT_SUSPENDED,
    RHINO_TASK_DEL_NOT_ALLOWED,
    RHINO_TASK_SUSPEND_NOT_ALLOWED,
    RHINO_TASK_CANCELED,
    RHINO_SUSPENDED_COUNT_OVF,
    RHINO_BEYOND_MAX_PRI,
    RHINO_PRI_CHG_NOT_ALLOWED,
    RHINO_INV_TASK_STATE,
    RHINO_IDLE_TASK_EXIST,

    RHINO_NO_PEND_WAIT = 400u,
    RHINO_BLK_ABORT,
    RHINO_BLK_TIMEOUT,
    RHINO_BLK_DEL,
    RHINO_BLK_INV_STATE,
    RHINO_BLK_POOL_SIZE_ERR,

    RHINO_TIMER_STATE_INV = 500u,

    RHINO_NO_THIS_EVENT_OPT = 600u,

    RHINO_BUF_QUEUE_INV_SIZE = 700u,
    RHINO_BUF_QUEUE_SIZE_ZERO,
    RHINO_BUF_QUEUE_FULL,
    RHINO_BUF_QUEUE_MSG_SIZE_OVERFLOW,
    RHINO_QUEUE_FULL,
    RHINO_QUEUE_NOT_FULL,

    RHINO_SEM_OVF = 800u,
    RHINO_SEM_TASK_WAITING,

    RHINO_MUTEX_NOT_RELEASED_BY_OWNER = 900u,
    RHINO_MUTEX_OWNER_NESTED,
    RHINO_MUTEX_NESTED_OVF,

    RHINO_NOT_CALLED_BY_INTRPT = 1000u,
    RHINO_TRY_AGAIN,

    RHINO_WORKQUEUE_EXIST = 1100u,
    RHINO_WORKQUEUE_NOT_EXIST,
    RHINO_WORKQUEUE_WORK_EXIST,
    RHINO_WORKQUEUE_BUSY,
    RHINO_WORKQUEUE_WORK_RUNNING,

    RHINO_TASK_STACK_OVF = 1200u,
    RHINO_INTRPT_STACK_OVF,

    RHINO_STATE_ALIGN = INT_MAX /* keep enum 4 bytes at 32bit machine */
} kstat_t;

/**
 * Critical error callback.
 *
 * @param[in]  err     error code
 *
 * @return none
 */
typedef void (*krhino_err_proc_t)(kstat_t err);

/**
 * Callback register, e.g. 'g_err_proc = app_error_handler();'
 */
extern krhino_err_proc_t g_err_proc;

/**
 * Error process entry.
 */
extern void k_err_proc_debug(kstat_t err, char *file, int line);

/**
 * Error proc for rhino module.
 */
#define k_err_proc(err) k_err_proc_debug(err, __FILE__, __LINE__)

/** @} */

#endif /* K_ERR_H */

