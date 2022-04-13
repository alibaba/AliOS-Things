/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef DEBUG_BACKTRACE_H
#define DEBUG_BACKTRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

void debug_backtrace_now(int32_t (*print_func)(const char *fmt, ...));

void debug_backtrace_task(char *taskname, int32_t (*print_func)(const char *fmt, ...));

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_BACKTRACE_H */
