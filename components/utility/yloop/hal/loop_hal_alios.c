/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>

static unsigned int used_bitmap;

void hal_loop_task_setspecific(unsigned int key, void *ctx)
{
    krhino_task_info_set(krhino_cur_task_get(), key, ctx);
}

void *hal_loop_task_getspecific(unsigned int key)
{
    void *ctx = NULL;

    krhino_task_info_get(krhino_cur_task_get(), key, &ctx);
    return ctx;
}

int hal_loop_task_key_create(unsigned int *key)
{
    int i;

    for (i = RHINO_CONFIG_TASK_INFO_NUM - 1; i >= 0; i--) {
        if (!((1 << i) & used_bitmap)) {
            used_bitmap |= 1 << i;
            *key = i;

            return 0;
        }
    }

    return -1;
}

void hal_loop_task_key_delete(unsigned int key)
{
    if (key >= RHINO_CONFIG_TASK_INFO_NUM) {
        return;
    }

    used_bitmap &= ~(1 << key);
}
