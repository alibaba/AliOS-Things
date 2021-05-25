/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <k_api.h>

/* printf call stack
   return levels of call stack */
int backtrace_now(char *pPC, int *pSP, int bt_space, int (*print_func)(const char *fmt, ...))
{
    char *PC;
    int  *SP;
    int   lvl;
    int   ret;

    (void) pPC;
    (void) pSP;
    (void) bt_space;

    if (print_func == NULL) {
        print_func = printf;
    }


    print_func("========== Call stack ==========\r\n");
    print_func("==========    End     ==========\r\n");
    return 0;
}


void backtrace_task(void *task, int (*print_func)(const char *fmt, ...))
{
    char    *PC;
    char    *LR;
    int     *SP;
    int      lvl;
    int      ret;
    ktask_t  *task_handle = (ktask_t *)task;

    if (print_func == NULL) {
        print_func = printf;
    }


    print_func("==========    End     ==========\r\n");
}

