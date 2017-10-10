/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TRACE_H
#define K_TRACE_H


#if (RHINO_CONFIG_TRACE > 0)
/* task trace function */
void _trace_init(void);
void _trace_task_switch(ktask_t *from, ktask_t *to);
void _trace_intrpt_task_switch(ktask_t *from, ktask_t *to);
void _trace_task_create(ktask_t *task);
void _trace_task_sleep(ktask_t *task, tick_t ticks);
void _trace_task_pri_change(ktask_t *task, ktask_t *task_pri_chg, uint8_t pri);
void _trace_task_suspend(ktask_t *task, ktask_t *task_suspended);
void _trace_task_resume(ktask_t *task, ktask_t *task_resumed);
void _trace_task_del(ktask_t *task, ktask_t *task_del);
void _trace_task_abort(ktask_t *task, ktask_t *task_abort);

/* semaphore trace function */
void _trace_sem_create(ktask_t *task, ksem_t *sem);
void _trace_sem_overflow(ktask_t *task, ksem_t *sem);
void _trace_sem_del(ktask_t *task, ksem_t *sem);
void _trace_sem_get_success(ktask_t *task, ksem_t *sem);
void _trace_sem_get_blk(ktask_t *task, ksem_t *sem, tick_t wait_option);
void _trace_sem_task_wake(ktask_t *task, ktask_t *task_waked_up, ksem_t *sem,
                          uint8_t opt_wake_all);
void _trace_sem_cnt_increase(ktask_t *task, ksem_t *sem);

/* mutex trace function */
void _trace_mutex_create(ktask_t *task, kmutex_t *mutex, const name_t *name);
void _trace_mutex_release(ktask_t *task, ktask_t *task_release,
                          uint8_t new_pri);
void _trace_mutex_get(ktask_t *task, kmutex_t *mutex, tick_t wait_option);
void _trace_task_pri_inv(ktask_t *task, ktask_t *mtxtsk);
void _trace_mutex_get_blk(ktask_t *task, kmutex_t *mutex, tick_t wait_option);
void _trace_mutex_release_success(ktask_t *task, kmutex_t *mutex);
void _trace_mutex_task_wake(ktask_t *task, ktask_t *task_waked_up,
                            kmutex_t *mutex);
void _trace_mutex_del(ktask_t *task, kmutex_t *mutex);

/* event trace function */
void _trace_event_create(ktask_t *task, kevent_t *event, const name_t *name,
                         uint32_t flags_init);
void _trace_event_get(ktask_t *task, kevent_t *event);
void _trace_event_get_blk(ktask_t *task, kevent_t *event, tick_t wait_option);
void _trace_event_task_wake(ktask_t *task, ktask_t *task_waked_up,
                            kevent_t *event);
void _trace_event_del(ktask_t *task, kevent_t *event);

/* buf_queue trace function */
void _trace_buf_queue_create(ktask_t *task, kbuf_queue_t *buf_queue);
void _trace_buf_max(ktask_t *task, kbuf_queue_t *buf_queue, void *p_void,
                    size_t msg_size);
void _trace_buf_post(ktask_t *task, kbuf_queue_t *buf_queue, void *p_void,
                     size_t msg_size);
void _trace_buf_queue_task_wake(ktask_t *task, ktask_t *task_waked_up,
                                kbuf_queue_t *buf_queue);
void _trace_buf_queue_get_blk(ktask_t *task, kbuf_queue_t *buf_queue,
                              tick_t wait_option);

/* timer trace function */
void _trace_timer_create(ktask_t *task, ktimer_t *timer);
void _trace_timer_del(ktask_t *task, ktimer_t *timer);

/* mblk trace function */
void _trace_mblk_pool_create(ktask_t *task, mblk_pool_t *pool);

/* mm trace function */
//void _trace_mm_pool_create(ktask_t *task, mm_pool_t *pool);

/* work queue trace */
void _trace_work_init(ktask_t *task, kwork_t *work);
void _trace_workqueue_create(ktask_t *task, kworkqueue_t *workqueue);
void _trace_workqueue_del(ktask_t *task, kworkqueue_t *workqueue);

/* task trace */
#define TRACE_INIT()                                   _trace_init()
#define TRACE_TASK_SWITCH(from, to)                    _trace_task_switch(from, to)
#define TRACE_TASK_CREATE(task)                        _trace_task_create(task)
#define TRACE_TASK_SLEEP(task, ticks)                  _trace_task_sleep(task, ticks)
#define TRACE_INTRPT_TASK_SWITCH(from, to)             _trace_intrpt_task_switch(from, to)
#define TRACE_TASK_PRI_CHANGE(task, task_pri_chg, pri) _trace_task_pri_change(task, task_pri_chg, pri)
#define TRACE_TASK_SUSPEND(task, task_suspended)       _trace_task_suspend(task, task_suspended)
#define TRACE_TASK_RESUME(task, task_resumed)          _trace_task_resume(task, task_resumed)
#define TRACE_TASK_DEL(task, task_del)                 _trace_task_del(task, task_del)
#define TRACE_TASK_WAIT_ABORT(task, task_abort)        _trace_task_abort(task, task_abort)

/* semaphore trace */
#define TRACE_SEM_CREATE(task, sem)                            _trace_sem_create(task, sem)
#define TRACE_SEM_OVERFLOW(task, sem)                          _trace_sem_overflow(task, sem)
#define TRACE_SEM_CNT_INCREASE(task, sem)                      _trace_sem_cnt_increase(task, sem)
#define TRACE_SEM_GET_SUCCESS(task, sem)                       _trace_sem_get_success(task, sem)
#define TRACE_SEM_GET_BLK(task, sem, wait_option)              _trace_sem_get_blk(task, sem, wait_option)
#define TRACE_SEM_TASK_WAKE(task, task_waked_up, sem, opt_wake_all) _trace_sem_task_wake(task, task_waked_up, sem, opt_wake_all)
#define TRACE_SEM_DEL(task, sem)                               _trace_sem_del(task, sem);

/* mutex trace */
#define TRACE_MUTEX_CREATE(task, mutex, name)             _trace_mutex_create(task, mutex, name)
#define TRACE_MUTEX_RELEASE(task, task_release, new_pri)  _trace_mutex_release(task, task_release, new_pri)
#define TRACE_MUTEX_GET(task, mutex, wait_option)         _trace_mutex_get(task, mutex, wait_option)
#define TRACE_TASK_PRI_INV(task, mtxtsk)                  _trace_task_pri_inv(task, mtxtsk)
#define TRACE_MUTEX_GET_BLK(task, mutex, wait_option)     _trace_mutex_get_blk(task, mutex, wait_option)
#define TRACE_MUTEX_RELEASE_SUCCESS(task, mutex)          _trace_mutex_release_success(task, mutex)
#define TRACE_MUTEX_TASK_WAKE(task, task_waked_up, mutex) _trace_mutex_task_wake(task, task_waked_up, mutex)
#define TRACE_MUTEX_DEL(task, mutex)                      _trace_mutex_del(task, mutex)

/* event trace */
#define TRACE_EVENT_CREATE(task, event, name, flags_init) _trace_event_create(task, event, name, flags_init)
#define TRACE_EVENT_GET(task, event)                      _trace_event_get(task, event)
#define TRACE_EVENT_GET_BLK(task, event, wait_option)     _trace_event_get_blk(task, event, wait_option)
#define TRACE_EVENT_TASK_WAKE(task, task_waked_up, event) _trace_event_task_wake(task, task_waked_up, event)
#define TRACE_EVENT_DEL(task, event)                      _trace_event_del(task, event)

/* buf_queue trace */
#define TRACE_BUF_QUEUE_CREATE(task, buf_queue)                       _trace_buf_queue_create(task, buf_queue)
#define TRACE_BUF_QUEUE_MAX(task, buf_queue, msg, msg_size)           _trace_buf_max(task, buf_queue, msg, msg_size)
#define TRACE_BUF_QUEUE_POST(task, buf_queue, msg, msg_size)          _trace_buf_post(task, buf_queue, msg, msg_size)
#define TRACE_BUF_QUEUE_TASK_WAKE(task, task_waked_up, queue)         _trace_buf_queue_task_wake(task, task_waked_up, queue)
#define TRACE_BUF_QUEUE_GET_BLK(task, buf_queue, wait_option)         _trace_buf_queue_get_blk(task, buf_queue, wait_option)

/* timer trace */
#define TRACE_TIMER_CREATE(task, timer)         _trace_timer_create(task, timer)
#define TRACE_TIMER_DEL(task, timer)            _trace_timer_del(task, timer)

/* mblk trace */
#define TRACE_MBLK_POOL_CREATE(task, pool)      _trace_mblk_pool_create(task, pool)

/* mm trace */
#define TRACE_MM_POOL_CREATE(task, pool)        _trace_mm_pool_create(task, pool)

/* mm region */
#define TRACE_MM_REGION_CREATE(task, regions)   _trace_mm_region_create(task, regions)

/* work queue trace */
#define TRACE_WORK_INIT(task, work)             _trace_work_init(task, work)
#define TRACE_WORKQUEUE_CREATE(task, workqueue) _trace_workqueue_create(task, workqueue)
#define TRACE_WORKQUEUE_DEL(task, workqueue)    _trace_workqueue_del(task, workqueue)

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
#endif
#endif

