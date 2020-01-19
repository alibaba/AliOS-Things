/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <aos/kernel.h>
#include "osal.h"

#define MESH_TASK_NAME_PREFIX  "umesh2_%d"

#define TASK_NAME_MAX_LEN      32  
#define  PLATFORM_WAIT_INFINITE (uint32_t)-1
int umesh_task_start( task_func cb,void *arg, int stack_size,enum umesh_task_prio  prio)
{
    aos_task_t task_handle ;
    char name[TASK_NAME_MAX_LEN + 1] = {0};
    static uint8_t cnt =0;
    snprintf(name, TASK_NAME_MAX_LEN, MESH_TASK_NAME_PREFIX, cnt++);
    return aos_task_new_ext(&task_handle,  name, cb, arg, stack_size, prio);
}

void *umesh_mutex_new(void)
{
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void umesh_mutex_free(void *mutex)
{
    if (NULL == mutex) {
        return;
    }
     aos_mutex_free((aos_mutex_t *)&mutex);
}

int umesh_mutex_lock(void *mutex)
{
    if (NULL == mutex) {
        
    }
    return aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
}

int umesh_mutex_unlock( void *mutex)
{
    if (NULL == mutex) {
        
    }
    return aos_mutex_unlock((aos_mutex_t *)&mutex);
}

void *umesh_semaphore_new(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem.hdl;
}

void umesh_semaphore_free(void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
}

void umesh_semaphore_post(void *sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

int umesh_semaphore_wait( void *sem,  uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        return aos_sem_wait((aos_sem_t *)&sem, AOS_WAIT_FOREVER);
    } else {
        return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
    }
}

void *umesh_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void umesh_free(void *ptr)
{
    aos_free(ptr);
}

void umesh_srand(uint32_t seed){
    aos_srand(seed);
}

uint32_t umesh_rand(){
    return (uint32_t)aos_rand();
}

uint64_t umesh_now_ms(){
    return aos_now_ms();
}

void umesh_msleep(uint32_t ms){
    aos_msleep(ms);
}