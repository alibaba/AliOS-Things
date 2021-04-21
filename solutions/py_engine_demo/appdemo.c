/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "mpy_main.h"
#include "aos/cli.h"
#include "miniunz.h"

typedef struct {
    int argc;
    char **argv;
} mpy_thread_args;

static mpy_thread_args *alloc_mpy_thread_args(int argc, char **argv)
{
    if (argv == NULL) {
        printf("%s:argc is illegal\n", __func__);
        return NULL;
    }

    mpy_thread_args *temp_args = (mpy_thread_args *)malloc(sizeof(mpy_thread_args));
    if (temp_args == NULL) {
        printf("%s;malloc mpy_thread_args failed\n", __func__);
        return NULL;
    }

    temp_args->argc = argc;
    temp_args->argv = (char **)malloc(sizeof(char *) * argc);
    if (temp_args->argv == NULL) {
        printf("%s:temp_args->argv alloc memory failed\n", __func__);
        free(temp_args);
        return NULL;
    }

    for (int i = 0; i < argc; i++) {
        temp_args->argv[i] = strdup(argv[i]);
    }

    return temp_args;
}

static int free_mpy_thread_args(mpy_thread_args *args)
{
    if (args == NULL) {
        printf("args is illegal\n");
        return -1;
    }

    for (int i = 0; i < args->argc; i++) {
        if (args->argv[i] != NULL) {
            free(args->argv[i]);
        }
    }

    free(args->argv);
    free(args);
    return 0;
}

int python_main_entry(void *p)
{
    tick_t   sleep_time;
    sleep_time = krhino_ms_to_ticks(10); /*  10ms to ticks */
    krhino_task_sleep(sleep_time); /*  sleep 10ms */

    mpy_thread_args *args = (mpy_thread_args *)p;
    if (args == NULL) {
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
    mpy_thread_args *args = alloc_mpy_thread_args(argc, argv);
    aos_task_new("python_main_entry", python_main_entry, (void *)args, 1024 * 20);
}

static struct cli_command python_command = {
    .name     = "python",
    .help     = "start micropython ",
    .function = handle_identity_cmd
};


static void handle_unzip_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *zippath  ;
    char *destpath ;
    int ret = 0 ;
    if(argc < 2) {
        printf("Error params,Usage: unzip /data/src.zip  /sdcard \r\n");
        return ;
    }

    if(argc == 2) {
        destpath ="/data";
    }else {
        destpath = argv[2] ;
    }
    zippath = argv[1] ;
    ret = miniUnzip(zippath,destpath);
    if(ret) {
        printf("unzip failed ,errno is %d \r\n",ret);
    }else {
        printf("unzip succeed \r\n");
    }
}


static struct cli_command unzip_command = {
    .name     = "unzip",
    .help     = "unzip /sdcard/src.zip  /data/ ",
    .function = handle_unzip_cmd
};


int application_start(int argc, char *argv[])
{
    int  ret = aos_cli_register_command(&unzip_command);
    if (ret) {
        printf("register micropython command failed \r\n");
    } 
    ret = aos_cli_register_command(&python_command);
    if (ret) {
        printf("register python command failed \r\n");
    } else {
        printf("register python command succeed \r\n");
    }
    return ret ;
}







