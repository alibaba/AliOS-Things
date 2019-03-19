/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <lwip/sockets.h>
#include <aos/log.h>
#include <aos/cli.h>
//#define ENABLE_PROC_MSG

#define APP_STACK_SIZE 0x400
#define KERNEL_STACK_SIZE 0x400
ktask_t *linkkit_task_obj;

#ifdef ENABLE_PROC_MSG
static size_t msg_id;
#endif

extern int linkkit_main(void *paras);
extern void log_no_cli_init(void);

void app_help(void)
{
    printf("app cli demo\r\n");
}

void linkkit_task_start(void)
{
    kstat_t stat;

    cpu_stack_t *ustack_buf;

    ustack_buf = malloc(APP_STACK_SIZE * sizeof(cpu_stack_t));
    if (NULL == ustack_buf) {
        printf("%s: alloc linkkitapp task stack failed\r\n", __func__);
        return -1;
    }

    stat = krhino_utask_create(&linkkit_task_obj, "linkkit_task", 0, 25 ,50 , ustack_buf,
                               APP_STACK_SIZE, KERNEL_STACK_SIZE, linkkit_main, 1);

    if (stat != RHINO_SUCCESS) {
        printf("%s: create linkkit task failed\r\n", __func__);
        free(ustack_buf);
        return -2;
    }

    return 0;
}

static struct cli_command app_help_cmd = {"app_help", "dump app cli help menu", app_help};
static struct cli_command linkkitapp_cmd = {"linkkitapp", "user space linkkit app", linkkit_task_start};

int application_start(int argc, char *argv[])
{
    int cnt = 0;

    log_no_cli_init();

    LOG(" app1 start to run");

    /* register app help demo cmd */
    if (!cli_register_command(&app_help_cmd)) {
        LOG("restered app_help cmd");
    }

    /* register linkkit app cmd */
    if (!cli_register_command(&linkkitapp_cmd)) {
        LOG("restered linkkitapp cmd");
    }

    while (1) {
        //printf("uapp1 run cnt %d\r\n", cnt++);
        krhino_task_sleep(100);
    }

    //return 0;
}

