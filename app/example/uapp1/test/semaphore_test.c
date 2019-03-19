
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <task.h>
#include <semaphore.h>
#include <systime.h>
#include <errnox.h>

#define LOOP_ROUND 10
#define STACK_SIZE 0x400

static task_t client_task;
static sem_t  client_server_sem;

static bool server_loop_exit;
static bool client_loop_exit;
static int  client_loop_cnt;

static int timedwait_test(void)
{
    int ret = 0;
    int stat;
    systime_t start, end;
    int wait_ms;

    sem_t sem;

    stat = sem_init(&sem, 0);
    if (0 != stat) {
        ret = -5;
        goto out;
    }

    start = sys_time();
    wait_ms = 200;
    stat = sem_timedwait(&sem, wait_ms);
    if (-EAGAIN != stat) {
        ret = -6;
        goto err1;
    }

    end = sys_time();
    if ((end < start) || (end - start < wait_ms)) {
        printf("%s: wait ticks error, start 0x%x, end 0x%x\r\n",
               __func__, (int)start, (int)end);
        ret = -7;
        goto err1;
    }

err1:
    if (0 != sem_destory(&sem)) {
        printf("%s: del sem failed\r\n", __func__);
        goto out;
    }

out:
    return ret;
}

static void client_task_run(void *arg)
{
    arg = arg;

    while (!server_loop_exit) {
        sem_wait(&client_server_sem);
        printf("\t\t client got sem\r\n");
        msleep(10);
        client_loop_cnt++;
    }

    client_loop_exit = 1;

    task_exit();
}

static int sem_task_test(void)
{
    int stat;
    int ret = 0;
    int i;

    server_loop_exit = 0;
    client_loop_exit = 0;
    client_loop_cnt = 0;

    stat = sem_init(&client_server_sem, 0);
    if (0 != stat) {
        return -8;
    }
    stat = task_create(&client_task, STACK_SIZE, client_task_run, NULL);
    if (0 != stat) {
        ret = -10;
        printf("%s: create sem_test_task failed, ret code %d\r\n",
               __func__, stat);
        goto del_sem;
    }

    for (i = 0; i < LOOP_ROUND; i++) {
        msleep(50);
        printf("server give sem\r\n");
        sem_post(&client_server_sem);
    }

    server_loop_exit = 1;
    while (!client_loop_exit) {
        msleep(1);
    }

    ret = (client_loop_cnt == LOOP_ROUND) ? 0: -10;

del_sem:
    if (0 != sem_destory(&client_server_sem)) {
        printf("%s: del sem failed\r\n", __func__);
        goto out;
    }

out:
    return ret;
}

int semaphore_test(void)
{
    int ret;

    ret = timedwait_test();
    if (ret) {
        return ret;
    }

    ret = sem_task_test();
    if (ret) {
        return ret;
    }

    return 0;
}

