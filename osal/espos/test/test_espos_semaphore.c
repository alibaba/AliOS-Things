/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "unity.h"
#include "espos_semaphore.h"
#include "espos_queue.h"
#include "espos_task.h"
#include "espos_time.h"
#include "driver/timer.h"
#include "test_espos_run_in_isr.h"

#define ESPOS_SEM_INVALID_HANDLE 0

static espos_sem_t sync_semaphore;
static espos_sem_t global_sema;
static espos_queue_t isr_queue;

TEST_CASE("ESPOS semaphore create delete test", "[espos]")
{
    uint32_t i;
    espos_sem_t sem;

    for (i = 0; i < 1000; i++) {
        if (espos_sem_create(&sem, 1, 1) != 0) {
            TEST_ASSERT(0);
            break;
        }
        if (espos_sem_del(sem) != 0) {
            TEST_ASSERT(0);
            break;
        }
    }
}

static void sema_give_task(void *p)
{
    uint8_t *taken = (uint8_t *) p;

    TEST_ASSERT(espos_sem_give(global_sema) == 0);

    // should switch to recv task and then switch back
    TEST_ASSERT(*taken == 1);

    espos_sem_give(sync_semaphore);
    espos_task_del(0);
}

static void sema_take_task(void *p)
{
    uint8_t *taken = (uint8_t *) p;

    TEST_ASSERT(espos_sem_take(global_sema, ESPOS_MAX_DELAY) == 0);

    // set taken flag
    *taken = 1;

    espos_sem_give(sync_semaphore);
    espos_task_del(0);
}

TEST_CASE("ESPOS semaphore take give test", "[espos]")
{
    espos_task_t new_task_handle;
    uint8_t taken = 0;

    TEST_ASSERT(espos_sem_create(&global_sema, 1, 0) == 0);

    espos_sem_create(&sync_semaphore, 2, 0);

    // recv task with higher priority, create recv task first
    espos_task_create_on_cpu(&new_task_handle, "sema_take_task", &taken, 2,
                             0, 1024, sema_take_task, 0, 0);
    espos_task_create_on_cpu(&new_task_handle, "sema_give_task", &taken, 1,
                             0, 1024, sema_give_task, 0, 0);

    // wait task exit
    espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY);
    espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY);
    espos_sem_del(sync_semaphore);

    TEST_ASSERT(espos_sem_del(global_sema) == 0);
}

TEST_CASE("ESPOS semaphore count test", "[espos]")
{
    uint8_t i;
    espos_sem_t sem;

    TEST_ASSERT(espos_sem_create(&sem, 5, 1) == 0);

    TEST_ASSERT(espos_sem_take(sem, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_trytake(sem) == -ETIMEDOUT);

    for (i = 0; i < 5; i++) {
        TEST_ASSERT(espos_sem_give(sem) == 0);
    }

    TEST_ASSERT(espos_sem_give(sem) == -EAGAIN);

    for (i = 0; i < 5; i++) {
        TEST_ASSERT(espos_sem_trytake(sem) == 0);
    }

    TEST_ASSERT(espos_sem_del(sem) == 0);
}

static void IRAM_ATTR semaphore_isr_test(void)
{
    uint8_t msg = 0;
    espos_sem_t sem;

    if (espos_sem_create(&sem, 2, 1) != -EINTR) {
        msg = 1;
    }

    if (espos_sem_take(global_sema, 0) != 0
            || espos_sem_give(global_sema) != 0
            || espos_sem_del(global_sema) != -EINTR) {
        msg = 1;
    }
    espos_queue_send(isr_queue, &msg, ESPOS_MAX_DELAY);
}

TEST_CASE("ESPOS use semaphore in ISR test", "[espos]")
{
    uint8_t msg;

    espos_queue_create(&isr_queue, 1, 1);
    TEST_ASSERT(espos_sem_create(&global_sema, 2, 1) == 0);

    espos_test_run_in_isr(semaphore_isr_test);
    espos_queue_recv(isr_queue, &msg, ESPOS_MAX_DELAY);
    TEST_ASSERT(msg == 0);
    espos_sem_del(global_sema);
    espos_queue_del(isr_queue);
}

TEST_CASE("ESPOS deleting a semaphore in use test", "[espos]")
{
    // TODO: this case is RTOS dependent
    espos_task_t new_task_handle;
    uint8_t taken = 0;

    TEST_ASSERT(espos_sem_create(&global_sema, 1, 0) == 0);

    espos_sem_create(&sync_semaphore, 1, 0);

    espos_task_create_on_cpu(&new_task_handle, "sema_take_task", &taken, ESPOS_TASK_PRIO_MAX,
                             0, 1024, sema_take_task, 0, 0);

    espos_sem_give(global_sema);

    // wait task exit
    espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY);
    TEST_ASSERT(taken == 1);
    espos_sem_del(sync_semaphore);

    TEST_ASSERT(espos_sem_del(global_sema) == 0);
}

TEST_CASE("ESPOS semaphore API invalid parameter test", "[espos]")
{
    TEST_ASSERT(espos_sem_create(NULL, 5, 1) == -EINVAL);

    TEST_ASSERT(espos_sem_take(ESPOS_SEM_INVALID_HANDLE, ESPOS_MAX_DELAY) == -EINVAL);

    TEST_ASSERT(espos_sem_give(ESPOS_SEM_INVALID_HANDLE) == -EINVAL);

    TEST_ASSERT(espos_sem_del(ESPOS_SEM_INVALID_HANDLE) == -EINVAL);
}

