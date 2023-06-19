/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "yunit/yunit.h"
#include <errno.h>

static aos_timer_t g_timer;
static aos_sem_t   sync_sem;
static int         timer_trigger_count = 0;

static void TIMER_aosapi_kernel_timer_param() {}
static void CASE_aosapi_kernel_timer_param()
{
    int ret;
#if 0
	ret = aos_timer_new(NULL, TIMER_aosapi_kernel_timer_param, NULL, 1000, 0);
	YUNIT_ASSERT(ret==RHINO_NULL_PTR);
#endif

    ret = aos_timer_create(&g_timer, NULL, NULL, 1000, 0);
    YUNIT_ASSERT(ret != 0);

#if 0
    ret = aos_timer_create(&g_timer, TIMER_aosapi_kernel_timer_param, NULL, 0, 0);
    YUNIT_ASSERT(ret != 0);
#endif

#if 0
	ret = aos_timer_start(NULL);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

#if 0
	ret = aos_timer_stop(NULL);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

#if 0
	aos_timer_free(NULL);
#endif

#if 0
	ret = aos_timer_change(NULL, 0);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif
    aos_timer_free(&g_timer);
}

///////////////////////////////////////////////////////////////////////////////////////////////
static void TIMER_aosapi_kernel_timer_norepeat(void *timer, void *arg)
{
    timer_trigger_count++;
    aos_timer_stop(&g_timer);
    aos_timer_free(&g_timer);
}
static void CASE_aosapi_kernel_timer_norepeat()
{
    int ret;
    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT(ret == 0);
    timer_trigger_count = 0;
    ret =aos_timer_create(&g_timer, TIMER_aosapi_kernel_timer_norepeat, NULL, 100, 0);
    YUNIT_ASSERT(ret == 0);
    ret = aos_timer_start(&g_timer);
    YUNIT_ASSERT(ret == 0);
    aos_msleep(1000);
    YUNIT_ASSERT(timer_trigger_count == 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
static void TIMER_aosapi_kernel_timer_repeat(void *timer, void *arg)
{
    timer_trigger_count++;
    if (timer_trigger_count == 10) {
        aos_timer_stop(&g_timer);
        aos_timer_free(&g_timer);
    }
}
static void CASE_aosapi_kernel_timer_repeat()
{
    int ret;

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT(ret == 0);
    timer_trigger_count = 0;
    ret =aos_timer_create(&g_timer, TIMER_aosapi_kernel_timer_repeat, NULL, 100, (AOS_TIMER_REPEAT | AOS_TIMER_AUTORUN));
    YUNIT_ASSERT(ret == 0);
    ret= aos_timer_start(&g_timer);
    YUNIT_ASSERT(ret == 0);
    aos_msleep(1500);
    YUNIT_ASSERT(timer_trigger_count == 10);
}

/**
 * 该示例使用定时器管理函数来控制定时器的执行，具体场景为创建一个周期性定时器，定时调用回调函数执行，\n\r
 * 停止定时器该变定时器的时间参数，则定时器按照修改后的时间间隔定时调用回调函数执行。
 * 示例说明如下：
 *  1. t0时刻，测试任务调用aos_timer_create()创建一个周期性的定时器，周期间隔为1秒，回调函数为timer1_func。然后测试任务调用aos_sleep()进入休眠状态。
 *  2. t1时刻，相对t0过去1秒，定时器到期，回调函数timer1_func被执行。该过程重复10次。
 *  3. tn时刻，测试任务休眠到期，调用aos_timer_stop()停止定时器。然后调用aos_timer_change()接口修改周期间隔为2秒。
 *  4. tn+1时刻，相对tn过去2秒，定时器到期，回调函数timer1_func被执行。该过程重复5次
 */

/* module name used by log print */
#define MODULE_NAME "aos_timer_test"

/* timer handle */
static aos_timer_t timer1;
/* timer trigger count */
static volatile int  timer_count = 0;
/* test flag */
static volatile int  test_flag = 0;
/**
 * timer callback function.
 * The current use case is executed every 1000ms.
 */
static void timer1_func(void *timer, void *arg)
{
    /*
    * Warning: an interface that causes a block is not allowed to be called within this function.
    * Blocking within this function will cause other timers to run incorrectly.
    * The printf function may also block on some platforms, so be careful how you call it.
    */

    timer_count++;
}

static void aos_timer_example_task(void *data)
{
    aos_status_t status;

    /**
     * Create timer. Timer starts automatically after successful creation.
     * some of the parameters are as follows:
     * fn:      timer1_func (this function is called when the timer expires)
     * ms:      1000 (the cycle of the timer)
     * option:  AOS_TIMER_REPEAT (set to periodic timer), AOS_TIMER_AUTORUN (set to auto start)
     */
    status = aos_timer_create(&timer1, timer1_func, NULL, 100, (AOS_TIMER_REPEAT | AOS_TIMER_AUTORUN));
    YUNIT_ASSERT(status == 0);
    status = aos_timer_start(&timer1);
    YUNIT_ASSERT(status == 0);
    if (status != 0) {
        printf("[%s]create timer error\n", MODULE_NAME);
        return;
    }
    aos_msleep(1080);
    /* stop the timer before modifying the timer parameter */
    aos_timer_stop(&timer1);
    if (timer_count == 10) {
        test_flag++;
    }
    printf("[%s]timer expires %d times\n", MODULE_NAME, timer_count);
    aos_timer_free(&timer1);

#if 0
    timer_count = 0;

    /* the timer cycle is modified to 2000ms */
    aos_timer_change(&timer1, 20);

    /* start the timer after the timer parameter modification is complete */
    aos_timer_start(&timer1);

    aos_msleep(100);

    if (timer_count == 5) {
        test_flag++;
    }
    printf("[%s]timer expires %d times\n", MODULE_NAME, timer_count);
#endif

}

static int aos_timer_test(void)
{
    aos_status_t status;
    aos_task_t task_handle;
    status = aos_task_create(&task_handle, "task_timer_test", aos_timer_example_task, NULL, NULL, 1024, 50, AOS_TASK_AUTORUN);
    if (status != 0) {
        printf("[%s]create %s error\n", MODULE_NAME, "task_timer_test");
        return -1;
    }

    aos_msleep(3000);

    if (test_flag == 1) {
        return 0;
    }

    return -1;
}

void kernel_test_timer(void)
{
    YUNIT_ASSERT(aos_timer_test() == 0);
}

void aosapi_kernel_timer_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.timer.new", kernel_test_timer);
    yunit_add_test_case(suite, "kernel.timer.param",CASE_aosapi_kernel_timer_param);
    yunit_add_test_case(suite, "kernel.timer.norepeat",CASE_aosapi_kernel_timer_norepeat);
    yunit_add_test_case(suite, "kernel.timer.repeat",CASE_aosapi_kernel_timer_repeat);
}
