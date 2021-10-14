/**
 * @file errno.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_RHINO_P_H
#define AOS_RHINO_P_H

#ifdef __cplusplus
extern "C" {
#endif

#include <k_api.h>
#include "aos/errno.h"
#include "aos/kernel.h"

#define CHECK_HANDLE(hdl)  do {                                               \
                                if (hdl == NULL || (void *)(*hdl) == NULL) {  \
                                    return -EINVAL;                          \
                                }                                            \
                            } while (0)

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static inline aos_status_t rhino2stderrno(int ret)
{
    switch (ret) {
    case RHINO_SUCCESS:
    case RHINO_MUTEX_OWNER_NESTED:
    case RHINO_STOPPED:
        return 0;
    case RHINO_SYS_SP_ERR:
    case RHINO_NULL_PTR:
    case RHINO_MM_FREE_ADDR_ERR:
        return -EFAULT;

    case RHINO_INV_PARAM:
    case RHINO_INV_ALIGN:
    case RHINO_KOBJ_TYPE_ERR:
    case RHINO_MM_POOL_SIZE_ERR:
    case RHINO_MM_ALLOC_SIZE_ERR:
    case RHINO_INV_SCHED_WAY:
    case RHINO_TASK_INV_STACK_SIZE:
    case RHINO_BEYOND_MAX_PRI:
    case RHINO_BUF_QUEUE_INV_SIZE:
    case RHINO_BUF_QUEUE_SIZE_ZERO:
    case RHINO_BUF_QUEUE_MSG_SIZE_OVERFLOW:
    case RHINO_QUEUE_FULL:
    case RHINO_QUEUE_NOT_FULL:
    case RHINO_SEM_OVF:
    case RHINO_WORKQUEUE_EXIST:
    case RHINO_WORKQUEUE_NOT_EXIST:
    case RHINO_WORKQUEUE_WORK_EXIST:
        return -EINVAL;

    case RHINO_BLK_TIMEOUT:
        return -ETIMEDOUT ;
    case RHINO_KOBJ_BLK:
        return -EAGAIN;

    case RHINO_NO_MEM:
        return -ENOMEM;

    case RHINO_KOBJ_DEL_ERR:
    case RHINO_SCHED_DISABLE:
    case RHINO_SCHED_ALREADY_ENABLED:
    case RHINO_SCHED_LOCK_COUNT_OVF:
    case RHINO_TASK_NOT_SUSPENDED:
    case RHINO_TASK_DEL_NOT_ALLOWED:
    case RHINO_TASK_SUSPEND_NOT_ALLOWED:
    case RHINO_SUSPENDED_COUNT_OVF:
    case RHINO_PRI_CHG_NOT_ALLOWED:
    case RHINO_NOT_CALLED_BY_INTRPT:
    case RHINO_NO_THIS_EVENT_OPT:
    case RHINO_TIMER_STATE_INV:
    case RHINO_BUF_QUEUE_FULL:
    case RHINO_SEM_TASK_WAITING:
    case RHINO_MUTEX_NOT_RELEASED_BY_OWNER:
    case RHINO_WORKQUEUE_WORK_RUNNING:
        return -EPERM;

    case RHINO_TRY_AGAIN:
    case RHINO_WORKQUEUE_BUSY:
        return -EAGAIN;

    default:
        return -1;
    }

    return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* AOS_RHINO_P_H */

