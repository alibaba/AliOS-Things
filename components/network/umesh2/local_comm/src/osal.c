/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <aos/kernel.h>
#include "osal.h"

#define MESH_TASK_NAME_PREFIX  "umesh2_s_%d"

#define TASK_NAME_MAX_LEN      32
#define  PLATFORM_WAIT_INFINITE (uint32_t)-1
int hal_task_start(task_func cb, void *arg, int stack_size, enum hal_task_prio  prio)
{
    aos_task_t task_handle ;
    char name[TASK_NAME_MAX_LEN + 1] = {0};
    static uint8_t cnt = 0;
    snprintf(name, TASK_NAME_MAX_LEN, MESH_TASK_NAME_PREFIX, cnt++);
    return aos_task_new_ext(&task_handle,  name, cb, arg, stack_size, prio);
}

void *hal_mutex_new(void)
{
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void hal_mutex_free(void *mutex)
{
    if (NULL == mutex) {
        return;
    }
    aos_mutex_free((aos_mutex_t *)&mutex);
}

int hal_mutex_lock(void *mutex)
{
    if (NULL == mutex) {

    }
    return aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
}

int hal_mutex_unlock(void *mutex)
{

    if (NULL == mutex) {

    }

    return aos_mutex_unlock((aos_mutex_t *)&mutex);
}

void *hal_semaphore_new(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem.hdl;
}

void hal_semaphore_free(void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
}

void hal_semaphore_post(void *sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

int hal_semaphore_wait(void *sem,  uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        return aos_sem_wait((aos_sem_t *)&sem, AOS_WAIT_FOREVER);
    } else {
        return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
    }
}

void *hal_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void hal_free(void *ptr)
{
    aos_free(ptr);
}

void hal_srand(uint32_t seed)
{
    aos_srand(seed);
}

uint32_t hal_rand()
{
    return (uint32_t)aos_rand();
}

uint64_t hal_now_ms()
{
    return aos_now_ms();
}

void hal_msleep(uint32_t ms)
{
    aos_msleep(ms);
}


void *hal_queue_new(void *buf, uint32_t size, int max_msg)
{
    static aos_queue_t queue;

    int ret = aos_queue_new(&queue, buf, size, max_msg);
    if (ret == 0) {
        return &queue;
    } else {
        return NULL;
    }
}

void hal_queue_free(void *queue)
{
    aos_queue_free((aos_queue_t *)queue);
}

int hal_queue_send(void *queue, void *msg, uint32_t size)
{
    return  aos_queue_send((aos_queue_t *)queue, msg, size);
}

int hal_queue_recv(void *queue, uint32_t ms, void *msg, unsigned int *size)
{
    return aos_queue_recv((aos_queue_t *)queue,  ms, msg, size);
}
