/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>


int hal_cmu_get_crystal_freq()
{
    return 260000;
}

int hal_fast_sys_timer_get()
{
    return 10000;
}

size_t cpu_intrpt_save(void)
{

}
void cpu_intrpt_restore(size_t cpsr)
{

}