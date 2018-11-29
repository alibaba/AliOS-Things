
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

#define LOOP_ROUND 10
#define STACK_SIZE 0x400

static cpu_stack_t *stack;
static ktask_t     *task_obj;
static ksem_t      *client_server_sem;

static bool server_loop_exit;
static bool client_loop_exit;
static int  client_loop_cnt;

static int wait_test(void)
{
    int ret = 0;
    kstat_t stat;
    sys_time_t start, end;
    int wait_ticks;

    ksem_t *sem;

    stat = krhino_sem_dyn_create(&sem, "test_sem", 0);
    if (RHINO_SUCCESS != stat) {
        ret = -5;
        goto out;
    }

    wait_ticks = 10;
    start = krhino_sys_tick_get();
    stat = krhino_sem_take(sem, wait_ticks);
    if (RHINO_BLK_TIMEOUT != stat) {
        ret = -6;
        goto err1;
    }

    end = krhino_sys_tick_get();
    if ((end < start) || (end - start < wait_ticks)) {
        printf("%s: wait ticks error, start 0x%x, end 0x%x\r\n",
               __func__, (int)start, (int)end);
        ret = -7;
        goto err1;
    }

err1:
    if (RHINO_SUCCESS != krhino_sem_dyn_del(sem)) {
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
        krhino_sem_take(client_server_sem, RHINO_WAIT_FOREVER);
        printf("\t\t client got sem\r\n");
        krhino_task_sleep(1);
        client_loop_cnt++;
    }

    client_loop_exit = 1;

    krhino_utask_del(krhino_cur_task_get());
}

static int client_server_test(void)
{
    kstat_t stat;
    int ret = 0;
    int i;

    server_loop_exit = 0;
    client_loop_exit = 0;
    client_loop_cnt = 0;

    stat = krhino_sem_dyn_create(&client_server_sem, "test_sem", 0);
    if (RHINO_SUCCESS != stat) {
        return -8;
    }

    stack = malloc(STACK_SIZE*sizeof(cpu_stack_t));
    if (stack == NULL) {
        printf("%s: alloc stack failed\r\n", __func__);
        return -9;
    }

    stat = krhino_utask_create(&task_obj, "sem_test_task", 0,
                               AOS_DEFAULT_APP_PRI, (tick_t)0,
                               stack, STACK_SIZE, STACK_SIZE,
                               (task_entry_t)client_task_run, 1);

    if (RHINO_SUCCESS != stat) {
        ret = -10;
        printf("%s: create sem_test_task failed, ret code %d\r\n",
               __func__, stat);
        goto del_sem;
    }

    for (i = 0; i < LOOP_ROUND; i++) {
        krhino_task_sleep(5);
        printf("server give sem\r\n");
        krhino_sem_give(client_server_sem);
    }

    server_loop_exit = 1;
    while (!client_loop_exit) {
        krhino_task_sleep(1);
    }

    ret = (client_loop_cnt == LOOP_ROUND) ? 0: -10;

del_sem:
    if (RHINO_SUCCESS != krhino_sem_dyn_del(client_server_sem)) {
        printf("%s: del sem failed\r\n", __func__);
        goto out;
    }

out:
    return ret;
}

int dynamic_semaphore_test(void)
{
    int ret;

    ret = wait_test();
    if (ret) {
        return ret;
    }

    ret = client_server_test();
    if (ret) {
        return ret;
    }
}

