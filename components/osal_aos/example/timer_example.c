/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/**
 * 该示例使用定时器管理函数来控制定时器的执行，具体场景为创建一个周期性定时器，定时调用回调函数执行，\n\r
 * 停止定时器该变定时器的时间参数，则定时器按照修改后的时间间隔定时调用回调函数执行。
 * 示例说明如下：
 *  1. t0时刻，测试任务调用aos_timer_new()创建一个周期性的定时器，周期间隔为1秒，回调函数为timer1_func。然后测试任务调用aos_sleep()进入休眠状态。
 *  2. t1时刻，相对t0过去1秒，定时器到期，回调函数timer1_func被执行。该过程重复10次。
 *  3. tn时刻，测试任务休眠到期，调用aos_timer_stop()停止定时器。然后调用aos_timer_change()接口修改周期间隔为2秒。
 *  4. tn+1时刻，相对tn过去2秒，定时器到期，回调函数timer1_func被执行。该过程重复5次
 */

/* module name used by log print */
#define MODULE_NAME "aos_timer_example"

/* timer handle */
static aos_timer_t timer1;
static volatile int timer_count = 0;

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
    status = aos_timer_create(&timer1, timer1_func, NULL, 1000, (AOS_TIMER_REPEAT | AOS_TIMER_AUTORUN));
    if (status != 0) {
         printf("[%s]create timer error\n", MODULE_NAME);
        return;
    }

    aos_msleep(10000);

    /* stop the timer before modifying the timer parameter */
    aos_timer_stop(&timer1);

    printf("[%s]timer expires %d times\n", MODULE_NAME, timer_count);

    timer_count = 0;

    /* the timer cycle is modified to 2000ms */
    aos_timer_change(&timer1, 2000);

    /* start the timer after the timer parameter modification is complete */
    aos_timer_start(&timer1);

    aos_msleep(10000);

    printf("[%s]timer expires %d times\n", MODULE_NAME, timer_count);

}

static void aos_timer_example(int argc, char **argv)
{
    aos_status_t status;
    aos_task_t task_handle;
    status = aos_task_create(&task_handle, "task_timer_test", aos_timer_example_task, NULL, NULL, 1024, 50, AOS_TASK_AUTORUN);
    if (status != 0) {
        printf("[%s]create %s error\n", MODULE_NAME, "task_timer_test");
        return;
    }
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_timer_example, timer_example, aos timer example)
#endif