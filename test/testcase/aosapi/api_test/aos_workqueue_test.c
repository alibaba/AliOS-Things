/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <aos/kernel.h>

#include <yunit.h>
#include <errno.h>

static aos_workqueue_t workqueue;
static aos_work_t      work;
static aos_sem_t       sync_sem;
static void            CASE_aosapi_kernel_workqueue_param()
{
    int ret;
#if 0
	// TODO: test fail(nullptr coredump)
	ret = aos_workqueue_create(NULL, 10, 1024);
	YUNIT_ASSERT(ret == RHINO_SUCCESS);
#endif

    ret = aos_workqueue_create(&workqueue, RHINO_CONFIG_PRI_MAX, 1024);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_workqueue_create(&workqueue, RHINO_CONFIG_PRI_MAX + 1, 1024);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

    // TODO: test fail(RHINO_TASK_INV_STACK_SIZE)
    ret = aos_workqueue_create(&workqueue, 10, 0);
    YUNIT_ASSERT_MSG(ret != RHINO_SUCCESS, "ret=%d", ret);

#if 0
	// TODO: test fail(nullptr coredump)
	aos_workqueue_del(NULL);
#endif
}

static void CASE_aosapi_kernel_workqueue_default()
{
    // TODO: not implement
    // aos_work_cancle() and aos_work_sched()
}

static void WORK_aosapi_kernel_workqueue_custom(void *arg)
{
    int i = 4;
    while (i--) {
        aos_msleep(1000);
        printf("workqueue:%d\n", i);
    }
    aos_sem_signal(&sync_sem);
}
static void CASE_aosapi_kernel_workqueue_custom()
{
    int ret = 0;

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_workqueue_create(&workqueue, 10, 1024);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_work_init(&work, WORK_aosapi_kernel_workqueue_custom, NULL, 100);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_work_run(&workqueue, &work);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret == RHINO_SUCCESS, "ret=%d", ret);
    aos_sem_free(&sync_sem);
    aos_work_destroy(&work);
}

static void WORK_aosapi_kernel_work_param(void *arg) {}
static void CASE_aosapi_kernel_work_param()
{
    int ret = 0;
#if 0
	// TODO: nullptr coredump
	ret = aos_work_init(NULL,WORK_aosapi_kernel_work_param,  NULL, 1000);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

    ret = aos_work_init(&work, NULL, NULL, 1024);
    YUNIT_ASSERT(ret != RHINO_SUCCESS);

#if 0
	// TODO: nullptr coredump
	ret = aos_work_run(NULL, &work);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

#if 0
	// TODO: nullptr coredump
	ret = aos_workqueue_create(&workqueue, 10, 1024);
	YUNIT_ASSERT(ret == RHINO_SUCCESS);
	aos_work_run(&workqueue, NULL);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
	aos_workqueue_del(&workqueue);
#endif

#if 0
	// TODO: not implement
	ret = aos_work_cancel(NULL);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

#if 0
	// TODO: not implement
	ret = aos_work_sched(NULL);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

    (void)WORK_aosapi_kernel_work_param;
}


void aosapi_kernel_workqueue_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.workqueue.param",
                        CASE_aosapi_kernel_workqueue_param);
    yunit_add_test_case(suite, "kernel.workqueue.default",
                        CASE_aosapi_kernel_workqueue_default);
    yunit_add_test_case(suite, "kernel.workqueue.custom",
                        CASE_aosapi_kernel_workqueue_custom);
    yunit_add_test_case(suite, "kernel.work.param",
                        CASE_aosapi_kernel_work_param);
}
