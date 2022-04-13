/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOG_SYSCALL_H
#define ULOG_SYSCALL_H

#include <stdarg.h>

typedef struct {
    unsigned char s;
    const char *mod;
    const char *f;
    unsigned long l;
    const char *fmt;
    va_list args;
} ulog_print_syscall_arg_t;

#endif /* ULOG_SYSCALL_H */

