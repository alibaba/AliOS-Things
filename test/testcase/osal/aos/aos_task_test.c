/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>

#include "yunit/yunit.h"

#include <errno.h>

static aos_sem_t sync_sem;
#define TEST_TASK_STACK_SIZE (8192)

////////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_task_new_param(void *arg)
{
    aos_task_exit(0);
}
static void CASE_aosapi_kernel_task_new_param()
{
    int ret = 0;
    ret     = aos_task_new(NULL, TASK_aosapi_kernel_task_new_param, NULL, 1024);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);

    ret = aos_task_new("TASK_aosapi_kernel_task_new_param", NULL, NULL, 1024);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);

#if 1
    ret = aos_task_new("TASK_aosapi_kernel_task_new_param",
                       TASK_aosapi_kernel_task_new_param, NULL, 0);
    YUNIT_ASSERT_MSG(ret != 0, "ret=%d", ret);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_task_new_batch(void *arg)
{
    aos_sem_signal(&sync_sem);
    aos_task_exit(0);
}
static void CASE_aosapi_kernel_task_new_batch()
{
    int       i             = 0;
    int       success_count = 0;
    int       ret           = 0;
    const int TASK_COUNT    = 10;
    for (i = 0; i < TASK_COUNT; i++) {
        ret = aos_sem_new(&sync_sem, 0);
        YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

        ret = aos_task_new("TASK_aosapi_kernel_task_new_batch",
                           TASK_aosapi_kernel_task_new_batch, NULL,
                           TEST_TASK_STACK_SIZE);
        YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
        if (ret != 0) {
            aos_sem_signal(&sync_sem);
        }
        ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
        YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

        aos_sem_free(&sync_sem);
        success_count += (ret == 0 ? 1 : 0);
        printf("task %d\t", success_count);
    }
    YUNIT_ASSERT(success_count == TASK_COUNT);
}

////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void TASK_aosapi_kernel_task_new_stack(void *arg)
{
    int array[2048];
    memset(array, 0, sizeof(int) * 2048);
    PRINT_TASK_INFO(krhino_cur_task_get());
    aos_task_exit(0);
}
#endif
static void CASE_aosapi_kernel_task_new_stack()
{
#if 0
	int ret = 0;
	ret = aos_task_new("TASK_aosapi_kernel_task_new_stack",
					   TASK_aosapi_kernel_task_new_stack, NULL, 256);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	aos_msleep(1000);
#endif

#if 0
	ret = aos_task_new("TASK_aosapi_kernel_task_new_stack",
			           TASK_aosapi_kernel_task_new_stack, NULL, 10);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	aos_msleep(1000);
#endif

#if 0
	ret = aos_task_new("TASK_aosapi_kernel_task_new_stack",
			           TASK_aosapi_kernel_task_new_stack, NULL, 20480000);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	aos_msleep(1000);
#endif
}

extern const char *aos_task_name(void);
////////////////////////////////////////////////////////////////////////////////////////////////
void TASK_aosapi_kernel_task_getname(void *arg)
{
    YUNIT_ASSERT(0 ==
                 strcmp(aos_task_name(), "TASK_aosapi_kernel_task_getname"));
    aos_sem_signal(&sync_sem);
    aos_task_exit(0);
}
static void CASE_aosapi_kernel_task_getname()
{
    int ret = 0;
    ret     = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    ret =
      aos_task_new("TASK_aosapi_kernel_task_getname",
                   TASK_aosapi_kernel_task_getname, NULL, TEST_TASK_STACK_SIZE);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);
    if (ret != 0) {
        aos_sem_signal(&sync_sem);
    }

    ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    YUNIT_ASSERT_MSG(ret == 0, "ret=%d", ret);

    aos_sem_free(&sync_sem);
}

static aos_task_t task_handle;
void TASK_aosapi_kernel_task_new_ext(void *arg)
{
	aos_sem_signal(&sync_sem);
	aos_task_exit(0);
}
static void CASE_aosapi_kernel_task_new_ext()
{
	int ret = 0;
	int success_count = 0;
	int i = 0;

	/*  */
	ret = aos_sem_new(&sync_sem, 0);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_task_new_ext(&task_handle, "TASK_aosapi_kernel_task_new_ext",
			               TASK_aosapi_kernel_task_new_ext, NULL,
			               TEST_TASK_STACK_SIZE, 50);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	//if(ret == 0) {
//		aos_sem_signal(&sync_sem);
//	}
	ret = aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	aos_sem_free(&sync_sem);
}
////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void TASK_aosapi_kernel_task_new_priority(void *arg)
{
	aos_sem_signal(&sync_sem);
	aos_task_exit(0);
}
static void CASE_aosapi_kernel_task_new_priority()
{
	int ret = 0;
	int success_count = 0;
	int i = 0;

#if 1
	/*  */
	ret = aos_sem_new(&sync_sem, 0);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_task_new_ext("TASK_aosapi_kernel_task_new_priority",
			               TASK_aosapi_kernel_task_new_priority, NULL,
			               TEST_TASK_STACK_SIZE, RHINO_CONFIG_PRI_MAX);
	YUNIT_ASSERT_MSG(ret==RHINO_BEYOND_MAX_PRI, "ret=%d", ret);
	if(ret == RHINO_BEYOND_MAX_PRI) {
		aos_sem_signal(&sync_sem);
	}
	ret = aos_sem_wait(&sync_sem, RHINO_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	aos_sem_free(&sync_sem);

	/*  */
	ret = aos_sem_new(&sync_sem, 0);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

	ret = aos_task_new_ext("TASK_aosapi_kernel_task_new_priority",
			               TASK_aosapi_kernel_task_new_priority, NULL,
						   TEST_TASK_STACK_SIZE, RHINO_CONFIG_USER_PRI_MAX);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	if(ret != 0) {
		aos_sem_signal(&sync_sem);
	}
	ret = aos_sem_wait(&sync_sem, RHINO_WAIT_FOREVER);
	YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
	aos_sem_free(&sync_sem);
#endif

	/*  */
#if 1
	int pris[] = {11,12,13};
	for(i=0; i<sizeof(pris)/sizeof(pris[0]); i++) {
		ret = aos_sem_new(&sync_sem, 0);
		YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

		ret = aos_task_new_ext("TASK_aosapi_kernel_task_new_priority",
				           TASK_aosapi_kernel_task_new_priority, NULL,
						   TEST_TASK_STACK_SIZE, pris[i]);
		YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);
		if(ret != 0) {
			aos_sem_signal(&sync_sem);
		}
		ret = aos_sem_wait(&sync_sem, RHINO_WAIT_FOREVER);
		YUNIT_ASSERT_MSG(ret==0, "ret=%d", ret);

		aos_sem_free(&sync_sem);
		success_count += (ret==0 ? 1 : 0);
		printf("\ttask prio=%d\n", pris[i]);
	}
	YUNIT_ASSERT_MSG(success_count==sizeof(pris)/sizeof(pris[0]), "success_count=%d", success_count);
#endif
}
#endif

/**
 * 该示例使用任务管理函数来控制任务的执行状态，具体场景为任务2因等待某个信号量进入阻塞状态，而此时被任务1将其挂起，则任务2仍然是处于阻塞状态，\n\r
 * 如果在此过程中等到信号量，则任务2会解除阻塞进入挂起状态；如果未等到信号量，则任务2恢复状态后仍然处于阻塞状态。\n\r
 * 示例说明如下：
 *   1. 在t0时刻，任务task1、task2分别通过aos_task_new()和aos_task_new_ext()函数调用被创建，之后task1进入就绪状态，而task2处于挂起状态。
 *   2. Task1得到运行后，在t1时刻调用aos_task_resume()将task2恢复，task2进入就绪状态，之后task1通过调用aos_msleep()进入休眠状态，task2因为task1休眠而获得CPU执行权，task2运行后因等待信号量进入阻塞状态。
 *   3. Task1在t2时刻因延迟到期得到运行，并调用aos_task_suspend()将task2挂起，task2此时的状态为阻塞挂起。之后task1通过调用aos_msleep()进入休眠状态。
 *   4. Task2在t3时刻因延迟到期得到运行，并调用aos_task_resume()将task2恢复，此时task2的状态为阻塞状态。之后task1通过调用aos_msleep()进入休眠状态。
 *   5. Task1在t4时刻因延迟到期得到运行，并调用aos_sem_signal()释放信号量，这时task2因等到信号量而进入就绪状态。待到task1再次进入休眠转改后task2得到运行，进入运行状态。
 */

/* module name used by log print */
#define MODULE_NAME "aos_task_test"

/* sem handle for synchronize of task1 and task2 */
static aos_sem_t g_testsync_sem;

/* task handle for task1 and task2*/
static aos_task_t task1_handle;
static aos_task_t task2_handle;

static int test_flag_1 = 0;
static int test_flag_2 = 0;

/* task entry for task1*/
void task1_entry()
{
    aos_status_t status;

    printf("[%s]task1 is running!\n", MODULE_NAME);
    printf("[%s]task1 resume task2!\n", MODULE_NAME);

    status = aos_task_resume(&task2_handle);

    if (0 != status) {
        printf("[%s]task1 resume task2 failed(%ld)!\n", MODULE_NAME, status);
        return;
    }
    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    aos_msleep(10);

    printf("[%s]task1 suspend task2!\n", MODULE_NAME);
    if (0 != aos_task_suspend(&task2_handle)) {
        printf("[%s]task1 resume task2 failed!\n", MODULE_NAME);
        return;
    }
    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    aos_msleep(10);

    printf("[%s]task1 resume task2 again!\n", MODULE_NAME);
    status = aos_task_resume(&task2_handle);

    if (0 != status) {
        printf("[%s]task1 resume task2 failed(%ld)!\n", MODULE_NAME, status);
        return;
    }

    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    aos_msleep(10);
    printf("[%s]task1 signal a semphone!\n", MODULE_NAME);
    aos_sem_signal(&g_testsync_sem);
    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    test_flag_1 = 1;
    aos_msleep(10);
    aos_task_exit(0);
}

/* task entry for task2*/
void task2_entry()
{
    printf("[%s]task2 is running!\n", MODULE_NAME);
    if (0 != aos_sem_wait(&g_testsync_sem, AOS_WAIT_FOREVER)) {
        printf("[%s]task2 wait semphone failed!\n", MODULE_NAME);
        return;
    }

    test_flag_2 = 1;

    printf("[%s]task2 get semphone and is running!\n", MODULE_NAME);
    aos_task_exit(0);
}

/* task example init function*/
static int aos_task_test(void)
{
    aos_status_t status;

    test_flag_1 = 0;
    test_flag_2 = 0;

    status = aos_sem_new(&g_testsync_sem, 0);
    if (status != 0) {
        printf("[%s]sem new failed, err=%ld\n", MODULE_NAME, status);
        return -1;
    }

    status = aos_task_create(&task2_handle, "task2", task2_entry, NULL, NULL, 4096, 50, AOS_TASK_NONE);
    if (status != 0) {
        aos_sem_free(&g_testsync_sem);
        printf("[%s]create %s error\n", MODULE_NAME, "task2");
        return -1;
    }

    status = aos_task_create(&task1_handle, "task1", task1_entry, NULL, NULL, 4096, 50, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_sem_free(&g_testsync_sem);
        printf("[%s]create %s error\n", MODULE_NAME, "task1");
        return -1;
    }

    aos_msleep(500);

    if ((test_flag_1 == 1) && (test_flag_2 == 1)) {
        return 0;
    }

    return -1;

}

void kernel_test_task(void)
{
    YUNIT_ASSERT(aos_task_test() == 0);
}

void aosapi_kernel_task_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.task.new", kernel_test_task);
    yunit_add_test_case(suite, "kernel.task.param",
                        CASE_aosapi_kernel_task_new_param);
    yunit_add_test_case(suite, "kernel.task.stack",
                        CASE_aosapi_kernel_task_new_stack);
    yunit_add_test_case(suite, "kernel.task.batchcreate",
                        CASE_aosapi_kernel_task_new_batch);
    yunit_add_test_case(suite, "kernel.task.ext",
                        CASE_aosapi_kernel_task_new_ext);
    //	yunit_add_test_case(suite, "kernel.task.priority",
    //CASE_aosapi_kernel_task_new_priority);
    yunit_add_test_case(suite, "kernel.task.getname",
                        CASE_aosapi_kernel_task_getname);
}
