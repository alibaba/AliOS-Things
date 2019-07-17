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

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...));

/* backtrace start with PC SP and LR
   return levels of call stack */
int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...));

#endif /* BACKTRACE_H */
