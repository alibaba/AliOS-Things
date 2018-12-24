/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <aos/kernel.h>

#include <yunit.h>


static aos_mutex_t g_mutex1;
static aos_mutex_t g_mutex2;
static aos_sem_t sync_sem;
static const uint32_t LOOP_COUNT = 1000000;

#define TEST_TASK_STACK_SIZE (8192)

///////////////////////////////////////////////////////////////////////////////////////////////
static void CASE_aosapi_kernel_mutex_param()
{
#if 0
	int ret;
	aos_mutex_t mutex;
	// FIXME: null pointer:coredump
	ret = aos_mutex_new(NULL);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif

#if 0
	// FIXME: null pointer:coredump
	ret = aos_mutex_lock(NULL, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif

#if 0
	// FIXME: null pointer:coredump
	ret = aos_mutex_unlock(NULL);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif

#if 0
	ret = aos_mutex_free(NULL);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_mutex_lock1(void *arg)
{
	int ret;
	int *pflag = (int*)arg;
	for(int i=0; i<LOOP_COUNT; i++) {
		ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
		YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

		(*pflag)++;

		ret = aos_mutex_unlock(&g_mutex1);
		YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	}
    aos_sem_signal(&sync_sem);
    aos_task_exit(0);
}
void TASK_aosapi_kernel_mutex_lock2(void *arg)
{
	int ret;
	int *pflag = (int*)arg;
	for(int i=0; i<LOOP_COUNT; i++) {
		ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
		YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

		(*pflag)--;

		ret= aos_mutex_unlock(&g_mutex1);
		YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	}
    aos_sem_signal(&sync_sem);
    aos_task_exit(0);
}
static void CASE_aosapi_kernel_mutex_lock()
{
	int ret = RHINO_SUCCESS;
	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    int flag = 0;
	ret = aos_task_new("TASK_aosapi_kernel_mutex_lock_wait1",
			           TASK_aosapi_kernel_mutex_lock1, &flag, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_lock_wait2",
			           TASK_aosapi_kernel_mutex_lock2, &flag, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    aos_mutex_free(&g_mutex1);
    aos_sem_free(&sync_sem);

    YUNIT_ASSERT_MSG(flag==0, "flag=%d", flag);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_mutex_deadlock1(void *arg)
{
	int ret;
	ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex2, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	aos_mutex_unlock(&g_mutex1);
	aos_sem_signal(&sync_sem);
}
void TASK_aosapi_kernel_mutex_deadlock2(void *arg)
{
	int ret;
	ret = aos_mutex_lock(&g_mutex2, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	aos_mutex_unlock(&g_mutex2);
	aos_sem_signal(&sync_sem);
}
static void CASE_aosapi_kernel_mutex_deadlock()
{
	int ret = RHINO_SUCCESS;
	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_mutex_new(&g_mutex2);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	ret = aos_sem_new(&sync_sem, 0);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_deadlock1",
			           TASK_aosapi_kernel_mutex_deadlock1, NULL, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_deadlock2",
			           TASK_aosapi_kernel_mutex_deadlock2, NULL, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

    aos_mutex_free(&g_mutex1);
    aos_sem_free(&sync_sem);
}

///////////////////////////////////////////////////////////////////////////////////////////////
static void CASE_aosapi_kernel_mutex_repeatlock()
{
	int ret;
	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1,AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	// TODO: test fail
	ret = aos_mutex_lock(&g_mutex1,AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	aos_mutex_free(&g_mutex1);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_mutex_lock_timeout(void *arg)
{
	int ret;
	ret = aos_mutex_lock(&g_mutex1, 2000);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1, RHINO_NO_WAIT);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	aos_sem_signal(&sync_sem);
	aos_task_exit(0);
}
static void CASE_aosapi_kernel_mutex_lock_timeout()
{
	int ret;
	ret = aos_sem_new(&sync_sem, 0);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_lock_timeout",
			           TASK_aosapi_kernel_mutex_lock_timeout, NULL, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	ret = aos_sem_wait(&sync_sem,AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);

	aos_sem_free(&sync_sem);
	aos_mutex_free(&g_mutex1);
}


void aosapi_kernel_mutex_test_entry(yunit_test_suite_t* suite)
{
	yunit_add_test_case(suite, "kernel.mutex.param", CASE_aosapi_kernel_mutex_param);
//	yunit_add_test_case(suite, "kernel.mutex.lockwait", CASE_aosapi_kernel_mutex_lock);
//	yunit_add_test_case(suite, "kernel.mutex.locktimeout", CASE_aosapi_kernel_mutex_lock_timeout);
	yunit_add_test_case(suite, "kernel.mutex.repeat", CASE_aosapi_kernel_mutex_repeatlock);
//	yunit_add_test_case(suite, "kernel.mutex.deadlock", CASE_aosapi_kernel_mutex_deadlock);

	(void)CASE_aosapi_kernel_mutex_deadlock;
	(void)CASE_aosapi_kernel_mutex_lock;
	(void)CASE_aosapi_kernel_mutex_lock_timeout;
	(void)CASE_aosapi_kernel_mutex_deadlock;

}


