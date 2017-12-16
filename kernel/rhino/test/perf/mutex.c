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
static ksem_t       *Shufhandle_1;
static kmutex_t    Shufhandle_0;
static ktask_t   *ShufTaskHandle[2];
static ksem_t       *ShufSynhandle;


static uint8_t task_pri_get(ktask_t *task)
{
    CPSR_ALLOC();

    uint8_t pri;

    RHINO_CRITICAL_ENTER();

    pri = task->prio;

    RHINO_CRITICAL_EXIT();

    return pri;
}

static void MutexShuf1(void *arg)
{
    unsigned long OurPriority;
    uint8_t oldpri;

    while (1) {
        krhino_mutex_lock(&Shufhandle_0, RHINO_WAIT_FOREVER);
        OurPriority = task_pri_get(krhino_cur_task_get());
        krhino_task_pri_change(ShufTaskHandle[1], OurPriority - 1, &oldpri);

        Starttime = hobbit_timer0_get_curval();
        krhino_mutex_unlock(&Shufhandle_0);

        krhino_sem_take(Shufhandle_1, RHINO_WAIT_FOREVER);

        if (ShufSwitch >= SWITCH_NUM) {
            krhino_sem_give(ShufSynhandle);
        }
    }
}


static void MutexShuf2(void *arg)
{
    unsigned long Priority;
    uint8_t oldpri;

    while (1) {
        krhino_mutex_lock(&Shufhandle_0, RHINO_WAIT_FOREVER);

        Endtime = hobbit_timer0_get_curval();
        Runtime = Starttime - Endtime;

        ShufBUFF[ShufSwitch] = (double)Runtime;

        krhino_mutex_unlock(&Shufhandle_0);
        krhino_sem_give(Shufhandle_1);
        ShufSwitch++;

        if (ShufSwitch >= SWITCH_NUM) {
            krhino_sem_give(ShufSynhandle);

        }

        Priority = task_pri_get(ShufTaskHandle[0]);
        krhino_task_pri_change(krhino_cur_task_get(), Priority + 1, &oldpri);
    }
}

void MutexShufTimetest(void *arg)
{
    unsigned long i ;

    WaitForNew_tick();
    ShufSwitch = 0;

    hobbit_timer0_stop();
    hobbit_timer0_init(0xffffffff);

    memset(ShufBUFF, 0, sizeof(double)*SWITCH_NUM);

    krhino_mutex_create(&Shufhandle_0, "mutex");
    krhino_sem_dyn_create(&Shufhandle_1, "sem", 0);

    krhino_sem_dyn_create(&ShufSynhandle, "sem", 0);

    krhino_task_dyn_create(&ShufTaskHandle[0], "test_task", 0, TASK_TEST_PRI + 1,
                           0, TASK_TEST_STACK_SIZE, MutexShuf1, 1);
    krhino_task_dyn_create(&ShufTaskHandle[1], "test_task", 0, TASK_TEST_PRI + 2,
                           0, TASK_TEST_STACK_SIZE, MutexShuf2, 1);

    hobbit_timer0_start();
    krhino_sem_take(ShufSynhandle, RHINO_WAIT_FOREVER);

    krhino_task_dyn_del(ShufTaskHandle[0]);
    krhino_task_dyn_del(ShufTaskHandle[1]);

    for (i = 0; i < SWITCH_NUM; i++) {
        ShufBUFF[i] = (double) Turn_to_Realtime(ShufBUFF[i]);
    }

    show_times_detail(ShufBUFF , SWITCH_NUM, "MutexShuf\t", 1);
    krhino_task_sleep(20);
    krhino_sem_give(SYNhandle);
}
