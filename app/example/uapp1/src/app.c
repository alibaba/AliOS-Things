/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <ucli.h>

//#define ENABLE_PROC_MSG
#define APP_STACK_SIZE 0x400

static ktask_t     app_obj;
static cpu_stack_t app_stack[APP_STACK_SIZE];
static ktimer_t    app_timer;

#ifdef ENABLE_PROC_MSG
static size_t msg_id;
#endif

static void app_run(void *arg)
{
    int cnt = 0;

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

#ifdef ENABLE_PROC_MSG
    char msg[20] = "hello message queue";
    krhino_msg_snd(msg_id, msg, 20);
#endif
}

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    cli_deinit();

    cli_init();

    printf(" app1 start to run\r\n");

    run_test_case();

#ifdef ENABLE_PROC_MSG
    msg_id = krhino_msg_get(0x1234, 0, 1024);
#endif
    krhino_timer_create(&app_timer, "app_timer", app_timer_cb,
                        10, 100, NULL, 1);

    krhino_utask_create(&app_obj, "application1", 0,
                        AOS_DEFAULT_APP_PRI, (tick_t)0, app_stack,
                        APP_STACK_SIZE, APP_STACK_SIZE,
                        (task_entry_t)app_run, 1);

    cli_loop();

    return 0;
}

