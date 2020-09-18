/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include "bt_hal_os.h"

int aos_timer_change_once(aos_timer_t *timer, int ms);
int aos_timer_is_valid(aos_timer_t *timer);

int hal_os_thread_new(hal_task_t *p_task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    return aos_task_new_ext(p_task, name, fn, arg, stack_size, prio);
}

int hal_os_thread_delete(const char *name)
{
    return aos_task_delete(name);
}

int hal_os_create_timer(hal_timer_t *timer_hdl, void (*fn)(void *, void *), void *args, int32_t timer_expire)
{
    return aos_timer_new_ext(timer_hdl, fn, args, timer_expire == 0 ? 0xFFFFFFUL : timer_expire, 0, 0);
}

int hal_os_timer_start(hal_timer_t *timer_hdl)
{
    return aos_timer_start(timer_hdl);
}

int hal_os_timer_stop(hal_timer_t *timer_hdl)
{
    return aos_timer_stop(timer_hdl);
}

int hal_os_timer_is_valid(void *timer)
{
    return aos_timer_is_valid(timer);
}

int hal_os_timer_change(void *timer, int ms)
{
    return aos_timer_change(timer, ms);
}
int hal_os_timer_change_once(void *timer, int ms)
{
    return aos_timer_change_once(timer, ms);
}

void hal_os_free_timer(void *timer)
{
    aos_timer_free(timer);
}

int hal_os_sem_new(hal_sem_t *sem, uint32_t max_count, uint32_t init_count)
{
    int ret = -1;

    if (sem) {
        ret = aos_sem_new((aos_sem_t*)sem, init_count);
    }

    return ret;
}

void hal_os_sem_give(hal_sem_t *sem)
{
    aos_sem_signal((aos_sem_t*)sem);
}

int hal_os_sem_take(hal_sem_t *sem, uint32_t timeout)
{
    return aos_sem_wait((aos_sem_t*)sem, timeout);
}

void hal_os_sem_free(hal_sem_t *sem)
{
    aos_sem_free((aos_sem_t*)sem);
}

int hal_os_sem_is_valid(hal_sem_t *sem)
{
    return aos_sem_is_valid((aos_sem_t*)sem);
}

int hal_os_queue_new(hal_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    return aos_queue_new(queue, buf, size, max_msg);
}

void hal_os_queue_free(hal_queue_t *queue)
{
    aos_queue_free(queue);
}

int hal_os_queue_send(hal_queue_t *queue, void *msg, unsigned int size)
{
    return aos_queue_send(queue, msg, size);
}

int hal_os_queue_recv(hal_queue_t *queue, unsigned int timeout, void *msg,
                   unsigned int *size)
{
    return aos_queue_recv(queue, timeout, msg, size);
}

void hal_os_delay(int ms)
{
    aos_msleep(ms);
}

long long hal_now_ms(void)
{
    return aos_now_ms();
}

void * hal_malloc(int size)
{
    return aos_malloc(size);
}

void hal_free(void *data)
{
    aos_free(data);
}
