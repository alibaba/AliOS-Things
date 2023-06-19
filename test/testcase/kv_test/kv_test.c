/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <yunit/yunit.h>
extern void kv_test_init(void);
extern void kv_test_flash(void);
extern void kv_test_lock(void);
extern void kv_test_malloc(void);
extern void kv_test_task(void);

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static void setup_pri_save(void)
{
}

static void teardown_pri_restore(void)
{
}

static yunit_test_case_t aos_kv_testcases[] = {
    //{ "kv_test_init", kv_test_init},
    //{ "kv_test_flash", kv_test_flash},
    //{ "kv_test_lock", kv_test_lock},
    //{ "kv_test_malloc", kv_test_malloc},
    { "kv_test_task", kv_test_task},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "kv", init, cleanup, setup, teardown, aos_kv_testcases },
    YUNIT_TEST_SUITE_NULL
};

void aos_kv_test(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(aos_kv_test);