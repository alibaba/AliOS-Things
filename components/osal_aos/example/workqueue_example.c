/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/**
 * 该示例用工作队列实现不同场景的任务执行。
 * 示例说明如下：
 * 1.
 * t0时刻，任务T调用aos_sem_create()创建一个信号量，调用aos_workqueue_create()创建一工作队列wq1。
 *      任务T然后调用aos_work_init()创建7个任务：work0、work1、work2、work3、work4、work5、work6，延迟参数分别为0、0、1、20、18、40、50。
 * 2. t1时刻，任务T调用aos_work_run()依次：
 *      a、将work0加入工作队列wq1
 *      b、将work1加入工作队列wq1
 *      c、将work1再次工作队列wq1
 *      d、将work2加入工作队列wq1
 *      e、将work2加入工作队列wq1
 *      f、将work3加入工作队列wq1
 *      g、将work4加入系统默认工作队列
 *      h、将work5加入系统默认工作队列
 *      i、将work6加入系统默认工作队列
 * 3. t2时刻，任务T调用aos_sem_wait()等待信号量进入阻塞状态，
 * 4.
 * t3时刻，work4被执行释放信号量，任务T解除阻塞，将work5再加入系统默认工作队列后因等待信号量进入阻塞状态
 * 5. t4时刻，work5被执行释放信号量，任务T解除阻塞，释放资源。
 */

/* module name used by log print */
#define MODULE_NAME "aos_workqueue_example"

#define RHINO_CONFIG_WORKQUEUE 1

#if (RHINO_CONFIG_WORKQUEUE > 0)

#define WORK_STACK_BUF 1024

#define TASK_WORKQUEUE_PRI 16

static size_t stack1_buf[WORK_STACK_BUF];

static aos_workqueue_t wq1;

static aos_work_t work0;
static aos_work_t work1;
static aos_work_t work2;
static aos_work_t work3;
static aos_work_t work4;
static aos_work_t work5;
static aos_work_t work6;

static aos_sem_t g_wq_test_sem;

static void work0_func(void *arg) { printf("--%s--\n", (char *)arg); }

static void work1_func(void *arg) { printf("--%s--\n", (char *)arg); }

static void work2_func(void *arg) { printf("--%s--\n", (char *)arg); }

static void work3_func(void *arg) { printf("--%s--\n", (char *)arg); }

static void work4_func(void *arg)
{
  printf("--%s--\n", (char *)arg);
  aos_sem_signal(&g_wq_test_sem);
}

static void work5_func(void *arg)
{
  printf("--%s--\n", (char *)arg);
  aos_sem_signal(&g_wq_test_sem);
}

static void work6_func(void *arg) { printf("--%s--\n", (char *)arg); }

static void aos_workqueue_example(int argc, char **argv)
{
  aos_status_t ret;

  size_t stack1_size = WORK_STACK_BUF;

  aos_sem_create(&g_wq_test_sem, 0, 0);

  /* creat workqueues */

  ret = aos_workqueue_create(&wq1, "WORKQUEUE1-TEST", TASK_WORKQUEUE_PRI,
                             stack1_buf, stack1_size);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  /* init works */
  ret = aos_work_init(&work0, work0_func, "WORK 0", 0);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  ret = aos_work_init(&work1, work1_func, "WORK 1", 0);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  ret = aos_work_init(&work2, work2_func, "WORK 2", 1);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  ret = aos_work_init(&work3, work3_func, "WORK 3", 20);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  ret = aos_work_init(&work4, work4_func, "WORK 4", 18);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  ret = aos_work_init(&work5, work5_func, "WORK 5", 40);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  ret = aos_work_init(&work6, work6_func, "WORK 6", 50);
  if (ret != 0) {
    aos_sem_signal(&g_wq_test_sem);
    return;
  }

  /* work run */
  aos_work_run(&wq1, &work0);
  aos_work_run(&wq1, &work1);
  aos_work_run(&wq1, &work1);

  aos_work_run(&wq1, &work2);
  aos_work_run(&wq1, &work2);
  aos_work_run(&wq1, &work3);

  aos_work_sched(&work4);
  aos_work_sched(&work5);
  aos_work_sched(&work6);

  /* wait for task4 */
  aos_sem_wait(&g_wq_test_sem, AOS_WAIT_FOREVER);

  aos_work_sched(&work5);

  /* wait for task6 */
  aos_sem_wait(&g_wq_test_sem, AOS_WAIT_FOREVER);

  aos_work_cancel(&work0);
  aos_work_cancel(&work1);
  aos_work_cancel(&work2);
  aos_work_cancel(&work3);
  aos_work_cancel(&work4);
  aos_work_cancel(&work5);
  aos_work_cancel(&work6);

  aos_work_destroy(&work0);
  aos_work_destroy(&work1);
  aos_work_destroy(&work2);
  aos_work_destroy(&work3);
  aos_work_destroy(&work4);
  aos_work_destroy(&work5);
  aos_work_destroy(&work6);

  aos_workqueue_del(&wq1);

  aos_sem_free(&g_wq_test_sem);

  return;
}

#endif

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_workqueue_example, workqueue_example,
                       aos workqueue example)
#endif
