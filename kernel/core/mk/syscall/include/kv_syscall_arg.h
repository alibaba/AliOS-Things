/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef KV_SYSCALL_ARG_H
#define KV_SYSCALL_ARG_H

typedef struct {
    const char *key;
    const void *value;
    int         len;
    int         sync;
    int         sec;
} kv_set_syscall_arg_t;

typedef struct {
    const char *key;
    void       *buffer;
    int        *buffer_len;
    int         sec;
} kv_get_syscall_arg_t;

typedef struct {
    const char *key;
} kv_del_syscall_arg_t;

#endif /* KV_SYSCALL_ARG_H */

