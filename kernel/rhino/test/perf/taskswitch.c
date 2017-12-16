/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "stdio.h"

#define TASK_TEST_PRI     20
#define TASK_MAIN_PRI     15
#define TASK_TEST_STACK_SIZE 1024

extern ksem_t *SYNhandle;
extern void WaitForNew_tick(void);
extern double   Turn_to_Realtime(double counter);
extern void show_times_detail(volatile double   *ft,  int nsamples,
                              char *title, uint32_t ignore_first);

extern void hobbit_timer0_stop(void);
extern void hobbit_timer0_init(uint32_t hz);
extern void hobbit_timer0_start(void);
extern uint32_t hobbit_timer0_get_curval(void);
extern void hobbit_timer0_clr(void);

#define   TASKSWITCH_NUM   100

static volatile unsigned long   TaskSwitch = 0;
static ktask_t       *xSwitchTaskHandle[2];
static volatile unsigned long   Starttime, Endtime;
static volatile   double        SwitchTimeBUFF[sizeof(double)*TASKSWITCH_NUM] ;
static ksem_t       *Switchhandle[2];
static ksem_t       *SwitchSynhandle;


static void SwitchTask3(void *arg)
{
    while (1) {
        for (TaskSwitch = 0; TaskSwitch < TASKSWITCH_NUM;) {
            Starttime = hobbit_timer0_get_curval();
            krhino_task_yield();
            Endtime = hobbit_timer0_get_curval();
            SwitchTimeBUFF[TaskSwitch++] = (double)(Starttime - Endtime);
        }

        krhino_sem_give(SwitchSynhandle);
    }
}

static void SwitchTask4(void *arg)
{
    while (1) {
        for (; TaskSwitch < TASKSWITCH_NUM;) {
            Endtime = hobbit_timer0_get_curval();
            SwitchTimeBUFF[TaskSwitch++] = (double)(Starttime - Endtime);
            Starttime = hobbit_timer0_get_curval();
            krhino_task_yield();
        }

        krhino_sem_give(SwitchSynhandle);
    }

}

void TaskYIELDtimeTest(void *arg)
{
    unsigned long i ;

    krhino_sem_dyn_create(&SwitchSynhandle, "switch", 0);
    TaskSwitch = 0;
    WaitForNew_tick();

    hobbit_timer0_stop();

    hobbit_timer0_init(0xffffffff);

    memset((void *)SwitchTimeBUFF, 0, sizeof(double)*TASKSWITCH_NUM);

    krhino_sem_dyn_create(&Switchhandle[0], "s1", 0);

    krhino_sem_dyn_create(&Switchhandle[1], "s1", 0);


    krhino_task_dyn_create(&xSwitchTaskHandle[0], "test_task", 0, TASK_TEST_PRI + 1,
                           0, TASK_TEST_STACK_SIZE, SwitchTask3, 1);

    krhino_task_dyn_create(&xSwitchTaskHandle[1], "test_task", 0, TASK_TEST_PRI + 1,
                           0, TASK_TEST_STACK_SIZE, SwitchTask4, 1);

    hobbit_timer0_start();

    krhino_sem_take(SwitchSynhandle, RHINO_WAIT_FOREVER);

    krhino_task_dyn_del(xSwitchTaskHandle[0]);
    krhino_task_dyn_del(xSwitchTaskHandle[1]);


    for (i = 0; i < TASKSWITCH_NUM; i++) {
        SwitchTimeBUFF[i] =  Turn_to_Realtime(SwitchTimeBUFF[i]);

    }

    show_times_detail(SwitchTimeBUFF , TASKSWITCH_NUM, "TaskYIELD\t", 1);
    krhino_task_sleep(10);

    krhino_sem_give(SYNhandle);

}
