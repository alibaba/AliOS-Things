/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "aos/kernel.h"
#include "at_opts.h"

void *atpsr_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void atpsr_free(void *ptr)
{
    aos_free(ptr);
}

#if ATPSR_SINGLE_TASK
void *atpsr_mutex_new(void)
{
    return (void*)1;
}

void atpsr_mutex_free(void *mutex)
{
    return;
}

void atpsr_mutex_lock(void *mutex)
{
    return;
}

void atpsr_mutex_unlock(void *mutex)
{
    return;
}

void *atpsr_sem_new(void)
{
    return (void*)1;
}

void atpsr_sem_free(void *sem)
{
    return;
}

void atpsr_sem_signal(void *sem)
{
    return;
}

int atpsr_sem_wait(void *sem, uint32_t timeout_ms)
{
    return 0;
}
#else
void *atpsr_mutex_new(void)
{
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void atpsr_mutex_free(void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_free((aos_mutex_t *)&mutex);
    }
}

void atpsr_mutex_lock(void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    }
}

void atpsr_mutex_unlock(void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_unlock((aos_mutex_t *)&mutex);
    }
}

void *atpsr_sem_new(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem.hdl;
}

void atpsr_sem_free(void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
}

void atpsr_sem_signal(void *sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

int atpsr_sem_wait(void *sem, uint32_t timeout_ms)
{
    return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
}

int atpsr_task_new_ext(void *task, char *name, void (*fn)(void *),
                       void *arg, int stack_size, int prio)
{
    if (task == NULL)
        return -1;

    return aos_task_new_ext((aos_task_t *)&task, name, fn, arg, stack_size,
                            prio);
}
#endif
