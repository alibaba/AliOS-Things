/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>

#define AOS_START_STACK 0x200

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];

extern int aos_run_app(void);

static void sys_init(void)
{
    stm32_soc_init();

    aos_run_app();
}


int main(void)
{
    aos_heap_set();

    aos_init();

    krhino_task_create(&demo_task_obj,
                        "aos-init", 0,
                        AOS_DEFAULT_APP_PRI,
                         0, demo_task_buf,
                         AOS_START_STACK,
                         (task_entry_t)sys_init,
                         1);

    aos_start();
}


