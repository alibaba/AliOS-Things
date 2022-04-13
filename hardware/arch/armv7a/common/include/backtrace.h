/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BACKTRACE_H
#define BACKTRACE_H

/* printf call stack
   return levels of call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...));

/* printf call stack for task
   return levels of call stack */
int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...));

/* printf call stack for running task
   return levels of call stack
   should be called from interrupt or panic*/
int backtrace_task_now(int (*print_func)(const char *fmt, ...));

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...));

/* backtrace start with PC SP and LR
   return levels of call stack */
int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...));

/**
 Get call stack, return levels of call stack
 trace[] output buffer
 size    buffer size
 offset  which lvl start
 */
int backtrace_now_get(char *trace[], int size, int offset);

#endif /* BACKTRACE_H */
