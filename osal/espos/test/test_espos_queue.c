/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "unity.h"
#include "espos_queue.h"
#include "espos_task.h"
#include "espos_semaphore.h"
#include "espos_time.h"
#include "test_espos_run_in_isr.h"

#define ESPOS_QUEUE_INVALID_OPT (ESPOS_QUEUE_SEND_OPT_NORMAL+10)
#define ESPOS_QUEUE_INVALID_POS (ESPOS_QUEUE_SEND_BACK+10)
#define ESPOS_QUEUE_INVALID_HANDLE 0

static espos_sem_t sync_semaphore;
static espos_queue_t global_queue;
static espos_queue_t isr_queue;

TEST_CASE("ESPOS queue create delete", "[espos]")
{
    uint32_t i;
    espos_queue_t queue = ESPOS_QUEUE_INVALID_HANDLE;

    for (i = 0; i < 1000; i++) {
        if (espos_queue_create(&queue, 16, 50) != 0) {
            TEST_ASSERT(0);
            break;
        }
        if (espos_queue_del(queue) != 0) {
            TEST_ASSERT(0);
            break;
        }
    }
}

static void queue_send_task(void *p)
{
    uint32_t msg = 0xF1F2F3F4;
    uint8_t *received = (uint8_t *) p;

    TEST_ASSERT(espos_queue_send(global_queue, &msg, ESPOS_MAX_DELAY) == 0);

    // should switch to recv task and then switch back
    TEST_ASSERT(*received == 1);

    TEST_ASSERT(espos_sem_give(sync_semaphore) == 0);
    TEST_ASSERT(espos_task_del(0) == 0);
}

static void queue_recv_task(void *p)
{
    uint32_t msg;
    uint8_t *received = (uint8_t *) p;

    TEST_ASSERT(espos_queue_recv(global_queue, &msg, ESPOS_MAX_DELAY) == 0);

    // check msg value
    TEST_ASSERT(msg = 0xF1F2F3F4);

    // set received flag
    *received = 1;
    TEST_ASSERT(espos_sem_give(sync_semaphore) == 0);
    TEST_ASSERT(espos_task_del(0) == 0);
}

TEST_CASE("ESPOS queue send receive", "[espos]")
{
    espos_task_t new_task_handle;
    uint8_t received = 0;

    TEST_ASSERT(espos_queue_create(&global_queue, 4, 10) == 0);

    TEST_ASSERT(espos_sem_create(&sync_semaphore, 2, 0) == 0);

    // recv task with higher priority, create recv task first
    TEST_ASSERT(espos_task_create_on_cpu(&new_task_handle, "queue_recv_task", &received, 2,
                             0, 1024, queue_recv_task, 0, 0) == 0);
    TEST_ASSERT(espos_task_create_on_cpu(&new_task_handle, "queue_send_task", &received, 1,
                             0, 1024, queue_send_task, 0, 0) == 0);

    // wait task exit
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(espos_sem_del(sync_semaphore) == 0);

    TEST_ASSERT(espos_queue_del(global_queue) == 0);
}

TEST_CASE("ESPOS queue flush", "[espos]")
{
    uint32_t msg = 0;
    espos_queue_t queue = ESPOS_QUEUE_INVALID_HANDLE;

    TEST_ASSERT(espos_queue_create(&queue, 4, 10) == 0);
    TEST_ASSERT(espos_queue_send_generic(queue, &msg, ESPOS_MAX_DELAY,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == 0);
    TEST_ASSERT(espos_queue_send_generic(queue, &msg, ESPOS_MAX_DELAY,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == 0);

    TEST_ASSERT(espos_queue_flush(queue) == 0);
    TEST_ASSERT(espos_queue_msg_waiting(queue) == 0);

    TEST_ASSERT(espos_queue_flush(queue) == 0);

    TEST_ASSERT(espos_queue_del(queue) == 0);
}

TEST_CASE("ESPOS queue send front/end", "[espos]")
{
    uint32_t msg;
    espos_queue_t queue = ESPOS_QUEUE_INVALID_HANDLE;

    TEST_ASSERT(espos_queue_create(&queue, 4, 10) == 0);
    msg = 0x01;
    TEST_ASSERT(espos_queue_send_generic(queue, &msg, ESPOS_MAX_DELAY,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == 0);
    msg = 0x02;
    TEST_ASSERT(espos_queue_send_generic(queue, &msg, ESPOS_MAX_DELAY,
                                         ESPOS_QUEUE_SEND_FRONT, ESPOS_QUEUE_SEND_OPT_NORMAL) == 0);
    msg = 0x03;
    TEST_ASSERT(espos_queue_send_generic(queue, &msg, ESPOS_MAX_DELAY,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == 0);

    TEST_ASSERT(espos_queue_recv(queue, &msg, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(msg == 0x02);

    TEST_ASSERT(espos_queue_recv(queue, &msg, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(msg == 0x01);

    TEST_ASSERT(espos_queue_recv(queue, &msg, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(msg == 0x03);

    TEST_ASSERT(espos_queue_del(queue) == 0);
}

TEST_CASE("ESPOS queue timeout test", "[espos]")
{
    uint32_t msg;
    espos_queue_t queue = ESPOS_QUEUE_INVALID_HANDLE;

    TEST_ASSERT(espos_queue_create(&queue, 4, 1) == 0);

    TEST_ASSERT(espos_queue_send_generic(queue, &msg, 0,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == 0);

    TEST_ASSERT(espos_queue_send_generic(queue, &msg, 0,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == -ETIMEDOUT);

    TEST_ASSERT(espos_queue_recv(queue, &msg, 0) == 0);

    TEST_ASSERT(espos_queue_recv(queue, &msg, 0) == -ETIMEDOUT);

    TEST_ASSERT(espos_queue_del(queue) == 0);
}

TEST_CASE("ESPOS queue create/delete API invalid param test", "[espos]")
{
    espos_queue_t queue = ESPOS_QUEUE_INVALID_HANDLE;

    TEST_ASSERT(espos_queue_create(&queue, 0xFFFF, 0xFFFFFF) == -ENOMEM);
    TEST_ASSERT(espos_queue_del(queue) == -EINVAL);
    TEST_ASSERT(espos_queue_create(NULL, 4, 10) == -EINVAL);
    TEST_ASSERT(espos_queue_create(&queue, 0, 10) == -EINVAL);
    TEST_ASSERT(espos_queue_create(&queue, 4, 0) == -EINVAL);

    TEST_ASSERT(espos_queue_del(ESPOS_QUEUE_INVALID_HANDLE) == -EINVAL)
}

TEST_CASE("ESPOS queue send API invalid param test", "[espos]")
{
    uint32_t msg;
    espos_queue_t queue = ESPOS_QUEUE_INVALID_HANDLE;

    TEST_ASSERT(espos_queue_create(&queue, 4, 10) == 0);

    TEST_ASSERT(espos_queue_send_generic(queue, NULL, 0,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == -EINVAL);
    TEST_ASSERT(espos_queue_send_generic(queue, &msg, 0,
                                         ESPOS_QUEUE_INVALID_POS, ESPOS_QUEUE_SEND_OPT_NORMAL) == -EINVAL);
    TEST_ASSERT(espos_queue_send_generic(queue, &msg, 0,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_INVALID_OPT) == -EINVAL);
    TEST_ASSERT(espos_queue_send_generic(ESPOS_QUEUE_INVALID_HANDLE, &msg, 0,
                                         ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_INVALID_OPT) == -EINVAL);

    TEST_ASSERT(espos_queue_del(queue) == 0);
}

TEST_CASE("ESPOS queue recv/wait/flush API invalid param test", "[espos]")
{
    uint32_t msg;
    espos_queue_t queue = ESPOS_QUEUE_INVALID_HANDLE;

    TEST_ASSERT(espos_queue_create(&queue, 4, 10) == 0);

    TEST_ASSERT(espos_queue_recv(ESPOS_QUEUE_INVALID_HANDLE, &msg, 0) == -EINVAL);
    TEST_ASSERT(espos_queue_recv(queue, NULL, 0) == -EINVAL);

    //TODO: don't konw what could happen if passing invalid queue handle
    TEST_ASSERT(espos_queue_msg_waiting(ESPOS_QUEUE_INVALID_HANDLE) == 0);

    TEST_ASSERT(espos_queue_flush(ESPOS_QUEUE_INVALID_HANDLE) == -EINVAL);

    TEST_ASSERT(espos_queue_del(queue) == 0);
}

TEST_CASE("ESPOS deleting a queue in use test", "[espos]")
{
    // TODO: this case is RTOS dependent
    espos_task_t new_task_handle;
    uint8_t received = 0;
    uint32_t msg = 0xF1F2F3F4;

    TEST_ASSERT(espos_queue_create(&global_queue, 4, 1) == 0);

    TEST_ASSERT(espos_sem_create(&sync_semaphore, 1, 0) == 0);

    TEST_ASSERT(espos_task_create_on_cpu(&new_task_handle, "queue_recv_task", &received, 1,
                                         0, 1024, queue_recv_task, 0, 0) == 0);

    TEST_ASSERT(espos_queue_send(global_queue, &msg, ESPOS_MAX_DELAY) == 0);

    // wait task exit
    TEST_ASSERT(espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY) == 0);
    TEST_ASSERT(received == 1);
    TEST_ASSERT(espos_sem_del(sync_semaphore) == 0);

    TEST_ASSERT(espos_queue_del(global_queue) == 0);
}

static void IRAM_ATTR queue_isr_test(void)
{
    uint8_t msg = 0;
    TEST_ASSERT(espos_queue_send_generic(isr_queue, &msg, ESPOS_MAX_DELAY,
                             ESPOS_QUEUE_SEND_BACK, ESPOS_QUEUE_SEND_OPT_NORMAL) == 0);
}

TEST_CASE("ESPOS send to queue in ISR test", "[espos]")
{
    uint8_t msg;
    espos_queue_create(&isr_queue, 1, 1);
    espos_test_run_in_isr(queue_isr_test);
    espos_queue_recv(isr_queue, &msg, ESPOS_MAX_DELAY);
    TEST_ASSERT(msg == 0);
    espos_queue_del(isr_queue);
}
