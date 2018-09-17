/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "alios_cpp.h"

using namespace AliOS;

Timer *pTimer_1;

static void timer1_task(void *arg)
{
    static int count = 0;

    printf("timer1 count  %d\n", count++);

    if (count == 10) {
        pTimer_1->stop();
        pTimer_1->destory();
    }
}

void test_timer(void)
{
    pTimer_1 = new Timer;

    pTimer_1->create("timer1", (timer_cb_t)timer1_task, 1000, NULL);

    pTimer_1->start();
}
