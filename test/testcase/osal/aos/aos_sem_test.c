/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>

#include "yunit/yunit.h"

#include <errno.h>


static aos_sem_t sem;

static void CASE_aosapi_kernel_sem_param()
{
    int         ret;
    aos_mutex_t mutex;
    bool is_valid;

#if 0
	// TODO: nullptr coredump
	ret = aos_sem_new(NULL, 0);
	YUNIT_ASSERT_MSG(ret==RHINO_NULL_PTR, "ret=%d", ret);
#endif

    // TODO: test fail
    ret = aos_sem_new(&sem, -1);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    if (ret == 0) {
        aos_sem_free(&sem);
    }

    is_valid = aos_sem_is_valid(&sem);
    YUNIT_ASSERT_MSG(is_valid==true, "is_valid=%d", is_valid);

    ret = aos_sem_new(&sem, -2);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    if (ret == 0) {
        aos_sem_free(&sem);
    }

    ret = aos_sem_new(&sem, 0x7FFFFFFF);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    if (ret == 0) {
        aos_sem_free(&sem);
    }

    // TODO: test fail
    ret = aos_sem_new(&sem, 0xFFFFFFFF);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    if (ret == 0) {
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
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
#endif

#if 0
    aos_mutex_new(&mutex);
    ret = aos_sem_wait((aos_sem_t *)&mutex, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);
    aos_mutex_free(&mutex);
#endif

    ret = aos_sem_new(&sem, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    ret = aos_sem_wait(&sem, AOS_NO_WAIT);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);
    aos_sem_free(&sem);

    ret = aos_sem_new(&sem, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    ret = aos_sem_wait(&sem, 1000);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);
    aos_sem_free(&sem);

    ret = aos_sem_new(&sem, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    ret = aos_sem_wait(&sem, 0);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);
    aos_sem_free(&sem);
}

static void CASE_aosapi_kernel_sem_normal() {}

/**
 * 该示例使用信号量实现多任务同步，具体场景为创建一个高优先级任务A，一个低优先级任务B，任务A和任务B同时等待同一信号量，\n\r
 * 此时测试任务T调用aos_sem_signal()释放信号量，任务A首先获得信号量，任务A操作完成后释放一次信号量，此时任务B获取信号量得到运行。
 * 示例说明如下：
 * 1. t0时刻，任务T调用aos_sem_new()创建一信号量，初始计数值为0。任务T然后调用aos_task_create()创建任务A和任务B，\n\r
 *    任务A优先级为30，任务B优先级为31。任务A和任务B运行后因等待信号量而阻塞。
 * 2. t1时刻，任务T调用aos_sem_signal()释放信号量，任务A获得信号量。
 * 3. t2时刻，任务A调用aos_sem_signal()释放信号量，任务B获得信号量。
 */

/* module name used by log print */
#define MODULE_NAME "aos_sem_test"

/* taskA parameters */
#define TASKA_NAME      "taskA"
#define TASKA_STACKSIZE 4096
#define TASKA_PRIO 30

/* taskB parameters */
#define TASKB_NAME      "taskB"
#define TASKB_STACKSIZE 4096
#define TASKB_PRIO 31

/* Static memory for static creation */
static aos_sem_t sem_handle;

static int test_cnt_A = 0;
static int test_cnt_B = 0;
static int test_num=0;

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

/* task entry for taskA and taskB*/
static void task_entry(void *arg)
{
    uint32_t      i = 0;
    aos_status_t  status;
    int test_count = 10;
    int *test_cnt = (int *)arg;

    while (test_count--) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a sem to access the critical resource.
         */
        status = aos_sem_wait(&sem_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            printf("[%s] %p sem wait error\n", MODULE_NAME, (void *)aos_task_self());
            continue;
        }

        /**
         * read critical resource.
         * If the four fields are not equal, the critical resource is destroyed.
         */
        printf("[%s]%p field1=%ld, field2=%ld, field3=%ld, field4=%ld\n", MODULE_NAME, (void *)aos_task_self(), record_status.field1,
             record_status.field2, record_status.field3, record_status.field4);

        /* modify critical resources */
        i = rand();
        record_status.field1 = i;
        record_status.field2 = i;
        record_status.field3 = i;
        record_status.field4 = i;

        /* a semaphore is released when critical resource access is complete */
        if(test_num==0) {
            aos_sem_signal_all(&sem_handle);
        } else {
            aos_sem_signal(&sem_handle);
        }

        (*test_cnt)++;

        aos_msleep(10); /* sleep 1000ms */
    }
}

static int aos_sem_test(void)
{
    aos_status_t status;
    aos_task_t   taskA_handle;
    aos_task_t   taskB_handle;

    test_cnt_A = 0;
    test_cnt_B = 0;

    /**
     * create sem. In this case, the sem is used to access the critical resource,
     * so count is initialized to 0
     */
    status = aos_sem_create(&sem_handle, 0, 0);
    if (status != 0) {
        printf("[%s]create sem error\n", MODULE_NAME);
        return -1;
    }

    /* TaskA and taskB both acess critical resources. */
    status = aos_task_create(&taskA_handle, TASKA_NAME, task_entry, &test_cnt_A, NULL, TASKA_STACKSIZE, TASKA_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_sem_free(&sem_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKA_NAME);
        return -1;
    }

    status = aos_task_create(&taskB_handle, TASKB_NAME, task_entry, &test_cnt_B, NULL, TASKB_STACKSIZE, TASKB_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_sem_free(&sem_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKB_NAME);
        return -1;
    }

    aos_msleep(100);

    /* a semaphore is released when critical resource access is complete */
    aos_sem_signal(&sem_handle);

    aos_msleep(500);

    if ((test_cnt_A == 10) && (test_cnt_A == 10)) {
        return 0;
    }

    return -1;

}

void kernel_test_sem(void)
{
    YUNIT_ASSERT(aos_sem_test() == 0);
}

void aosapi_kernel_sem_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.sem.new", kernel_test_sem);
    yunit_add_test_case(suite, "kernel.sem.param",
                        CASE_aosapi_kernel_sem_param);
    yunit_add_test_case(suite, "kernel.sem.normal",
                        CASE_aosapi_kernel_sem_normal);
}
