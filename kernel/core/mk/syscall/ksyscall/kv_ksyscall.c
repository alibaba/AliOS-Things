/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "kv_syscall_arg.h"
#include "fs/kv_api.h"

int32_t sys_kv_set_stub(void *arg)
{
    kv_set_syscall_arg_t *_arg = arg;

    if (_arg->sec) {
        return kv_item_secure_set(_arg->key, _arg->value, _arg->len);
    } else {
        return kv_item_set(_arg->key, _arg->value, _arg->len);
    }
}

int32_t sys_kv_get_stub(void *arg)
{
    kv_get_syscall_arg_t *_arg = arg;

    if (_arg->sec) {
        return kv_item_secure_get(_arg->key, _arg->buffer, _arg->buffer_len);
    } else {
        return kv_item_get(_arg->key, _arg->buffer, _arg->buffer_len);
    }
}

int32_t sys_kv_del_stub(void *arg)
{
    kv_del_syscall_arg_t *_arg = arg;

    return kv_item_delete(_arg->key);
}

