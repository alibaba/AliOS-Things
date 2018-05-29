/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <misc/util.h>
#include <misc/dlist.h>

#include <aos/aos.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)

#include <common/log.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "atomic.h"

#ifdef VCALL_RHINO
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
#endif

void k_queue_init(struct k_queue *queue)
{
    void *msg_start;
    int size = 20;
    int stat;
    uint8_t blk_size = sizeof(void *) + 1;

    msg_start = (void*)aos_malloc(size * blk_size);
    assert(msg_start);

    queue->_queue = (aos_queue_t *)aos_malloc(sizeof(aos_queue_t));
    assert(queue->_queue);

    stat = aos_queue_new(queue->_queue, msg_start, size * blk_size, sizeof(void *));
    assert(stat == 0);

    sys_dlist_init(&queue->poll_events);
}

void k_queue_cancel_wait(struct k_queue *queue)
{
}

void k_queue_insert(struct k_queue *queue, void *prev, void *data)
{
    int ret;

    ret = aos_queue_send(queue->_queue, &data, sizeof(void *));
    assert(ret == 0);
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

    for (buf_tail = (struct net_buf *)head; buf_tail; buf_tail = buf_tail->frags) {
        k_queue_append(queue, buf_tail);
    }
}

void *k_queue_get(struct k_queue *queue, s32_t timeout)
{
    void *msg = NULL;
    unsigned int t = timeout;
    unsigned int len;

    if (timeout == K_FOREVER) {
        t = AOS_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = AOS_NO_WAIT;
    }

    aos_queue_recv(queue->_queue, t, &msg, &len);
    return msg;
}

int k_queue_is_empty(struct k_queue *queue)
{
#ifdef VCALL_RHINO
    kbuf_queue_t *k_queue = queue->_queue->hdl;

    return k_queue->cur_num? 0: 1;
#endif
    return 0;
}

int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit)
{
    int ret;

    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = aos_sem_new(&sem->sem, initial_count);
    sys_dlist_init(&sem->poll_events);
    return ret;
}

int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    unsigned int t = timeout;

    if (timeout == K_FOREVER) {
        t = AOS_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = AOS_NO_WAIT;
    }
    return aos_sem_wait(&sem->sem, t);
}

int k_sem_give(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    aos_sem_signal(&sem->sem);
    return 0;
}

int k_sem_delete(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    aos_sem_free(&sem->sem);
    return 0;
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
#ifdef VCALL_RHINO
    sem_count_t count;
    ksem_t *k_sem = (ksem_t *)&(sem->sem.hdl);

    krhino_sem_count_get(k_sem, &count);
    return (int)count;
#endif
    return 0;
}


void k_mutex_init(struct k_mutex *mutex)
{

    if (NULL == mutex) {
        BT_ERR("mutex is NULL\n");
        return ;
    }

    aos_mutex_new(&mutex->mutex);
    sys_dlist_init(&mutex->poll_events);
    return ;
}


int k_mutex_lock(struct k_mutex *mutex, s32_t timeout)
{
    unsigned int t = timeout;

    if (timeout == K_FOREVER) {
        t = AOS_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = AOS_NO_WAIT;
    }
    return aos_mutex_lock(&mutex->mutex, t);
    
}

void k_mutex_unlock(struct k_mutex *mutex)
{
    if (NULL == mutex) {
        BT_ERR("mutex is NULL\n");
        return;
    }

    aos_mutex_unlock(&mutex->mutex);
    return ;
}


int64_t k_uptime_get()
{
    return aos_now_ms();
}

typedef void (*task_entry_t)(void *args);
int k_thread_create(struct k_thread *new_thread, k_thread_stack_t *stack,
                    size_t stack_size, k_thread_entry_t entry,
                    void *p1, void *p2, void *p3,
                    int prio, u32_t options, s32_t delay)
{
    int ret;

    ret = aos_task_new_ext(&(new_thread->task), "ble", (task_entry_t)entry, p1, stack_size, prio);
    if (ret) {
        SYS_LOG_ERR("create ble task fail\n");
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

void _SysFatalErrorHandler(unsigned int reason,
                           const void *pEsf)
{
};

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,handle %p,args %p", timer, handle, args);
    timer->handler = handle;
    timer->args = args;
    aos_timer_new_ext(&timer->timer, timer->handler, args, 1000, 0, 0);
}

void k_timer_start(k_timer_t *timer, uint32_t timeout)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,timeout %u", timer, timeout);
    timer->timeout = timeout;
    timer->start_ms = aos_now_ms();
    aos_timer_stop(&timer->timer);
    aos_timer_change(&timer->timer, timeout);
    aos_timer_start(&timer->timer);
}

void k_timer_stop(k_timer_t *timer)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p", timer);
    aos_timer_stop(&timer->timer);
}

void k_sleep(s32_t duration)
{
    aos_msleep(duration);
}

unsigned int find_msb_set(u32_t data)
{
    uint32_t count = 0;
    uint32_t mask = 0x80000000;

    if (!data) {
        return 0;
    }
    while((data & mask) == 0) {
        count += 1u;
        mask = mask >> 1u;
    }
    return (32 - count);
}

unsigned int find_lsb_set(u32_t data)
{
    uint32_t count = 0;
    uint32_t mask = 0x00000001;

    if (!data) {
        return 0;
    }
    while((data & mask) == 0) {
        count += 1u;
        mask = mask >> 1u;
    }
    return (count);
}
