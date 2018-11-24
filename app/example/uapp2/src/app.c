/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

/* #define ENABLE_PROC_MSG */

#define APP_STACK_SIZE 0x400

static ktask_t     *app_obj;
static cpu_stack_t *stack;

static void app_run(void *arg)
{
    int    cnt = 0;
#ifdef ENABLE_PROC_MSG
    size_t msg_id;
    char   msg[20];
    size_t size;

    msg_id = krhino_msg_get(0x1234, 0, 1024);
#endif

    while (1) {
        printf("uapp2 app cnt 0x%x\r\n", cnt);
        cnt++;

#ifdef ENABLE_PROC_MSG
        if (msg_id) {
            size = 0;
            memset(msg, 0, 20);
            krhino_msg_recv(msg_id, RHINO_WAIT_FOREVER, msg, &size);
            if (size) {
                printf("app2 recv %d bytes: %s\r\n",size, msg);
            } else {
                printf("app2 recv err\r\n");
            }
        }
#else
        krhino_task_sleep(200);
#endif
    }
}

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    printf("uapp2 start to run\r\n");

    stack = malloc(APP_STACK_SIZE*sizeof(cpu_stack_t));
    if (stack != NULL) {
        krhino_utask_create(&app_obj, "application2", 0, AOS_DEFAULT_APP_PRI,
                            (tick_t)0, stack, APP_STACK_SIZE,APP_STACK_SIZE,
                            (task_entry_t)app_run, 1);
    }

    while (1) {
        krhino_task_sleep(200);
        printf("uapp2 main cnt 0x%x\r\n", cnt++);
    }

    return 0;
}

