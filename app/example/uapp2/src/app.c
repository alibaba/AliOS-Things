/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <hal/hal.h>

#define APP_STACK_SIZE 0X400

static ktask_t app_obj;
cpu_stack_t app_stack[APP_STACK_SIZE];

static void app_run(void *arg)
{
    int cnt = 0;

    ktask_t *cur = krhino_cur_task_get();
    int pid = cur->pid;
    int addr, val;

    //run_test_case();

    while (1) {
        printf("app2 run_cnt 0x%x\r\n", cnt);
        cnt++;
        krhino_task_sleep(200);
    }
}

extern void mutex_test(void);
extern void time_test(void);

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    printf("app2 start to run\r\n");

    krhino_utask_create(&app_obj, "app", 0,
                        AOS_DEFAULT_APP_PRI,
                        (tick_t)0, app_stack,
                        APP_STACK_SIZE,
                        APP_STACK_SIZE,
                        (task_entry_t)app_run,
                         1);

    while (1) {
        printf("app2 syscall cnt 0x%x\r\n", cnt++);
        krhino_task_sleep(200);
    }

    return 0;
}

