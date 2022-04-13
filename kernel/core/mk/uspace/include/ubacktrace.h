/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UBACKTRACE_H
#define UBACKTRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 Get call stack, return levels of call stack
 trace[] output buffer
 size    buffer size
 offset  which lvl start
 */
int ubacktrace_dump(void *trace[], int size, int offset);

/* printf call stack
   return levels of call stack */
int ubacktrace_show(int (*print_func)(const char *fmt, ...));

#ifdef __cplusplus
}
#endif

#endif /* UBACKTRACE_H */

