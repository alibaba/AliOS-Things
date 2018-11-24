/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

#define LOOP_ROUND 50
#define STACK_SIZE 0x400

static ktask_t  *mutex_task_obj;

static int child_task_cnt = 0;

static void child_task_run(void *arg)
{

    child_task_cnt += 1;

    krhino_utask_del(krhino_cur_task_get());
}

int task_test(void)
{
    cpu_stack_t *stack;
    kstat_t      stat;
    int          i;

    for (i = 0; i < LOOP_ROUND; i++) {
        stack = malloc(STACK_SIZE*sizeof(cpu_stack_t));
        if (stack == NULL) {
            printf("%s, allocate stack failed\r\n", __func__);
            goto out;
        }

        stat = krhino_utask_create(&mutex_task_obj, "child_test_task", 0,
                                   AOS_DEFAULT_APP_PRI, (tick_t)0, stack,
                                   STACK_SIZE, STACK_SIZE,
                                   (task_entry_t)child_task_run, 1);

        if (stat != RHINO_SUCCESS) {
            printf("%s create mutex_test task failed, ret code %d\r\n",
                    __func__, stat);
            goto out;
        }

        krhino_task_sleep(1);
    }

    /* wait all child tasks exit */
    krhino_task_sleep(10);

out:
    return (child_task_cnt == LOOP_ROUND)? 0: -1;
}

