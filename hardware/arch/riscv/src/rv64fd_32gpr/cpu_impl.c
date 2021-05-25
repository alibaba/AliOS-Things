/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_config.h>

#if (RHINO_CONFIG_STACK_OVF_CHECK_HW != 0)
void cpu_intrpt_stack_protect(void)
{
}

void task_stack_crash_warning(void)
{
    printf("****The task stack base has been broken !!!****\n");
}

void cpu_task_stack_protect(cpu_stack_t *base, size_t size)
{

}
#endif
