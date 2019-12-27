/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "test_bed.h"

void *osa_sem_create(const char *name, uint32_t count)
{
    return (void *)-1;
}

int32_t osa_sem_take(void *sem, uint32_t ticks)
{
    return 0;
}

int32_t osa_sem_give(void *sem)
{
    return 0;
}

int32_t osa_sem_destory(void *sem)
{
    return 0;
}
