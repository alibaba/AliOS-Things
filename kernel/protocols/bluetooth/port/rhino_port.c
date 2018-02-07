/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <zephyr.h>
#include <misc/util.h>
#include <misc/dlist.h>

#include <aos/aos.h>
#include "k_sys.h"
#include "k_task.h"
#include "k_stats.h"
#include "k_sem.h"
#include "k_queue.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)

#include <common/log.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <k_config.h>
#include <k_default_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_critical.h>
#include <k_sys.h>
#include <k_bitmap.h>
#include <k_list.h>
#include <k_obj.h>
#include <k_sched.h>
#include <k_task.h>
#include <k_ringbuf.h>
#include <k_queue.h>
#include <k_buf_queue.h>
#include <k_sem.h>
#include <k_task_sem.h>
#include <k_mutex.h>
#include <k_timer.h>
#include <k_time.h>
#include <k_event.h>
#include <k_stats.h>
#include <k_mm_debug.h>
#include <k_mm_blk.h>
#include <k_mm_region.h>
#include <k_mm.h>
#include <k_workqueue.h>
#include <k_internal.h>
#include <k_trace.h>
#include <k_soc.h>
#include <k_hook.h>
#include <port.h>
#include <k_endian.h>
#include "atomic.h"

extern kstat_t krhino_queue_dyn_create(kqueue_t **queue, const name_t *name, size_t msg_num);
extern kstat_t krhino_sem_dyn_create(ksem_t **sem, const name_t *name, sem_count_t count);
extern kstat_t krhino_sem_dyn_del(ksem_t *sem);
extern kstat_t krhino_task_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                      uint8_t pri, tick_t ticks, size_t stack,
                                      task_entry_t entry, uint8_t autorun);

#if defined(__cplusplus)
extern "C"
{
#endif

void k_lifo_init(struct k_lifo *lifo)
{
    kstat_t ret;
    const char *name = "ble_lifo";
    size_t msg_num = 20;

    if (NULL == lifo) {
        BT_ERR("lifo is NULL");
        return;
    }

    ret = krhino_queue_dyn_create(&lifo->_queue, name, msg_num);
    if (RHINO_SUCCESS != ret) {
        BT_ERR("lifo %s %p creat fail,%d\n",name, lifo, ret);
    }

    sys_dlist_init(&lifo->poll_events);

#if LIFO_DEBUG
    lifo->total_count = msg_num;
    lifo->count = 0;
#endif
}

void  k_lifo_put(struct k_lifo *lifo, void *data)
{
    kstat_t ret;

    if (NULL == lifo) {
        BT_ERR("lifo is NULL");
        return;
    }

    ret = krhino_queue_back_send(lifo->_queue, data);

    if (RHINO_SUCCESS != ret) {
#if LIFO_DEBUG
        BT_ERR("send msg to lifo %p count %d,total_count %d,fail,%d",
                lifo, lifo->count, lifo->total_count, ret);
#else
        BT_ERR("send msg to lifo %p fail,%d",lifo, ret);
#endif
        return;
    }
#if LIFO_DEBUG
    lifo->count++;
#endif
}

void *k_lifo_get(struct k_lifo *lifo, tick_t timeout)
{
    void *msg;
    tick_t t;

    if (NULL == lifo) {
        BT_ERR("lifo is NULL");
        return NULL;
    }

    if (timeout == K_FOREVER) {
        t = RHINO_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = RHINO_NO_WAIT;
    } else {
        t = krhino_ms_to_ticks(timeout);
    }

    krhino_queue_recv(lifo->_queue, t, &msg);

#if LIFO_DEBUG
    if (msg)
    {
        lifo->count--;
    }
#endif
    return msg;
}

void k_fifo_init(struct k_fifo *fifo)
{
    kstat_t ret;
    const char *name = "ble_fifo";
    size_t msg_len = 20;

    if (NULL == fifo) {
        BT_ERR("fifo is NULL");
        return;
    }
    ret = krhino_queue_dyn_create(&fifo->_queue, name, msg_len);

    if (ret) {
        BT_ERR("fifo %s %p creat fail,%d\n", name, fifo, ret);
        return;
    }

    sys_dlist_init(&fifo->poll_events);

#if FIFO_DEBUG
    fifo->total_count = msg_len;
    fifo->count = 0;
#endif
}

void *k_fifo_get(struct k_fifo *fifo, tick_t timeout)
{
    void *msg = NULL;
    tick_t t;

    if (NULL == fifo) {
        BT_ERR("fifo is NULL");
        return NULL;
    }

    if (timeout == K_FOREVER) {
        t = RHINO_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = RHINO_NO_WAIT;
    } else {
        t =  krhino_ms_to_ticks(timeout);
    }

    krhino_queue_recv(fifo->_queue, t, &msg);

#if FIFO_DEBUG
    if (msg)
    {
        fifo->count--;
    }
#endif
    return msg;
}

void k_fifo_put(struct k_fifo *fifo, void *msg)
{
    kstat_t ret;

    if (NULL == fifo) {
        BT_ERR("fifo is NULL");
        return;
    }

    ret = krhino_queue_back_send(fifo->_queue, msg);

    if (RHINO_SUCCESS != ret) {
#if FIFO_DEBUG
        BT_ERR("send msg to fifo %p count %d,total_count %d,fail,%d",
                        fifo, fifo->count, fifo->total_count, ret);
#else
        BT_ERR("send msg to fifo %p fail,%d", fifo, ret);
#endif
        return;
    }
#if FIFO_DEBUG
    fifo->count++;
#endif
}

void k_fifo_put_list(struct k_fifo *fifo, void *head, void *tail)
{
    struct net_buf *buf_tail = (struct net_buf *)head;

    for (buf_tail = (struct net_buf *)head; buf_tail; buf_tail = buf_tail->frags) {
        k_fifo_put(fifo, buf_tail);
    }
}

void k_fifo_cancel_wait(struct k_fifo *fifo)
{

}

int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit)
{
    kstat_t ret;

    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = krhino_sem_create(&sem->sem, "", limit);

    if (RHINO_SUCCESS == ret && initial_count != limit) {
        ret = krhino_sem_count_set(&sem->sem, (sem_count_t)initial_count);
    }
    sys_dlist_init(&sem->poll_events);

    return ret;
}

int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    tick_t t;

    if (timeout == K_FOREVER) {
        t = RHINO_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = RHINO_NO_WAIT;
    } else {
        t =  krhino_ms_to_ticks(timeout);
    }

    return krhino_sem_take(&sem->sem, t);
}

int k_sem_give(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    return krhino_sem_give(&sem->sem);
}

int k_sem_delete(struct k_sem *sem)
{
    kstat_t ret = 0;

    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = krhino_sem_del(&sem->sem);

    if (RHINO_SUCCESS != ret) {
        BT_ERR("delete sem fail,%d", ret);
    }

    return ret;
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
    return sem->sem.count;
}

int64_t k_uptime_get()
{
    return krhino_ticks_to_ms(krhino_sys_tick_get());
}

typedef void (*task_entry_t)(void *args);
int k_thread_create(struct k_thread *new_thread, k_thread_stack_t *stack,
                    size_t stack_size, k_thread_entry_t entry,
                    void *p1, void *p2, void *p3,
                    int prio, u32_t options, s32_t delay)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&new_thread->task, "ble-task", p1, prio, 0, \
                                 stack_size, (task_entry_t)entry, 1);

    if (ret) {
        SYS_LOG_ERR("create task ble task fail\n");
        return ret;
    }

    return ret;
}

int k_yield()
{
    return krhino_task_yield();
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

#define ms2tick(ms) \
    ((ms * RHINO_CONFIG_TICKS_PER_SECOND + 999) / 1000)

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,handle %p,args %p", timer, handle, args);
    timer->handler = handle;
    timer->args = args;
    krhino_timer_create(&timer->timer, "bttimer", timer->handler, ms2tick(1000), 0, timer->args, 0);
}

void k_timer_start(k_timer_t *timer, uint32_t timeout)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,timeout %u", timer, timeout);
    timer->timeout = timeout;
    timer->start_ms = aos_now_ms();
    krhino_timer_stop(&timer->timer);
    krhino_timer_change(&timer->timer, ms2tick(timeout), 0);
    krhino_timer_start(&timer->timer);
}

void k_timer_stop(k_timer_t *timer)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p", timer);
    krhino_timer_stop(&timer->timer);
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

#if defined(__cplusplus)
}
#endif
