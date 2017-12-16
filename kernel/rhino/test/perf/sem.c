/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "stdio.h"

#define TASK_TEST_PRI     20
#define TASK_MAIN_PRI     15
#define TASK_TEST_STACK_SIZE 1024
#define  SWITCH_NUM  100

extern ksem_t   *SYNhandle;
extern void WaitForNew_tick(void);
extern double   Turn_to_Realtime(double counter);
extern void show_times_detail(volatile double   *ft,  int nsamples,
                              char *title, uint32_t ignore_first);

extern void hobbit_timer0_stop(void);
extern void hobbit_timer0_init(uint32_t hz);
extern void hobbit_timer0_start(void);
extern uint32_t hobbit_timer0_get_curval(void);
extern void hobbit_timer0_clr(void);

static volatile unsigned long   Starttime, Endtime, Runtime;
static double           ShufBUFF[sizeof(double)*SWITCH_NUM] ;
static volatile unsigned  long  ShufSwitch = 0;
static ksem_t       *Shufhandle[2];
static ktask_t   *ShufTaskHandle[2];
static ksem_t       *ShufSynhandle;


static void BinaryShuf1(void *arg)
{
    while (1) {

        Starttime = hobbit_timer0_get_curval();
        krhino_sem_give(Shufhandle[0]);
        krhino_sem_take(Shufhandle[1], RHINO_WAIT_FOREVER);

        if (ShufSwitch >= SWITCH_NUM) {
            krhino_sem_give(ShufSynhandle);
        }
    }
}


static void BinaryShuf2(void *arg)
{
    WaitForNew_tick();

    while (1) {
        krhino_sem_take(Shufhandle[0], RHINO_WAIT_FOREVER);

        Endtime = hobbit_timer0_get_curval();
        Runtime = Starttime - Endtime;

        ShufBUFF[ShufSwitch] = Runtime;
        krhino_sem_give(Shufhandle[1]);
        ShufSwitch++;

        if (ShufSwitch >= SWITCH_NUM) {
            krhino_sem_give(ShufSynhandle);
        }
    }
}

void BinaryShufTimetest(void *arg)
{
    unsigned long i ;
    WaitForNew_tick();

    krhino_sem_dyn_create(&ShufSynhandle, "synsem", 0);
    ShufSwitch = 0;

    hobbit_timer0_stop();
    hobbit_timer0_init(0xffffffff);

    memset(ShufBUFF, 0, sizeof(double)*SWITCH_NUM);

    krhino_sem_dyn_create(&Shufhandle[0], "sem", 0);
    krhino_sem_dyn_create(&Shufhandle[1], "sem", 0);


    krhino_task_dyn_create(&ShufTaskHandle[0], "test_task", 0, TASK_TEST_PRI + 1,
                           0, TASK_TEST_STACK_SIZE, BinaryShuf1, 1);
    krhino_task_dyn_create(&ShufTaskHandle[1], "test_task", 0, TASK_TEST_PRI + 1,
                           0, TASK_TEST_STACK_SIZE, BinaryShuf2, 1);

    hobbit_timer0_start();

    krhino_sem_take(ShufSynhandle, RHINO_WAIT_FOREVER);

    krhino_task_dyn_del(ShufTaskHandle[0]);
    krhino_task_dyn_del(ShufTaskHandle[1]);

    for (i = 0; i < SWITCH_NUM; i++) {
        ShufBUFF[i] = (double) Turn_to_Realtime(ShufBUFF[i]);
    }

    show_times_detail(ShufBUFF , SWITCH_NUM, "BinaryShuf\t", 1);
    krhino_task_sleep(30);
    krhino_sem_give(SYNhandle);
}
