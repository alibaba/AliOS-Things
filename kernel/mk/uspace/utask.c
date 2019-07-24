/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "k_api.h"
#include "utask.h"

kstat_t krhino_utask_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                uint8_t pri, tick_t ticks, size_t ustack,
                                size_t kstack, task_entry_t entry, uint8_t autorun)
{
    cpu_stack_t *ustack_buf;

    ustack_buf = (cpu_stack_t*)malloc(ustack * sizeof(cpu_stack_t));
    if (NULL == ustack_buf) {
        return RHINO_NO_MEM;
    }

    memset(ustack_buf, 0, ustack * sizeof(cpu_stack_t));

    return krhino_utask_create(task, name, arg, pri, ticks, ustack_buf,
                               ustack, kstack, entry, autorun);
}

