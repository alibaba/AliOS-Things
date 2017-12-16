/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "stdio.h"

#define RELOAD    (unsigned long )40000
#define NUM   100

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

static double       InTimeBUFF[sizeof(double)*NUM] ;
static volatile     uint32_t INT_NUM ;
static unsigned long    Endtime;
static unsigned long    starttime;


void IntRealtimetest(void *arg)
{
    WaitForNew_tick();
    unsigned short  i;
    INT_NUM = 0;

    memset(InTimeBUFF, 0, sizeof(double)*NUM);

    hobbit_timer0_stop();
    hobbit_timer0_init(RELOAD);
    hobbit_timer0_start();

    while (INT_NUM < NUM);

    for (i = 0; i < NUM; i++) {
        InTimeBUFF[i] = (double) Turn_to_Realtime(InTimeBUFF[i]);
    }

    show_times_detail(InTimeBUFF, NUM, "IntTime\t", 1);

    krhino_sem_give(SYNhandle);

    krhino_task_yield();

}

void SoftIntRealtimetest(void *arg)
{
    WaitForNew_tick();
    unsigned short  i;
    INT_NUM = 0;

    hobbit_timer0_stop();
    hobbit_timer0_init(RELOAD);

    memset(InTimeBUFF, 0, sizeof(double)*NUM);

    hobbit_timer0_start();

    do {
        starttime = hobbit_timer0_get_curval();
        __asm__("trap 1");

    } while (INT_NUM < NUM);

    hobbit_timer0_stop();

    for (i = 0; i < NUM; i++) {
        InTimeBUFF[i] = (double) Turn_to_Realtime(InTimeBUFF[i]);
    }

    show_times_detail(InTimeBUFF, NUM, "SoftIntTime\t", 1);

    krhino_sem_give(SYNhandle);

    krhino_task_yield();

}

#ifdef CONFIG_TEST_PERFORMANCE
void __attribute__((isr)) timer0_handler(void)
{
    Endtime = hobbit_timer0_get_curval();
    hobbit_timer0_stop();
    hobbit_timer0_clr();
    InTimeBUFF[INT_NUM] = (double)(RELOAD - Endtime);
    INT_NUM ++;

    if (INT_NUM < NUM)   {
        hobbit_timer0_start();
    }
}
#else
void timer0_handler(void)
{
    Endtime = hobbit_timer0_get_curval();
    hobbit_timer0_stop();
    hobbit_timer0_clr();
    InTimeBUFF[INT_NUM] = (double)(RELOAD - Endtime);
    INT_NUM ++;

    if (INT_NUM < NUM)   {
        hobbit_timer0_start();
    }
}
#endif

