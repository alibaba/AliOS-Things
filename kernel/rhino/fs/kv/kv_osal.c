/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "kv_conf.h"
#include "kv_osal.h"
#include "hal/soc/flash.h"

int kv_os_partition_read(uint32_t offset, void *buf, size_t nbytes)
{
    return hal_flash_read((hal_partition_t)KV_PTN, &offset, buf, nbytes);
}

int kv_os_partition_write(uint32_t offset, void *buf, size_t nbytes)
{
    return hal_flash_write((hal_partition_t)KV_PTN, &offset, buf, nbytes);
}

int kv_os_partition_erase(uint32_t offset, uint32_t size)
{
    return hal_flash_erase((hal_partition_t)KV_PTN, offset, size);
}

int kv_os_task_start(const char *name, void (*fn)(void *), void *arg, 
                     int stack)
{
    ktask_t *task_handle = NULL;
    return krhino_task_dyn_create(&task_handle, name, arg, KV_TASK_PRI, 0, 
                                  stack / sizeof(cpu_stack_t), fn, 1u);
}

void kv_os_task_del(void)
{
    krhino_task_dyn_del(NULL);
}

