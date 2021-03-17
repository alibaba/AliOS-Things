/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/cli.h"
#include "board.h"
#include <k_api.h>
#include "haascv_main.h"

typedef struct
{
    int argc;
    char **argv;
} mpy_thread_args;

static mpy_thread_args* alloc_mpy_thread_args(int argc, char **argv)
{
    if (argv == NULL)
    {
        printf("%s:argc is illegal\n", __func__);
        return NULL;
    }

    mpy_thread_args *temp_args = (mpy_thread_args*)malloc(sizeof(mpy_thread_args));
    if (temp_args == NULL)
    {
        printf("%s;malloc mpy_thread_args failed\n", __func__);
        return NULL;
    }

    temp_args->argc = argc;
    temp_args->argv = (char**)malloc(sizeof(char*) * argc);
    if (temp_args->argv == NULL)
    {
        printf("%s:temp_args->argv alloc memory failed\n");
        return NULL;
    }

    //printf("%s;temp_args->argc = %d;argc = %d;\n", __func__, temp_args->argc, argc);
    for (int i = 0; i < argc; i++)
    {
        temp_args->argv[i] = strdup(argv[i]);
        //printf("temp_args->argv[i] = %s;argv[i] = %s; index = %d;\n", temp_args->argv[i], argv[i], i);
    }

    //printf("%s:out temp_args = %p;", __func__, temp_args);
    return temp_args;
}

static int free_mpy_thread_args(mpy_thread_args *args)
{
    if (args == NULL)
    {
        printf("%s:args is illegal\n");
        return -1;
    }

    //printf("%s;args->argc = %d;\n", __func__, args->argc);
    for (int i = 0; i < args->argc; i++)
    {
       // printf("args->argv[i] = %s;index = %d;\n", args->argv[i], i);
        if (args->argv[i] != NULL)
        {
            //printf("free argv = %p\n", args->argv[i]);
            free(args->argv[i]);
        }
    }

    free(args->argv);
    free(args);
    return 0;
}

int python_main_entry(void *p)
{
    //printf("global_argc = %d;global_argv = %p;%s;\n", global_argc, global_argv, __func__);
    tick_t   sleep_time;
    sleep_time = krhino_ms_to_ticks(10); /*  10ms to ticks */
    krhino_task_sleep(sleep_time); /*  sleep 10ms */

    mpy_thread_args* args = (mpy_thread_args*)p;
    if (args == NULL)
    {
        printf("%s:args is illegal\n", __func__);
        return -1;
    }

    mpy_init();
    mpy_run(args->argc, args->argv);
    free_mpy_thread_args(args);

    return 0;
}

static void handle_identity_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    //printf("argc = %d;argv = %p; %s;\n", argc, argv, __func__);
    mpy_thread_args* args = alloc_mpy_thread_args(argc, argv);
    aos_task_new("python_main_entry", python_main_entry, (void*)args, 1024*20);
}

static struct cli_command command = {
    .name     = "python",
    .help     = "start micropython ",
    .function = handle_identity_cmd
};

int application_start(int argc, char *argv[])
{
    int  ret = aos_cli_register_command(&command);
    if (ret) {
        printf("register micropython command failed \n");
    }else{
        printf("register micropython command succeed \n");
    }
}
