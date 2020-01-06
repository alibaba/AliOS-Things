/**
 * @file k_trace.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_TRACE_H
#define K_TRACE_H

/** @addtogroup aos_rhino trace
 *  Trace
 *
 *  @{
 */

/* task trace */
#define TRACE_INIT()
#define TRACE_TASK_SWITCH(from, to)
#define TRACE_TASK_CREATE(task)
#define TRACE_TASK_SLEEP(task, ticks)
#define TRACE_INTRPT_TASK_SWITCH(from, to)
#define TRACE_TASK_PRI_CHANGE(task, task_pri_chg, pri)
#define TRACE_TASK_SUSPEND(task, task_suspended)
#define TRACE_TASK_RESUME(task, task_resumed)
#define TRACE_TASK_DEL(task, task_del)
#define TRACE_TASK_WAIT_ABORT(task, task_abort)

/* semaphore trace */
#define TRACE_SEM_CREATE(task, sem)
#define TRACE_SEM_OVERFLOW(task, sem)
#define TRACE_SEM_CNT_INCREASE(task, sem)
#define TRACE_SEM_GET_SUCCESS(task, sem)
#define TRACE_SEM_GET_BLK(task, sem, wait_option)
#define TRACE_SEM_TASK_WAKE(task, task_waked_up, sem, opt_wake_all)
#define TRACE_SEM_DEL(task, sem)

/* mutex trace */
#define TRACE_MUTEX_CREATE(task, mutex, name)
#define TRACE_MUTEX_RELEASE(task, task_release, new_pri)
#define TRACE_MUTEX_GET(task, mutex, wait_option)
#define TRACE_TASK_PRI_INV(task, mtxtsk)
#define TRACE_MUTEX_GET_BLK(task, mutex, wait_option)
#define TRACE_MUTEX_RELEASE_SUCCESS(task, mutex)
#define TRACE_MUTEX_TASK_WAKE(task, task_waked_up, mutex)
#define TRACE_MUTEX_DEL(task, mutex)

/* event trace */
#define TRACE_EVENT_CREATE(task, event, name, flags_init)
#define TRACE_EVENT_GET(task, event)
#define TRACE_EVENT_GET_BLK(task, event, wait_option)
#define TRACE_EVENT_TASK_WAKE(task, task_waked_up, event)
#define TRACE_EVENT_DEL(task, event)

/* buf_queue trace */
#define TRACE_BUF_QUEUE_CREATE(task, buf_queue)
#define TRACE_BUF_QUEUE_MAX(task, buf_queue, msg, msg_size)
#define TRACE_BUF_QUEUE_POST(task, buf_queue, msg, msg_size)
#define TRACE_BUF_QUEUE_TASK_WAKE(task, task_waked_up, queue)
#define TRACE_BUF_QUEUE_GET_BLK(task, buf_queue, wait_option)

/* timer trace */
#define TRACE_TIMER_CREATE(task, timer)
#define TRACE_TIMER_DEL(task, timer)

/* MBLK trace */
#define TRACE_MBLK_POOL_CREATE(task, pool)

/* MM trace */
#define TRACE_MM_POOL_CREATE(task, pool)

/* MM region trace*/
#define TRACE_MM_REGION_CREATE(task, regions)

/* work queue trace */
#define TRACE_WORK_INIT(task, work)
#define TRACE_WORKQUEUE_CREATE(task, workqueue)
#define TRACE_WORKQUEUE_DEL(task, workqueue)

/** @} */

#endif

