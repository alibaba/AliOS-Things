/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "bsp.h"
#include <ior7f0c004m.h>
#include <ior7f0c004m_ext.h>
void LED_Init(void);

/* 200 * cpu_stack_t = 400byte */
#define DEMO_TASK_STACKSIZE     200
#define DEMO_TASK_PRIORITY1     21
#define DEMO_TASK_PRIORITY2     22

extern void tick_init(uint32_t ticks_per_sec);

static ktask_t     demo_task_obj1;
static ktask_t     demo_task_obj2;

static cpu_stack_t demo_task_buf1[DEMO_TASK_STACKSIZE];
static cpu_stack_t demo_task_buf2[DEMO_TASK_STACKSIZE];

static uint32_t    tst_cnt;
static ksem_t      sem_tst;

void demo_task1(void *arg)
{
    tick_init(RHINO_CONFIG_TICKS_PER_SECOND);
    krhino_sem_create(&sem_tst, "sem_tst", 0);
    while (1)
    {
        //P7_bit.no7 = ~P7_bit.no7 ;
        P7_bit.no7 = 1;
        krhino_sem_give(&sem_tst);
        krhino_task_sleep(10000/RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

void demo_task2(void *arg)
{
    while (1)
    {
        tst_cnt++;
        P7_bit.no7 = 0;
        krhino_sem_take(&sem_tst, RHINO_WAIT_FOREVER);
        //krhino_task_sleep(50);
    }
}

int main(void)
{
    krhino_init();
    LED_Init();

    krhino_task_create(&demo_task_obj1, "demo_task", 0,DEMO_TASK_PRIORITY1, 
        50, demo_task_buf1, DEMO_TASK_STACKSIZE, demo_task1, 1);

    krhino_task_create(&demo_task_obj2, "demo_task2", 0,DEMO_TASK_PRIORITY2, 
        50, demo_task_buf2, DEMO_TASK_STACKSIZE, demo_task2, 1);
    
    krhino_start();
    
    return 0;
}


void LED_Init(void)
{
  PFSEG2_bit.no3 = 0;
  PM7_bit.no7 = 0 ;
}
