/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "unity.h"
#include "espos_spinlock.h"
#include "espos_task.h"
#include "espos_queue.h"
#include "espos_semaphore.h"
#include "test_espos_run_in_isr.h"

#define ESPOS_SPINLOCK_INVALID_HANDLE NULL

#define ESPOS_SPINLOCK_ADD_TEST_COUNT 0xFFFF

static espos_sem_t sync_semaphore;
static espos_queue_t isr_queue;
static espos_spinlock_t global_spinlock;
static int count;

TEST_CASE("ESPOS spinlock create delete test", "[espos]")
{
    espos_spinlock_t spinlock;

    for (int i = 0; i < 1000; i++) {
        if (espos_spinlock_create(&spinlock) != 0) {
            TEST_ASSERT(0);
        }
        if (espos_spinlock_del(&spinlock) != 0) {
            TEST_ASSERT(0);
        }
    }
}

static void spinlock_test_task(void *p)
{
    for (int i = 0; i < ESPOS_SPINLOCK_ADD_TEST_COUNT; i++) {
        TEST_ASSERT(espos_spinlock_lock(&global_spinlock) == 0);
        count++;
        TEST_ASSERT(espos_spinlock_unlock(&global_spinlock) == 0);
    }
    TEST_ASSERT(espos_sem_give(sync_semaphore) == 0);
    TEST_ASSERT(espos_task_del(0) == 0);
}

TEST_CASE("ESPOS spinlock lock unlock test", "[espos]")
{
    espos_task_t new_task_handle;

    TEST_ASSERT(espos_spinlock_create(&global_spinlock) == 0);

    TEST_ASSERT(espos_sem_create(&sync_semaphore, 2, 0) == 0);
    count = 0;

    TEST_ASSERT(espos_task_create(&new_task_handle, "spinlock_test_task1", NULL, 1,
                      0, 1024, spinlock_test_task, 0) == 0);
    TEST_ASSERT(espos_task_create(&new_task_handle, "spinlock_test_task2", NULL, 1,
                      0, 1024, spinlock_test_task, 0) == 0);

    // wait task exit
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_del(sync_semaphore) == 0);

    TEST_ASSERT(count == (ESPOS_SPINLOCK_ADD_TEST_COUNT << 1));
    TEST_ASSERT(espos_spinlock_del(&global_spinlock) == 0);
}

TEST_CASE("ESPOS spinlock trylock test", "[espos]")
{
    espos_spinlock_t spinlock;

    TEST_ASSERT(espos_spinlock_create(&spinlock) == 0);

    TEST_ASSERT(espos_spinlock_trylock(&spinlock) == 0);
    TEST_ASSERT(espos_spinlock_trylock(&spinlock) == -EAGAIN);

    TEST_ASSERT(espos_spinlock_unlock(&spinlock) == 0);

    TEST_ASSERT(espos_spinlock_del(&spinlock) == 0);
}

static void IRAM_ATTR spinlock_isr_test(void)
{
    uint8_t msg = 0;

    if (espos_spinlock_lock(&global_spinlock) != 0
            || espos_spinlock_unlock(&global_spinlock) != 0) {
        msg = 1;
    }
    TEST_ASSERT(espos_queue_send(isr_queue, &msg, ESPOS_MAX_DELAY) == 0);
}

TEST_CASE("ESPOS spinlock use in ISR test", "[espos]")
{
    uint8_t msg;
    TEST_ASSERT(espos_queue_create(&isr_queue, 1, 1) == 0);
    TEST_ASSERT(espos_spinlock_create(&global_spinlock) == 0);
    espos_test_run_in_isr(spinlock_isr_test);
    TEST_ASSERT(espos_queue_recv(isr_queue, &msg, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(msg == 0);
    TEST_ASSERT(espos_queue_del(isr_queue) == 0);
    TEST_ASSERT(espos_spinlock_del(&global_spinlock) == 0);
}

TEST_CASE("ESPOS spinlock API invalid parameter test", "[espos]")
{
    TEST_ASSERT(espos_spinlock_create(NULL) == -EINVAL);

    TEST_ASSERT(espos_spinlock_lock(NULL) == -EINVAL);

    TEST_ASSERT(espos_spinlock_trylock(NULL) == -EINVAL);

    TEST_ASSERT(espos_spinlock_unlock(NULL) == -EINVAL);

    TEST_ASSERT(espos_spinlock_del(ESPOS_SPINLOCK_INVALID_HANDLE) == -EINVAL);
}
