/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "syscall.h"
#include "kv_syscall_arg.h"
#include "syscall_no.h"

int32_t kv_set(const char *key, const void *value, int len, int sync)
{
    kv_set_syscall_arg_t arg;

    arg.key   = key;
    arg.value = value;
    arg.len   = len;
    arg.sync  = sync;
    arg.sec   = 0;

    return (int32_t)SYSCALL(SYS_KV_SET, &arg);
}

int32_t kv_get(const char *key, void *buffer, int *buffer_len)
{
    kv_get_syscall_arg_t arg;

    arg.key        = key;
    arg.buffer     = buffer;
    arg.buffer_len = buffer_len;
    arg.sec        = 0;

    return (int32_t)SYSCALL(SYS_KV_GET, &arg);
}

int32_t kv_sec_set(const char *key, const void *value, int len, int sync)
{
    kv_set_syscall_arg_t arg;

    arg.key   = key;
    arg.value = value;
    arg.len   = len;
    arg.sync  = sync;
    arg.sec   = 1;

    return (int32_t)SYSCALL(SYS_KV_SET, &arg);
}

int32_t kv_sec_get(const char *key, void *buffer, int *buffer_len)
{
    kv_get_syscall_arg_t arg;

    arg.key        = key;
    arg.buffer     = buffer;
    arg.buffer_len = buffer_len;
    arg.sec        = 1;

    return (int32_t)SYSCALL(SYS_KV_GET, &arg);

}

int32_t kv_del_stub(const char *key)
{
    kv_del_syscall_arg_t arg;

    arg.key = key;

    return (int32_t)SYSCALL(SYS_KV_DEL, &arg);
}

