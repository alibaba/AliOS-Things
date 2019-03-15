/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "unity.h"
#include "espos_mutex.h"
#include "espos_semaphore.h"
#include "espos_task.h"
#include "espos_queue.h"
#include "test_espos_run_in_isr.h"

#define ESPOS_MUTEX_ADD_TEST_COUNT 0xFFFF
#define ESPOS_MUTEX_CREATE_INVALID ESPOS_MUTEX_NORMAL + 10
#define ESPOS_MUTEX_INVALID_HANDLE 0

static espos_sem_t sync_semaphore;
static espos_mutex_t global_mutex;
static espos_queue_t isr_queue;
static int count;

TEST_CASE("ESPOS mutex create delete test", "[espos]")
{
    uint32_t i;
    espos_queue_t mutex = 0;

    for (i = 0; i < 1000; i++) {
        if (espos_mutex_create(&mutex, ESPOS_MUTEX_NORMAL) != 0) {
            TEST_ASSERT(0);
            break;
        }
        if (espos_mutex_del(mutex) != 0) {
            TEST_ASSERT(0);
            break;
        }
    }
}

static void mutex_test_task(void *p)
{
    uint32_t i;

    for (i = 0; i < ESPOS_MUTEX_ADD_TEST_COUNT; i++) {
        TEST_ASSERT(espos_mutex_lock(global_mutex, ESPOS_MAX_DELAY) == 0);
        count++;
        TEST_ASSERT(espos_mutex_unlock(global_mutex) == 0);
    }
    TEST_ASSERT(espos_sem_give(sync_semaphore) == 0);
    TEST_ASSERT(espos_task_del(0) == 0);
}

TEST_CASE("ESPOS mutex lock unlock test", "[espos]")
{
    espos_task_t new_task_handle;

    TEST_ASSERT(espos_mutex_create(&global_mutex, ESPOS_MUTEX_NORMAL) == 0);

    TEST_ASSERT(espos_sem_create(&sync_semaphore, 2, 0) == 0);
    count = 0;

    TEST_ASSERT(espos_task_create(&new_task_handle, "mutex_test_task1", NULL, 1,
                      0, 2048, mutex_test_task, 0) == 0);
    TEST_ASSERT(espos_task_create(&new_task_handle, "mutex_test_task2", NULL, 1,
                      0, 2048, mutex_test_task, 0) == 0);

    // wait task exit
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_del(sync_semaphore) == 0);

    TEST_ASSERT(count == (ESPOS_MUTEX_ADD_TEST_COUNT << 1));
    TEST_ASSERT(espos_mutex_del(global_mutex) == 0);
}

static void mutex_lock_test_task(void *p)
{
    TEST_ASSERT(espos_mutex_lock(global_mutex, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_mutex_unlock(global_mutex) == 0);
    TEST_ASSERT(espos_sem_give(sync_semaphore) == 0);
    TEST_ASSERT(espos_task_del(0) == 0);
}

TEST_CASE("ESPOS delete mutex in use test", "[espos]")
{
    // TODO: this case is RTOS dependent
    espos_task_t new_task_handle;

    TEST_ASSERT(espos_mutex_create(&global_mutex, ESPOS_MUTEX_NORMAL) == 0);

    TEST_ASSERT(espos_sem_create(&sync_semaphore, 1, 0) == 0);

    TEST_ASSERT(espos_mutex_lock(global_mutex, ESPOS_MAX_DELAY) == 0);

    TEST_ASSERT(espos_task_create(&new_task_handle, "mutex_lock_test_task", NULL, ESPOS_TASK_PRIO_MAX,
                      0, 2048, mutex_lock_test_task, 0) == 0);


    TEST_ASSERT(espos_mutex_unlock(global_mutex) == 0);

    // wait task exit
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);

    TEST_ASSERT(espos_mutex_del(global_mutex) == 0);
    TEST_ASSERT(espos_sem_del(sync_semaphore) == 0);
}

static void mutex_unlock_test_task(void *p)
{
    TEST_ASSERT(espos_mutex_unlock(global_mutex) == -EPERM);
    TEST_ASSERT(espos_sem_give(sync_semaphore) == 0);
    TEST_ASSERT(espos_task_del(0) == 0);
}

TEST_CASE("ESPOS mutex unlock by different task test", "[espos]")
{
    espos_task_t new_task_handle;

    TEST_ASSERT(espos_mutex_create(&global_mutex, ESPOS_MUTEX_NORMAL) == 0);

    TEST_ASSERT(espos_sem_create(&sync_semaphore, 1, 0) == 0);

    TEST_ASSERT(espos_mutex_lock(global_mutex, ESPOS_MAX_DELAY) == 0);

    TEST_ASSERT(espos_task_create(&new_task_handle, "mutex_unlock_test_task", NULL, ESPOS_TASK_PRIO_MAX,
                      0, 1024, mutex_unlock_test_task, 0) == 0);

    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);

    TEST_ASSERT(espos_mutex_del(global_mutex) == 0);

    TEST_ASSERT(espos_sem_del(sync_semaphore) == 0);
}

TEST_CASE("ESPOS mutex lock timeout test", "[espos]")
{
    espos_queue_t mutex;

    TEST_ASSERT(espos_mutex_create(&mutex, ESPOS_MUTEX_NORMAL) == 0);

    TEST_ASSERT(espos_mutex_lock(mutex, ESPOS_NO_DELAY) == 0);
    TEST_ASSERT(espos_mutex_lock(mutex, ESPOS_NO_DELAY) == -ETIMEDOUT);

    TEST_ASSERT(espos_mutex_del(mutex) == 0);
}

static void mutex_lock_hold_test_task(void *p)
{
    espos_sem_t sem = (espos_sem_t) p;

    TEST_ASSERT(espos_mutex_lock(global_mutex, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_give(sync_semaphore) == 0);
    TEST_ASSERT(espos_sem_take(sem, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_del(sem) == 0);
    TEST_ASSERT(espos_task_del(0) == 0);
}

TEST_CASE("ESPOS mutex lock get holder test", "[espos]")
{
    espos_task_t new_task_handle;
    espos_sem_t sem;

    TEST_ASSERT(espos_mutex_create(&global_mutex, ESPOS_MUTEX_NORMAL) == 0);
    TEST_ASSERT(espos_sem_create(&sync_semaphore, 1, 0) == 0);
    TEST_ASSERT(espos_sem_create(&sem, 1, 0) == 0);

    TEST_ASSERT(espos_mutex_lock(global_mutex, ESPOS_NO_DELAY) == 0);
    TEST_ASSERT(espos_mutex_get_holder(global_mutex) == espos_task_get_current());
    TEST_ASSERT(espos_mutex_unlock(global_mutex) == 0);

    TEST_ASSERT(espos_task_create(&new_task_handle, "mutex_lock_hold_test_task", (void *)sem, ESPOS_TASK_PRIO_MAX,
                                  0, 2048, mutex_lock_hold_test_task, 0) == 0);

    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_mutex_get_holder(global_mutex) == new_task_handle);
    TEST_ASSERT(espos_sem_give(sem) == 0);

    TEST_ASSERT(espos_mutex_del(global_mutex) == 0);
}

TEST_CASE("ESPOS mutex recursive lock unlock test", "[espos]")
{
    //TODO: to add this case if recursive lock is supported
}

static void IRAM_ATTR mutex_isr_test(void)
{
    uint8_t msg = 0;

    if (espos_mutex_lock(global_mutex, 0) != -EINTR
            || espos_mutex_unlock(global_mutex) != -EINTR) {
        msg = 1;
    }
    TEST_ASSERT(espos_queue_send(isr_queue, &msg, ESPOS_MAX_DELAY) == 0);
}

TEST_CASE("ESPOS use mutex in ISR test", "[espos]")
{
    uint8_t msg;

    TEST_ASSERT(espos_queue_create(&isr_queue, 1, 1) == 0);
    TEST_ASSERT(espos_mutex_create(&global_mutex, ESPOS_MUTEX_NORMAL) == 0);

    espos_test_run_in_isr(mutex_isr_test);
    TEST_ASSERT(espos_queue_recv(isr_queue, &msg, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(msg == 0);
    TEST_ASSERT(espos_mutex_del(global_mutex) == 0);
    TEST_ASSERT(espos_queue_del(isr_queue) == 0);
}

TEST_CASE("ESPOS mutex API invalid parameter test", "[espos]")
{
    espos_queue_t mutex;

    TEST_ASSERT(espos_mutex_create(NULL, ESPOS_MUTEX_NORMAL) == -EINVAL);
    TEST_ASSERT(espos_mutex_create(&mutex, ESPOS_MUTEX_CREATE_INVALID) == -EINVAL);

    TEST_ASSERT(espos_mutex_lock(ESPOS_MUTEX_INVALID_HANDLE, 0) == -EINVAL);

    TEST_ASSERT(espos_mutex_unlock(ESPOS_MUTEX_INVALID_HANDLE) == -EINVAL);

    TEST_ASSERT(espos_mutex_del(ESPOS_MUTEX_INVALID_HANDLE) == -EINVAL);
}
