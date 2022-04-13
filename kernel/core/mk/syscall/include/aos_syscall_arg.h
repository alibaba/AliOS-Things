/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AOS_SYSCALL_H
#define AOS_SYSCALL_H

typedef struct {
    char *buf;
    size_t   len;
} aos_version_str_get_syscall_arg_t;

typedef struct {
    void  *dst;
    void  *src;
    size_t len;
} aos_ipc_copy_syscall_arg_t;

typedef struct {
    void   *console;
    void   *buf;
    size_t  size;
} aos_print_syscall_arg_t;

#endif /* AOS_SYSCALL_H */

