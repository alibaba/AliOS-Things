/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "stdio.h"

#define TASK_TEST_PRI     20
#define TASK_MAIN_PRI     15
#define TASK_TEST_STACK_SIZE 1024

#define  PreeTime  100

extern ksem_t *SYNhandle;
extern void WaitForNew_tick(void);
extern double Turn_to_Realtime(double counter);
extern void show_times_detail(volatile double   *ft,  int nsamples,
                              char *title, uint32_t ignore_first);

extern void hobbit_timer0_stop(void);
extern void hobbit_timer0_init(uint32_t hz);
extern void hobbit_timer0_start(void);
extern uint32_t hobbit_timer0_get_curval(void);
extern void hobbit_timer0_clr(void);

static ksem_t       *PreeSynhandle;
static double           PreeBUFF[sizeof(double)*PreeTime] ;
static  ktask_t      *PreeTaskHandle[5];
static volatile unsigned long   PreeCount = 0;
static volatile unsigned long   Starttime = 0, Endtime = 0, Runtime = 0;

static void Preemption0(void *arg)
{
    while (1) {

        Starttime = hobbit_timer0_get_curval();
        krhino_task_resume(PreeTaskHandle[1]);

        if (PreeCount >= PreeTime) {
            krhino_sem_give(PreeSynhandle);
        }
    }

}


static void Preemption1(void *arg)
{
    while (1) {

        Endtime = hobbit_timer0_get_curval();
        Runtime = Starttime - Endtime;

        PreeBUFF[PreeCount++] = (double)Runtime;

        if (PreeCount >= PreeTime) {
            krhino_sem_give(PreeSynhandle);
        }

        Starttime =  hobbit_timer0_get_curval();
        krhino_task_resume(PreeTaskHandle[2]);

        krhino_task_suspend(krhino_cur_task_get());
    }
}

static void Preemption2(void *arg)
{
    while (1) {

        Endtime = hobbit_timer0_get_curval();
        Runtime = Starttime - Endtime;
        PreeBUFF[PreeCount++] = (double)Runtime;

        if (PreeCount >= PreeTime) {
            krhino_sem_give(PreeSynhandle);
        }

        Starttime =  hobbit_timer0_get_curval();
        krhino_task_resume(PreeTaskHandle[3]);

        krhino_task_suspend(krhino_cur_task_get());
    }
}

static void Preemption3(void *arg)
{
    while (1) {

        Endtime = hobbit_timer0_get_curval();
        Runtime = Starttime - Endtime;

        PreeBUFF[PreeCount++] = (double)Runtime;

        if (PreeCount >= PreeTime) {
            krhino_sem_give(PreeSynhandle);
        }

        krhino_task_suspend(krhino_cur_task_get());
    }
}


void  PreemptionTimetest(void *arg)
{
    unsigned long i ;
    WaitForNew_tick();
    krhino_sem_dyn_create(&PreeSynhandle, "pree", 0);

    hobbit_timer0_stop();
    hobbit_timer0_init(0xffffffff);

    memset(PreeBUFF, 0, sizeof(double)*PreeTime);
    krhino_task_dyn_create(&PreeTaskHandle[0], "test_task", 0, TASK_TEST_PRI + 4,
                           0, TASK_TEST_STACK_SIZE, Preemption0, 1);
    krhino_task_dyn_create(&PreeTaskHandle[1], "test_task", 0, TASK_TEST_PRI + 3,
                           0, TASK_TEST_STACK_SIZE, Preemption1, 1);
    krhino_task_dyn_create(&PreeTaskHandle[2], "test_task", 0, TASK_TEST_PRI + 2,
                           0, TASK_TEST_STACK_SIZE, Preemption2, 1);
    krhino_task_dyn_create(&PreeTaskHandle[3], "test_task", 0, TASK_TEST_PRI + 1,
                           0, TASK_TEST_STACK_SIZE, Preemption3, 1);
    krhino_task_suspend(PreeTaskHandle[1]);
    krhino_task_suspend(PreeTaskHandle[2]);
    krhino_task_suspend(PreeTaskHandle[3]);

    hobbit_timer0_start();
    krhino_sem_take(PreeSynhandle, RHINO_WAIT_FOREVER);

    krhino_task_dyn_del(PreeTaskHandle[0]);
    krhino_task_dyn_del(PreeTaskHandle[1]);
    krhino_task_dyn_del(PreeTaskHandle[2]);
    krhino_task_dyn_del(PreeTaskHandle[3]);

    for (i = 0; i < PreeTime; i++) {
        PreeBUFF[i] = (double) Turn_to_Realtime(PreeBUFF[i]);
    }

    show_times_detail(PreeBUFF , PreeTime, "TaskPree\t", 1);
    krhino_task_sleep(5);
    krhino_sem_give(SYNhandle);
}
