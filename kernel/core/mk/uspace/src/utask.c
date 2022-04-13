/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "k_api.h"
#include "utask.h"

void krhino_utask_deathbed(void)
{
    //printf("krhino_utask_deathbed\r\n");

    /* delete current task */
    krhino_utask_del(NULL);
}

kstat_t krhino_utask_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                uint8_t pri, tick_t ticks, size_t ustack,
                                size_t kstack, task_entry_t entry, uint8_t autorun)
{
    kstat_t      ret;
    cpu_stack_t *ustack_buf;
    size_t       size;
    uint8_t      flag = 0;

    size = ustack * sizeof(cpu_stack_t) + sizeof(struct utask_couple_data);

    ustack_buf = (cpu_stack_t *)malloc(size);
    if (NULL == ustack_buf) {
        return RHINO_NO_MEM;
    }

    memset(ustack_buf, 0, size);

    if (autorun) {
        flag |= UTASK_AUTORUN;
    }

    flag |= USTACK_DYN_ALLOC;

    ret =  krhino_utask_create(task, name, arg, pri, ticks, ustack_buf, ustack,
                               kstack, entry, flag);
    if (ret != RHINO_SUCCESS) {
        free(ustack_buf);
    }

    return ret;
}

kstat_t krhino_task_dyn_create(ktask_t **task, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, size_t stack,
                               task_entry_t entry, uint8_t autorun)

{
    kstat_t      ret;
    cpu_stack_t *ustack_buf;
    size_t       size;
    uint8_t      flag = 0;

    size = stack * sizeof(cpu_stack_t) + sizeof(struct utask_couple_data);

    ustack_buf = (cpu_stack_t *)malloc(size);
    if (NULL == ustack_buf) {
        return RHINO_NO_MEM;
    }

    memset(ustack_buf, 0, size);

    if (autorun) {
        flag |= UTASK_AUTORUN;
    }

    flag |= USTACK_DYN_ALLOC;

    ret =  krhino_utask_create(task, name, arg, pri, ticks, ustack_buf, stack,
                               stack, entry, flag);
    if (ret != RHINO_SUCCESS) {
        free(ustack_buf);
    }

    return ret;
}
