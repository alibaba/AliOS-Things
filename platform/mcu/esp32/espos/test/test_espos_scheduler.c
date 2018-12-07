/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "unity.h"
#include "espos_task.h"
#include "espos_scheduler.h"
#include "espos_spinlock.h"
#include "test_espos_run_in_isr.h"

#define ESPOS_SCHEDULER_INVALID_HANDLE -1

uint32_t count;

TEST_CASE("ESPOS scheduler critical section test", "[espos]")
{
    //TODO: how to test critical section?
}

static void count_task(void *p)
{
    count ++;
    TEST_ASSERT(espos_task_del(0) == 0);
}

TEST_CASE("ESPOS scheduler preempt suspend resume test", "[espos]")
{
    espos_task_t new_task_handle;

    count = 0;

    TEST_ASSERT(espos_task_create_on_cpu(&new_task_handle, "count_task", NULL, 10,
                             0, 1024, count_task, 0, 0) == 0);
    TEST_ASSERT(espos_task_create_on_cpu(&new_task_handle, "count_task", NULL, 10,
                             0, 1024, count_task, 0, 0) == 0);

    TEST_ASSERT(count == 2);

    count = 0;

    TEST_ASSERT(espos_preempt_suspend_local() == 0);

    TEST_ASSERT(espos_task_create_on_cpu(&new_task_handle, "count_task", NULL, 10,
                             0, 1024, count_task, 0, 0) == 0);
    TEST_ASSERT(espos_task_create_on_cpu(&new_task_handle, "count_task", NULL, 10,
                             0, 1024, count_task, 0, 0) == 0);

    TEST_ASSERT(count == 0);
    TEST_ASSERT(espos_preempt_resume_local() == 0);
}
