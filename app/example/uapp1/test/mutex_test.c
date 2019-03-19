/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <task.h>
#include <mutex.h>
#include <systime.h>
#include <errnox.h>

#define LOOP_ROUND 50
#define STACK_SIZE 0x400

static task_t  mutex_task;
static task_t  timed_task;
static mutex_t child_mutex;
static mutex_t timed_mutex;

static bool loop_exit;
static bool del_mutex;
bool   timedlock_ret;
static int  mutex_cnt;

static void timed_func(void *arg)
{
    int stat;
    systime_t start, end;
    int wait_ms;

    start = sys_time();
    wait_ms = 200;
    stat = mutex_timedlock(&timed_mutex, wait_ms);
    if (-EAGAIN != stat) {
        goto out;
    }

    end = sys_time();
    if ((end < start) || (end - start < wait_ms)) {
        printf("%s: wait ticks error, start 0x%x, end 0x%x\r\n",
               __func__, (int)start, (int)end);
        goto out;
    }

    timedlock_ret = 1;

out:
    task_exit();
}

static int timedlock_test(void)
{
    int ret;
    int stat;

    timedlock_ret = 0;

    stat = mutex_init(&timed_mutex);
    if (0 != stat) {
        ret = -1;
        goto out;
    }

    stat = task_create(&timed_task, STACK_SIZE, timed_func, NULL);

    msleep(500);

    if (timedlock_ret) {
        ret = 0;
    }

    if (0 != mutex_destory(&timed_mutex)) {
        printf("%s: del sem failed\r\n", __func__);
        goto out;
    }

out:
    return ret;
}

static void child_task_run(void *arg)
{
    while (1) {
        if (loop_exit)
            break;

        mutex_lock(&child_mutex);
        mutex_cnt++;
        msleep(10);
        mutex_unlock(&child_mutex);
    }

    del_mutex = 1;

    task_exit();
}

static int mutex_task_test(void)
{
    int stat;

    loop_exit = 0;
    del_mutex = 0;
    mutex_cnt = 0;

    mutex_init(&child_mutex);

    stat = task_create(&mutex_task, STACK_SIZE, child_task_run, NULL);
    if (stat != 0) {
        printf("%s create mutex_test task failed, ret code %d\r\n",
                __func__, stat);
        goto out;
    }

    for (int i = 0; i < LOOP_ROUND; i++) {
        mutex_lock(&child_mutex);
        msleep(20);
        mutex_unlock(&child_mutex);
    }

    loop_exit = 1;

    while (!del_mutex) {
        msleep(10);
    }

out:
    mutex_destory(&child_mutex);

    return (mutex_cnt == LOOP_ROUND)? 0: -1;
}

int mutex_test(void)
{
    int ret;

    ret = timedlock_test();
    if (ret) {
        return ret;
    }

    return mutex_task_test();
}

