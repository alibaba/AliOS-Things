/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "k_api.h"

/* part of ktask_t */
typedef struct
{
    void *task_stack;
}ktask_t_shadow;

//#define OS_BACKTRACE_DEBUG

extern void krhino_task_deathbed(void);
extern ktask_t_shadow *debug_task_find(char *name);
extern int debug_task_is_running(ktask_t_shadow *task);
extern void *debug_task_stack_bottom(ktask_t_shadow *task);
extern char *k_int2str(int num, char *str);

void backtrace_handle(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...));

/* printf call stack
   return levels of call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    return 0;
}

/* printf call stack for task
   return levels of call stack */
void backtrace_task(void *task, int (*print_func)(const char *fmt, ...))
{

}

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...))
{
    return 0;
}

/* backtrace start with PC SP and LR
   return levels of call stack */
int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...))
{
    return 0;
}


void *g_back_trace;
/**
 Get call stack, return levels of call stack
 trace[] output buffer
 size    buffer size
 offset  which lvl start
 */
int backtrace_now_get(void *trace[], int size, int offset)
{
    return 0;
}

void backtrace_handle(char *PC, int *SP, char *LR,
                      int (*print_func)(const char *fmt, ...))
{
    return;
}
