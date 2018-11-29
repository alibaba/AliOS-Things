/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

#define LOOP_ROUND 50
#define STACK_SIZE 0x400

static ktask_t  *mutex_task_obj;
static kmutex_t  test_mutex;
static bool      loop_exit;
static bool      del_mutex;
static int       mutex_cnt;

static void second_task_run(void *arg)
{
    while (1) {
        if (loop_exit)
            break;

        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        mutex_cnt++;
        krhino_task_sleep(5);
        krhino_mutex_unlock(&test_mutex);
    }

    del_mutex = 1;

    krhino_utask_del(krhino_cur_task_get());
}

int static_mutex_test(void)
{
    cpu_stack_t *stack;
    kstat_t      stat;

    loop_exit = 0;
    del_mutex = 0;
    mutex_cnt = 0;

    krhino_mutex_create(&test_mutex, "test_mutex");

    stack = malloc(STACK_SIZE*sizeof(cpu_stack_t));
    if (stack == NULL) {
        printf("%s, allocate stack failed\r\n", __func__);
        return -2;
    }

    stat = krhino_utask_create(&mutex_task_obj, "mutex_test_task", 0,
                               AOS_DEFAULT_APP_PRI, (tick_t)0, stack,
                               STACK_SIZE, STACK_SIZE,
                               (task_entry_t)second_task_run, 1);

    if (stat != RHINO_SUCCESS) {
        printf("%s create mutex_test task failed, ret code %d\r\n",
                __func__, stat);
        goto out;
    }

    for (int i = 0; i < LOOP_ROUND; i++) {
        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        krhino_task_sleep(2);
        krhino_mutex_unlock(&test_mutex);
    }

    loop_exit = 1;

    while (!del_mutex) {
        krhino_task_sleep(1);
    }

out:
    krhino_mutex_del(&test_mutex);

    return (mutex_cnt == LOOP_ROUND)? 0: -1;
}

