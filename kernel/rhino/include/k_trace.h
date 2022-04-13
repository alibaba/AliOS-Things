/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TRACE_H
#define K_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_rhino trace
 *  Trace
 *
 *  @{
 */

#ifdef AOS_COMP_TRACE

#define SYSVIEW_TRACE_ID_SEM_BASE         (40u)
#define SYSVIEW_TRACE_ID_SEM_CREATE       ( 1u + SYSVIEW_TRACE_ID_SEM_BASE)
#define SYSVIEW_TRACE_ID_SEM_OVERFLOW       ( 2u + SYSVIEW_TRACE_ID_SEM_BASE)
#define SYSVIEW_TRACE_ID_SEM_CNT_INCREASE       ( 3u + SYSVIEW_TRACE_ID_SEM_BASE)
#define SYSVIEW_TRACE_ID_SEM_GET_SUCCESS       ( 4u + SYSVIEW_TRACE_ID_SEM_BASE)
#define SYSVIEW_TRACE_ID_SEM_GET_BLK       ( 5u + SYSVIEW_TRACE_ID_SEM_BASE)
#define SYSVIEW_TRACE_ID_SEM_TASK_WAKE       ( 6u + SYSVIEW_TRACE_ID_SEM_BASE)
#define SYSVIEW_TRACE_ID_SEM_DEL       ( 7u + SYSVIEW_TRACE_ID_SEM_BASE)
#define SYSVIEW_TRACE_ID_SEM_GIVE  ( 8u + SYSVIEW_TRACE_ID_SEM_BASE)

#define SYSVIEW_TRACE_ID_MUTEX_BASE       (50u)
#define SYSVIEW_TRACE_ID_MUTEX_CREATE    ( 1u + SYSVIEW_TRACE_ID_MUTEX_BASE)
#define SYSVIEW_TRACE_ID_MUTEX_RELEASE    ( 2u + SYSVIEW_TRACE_ID_MUTEX_BASE)
#define SYSVIEW_TRACE_ID_MUTEX_GET    ( 3u + SYSVIEW_TRACE_ID_MUTEX_BASE)
#define SYSVIEW_TRACE_ID_TASK_PRI_INV    ( 4u + SYSVIEW_TRACE_ID_MUTEX_BASE)
#define SYSVIEW_TRACE_ID_MUTEX_GET_BLK    ( 5u + SYSVIEW_TRACE_ID_MUTEX_BASE)
#define SYSVIEW_TRACE_ID_MUTEX_RELEASE_SUCCESS    ( 6u + SYSVIEW_TRACE_ID_MUTEX_BASE)
#define SYSVIEW_TRACE_ID_MUTEX_TASK_WAKE    ( 7u + SYSVIEW_TRACE_ID_MUTEX_BASE)
#define SYSVIEW_TRACE_ID_MUTEX_DEL    ( 8u + SYSVIEW_TRACE_ID_MUTEX_BASE)

#define SYSVIEW_TRACE_ID_EVENT_BASE       (60u)
#define SYSVIEW_TRACE_ID_EVENT_CREATE    ( 1u + SYSVIEW_TRACE_ID_EVENT_BASE)
#define SYSVIEW_TRACE_ID_EVENT_GET    ( 2u + SYSVIEW_TRACE_ID_EVENT_BASE)
#define SYSVIEW_TRACE_ID_EVENT_GET_BLK    ( 3u + SYSVIEW_TRACE_ID_EVENT_BASE)
#define SYSVIEW_TRACE_ID_EVENT_TASK_WAKE    ( 4u + SYSVIEW_TRACE_ID_EVENT_BASE)
#define SYSVIEW_TRACE_ID_EVENT_DEL    ( 5u + SYSVIEW_TRACE_ID_EVENT_BASE)


#define SYSVIEW_TRACE_ID_BUF_QUEUE_BASE       (70u)
#define SYSVIEW_TRACE_ID_BUF_QUEUE_CREATE    ( 1u + SYSVIEW_TRACE_ID_BUF_QUEUE_BASE)
#define SYSVIEW_TRACE_ID_BUF_QUEUE_MAX    ( 2u + SYSVIEW_TRACE_ID_BUF_QUEUE_BASE)
#define SYSVIEW_TRACE_ID_BUF_QUEUE_POST    ( 3u + SYSVIEW_TRACE_ID_BUF_QUEUE_BASE)
#define SYSVIEW_TRACE_ID_BUF_QUEUE_TASK_WAKE    ( 4u + SYSVIEW_TRACE_ID_BUF_QUEUE_BASE)
#define SYSVIEW_TRACE_ID_BUF_QUEUE_GET_BLK    ( 5u + SYSVIEW_TRACE_ID_BUF_QUEUE_BASE)

#define SYSVIEW_TRACE_ID_WORKQUEUE_BASE       (80u)
#define SYSVIEW_TRACE_ID_WORKQUEUE_INIT    ( 1u + SYSVIEW_TRACE_ID_WORKQUEUE_BASE)
#define SYSVIEW_TRACE_ID_WORKQUEUE_CREATE    ( 2u + SYSVIEW_TRACE_ID_WORKQUEUE_BASE)
#define SYSVIEW_TRACE_ID_WORKQUEUE_DEL    ( 3u + SYSVIEW_TRACE_ID_WORKQUEUE_BASE)


/* interrupt trace */
void trace_intrpt_exit(void);
void trace_intrpt_enter(void);

#if 0
#define TRACE_INTRPT_ENTETR() trace_intrpt_enter()
#define TRACE_INTRPT_EXIT()  trace_intrpt_exit()
#else
#define TRACE_INTRPT_ENTETR()
#define TRACE_INTRPT_EXIT()
#endif

/* task trace */
void trace_task_switch(ktask_t *from, ktask_t *to);
void trace_intrpt_task_switch(ktask_t *from, ktask_t *to);
void trace_init(void);
void trace_task_idel(void);
void trace_task_create(ktask_t *task);
void trace_task_wait_abort(ktask_t *task, ktask_t *task_abort);
void trace_task_resume(ktask_t *task, ktask_t *task_resumed);
void trace_task_suspend(ktask_t *task, ktask_t *task_suspended);
void trace_task_start_ready(ktask_t *task);
void trace_task_stop_ready(ktask_t *task);

#define TRACE_INIT()  trace_init()
#define TRACE_TASK_SWITCH(from, to)   trace_task_switch(from, to)
#define TRACE_TASK_CREATE(task)   trace_task_create(task)
#define TRACE_TASK_SLEEP(task, ticks)
#define TRACE_INTRPT_TASK_SWITCH(from, to) trace_intrpt_task_switch(from, to)
#define TRACE_TASK_PRI_CHANGE(task, task_pri_chg, pri)
#define TRACE_TASK_SUSPEND(task, task_suspended) trace_task_suspend(task, task_suspended)
#define TRACE_TASK_RESUME(task, task_resumed) trace_task_resume(task, task_resumed)
#define TRACE_TASK_DEL(task, task_del)
#define TRACE_TASK_WAIT_ABORT(task, task_abort)  trace_task_wait_abort(task, task_abort)
#define TRACE_TASK_START_READY(task)  trace_task_start_ready(task)
#define TRACE_TASK_STOP_READY(task)  trace_task_stop_ready(task)

/* semaphore trace */
void trace_sem_create(ktask_t *task, ksem_t *sem);
void trace_sem_overflow(ktask_t *task, ksem_t *sem);
void trace_sem_cnt_increase(ktask_t *task, ksem_t *sem);
void trace_sem_get_success(ktask_t *task, ksem_t *sem);
void trace_sem_get_blk(ktask_t *task, ksem_t *sem);
void trace_sem_task_wake(ktask_t *task, ktask_t *task_waked_up, ksem_t *sem, uint8_t opt_wake_all);
void trace_sem_del(ktask_t *task, ksem_t *sem);
void trace_sem_give(ksem_t *sem, uint8_t opt_wake_all);

#define TRACE_SEM_CREATE(task, sem) trace_sem_create(task, sem)
#define TRACE_SEM_OVERFLOW(task, sem)  trace_sem_overflow(task, sem)
#define TRACE_SEM_CNT_INCREASE(task, sem) trace_sem_cnt_increase(task, sem)
#define TRACE_SEM_GET_SUCCESS(task, sem) trace_sem_get_success(task, sem)
#define TRACE_SEM_GET_BLK(task, sem, wait_option) trace_sem_get_blk(task, sem)
#define TRACE_SEM_TASK_WAKE(task, task_waked_up, sem, opt_wake_all) trace_sem_task_wake(task, task_waked_up, sem, opt_wake_all)
#define TRACE_SEM_DEL(task, sem) trace_sem_del(task, sem)
#define TRACE_SEM_GIVE(sem, opt_wake_all) trace_sem_give(sem, opt_wake_all)

/* mutex trace */
void trace_mutex_create(ktask_t *task, kmutex_t *mutex, const name_t *name);
void trace_mutex_release(ktask_t *task, ktask_t *task_release, uint8_t new_pri);
void trace_mutex_get(ktask_t *task, kmutex_t *mutex, tick_t wait_option);
void trace_task_pri_inv(ktask_t *task, ktask_t *mtxtsk);
void trace_mutex_get_blk(ktask_t *task, kmutex_t *mutex, tick_t wait_option);
void trace_mutex_release_success(ktask_t *task, kmutex_t *mutex);
void trace_mutex_task_wake(ktask_t *task, ktask_t *task_waked_up, kmutex_t *mutex);
void trace_mutex_del(ktask_t *task, kmutex_t *mutex);

#define TRACE_MUTEX_CREATE(task, mutex, name)  trace_mutex_create(task, mutex, name)
#define TRACE_MUTEX_RELEASE(task, task_release, new_pri)  trace_mutex_release(task, task_release, new_pri)
#define TRACE_MUTEX_GET(task, mutex, wait_option)  trace_mutex_get(task, mutex, wait_option)
#define TRACE_TASK_PRI_INV(task, mtxtsk)  trace_task_pri_inv(task, mtxtsk)
#define TRACE_MUTEX_GET_BLK(task, mutex, wait_option)  trace_mutex_get_blk(task, mutex, wait_option)
#define TRACE_MUTEX_RELEASE_SUCCESS(task, mutex)  trace_mutex_release_success(task, mutex)
#define TRACE_MUTEX_TASK_WAKE(task, task_waked_up, mutex)  trace_mutex_task_wake(task, task_waked_up, mutex)
#define TRACE_MUTEX_DEL(task, mutex)  trace_mutex_del(task, mutex)

/* event trace */
void trace_event_create(ktask_t *task, kevent_t *event, const name_t *name, uint32_t flags_init);
void trace_event_get(ktask_t *task, kevent_t *event);
void trace_event_get_blk(ktask_t *task, kevent_t *event, tick_t wait_option);
void trace_event_task_wake(ktask_t *task, ktask_t *task_waked_up, kevent_t *event);
void trace_event_del(ktask_t *task, kevent_t *event);

#define TRACE_EVENT_CREATE(task, event, name, flags_init) trace_event_create(task, event, name, flags_init)
#define TRACE_EVENT_GET(task, event)  trace_event_get(task, event)
#define TRACE_EVENT_GET_BLK(task, event, wait_option)  trace_event_get_blk(task, event, wait_option)
#define TRACE_EVENT_TASK_WAKE(task, task_waked_up, event)  trace_event_task_wake(task, task_waked_up, event)
#define TRACE_EVENT_DEL(task, event)  trace_event_del(task, event)

/* buf_queue trace */
void trace_buf_queue_create(ktask_t *task, kqueue_t *buf_queue);
void trace_buf_queue_max(ktask_t *task, kqueue_t *buf_queue, void *msg, size_t msg_size);
void trace_buf_queue_post(ktask_t *task, kqueue_t *buf_queue, void *msg, size_t msg_size);
void trace_buf_queue_task_wake(ktask_t *task, ktask_t *task_waked_up, kqueue_t *buf_queue);
void trace_buf_queue_get_blk(ktask_t *task, kqueue_t *buf_queue, tick_t wait_option);

#define TRACE_BUF_QUEUE_CREATE(task, buf_queue)  trace_buf_queue_create(task, buf_queue)
#define TRACE_BUF_QUEUE_MAX(task, buf_queue, msg, msg_size) trace_buf_queue_max(task, buf_queue, msg, msg_size)
#define TRACE_BUF_QUEUE_POST(task, buf_queue, msg, msg_size)  trace_buf_queue_post(task, buf_queue, msg, msg_size)
#define TRACE_BUF_QUEUE_TASK_WAKE(task, task_waked_up, queue)  trace_buf_queue_task_wake(task, task_waked_up, queue)
#define TRACE_BUF_QUEUE_GET_BLK(task, buf_queue, wait_option)  trace_buf_queue_get_blk(task, buf_queue, wait_option)

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
void trace_workqueue_init(ktask_t *task, kwork_t *work);
void trace_workqueue_create(ktask_t *task, kworkqueue_t *workqueue);
void trace_workqueue_del(ktask_t *task, kworkqueue_t *workqueue);

#define TRACE_WORK_INIT(task, work)   trace_workqueue_init(task, work)
#define TRACE_WORKQUEUE_CREATE(task, workqueue)  trace_workqueue_create(task, workqueue)
#define TRACE_WORKQUEUE_DEL(task, workqueue)  trace_workqueue_del(task, workqueue)

int uart_port_init(void);
#else

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
#define TRACE_TASK_START_READY(task)
#define TRACE_TASK_STOP_READY(task)

/* interuppt trace */
#define TRACE_INTRPT_ENTETR()
#define TRACE_INTRPT_EXIT()

/* semaphore trace */
#define TRACE_SEM_CREATE(task, sem)
#define TRACE_SEM_OVERFLOW(task, sem)
#define TRACE_SEM_CNT_INCREASE(task, sem)
#define TRACE_SEM_GET_SUCCESS(task, sem)
#define TRACE_SEM_GET_BLK(task, sem, wait_option)
#define TRACE_SEM_TASK_WAKE(task, task_waked_up, sem, opt_wake_all)
#define TRACE_SEM_DEL(task, sem)
#define TRACE_SEM_GIVE(sem, opt_wake_all)

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

#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif

