/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <ucli.h>

/* #define ENABLE_PROC_MSG */

#define APP_STACK_SIZE 0x400

static ktimer_t app_timer;

#ifdef ENABLE_PROC_MSG
static size_t msg_id;
#endif

static void app_timer_cb(void *timer, void *arg)
{
    static int timer_cnt = 0;

    timer_cnt++;

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

    cli_loop();

    return 0;
}

