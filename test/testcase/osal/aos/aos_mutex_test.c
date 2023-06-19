/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "aos/kernel.h"

#include "yunit/yunit.h"
#include"aos_system.h"


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
	int ret, i;
	int *pflag = (int*)arg;
	for(i = 0; i < LOOP_COUNT; i++) {
		ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
		YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

		(*pflag)++;

		ret = aos_mutex_unlock(&g_mutex1);
		YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	}
    aos_sem_signal(&sync_sem);
    aos_task_exit(0);
}
void TASK_aosapi_kernel_mutex_lock2(void *arg)
{
	int ret, i;
	int *pflag = (int*)arg;
	for(i = 0; i < LOOP_COUNT; i++) {
		ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
		YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

		(*pflag)--;

		ret= aos_mutex_unlock(&g_mutex1);
		YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	}
    aos_sem_signal(&sync_sem);
    aos_task_exit(0);
}
static void CASE_aosapi_kernel_mutex_lock()
{
	int ret = 0;
	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    int flag = 0;
	ret = aos_task_new("TASK_aosapi_kernel_mutex_lock_wait1",
			           TASK_aosapi_kernel_mutex_lock1, &flag, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_lock_wait2",
			           TASK_aosapi_kernel_mutex_lock2, &flag, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    aos_mutex_free(&g_mutex1);
    aos_sem_free(&sync_sem);

    YUNIT_ASSERT_MSG(flag==0, "flag=%d", flag);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_mutex_deadlock1(void *arg)
{
	int ret;
	ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex2, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	aos_mutex_unlock(&g_mutex1);
	aos_sem_signal(&sync_sem);
}
void TASK_aosapi_kernel_mutex_deadlock2(void *arg)
{
	int ret;
	ret = aos_mutex_lock(&g_mutex2, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	aos_mutex_unlock(&g_mutex2);
	aos_sem_signal(&sync_sem);
}
static void CASE_aosapi_kernel_mutex_deadlock()
{
	int ret = 0;
	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_mutex_new(&g_mutex2);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	ret = aos_sem_new(&sync_sem, 0);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_deadlock1",
			           TASK_aosapi_kernel_mutex_deadlock1, NULL, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_deadlock2",
			           TASK_aosapi_kernel_mutex_deadlock2, NULL, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

    aos_mutex_free(&g_mutex1);
    aos_sem_free(&sync_sem);
}

///////////////////////////////////////////////////////////////////////////////////////////////
static void CASE_aosapi_kernel_mutex_repeatlock()
{
	int ret;
	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1,AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	// TODO: test fail
	ret = aos_mutex_lock(&g_mutex1,AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	aos_mutex_free(&g_mutex1);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_mutex_lock_timeout(void *arg)
{
	int ret;
	ret = aos_mutex_lock(&g_mutex1, 2000);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1, AOS_NO_WAIT);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	aos_sem_signal(&sync_sem);
	aos_task_exit(0);
}
static void CASE_aosapi_kernel_mutex_lock_timeout()
{
	int ret;
	ret = aos_sem_new(&sync_sem, 0);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_mutex_new(&g_mutex1);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_mutex_lock(&g_mutex1, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_task_new("TASK_aosapi_kernel_mutex_lock_timeout",
			           TASK_aosapi_kernel_mutex_lock_timeout, NULL, TEST_TASK_STACK_SIZE);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_sem_wait(&sync_sem,AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	aos_sem_free(&sync_sem);
	aos_mutex_free(&g_mutex1);
}

/**
 * 该示例使用互斥量实现共享资源的互斥访问，具体场景为创建任务A和认为B，以及一互斥量。任务A和任务B使用互斥量同时访问共享数据区，访问共享数据区时使用互斥量做保护。
 * 示例说明如下：
 * 1. t0时刻，任务T调用aos_mutex_create()创建一互斥量。任务T然后调用aos_task_create()创建任务A和任务B。任务A得到运行，并获取互斥量对数据区record_status进行读写操作。
 * 2. t1时刻，任务A因时间片耗尽，让出CPU，任务B得到运行。
 * 3. t2时刻，任务B因无法获得互斥量，进入阻塞状态，任务A得到运行。
 * 4. t3时刻，任务A对数据区record_status的操作完成，释放互斥量，任务B获得互斥量开始对数据区record_status进行读写操作。
 */

/* module name used by log print*/
#define MODULE_NAME "aos_mutex_test"

/* taskA parameters */
#define TASKA_NAME      "taskA"
#define TASKA_PRI       50
#define TASKA_STACKSIZE 4096

/* taskB parameters */
#define TASKB_NAME      "taskB"
#define TASKB_PRI       50
#define TASKB_STACKSIZE 4096

/* mutex handle */
static aos_mutex_t mutex_handle;

static int test_cnt_A = 0;
static int test_cnt_B = 0;

/*
 * simulated critical section resources.
 * The critical resource consistency condition is that all four fields are equal,
 * otherwise the critical resource is destroyed.
 */
static struct record {
    uint32_t field1;
    uint32_t field2;
    uint32_t field3;
    uint32_t field4;
} record_status;

/* task entry */
static void task_entry(void *arg)
{
    uint32_t     i = 0;
    aos_status_t status;
    int test_count = 10;
    int *run_count = arg;

    while (test_count--) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a mutex to access the critical resource.
         */
        status = aos_mutex_lock(&mutex_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            printf("[%s] %p mutex lock error\n", MODULE_NAME, (void *)aos_task_self());
            continue;
        }

        /**
         * read critical resource.
         * If the four fields are not equal, the critical resource is destroyed.
         */
        printf("[%s] %p field1=%ld, field2=%ld, field3=%ld, field4=%ld\n", MODULE_NAME,(void *)aos_task_self(), record_status.field1,
             record_status.field2, record_status.field3, record_status.field4);
        printf("[%s] priority=%x \n",MODULE_NAME,aos_get_default_task_priority());
        /* modify critical resources */
        i = rand();
        record_status.field1 = i;
        record_status.field2 = i;
        record_status.field3 = i;
        record_status.field4 = i;

        status = aos_mutex_unlock(&mutex_handle);
        if (status != 0) {
            printf("[%s]%p mutex unlock error\n", MODULE_NAME, (void *)aos_task_self());
            return;
        }

        (*run_count)++;

        aos_msleep(10); /* sleep 1000ms */
    }
}

static int aos_mutex_test(void)
{
    aos_status_t status;
    bool is_valid;

    test_cnt_A = 0;
    test_cnt_B = 0;

    /* create mutex statically */
    status = aos_mutex_create(&mutex_handle, 0);
    if (status != 0) {
        printf("[%s]create mutex error\n", MODULE_NAME);
        return -1;
    }

    is_valid = aos_mutex_is_valid(&mutex_handle);
    YUNIT_ASSERT_MSG(is_valid==true, "is_valid=%d", is_valid);

    /* Task1 and task2 both acess critical resources. */
    status = aos_task_new(TASKA_NAME, task_entry, &test_cnt_A, TASKA_STACKSIZE);
    if (status != 0) {
        aos_mutex_free(&mutex_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKA_NAME);
        return -1;
    }

    status = aos_task_new(TASKB_NAME, task_entry, &test_cnt_B, TASKB_STACKSIZE);
    if (status != 0) {
        aos_mutex_free(&mutex_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKB_NAME);
        return -1;
    }

    aos_msleep(500);

    if ((test_cnt_A == 10) && (test_cnt_B == 10)) {
        return 0;
    }
    return -1;
}

void kernel_test_mutex(void)
{
    YUNIT_ASSERT(aos_mutex_test() == 0);
}

void aosapi_kernel_mutex_test_entry(yunit_test_suite_t* suite)
{
	yunit_add_test_case(suite, "kernel.mutex.new", kernel_test_mutex);
	yunit_add_test_case(suite, "kernel.mutex.param", CASE_aosapi_kernel_mutex_param);
	yunit_add_test_case(suite, "kernel.mutex.lockwait", CASE_aosapi_kernel_mutex_lock);
//	yunit_add_test_case(suite, "kernel.mutex.locktimeout", CASE_aosapi_kernel_mutex_lock_timeout);
//	yunit_add_test_case(suite, "kernel.mutex.repeat", CASE_aosapi_kernel_mutex_repeatlock);
//	yunit_add_test_case(suite, "kernel.mutex.deadlock", CASE_aosapi_kernel_mutex_deadlock);

	(void)CASE_aosapi_kernel_mutex_deadlock;
	(void)CASE_aosapi_kernel_mutex_lock;
	(void)CASE_aosapi_kernel_mutex_lock_timeout;
	(void)CASE_aosapi_kernel_mutex_deadlock;

}


