/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <k_api.h>
#include <test_fw.h>
#include "ysh_unit_test.h"

#define TEST_YSH_CMD_DAEMON_RPI 8
#define TASK_TEST_STACK_SIZE    512

#ifdef CSP_LINUXHOST
static ktask_t     *task_0_test;
extern ktask_t      g_ysh_task;
extern int32_t     ysh_init(void);
extern void        ysh_task_start(void);

#ifdef CONFIG_YSH_SIMULATE
extern uint8_t      g_ysh_queue_init;
extern kbuf_queue_t g_ysh_simulate_queue;
#endif

static void task_ysh0_entry(void *arg)
{
#ifdef CONFIG_YSH_SIMULATE
    kstat_t tmp;
    char   *cmd = NULL;
    int     ret = ysh_init();
    YSH_VAL_CHK(ret == 0);

    if (g_ysh_task.task_name == NULL) {
        ysh_task_start();
    }

    krhino_task_sleep(1);
    if (g_ysh_queue_init == 0) {
        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
        return ;
    }

    krhino_task_sleep(1);

    cmd = "help";
    tmp = krhino_buf_queue_send(&g_ysh_simulate_queue, cmd, strlen(cmd));
    YSH_VAL_CHK(tmp == RHINO_SUCCESS);
    krhino_task_sleep(1);

    cmd = "dumpsys task";
    tmp = krhino_buf_queue_send(&g_ysh_simulate_queue, cmd, strlen(cmd));

    krhino_task_sleep(1);

    cmd = "dumpsys info";
    tmp = krhino_buf_queue_send(&g_ysh_simulate_queue, cmd, strlen(cmd));

    krhino_task_sleep(1);


    cmd = "bt";
    tmp = krhino_buf_queue_send(&g_ysh_simulate_queue, cmd, strlen(cmd));

    krhino_task_sleep(1);

    cmd = "test";
    tmp = krhino_buf_queue_send(&g_ysh_simulate_queue, cmd, strlen(cmd));

    krhino_task_sleep(1);

    if (test_case_check_err == 0) {
        test_case_success++;
        PRINT_RESULT("ysh test", PASS);
    } else {
        test_case_check_err = 0;
        test_case_fail++;
        PRINT_RESULT("ysh test", FAIL);
    }

    krhino_task_del(&g_ysh_task);
#endif
    next_test_case_notify();
    krhino_task_dyn_del(task_0_test);
}

void ysh_cmd_test(void)
{
    kstat_t ret;
    test_case_check_err = 0;
    ret = krhino_task_dyn_create(&task_0_test, "ysh_cmd_daemon", 0,
                                 TEST_YSH_CMD_DAEMON_RPI,
                                 0, TASK_TEST_STACK_SIZE, task_ysh0_entry, 1);

    YSH_VAL_CHK(ret == RHINO_SUCCESS);
    krhino_task_sleep(10);
    return;
}
#endif

