/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef DEBUG_SYSCALL_ARG_H
#define DEBUG_SYSCALL_ARG_H

#include <stdint.h>
#include <limits.h>
#include "k_err.h"

typedef struct {
    char *pc;
    int  *sp;
    int   bt_space;
    int (*print_func)(const char *fmt, ...);
} backtrace_now_syscall_arg_t;

typedef struct {
    char *pc;
    int  *sp;
    int   bt_space;
    void **trace;
    int   size;
    int   offset;
} backtrace_now_get_syscall_arg_t;

typedef struct {
    kstat_t err;
    char   *file;
    int     line;
    int     inuser;
} debug_fatal_error_syscall_arg_t;

typedef struct {
    int          i;
    uintptr_t     addr;
} debug_watchpoint_syscall_arg_t;

typedef struct {
    void *mm_head_addr;
} debug_umm_syscall_arg_t;

#endif /* DEBUG_SYSCALL_ARG_H */

