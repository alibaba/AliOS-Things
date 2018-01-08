/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <string.h>
#include <misc/util.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)

#include <bluetooth/log.h>
#include "csp.h"
#include "freertos/timers.h"

#if defined(__cplusplus)
extern "C"
{
#endif

void k_lifo_init(struct k_lifo *lifo, const char *name, void **start, size_t msg_num)
{
    k_mbox_t *mbox;
    UNUSED(name);
    UNUSED(start);
    k_mbox_new(&mbox, msg_num);
    lifo->_queue = mbox;
    BT_DBG("lifo %p,lifo->_queue %p\n", lifo, lifo->_queue);
}

void  k_lifo_put(struct k_lifo *lifo, void *data)
{
    BT_DBG("lifo %p,lifo->_queue %p,data %p\n", lifo, lifo->_queue, data);
    k_mbox_post((k_mbox_t *)lifo->_queue, data);
}

void *k_lifo_get(struct k_lifo *lifo, uint32_t timeout)
{
    void *msg = NULL;
    BT_DBG("lifo %p,lifo->_queue %p\n", lifo, lifo->_queue);

    if (K_FOREVER == timeout) {
        k_mbox_fetch((k_mbox_t *)lifo->_queue, &msg, 0);
    } else if (K_NO_WAIT == timeout) {
        k_mbox_tryfetch((k_mbox_t *)lifo->_queue, &msg);
    } else {
        k_mbox_fetch((k_mbox_t *)lifo->_queue, &msg, timeout);
    }

    return msg;
}

void k_fifo_init(struct k_fifo *fifo, const char *name, void **start, size_t msg_num)
{
    k_mbox_t *mbox;
    UNUSED(name);
    UNUSED(start);
    BT_DBG("");
    k_mbox_new(&mbox, msg_num);
    fifo->_queue = mbox;
    BT_DBG("fifo %p,fifo->_queue %p\n", fifo, fifo->_queue);
}

void *k_fifo_get(struct k_fifo *fifo, uint32_t timeout)
{
    void *msg = NULL;
    BT_DBG("fifo %p,fifo->_queue %p\n", fifo, fifo->_queue);

    if (K_FOREVER == timeout) {
        k_mbox_fetch((k_mbox_t *)fifo->_queue, &msg, 0);
    } else if (K_NO_WAIT == timeout) {
        k_mbox_tryfetch((k_mbox_t *)fifo->_queue, &msg);
    } else {
        k_mbox_fetch((k_mbox_t *)fifo->_queue, &msg, timeout);
    }

    return msg;
}

void k_fifo_put(struct k_fifo *fifo, void *msg)
{
    BT_DBG("fifo %p,fifo->_queue %p,msg %p\n", fifo, fifo->_queue, msg);
    k_mbox_post((k_mbox_t *)fifo->_queue, msg);
}

int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit)
{
    int ret;
    csp_sem_t *psem = (csp_sem_t *)malloc(sizeof(csp_sem_t));

    if (NULL == sem || NULL == psem) {
        BT_ERR("sem init fail\n");
        return -EINVAL;
    }

    ret = csp_sem_new(psem, initial_count);

    if (ret) {
        BT_ERR("sem init fail\n");
        return ret;
    }

    sem->sem = psem;
    BT_DBG("sem->hdl %p,count %d", psem->hdl, initial_count);
    return 0;
}

int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    csp_sem_t *psem;

    if (NULL == sem || NULL == sem->sem) {
        BT_ERR("sem give fail\n");
        return -EINVAL;
    }

    psem = (csp_sem_t *)sem->sem;
    BT_DBG("sem->hdl %p,timeout %d", psem->hdl, timeout);

    if (timeout == K_FOREVER) {
        timeout = 0;
    } else if (K_NO_WAIT == timeout) {
        timeout = 1;
    }

    return csp_sem_wait(*psem, timeout);
}

int k_sem_give(struct k_sem *sem)
{
    csp_sem_t *psem;

    if (NULL == sem || NULL == sem->sem) {
        BT_ERR("sem give fail\n");
        return -EINVAL;
    }

    psem = (csp_sem_t *)sem->sem;
    BT_DBG("sem->hdl %p", psem->hdl);
    csp_sem_signal(*psem);
    return 0;
}

int k_sem_delete(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    csp_sem_free(sem->sem);
    free(sem->sem);
    sem->sem = NULL;
    return 0;
}

int64_t k_uptime_get()
{
    return csp_now();
}

int k_thread_spawn(const char *name, uint32_t *stack, uint32_t stack_size, \
                   k_thread_entry_t fn, void *arg, int prio)

{
    int ret;
    ret = csp_task_new_ext(name, fn, arg, stack_size * sizeof(int), prio);

    if (ret) {
        BT_ERR("creat task %s fail\n", name);
    }

    return ret;
}

int k_yield()
{
    return 0;
}


unsigned int irq_lock()
{
    return 0;
}

void irq_unlock(unsigned int key)
{
}

void vTimerCallback(TimerHandle_t pxTimer)
{
    k_timer_t *timer = pvTimerGetTimerID(pxTimer);
    BT_INFO("px timer %p,timer %p,timer handler %p", pxTimer, timer, timer->handler);

    if (timer && timer->handler) {
        timer->handler(timer->args);
    }
}

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args)
{
    TimerHandle_t timer_handle;
    ASSERT(timer, "timer is NULL");

    timer_handle = xTimerCreate("", 1000 / portTICK_RATE_MS, 0, (void *)timer, vTimerCallback);

    if (!timer_handle) {
        BT_ERR("timer init fail");
        return;
    }

    timer->handler = handle;
    timer->args = args;
    timer->timer = timer_handle;
}
void k_timer_start(k_timer_t *timer, uint32_t timeout)
{
    ASSERT(timer, "timer is NULL");

    if (timer->timer) {
        BT_INFO("px timer %p,timer %p,timer handler %p", timer->timer, timer, timer->handler);

        if (xTimerIsTimerActive(timer->timer)) {
            xTimerStop(timer->timer, 0);
        }

        if (xTimerChangePeriod(timer->timer, timeout / portTICK_PERIOD_MS, 100) != pdPASS) {
            BT_ERR("timer start fail");
            return;
        }
    } else {
        BT_ERR("timer creat fail");
    }
}
void k_timer_stop(k_timer_t *timer)
{
    ASSERT(timer, "timer is NULL");

    if (timer->timer) {
        BT_INFO("px timer %p,timer %p", timer->timer, timer);
        xTimerStop(timer->timer, 0);
    }
}


#if defined(__cplusplus)
}
#endif
