/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <aos/kernel.h>

#include <yunit.h>
#include <errno.h>


static aos_sem_t sem;

static void CASE_aosapi_kernel_sem_param()
{
	int ret;
	aos_mutex_t mutex;

#if 0
	// TODO: nullptr coredump
	ret = aos_sem_new(NULL, 0);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif

	// TODO: test fail
	ret = aos_sem_new(&sem, -1);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	if(ret==RHINO_SUCCESS) {
		aos_sem_free(&sem);
	}

	ret = aos_sem_new(&sem, -2);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	if(ret==RHINO_SUCCESS) {
		aos_sem_free(&sem);
	}

	ret = aos_sem_new(&sem, 0x7FFFFFFF);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	if(ret==RHINO_SUCCESS) {
		aos_sem_free(&sem);
	}

	// TODO: test fail
	ret = aos_sem_new(&sem, 0xFFFFFFFF);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	if(ret==RHINO_SUCCESS) {
		aos_sem_free(&sem);
	}

#if 0
	// TODO: nullptr param
	aos_sem_signal(NULL);
#endif

#if 0
	aos_mutex_new(&mutex);
	aos_sem_signal((aos_sem_t*)&mutex);
	aos_mutex_free(&mutex);
#endif

#if 0
	// TODO: nullptr param
	ret = aos_sem_wait(NULL, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
#endif

	aos_mutex_new(&mutex);
	ret = aos_sem_wait((aos_sem_t*)&mutex, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==(-EINVAL), "ret=%d", ret);
	aos_mutex_free(&mutex);

	ret = aos_sem_new(&sem, 0);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	ret = aos_sem_wait(&sem, AOS_NO_WAIT);
	YUNIT_ASSERT_MSG(ret==(-EPERM), "ret=%d", ret);
	aos_sem_free(&sem);

	ret = aos_sem_new(&sem, 0);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	ret = aos_sem_wait(&sem, 1000);
	YUNIT_ASSERT_MSG(ret==(-EPERM), "ret=%d", ret);
	aos_sem_free(&sem);

	ret = aos_sem_new(&sem, 0);
	YUNIT_ASSERT_MSG(ret==RHINO_SUCCESS, "ret=%d", ret);
	ret = aos_sem_wait(&sem, 0);
	YUNIT_ASSERT_MSG(ret==(-EPERM), "ret=%d", ret);
	aos_sem_free(&sem);
}

static void CASE_aosapi_kernel_sem_normal()
{

}


void aosapi_kernel_sem_test_entry(yunit_test_suite_t *suite)
{
	yunit_add_test_case(suite, "kernel.sem.param", CASE_aosapi_kernel_sem_param);
	yunit_add_test_case(suite, "kernel.sem.normal", CASE_aosapi_kernel_sem_normal);
}


