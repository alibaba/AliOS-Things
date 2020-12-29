/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <ble_os.h>
#include <misc/util.h>
#include <misc/dlist.h>

//#include <aos/aos.h> //yulong removed

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)

#include <common/log.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "atomic.h"
#define OSAL_RHINO

#ifdef OSAL_RHINO
#include <k_default_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_sys.h>
#include <k_list.h>
#include <k_ringbuf.h>
#include <k_obj.h>
#include <k_sem.h>
#include <k_queue.h>
#include <k_buf_queue.h>
#include <k_stats.h>
#include <k_time.h>
#include <k_task.h>
#include <port.h>
#include "errno.h"
#endif

void k_queue_init(struct k_queue *queue)
{
    int     stat;

    stat = krhino_sem_create(&queue->sem, "ble", 0);

    if (stat) {
        //SYS_LOG_ERR("buf queue exhausted\n");
    }

    sys_slist_init(&queue->queue_list);
    sys_dlist_init(&queue->poll_events);
}

static inline void handle_poll_events(struct k_queue *queue, bt_u32_t state)
{
    _handle_obj_poll_events(&queue->poll_events, state);
}

void k_queue_cancel_wait(struct k_queue *queue)
{
    krhino_sem_give(&queue->sem);
    handle_poll_events(queue, K_POLL_STATE_NOT_READY);
}

void k_queue_insert(struct k_queue *queue, void *prev, void *node)
{
    unsigned int key;
    key = irq_lock();

    sys_slist_append(&queue->queue_list, node);
    irq_unlock(key);
    krhino_sem_give(&queue->sem);
    handle_poll_events(queue, K_POLL_STATE_DATA_AVAILABLE);
}

long long k_now_ms()
{
    return aos_now_ms();
}

void k_queue_append(struct k_queue *queue, void *data)
{
    k_queue_insert(queue, NULL, data);
}

void k_queue_prepend(struct k_queue *queue, void *data)
{
    k_queue_insert(queue, NULL, data);
}

void k_queue_append_list(struct k_queue *queue, void *head, void *tail)
{
    struct net_buf *buf_tail = (struct net_buf *)head;

    for (buf_tail = (struct net_buf *)head; buf_tail;
         buf_tail = buf_tail->frags) {
        k_queue_append(queue, buf_tail);
    }

    handle_poll_events(queue, K_POLL_STATE_DATA_AVAILABLE);
}

void *k_queue_get(struct k_queue *queue, int32_t timeout)
{
    int ret;
    void        *msg = NULL;
    tick_t       ticks;

    if (timeout == K_FOREVER) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(timeout);
    }

    ret = krhino_sem_take(&queue->sem, ticks);

    if (ret) {
        return NULL;
    } else {
        unsigned int key;
        key = irq_lock();
        msg =  sys_slist_get(&queue->queue_list);
        irq_unlock(key);
        return msg;
    }
}

int k_queue_is_empty(struct k_queue *queue)
{
    return (int)sys_slist_is_empty(&queue->queue_list);
}

int k_queue_count(struct k_queue *queue)
{
    return queue->sem.count;
}

int k_sem_init(struct k_sem *sem, unsigned int initial_count,
               unsigned int limit)
{
    int ret;

    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = krhino_sem_create(&sem->sem, "ble", initial_count);
	if (ret != 0)
		BT_ERR("%s failed: %d", __func__, ret);
    sys_dlist_init(&sem->poll_events);
    return ret;
}

int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    tick_t ticks;
	int ret;

    if (timeout == K_FOREVER) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(timeout);
    }

    ret = krhino_sem_take(&sem->sem, ticks);
	if (ret != 0)
		BT_ERR("%s failed: %d", __func__, ret);
	return ret;
}

int k_sem_give(struct k_sem *sem)
{
	int ret;
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = krhino_sem_give(&sem->sem);
	if (ret != 0)
		BT_ERR("%s failed: %d", __func__, ret);
    return 0;
}

int k_sem_delete(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    krhino_sem_del(&sem->sem);
    return 0;
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
#ifdef OSAL_RHINO
    sem_count_t count;

    krhino_sem_count_get(&sem->sem, &count);
    return (int)count;
#endif
    return 0;
}


void k_mutex_init(struct k_mutex *mutex)
{
    int stat;

    if (NULL == mutex) {
        BT_ERR("mutex is NULL\n");
        return;
    }

    stat = krhino_mutex_create(&mutex->mutex, "ble");

    if (stat) {
        BT_ERR("mutex buffer over\n");
    }

    sys_dlist_init(&mutex->poll_events);
    return;
}

int k_mutex_lock(struct k_mutex *mutex, bt_s32_t timeout)
{
    tick_t ticks;
	int ret;

    if (timeout == K_FOREVER) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(timeout);
    }

    ret =  krhino_mutex_lock(&mutex->mutex, ticks);
	if (ret != 0)
		BT_ERR("%s failed: %d", __func__, ret);
		
	return ret;
}

void k_mutex_unlock(struct k_mutex *mutex)
{
	int ret;
    if (NULL == mutex) {
        BT_ERR("mutex is NULL\n");
        return;
    }

    ret = krhino_mutex_unlock(&mutex->mutex);
	if (ret != 0)
		BT_ERR("%s failed: %d", __func__, ret);
}

int64_t k_uptime_get()
{
    return krhino_sys_time_get();
}

uint32_t k_uptime_get_32()
{
    return (uint32_t)krhino_sys_time_get();
}

int k_thread_spawn(struct k_thread *thread, const char *name, uint32_t *stack, uint32_t stack_size, \
                   k_thread_entry_t fn, void *arg, int prio)
{
    int ret;

    if (stack == NULL || thread == NULL) {
        BT_ERR("thread || stack is NULL");
        return -1;
    }

    ret = krhino_task_create(&thread->task, name, arg, prio, 0, stack, stack_size, fn, 1);

    if (ret) {
        BT_ERR("creat task %s fail %d\n", name, ret);
        return ret;
    }

    return ret;
}


int k_yield(void)
{
    return 0;
}

unsigned int irq_lock(void)
{
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    return cpsr;
}

void irq_unlock(unsigned int key)
{
    CPSR_ALLOC();
    cpsr = key;
    RHINO_CPU_INTRPT_ENABLE();
}

void _SysFatalErrorHandler(unsigned int reason, const void *pEsf) {};

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args)
{
    int ret;
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,handle %p,args %p", timer, handle, args);
    timer->handler = handle;
    timer->args    = args;
    timer->timeout  = 0;
    ret =
        krhino_timer_create(&timer->timer, "AOS", (timer_cb_t)(timer->handler),
                            krhino_ms_to_ticks(1000), 0, args, 0);

    if (ret) {
        BT_DBG("fail to create a timer");
    }
}

void k_timer_start(k_timer_t *timer, uint32_t timeout)
{
    int ret;
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,timeout %u", timer, timeout);
    timer->timeout  = timeout;
    timer->start_ms = aos_now_ms();

    ret = krhino_timer_stop(&timer->timer);

    if (ret) {
        BT_ERR("fail to stop timer");
    }

    ret = krhino_timer_change(&timer->timer, krhino_ms_to_ticks(timeout), 0);

    if (ret) {
        BT_ERR("fail to change timeout");
    }

    ret = krhino_timer_start(&timer->timer);

    if (ret) {
        BT_ERR("fail to start timer");
    }
}

void k_timer_stop(k_timer_t *timer)
{
    int ret;
    ASSERT(timer, "timer is NULL");

    /**
     * Timer may be reused, so its timeout value
     * should be cleared when stopped.
    */
    if (!timer->timeout) {
        return;
    }

    BT_DBG("timer %p", timer);
    ret = krhino_timer_stop(&timer->timer);

    if (ret) {
        BT_ERR("fail to stop timer");
    }

    timer->timeout = 0;
}

void k_sleep(int32_t duration)
{
    aos_msleep(duration);
}

void *k_current_get(void)
{
    void *task;
    task = krhino_cur_task_get();
    return task;
}

bool k_timer_is_started(k_timer_t *timer)
{
    ASSERT(timer, "timer is NULL");

    return timer->timeout ? true : false;
}

const char *log_strdup(const char *str)
{
    return str;
}

void *k_malloc(bt_u32_t size)
{
    return aos_malloc(size);
}



