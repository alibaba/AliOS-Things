/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BACKTRACE_H
#define BACKTRACE_H

/* printf call stack for context */
int backtraceContext(char *PC, char *LR, int *SP,
                     int (*print_func)(const char *fmt, ...));

/* printf call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...));

/* printf call stack for task */
int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...));

#endif /* BACKTRACE_H */
