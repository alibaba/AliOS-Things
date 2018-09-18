/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_dbg_api.h"

#if (RHINO_CONFIG_BACKTRACE > 0)

/* follow functions should defined by arch\...\backtrace.c */
extern int backtraceNow(int (*print_func)(const char *fmt, ...));
extern int backtraceTask(char *taskname,
                         int (*print_func)(const char *fmt, ...));

void krhino_backtrace_now()
{
    backtraceNow(printf);
}

void krhino_backtrace_task(char *taskname)
{
    backtraceTask(taskname, printf);
}

#endif
