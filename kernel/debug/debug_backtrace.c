/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>

#if (DEBUG_CONFIG_BACKTRACE > 0)

/* follow functions should defined by arch\...\backtrace.c */
extern int backtrace_now(int (*print_func)(const char *fmt, ...));
extern int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...));

void debug_backtrace_now()
{
    backtrace_now(printf);
}

void debug_backtrace_task(char *taskname)
{
    backtrace_task(taskname, printf);
}

#endif /* (DEBUG_CONFIG_BACKTRACE > 0) */
