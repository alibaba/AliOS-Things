/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yunit/yunit.h"

static int init(void)
{
    return 0;
}
static int cleanup(void)
{
    return 0;
}
static void setup(void) {}
static void teardown(void) {}

extern void aosapi_kernel_sys_test_entry(yunit_test_suite_t *suite);
extern void aosapi_kernel_task_test_entry(yunit_test_suite_t *suite);
extern void aosapi_kernel_mutex_test_entry(yunit_test_suite_t *suite);
extern void aosapi_kernel_timer_test_entry(yunit_test_suite_t *suite);
extern void aosapi_kernel_queue_test_entry(yunit_test_suite_t *suite);
extern void aosapi_kernel_workqueue_test_entry(yunit_test_suite_t *suite);
extern void aosapi_kernel_mm_test_entry(yunit_test_suite_t *suite);
extern void aosapi_kernel_sem_test_entry(yunit_test_suite_t *suite);

void test_api(void)
{
    yunit_test_suite_t *suite;
    suite = yunit_add_test_suite("aosapi", init, cleanup, setup, teardown);

    /* kernel */
    aosapi_kernel_sys_test_entry(suite);
    aosapi_kernel_task_test_entry(suite);
    aosapi_kernel_mutex_test_entry(suite);
    aosapi_kernel_sem_test_entry(suite);
    aosapi_kernel_timer_test_entry(suite);

    aosapi_kernel_queue_test_entry(suite);
    aosapi_kernel_mm_test_entry(suite);
#if (RHINO_CONFIG_WORKQUEUE > 0)
    aosapi_kernel_workqueue_test_entry(suite);
#endif
}
AOS_TESTCASE(test_api);
