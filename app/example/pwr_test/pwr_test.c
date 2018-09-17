/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include <cpu_pwr_api.h>

#define DEMO_TASK_PRI 20
#define DEMO_TASK_STACKSIZE 256

static ktask_t     demotask1_tcb;
static ktask_t     demotask2_tcb;
static cpu_stack_t demotask1_stack[DEMO_TASK_STACKSIZE];
static cpu_stack_t demotask2_stack[DEMO_TASK_STACKSIZE];

extern idle_count_t g_idle_count[RHINO_CONFIG_CPU_NUM];

void demo_task1(void *arg)
{
    int count1 = 0;

    while (1) {
        printf("count1 = %d, idle = %d\n", count1++,
               (unsigned int)g_idle_count[0]);
        // printf("count1 = %d\n", count1++);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

void demo_task2(void *arg)
{
    int count2 = 0;

    while (1) {
        printf("count2 = %d\n", count2++);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND * 2);
    }
}

int application_start(int argc, char *argv[])
{
    LOG("pwr test started.");

    cpu_pwrmgmt_init();

    krhino_task_create(&demotask1_tcb, "demo1", 0, DEMO_TASK_PRI, 50,
                       demotask1_stack, DEMO_TASK_STACKSIZE, demo_task1, 1);

    krhino_task_create(&demotask2_tcb, "demo2", 0, DEMO_TASK_PRI, 50,
                       demotask2_stack, DEMO_TASK_STACKSIZE, demo_task2, 1);

    return 0;
}
