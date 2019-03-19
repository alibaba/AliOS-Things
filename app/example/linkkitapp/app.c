/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <lwip/sockets.h>
#include <aos/log.h>
#include <aos/cli.h>
#include <tlsf.h>

#define APP_STACK_SIZE 0x2000
#define KERNEL_STACK_SIZE 0x1000

extern int linkkit_main(void *paras);

static ktask_t *linkkit_task_obj;

void app_help(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int i;

    for (i = 0; i < argc; i++) {
        printf("%s: argv[%d]: %s\r\n", __func__, i, argv[i]);
    }

    printf("app cli demo\r\n");
}

void dump_app_heap_stat(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    size_t total_size;
    size_t used;
    size_t free;
    size_t max_free_block_size;

    total_size = tlsf_get_total_size();
    used = tlsf_get_used_size();
    free = tlsf_get_free_size();
    max_free_block_size = tlsf_get_max_free_block_size();

    printf("Total       Used        Free        FreeBlockMaxSize\r\n");
    printf("0x%08x  0x%08x  0x%08x  0x%08x\r\n",
           total_size, used, free, max_free_block_size);
}

static struct cli_command app_help_cmd = {"app_help", "dump app cli help menu", app_help};
static struct cli_command app_heap_stat = {"app_heap", "dump app heap statistics", dump_app_heap_stat};

static void linkkit_task(void *arg)
{
     printf("linkkit_task start to run\r\n");

     linkkit_main(NULL);

     krhino_uprocess_exit();
}

void create_linkkit_task(void)
{
    kstat_t stat;
    cpu_stack_t *ustack_buf;

    ustack_buf = malloc(APP_STACK_SIZE * sizeof(cpu_stack_t));
    if (NULL == ustack_buf) {
        printf("%s: alloc linkkit task stack failed\r\n", __func__);
        return;
    }

    stat = krhino_utask_create(&linkkit_task_obj, "linkkit_task", 0, 25 ,50 , ustack_buf,
		               APP_STACK_SIZE, KERNEL_STACK_SIZE, linkkit_task, 1);

    if (stat != RHINO_SUCCESS) {
	printf("%s: create linkkit task failed\r\n", __func__);
	free(ustack_buf);
        return;
    }
}

int application_start(int argc, char *argv[])
{
    LOG(" app1 start to run");


    /* register app help demo cmd */
    if (!aos_cli_register_command(&app_help_cmd)) {
        LOG("restered app_help cmd");
    }

    if (!aos_cli_register_command(&app_heap_stat)) {
        LOG("restered app_heap_stat");
    }

    create_linkkit_task();

    krhino_uprocess_join();

    return 0;
}

