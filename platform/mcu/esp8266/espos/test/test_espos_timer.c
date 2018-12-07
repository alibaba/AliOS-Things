/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "unity.h"
#include "espos_timer.h"
#include "espos_semaphore.h"

#define ESPOS_TIMER_INVALID_OPT 0xFF
#define ESPOS_TIMER_INVALID_HANDLE 0
#define ESPOS_TIMER_CHANGE_OPT_INVALID 5

static const char *TIMER_NAME = "timer_test";

static espos_sem_t sync_sema;
static espos_timer_t global_timer;
static int count;

static void espos_test_timer_dummy_cb(espos_timer_t timer, void *arg)
{
}

TEST_CASE("ESPOS timer create delete test", "[espos]")
{
    uint32_t i;
    espos_timer_t timer = ESPOS_TIMER_INVALID_HANDLE;

    for (i = 0; i < 1000; i++) {
        if (espos_timer_create(&timer, TIMER_NAME, espos_test_timer_dummy_cb, NULL,
                               1, ESPOS_TIMER_NO_AUTO_RUN) != 0) {
            TEST_ASSERT(0);
            break;
        }
        if (espos_timer_del(timer) != 0) {
            TEST_ASSERT(0);
            break;
        }
    }
}

static void espos_test_timer_cb(espos_timer_t timer, void *arg)
{
    TEST_ASSERT(global_timer == timer);
    TEST_ASSERT(arg == NULL);
    count++;
    TEST_ASSERT(espos_sem_give(sync_sema) == 0);
}

TEST_CASE("ESPOS timer no auto run test", "[espos]")
{
    uint32_t i;

    TEST_ASSERT(espos_sem_create(&sync_sema, 1, 0) == 0);

    TEST_ASSERT(espos_timer_create(&global_timer, TIMER_NAME, espos_test_timer_cb, NULL,
                                   espos_ms_to_ticks(20), ESPOS_TIMER_NO_AUTO_RUN) == 0);

    count = 0;

    for (i = 0; i < 100; i++) {
        TEST_ASSERT(espos_timer_start(global_timer) == 0);
        TEST_ASSERT(espos_sem_take(sync_sema, ESPOS_MAX_DELAY) == 0);
    }

    TEST_ASSERT(count == 100);
    TEST_ASSERT(espos_sem_del(sync_sema) == 0);
    TEST_ASSERT(espos_timer_del(global_timer) == 0);
}

TEST_CASE("ESPOS timer auto run test", "[espos]")
{
    uint32_t i;
    espos_sem_create(&sync_sema, 1, 0);

    TEST_ASSERT(espos_timer_create(&global_timer, TIMER_NAME, espos_test_timer_cb, NULL,
                                   espos_ms_to_ticks(20), ESPOS_TIMER_AUTO_RUN) == 0);

    count = 0;
    TEST_ASSERT(espos_timer_start(global_timer) == 0);

    for (i = 0; i < 100; i++) {
        espos_sem_take(sync_sema, ESPOS_MAX_DELAY);
    }

    TEST_ASSERT(espos_timer_stop(global_timer) == 0);

    TEST_ASSERT(count == 100);
    TEST_ASSERT(espos_sem_del(sync_sema) == 0);
    TEST_ASSERT(espos_timer_del(global_timer) == 0);
}

TEST_CASE("ESPOS timer auto run test", "[espos]")
{
    espos_timer_t timer;

    TEST_ASSERT(espos_sem_create(&sync_sema, 1, 0) == 0);

    TEST_ASSERT(espos_timer_create(&timer, TIMER_NAME, espos_test_timer_cb, NULL,
                                   20, ESPOS_TIMER_NO_AUTO_RUN) == 0);

    TEST_ASSERT(espos_timer_start(timer) == 0);
    TEST_ASSERT(espos_timer_start(timer) == 0);

    TEST_ASSERT(espos_sem_take(sync_sema, 200) == 0);

    TEST_ASSERT(espos_timer_stop(timer) == 0);
    TEST_ASSERT(espos_timer_stop(timer) == 0);

    TEST_ASSERT(espos_sem_take(sync_sema, 200) == -ETIMEDOUT);

    TEST_ASSERT(espos_timer_del(timer) == 0);
}

TEST_CASE("ESPOS timer change test", "[espos]")
{
    TEST_ASSERT(espos_sem_create(&sync_sema, 1, 0) == 0);

    TEST_ASSERT(espos_timer_create(&global_timer, TIMER_NAME, espos_test_timer_cb, NULL,
                                   20, ESPOS_TIMER_AUTO_RUN) == 0);

    count = 0;

    TEST_ASSERT(espos_timer_start(global_timer) == 0);

    TEST_ASSERT(espos_sem_take(sync_sema, ESPOS_MAX_DELAY) == 0);

    TEST_ASSERT(espos_sem_take(sync_sema, 200) == 0);
    TEST_ASSERT(espos_sem_take(sync_sema, 200) == 0);

    TEST_ASSERT(espos_timer_stop(global_timer) == 0);

    TEST_ASSERT(count == 3);
    TEST_ASSERT(espos_sem_del(sync_sema) == 0);
    TEST_ASSERT(espos_timer_del(global_timer) == 0);
}

TEST_CASE("ESPOS timer API param test", "[espos]")
{
    espos_timer_t timer;
    TEST_ASSERT(espos_timer_create(&timer, NULL, NULL, espos_test_timer_cb,
                                   20, ESPOS_TIMER_NO_AUTO_RUN) == -EINVAL);

    TEST_ASSERT(espos_timer_create(NULL, NULL, espos_test_timer_cb, NULL,
                                   20, ESPOS_TIMER_AUTO_RUN) == -EINVAL);
    TEST_ASSERT(espos_timer_create(&timer, NULL, NULL, NULL,
                                   20, ESPOS_TIMER_AUTO_RUN) == -EINVAL);
    TEST_ASSERT(espos_timer_create(&timer, NULL, NULL, espos_test_timer_cb,
                                   20, ESPOS_TIMER_INVALID_OPT) == -EINVAL);

    TEST_ASSERT(espos_timer_start(ESPOS_TIMER_INVALID_HANDLE) == -EINVAL);

    TEST_ASSERT(espos_timer_stop(ESPOS_TIMER_INVALID_HANDLE) == -EINVAL);

    TEST_ASSERT(espos_timer_change(ESPOS_TIMER_INVALID_HANDLE,
                                   ESPOS_TIMER_CHANGE_ONCE, NULL) == -EINVAL);
    TEST_ASSERT(espos_timer_change(timer,
                                   ESPOS_TIMER_CHANGE_OPT_INVALID, NULL) == -EINVAL);

    TEST_ASSERT(espos_timer_del(ESPOS_TIMER_INVALID_HANDLE) == -EINVAL);
}
