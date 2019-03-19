/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <timer.h>

#define LOOP_ROUND 20
#define PERIOD     10

static timer_t g_timer;
static sem_t   timer_sem;

static int timer_cb_cnt;

static void timer_cb(timer_t timer, void *arg)
{
    timer_cb_cnt++;

    printf("%s: timer cb cnt %d \r\n", __func__, timer_cb_cnt);
    if (timer_cb_cnt == LOOP_ROUND) {
        timer_stop(&g_timer);
        printf("%s: post timer_sem\r\n", __func__);
        sem_post(&timer_sem);
    }
}

int timer_test(void)
{
    int stat;

    timer_cb_cnt = 0;

    stat = sem_init(&timer_sem, 0);
    if (stat != 0) {
        printf("%s: create sem failed \r\n", __func__);
        return -2;
    }

    stat = timer_create(&g_timer, (timer_func)timer_cb, NULL, 1, PERIOD, 1);
    if (stat != 0) {
        printf("%s: create timer failed\r\n", __func__);
        goto out1;
    }

    stat = sem_wait(&timer_sem);
    if (stat != 0) {
        printf("%s: sem take fail, stat %d\r\n", __func__, stat);
        goto out2;
    }

out2:
    stat = timer_destory(&g_timer);
    if (stat != 0) {
        printf("%s: timer del failed \r\n", __func__);
        goto out1;
    }

out1:
    sem_destory(&timer_sem);

    return (timer_cb_cnt == LOOP_ROUND)? 0: -1;
}

