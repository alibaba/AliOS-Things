/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <hal/hal.h>
#include <ucli.h>

#define APP_STACK_SIZE 0x400

static ktask_t app_obj;
cpu_stack_t app_stack[APP_STACK_SIZE];

static ktimer_t app_timer;

static void app_run(void *arg)
{
    int cnt = 0;

    ktask_t *cur = krhino_cur_task_get();

    while (1) {
        printf("uapp1 app cnt 0x%x\r\n", cnt);
        cnt++;
        krhino_task_sleep(200);
    }
}

static void app_timer_cb(void *timer, void *arg)
{
    static int cnt = 0;

    printf("app timer cb called %d\r\n", cnt++);
}

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    cli_deinit();

    cli_init();

    printf(" app1 start to run\r\n");

    run_test_case();

    krhino_timer_create(&app_timer, "app_timer", app_timer_cb,
                        10, 100, NULL, 1);

    krhino_utask_create(&app_obj, "application1", 0,
                        AOS_DEFAULT_APP_PRI,
                        (tick_t)0, app_stack,
                        APP_STACK_SIZE,
                        APP_STACK_SIZE,
                        (task_entry_t)app_run,
                        1);

    cli_loop();

    return 0;
}

