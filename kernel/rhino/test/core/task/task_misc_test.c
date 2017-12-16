/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#define TASK_TEST_STACK_SIZE 512
static ktask_t *task_misc;
static ktask_t  task_misc2;
static ktask_t *task_misc3;
static ktask_t *task_misc4;

static ksem_t *sem;
static ksem_t *sem2;
static ksem_t *sem3;

static ksem_t       *sem4;
static kmutex_t     *mutex4;
static kevent_t     *event4;
static ktimer_t     *timer4;
static kbuf_queue_t *buf_queue4;
static kqueue_t     *queue4;

static kmutex_t mutex;

void task_misc_entry2(void *arg)
{
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 10);

    krhino_task_del(NULL);
    krhino_task_dyn_del(&task_misc2);
    krhino_task_del(&task_misc2);
}

void task_misc_entry3(void *arg)
{
    krhino_sem_dyn_create(&sem3, "sem_misc33", 0);
    krhino_sem_take(sem3, RHINO_WAIT_FOREVER);

    krhino_sched_disable();
    krhino_task_dyn_del(task_misc3);
    krhino_sched_enable();

    krhino_task_dyn_del(task_misc3);
}

void task_misc_entry4(void *arg)
{
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    krhino_task_dyn_del(task_misc4);
}


static void timer_cb4(void *timer, void *arg)
{


}

void task_misc_entry(void *arg)
{
    size_t       task_free;
    uint8_t      old_pri;
    kstat_t      ret;
    uint8_t      policy;
    void        *info;
    cpu_stack_t *task_misc2_stack;
    ktask_t       tmp;

    CPSR_ALLOC();

    krhino_task_resume(task_misc3);
    krhino_task_del(task_misc3);

    krhino_sched_disable();
    krhino_task_suspend(krhino_cur_task_get());
    krhino_sched_enable();

    krhino_task_suspend(NULL);

    krhino_task_sleep(0);

    krhino_sched_disable();
    krhino_task_sleep(1);
    krhino_sched_enable();

    krhino_task_yield();

    krhino_task_stack_min_free(NULL, &task_free);
    krhino_task_stack_min_free(task_misc, NULL);
    krhino_task_stack_min_free(task_misc, &task_free);

    krhino_mutex_create(&mutex, "test");
    krhino_mutex_lock(&mutex, RHINO_WAIT_FOREVER);

    ret = krhino_task_pri_change(task_misc, 15, &old_pri);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_task_pri_change", PASS);
    } else {
        PRINT_RESULT("krhino_task_pri_change", FAIL);
        test_case_fail++;
    }

    krhino_mutex_unlock(&mutex);

    ret = krhino_task_pri_change(NULL, 15, &old_pri);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_task_pri_change para 1", PASS);
    } else {
        PRINT_RESULT("krhino_task_pri_change para 1", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_pri_change(task_misc, 15, NULL);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_task_pri_change para 2", PASS);
    } else {
        PRINT_RESULT("krhino_task_pri_change para 2", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_pri_change(task_misc, RHINO_IDLE_PRI, &old_pri);

    if (ret == RHINO_PRI_CHG_NOT_ALLOWED) {
        test_case_success++;
        PRINT_RESULT("krhino_task_pri_change para 3", PASS);
    } else {
        PRINT_RESULT("krhino_task_pri_change para 3", FAIL);
        test_case_fail++;
    }

    task_misc2_stack = krhino_mm_alloc(TASK_TEST_STACK_SIZE);

    if (task_misc2_stack == NULL) {

        PRINT_RESULT("krhino_mm_alloc failed", FAIL);
    }

    krhino_mutex_lock(&mutex, RHINO_WAIT_FOREVER);

    krhino_task_create(NULL, "task_misc2", NULL, 1,
                       0, task_misc2_stack, TASK_TEST_STACK_SIZE / sizeof(cpu_stack_t),
                       task_misc_entry2, 1);

    krhino_task_create(&task_misc2, NULL, NULL, 1,
                       0, task_misc2_stack, TASK_TEST_STACK_SIZE / sizeof(cpu_stack_t),
                       task_misc_entry2, 1);

    krhino_task_create(&task_misc2, "task_misc2", NULL, 1,
                       0, task_misc2_stack, TASK_TEST_STACK_SIZE / sizeof(cpu_stack_t),
                       NULL, 1);

    krhino_task_create(&task_misc2, "task_misc2", NULL, 1,
                       0, NULL, TASK_TEST_STACK_SIZE / sizeof(cpu_stack_t),
                       task_misc_entry2, 1);

    krhino_task_create(&task_misc2, "task_misc2", NULL, RHINO_CONFIG_PRI_MAX,
                       0, task_misc2_stack, 0, task_misc_entry2, 1);

    krhino_task_create(&task_misc2, "task_misc2", NULL, RHINO_CONFIG_PRI_MAX,
                       0, task_misc2_stack, 0, task_misc_entry2, 1);

    krhino_task_create(&task_misc2, "task_misc2", NULL, RHINO_CONFIG_PRI_MAX,
                       0, task_misc2_stack, TASK_TEST_STACK_SIZE / sizeof(cpu_stack_t),
                       task_misc_entry2, 1);

    krhino_task_create(&task_misc2, "task_misc2", NULL, RHINO_IDLE_PRI,
                       50, task_misc2_stack, TASK_TEST_STACK_SIZE / sizeof(cpu_stack_t),
                       task_misc_entry2, 1);

    krhino_task_create(&task_misc2, "task_misc2", NULL, 1,
                       50, task_misc2_stack, TASK_TEST_STACK_SIZE / sizeof(cpu_stack_t),
                       task_misc_entry2, 1);

    krhino_mutex_unlock(&mutex);

    ret = krhino_task_time_slice_set(NULL, 30);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_task_time_slice_set para 1", PASS);
    } else {
        PRINT_RESULT("krhino_task_time_slice_set para 1", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_time_slice_set(task_misc, 0);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_task_time_slice_set para 2", PASS);
    } else {
        PRINT_RESULT("krhino_task_time_slice_set para 2", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_time_slice_set(task_misc, 20);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_task_time_slice_set para 3", PASS);
    } else {
        PRINT_RESULT("krhino_task_time_slice_set para 3", FAIL);
        test_case_fail++;
    }

    krhino_sched_policy_set(task_misc, 0x11);

    ret = krhino_sched_policy_set(task_misc, KSCHED_FIFO);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_sched_policy_set para 1", PASS);
    } else {
        PRINT_RESULT("krhino_sched_policy_set para 1", FAIL);
        test_case_fail++;
    }

    ret = krhino_sched_policy_set(task_misc, KSCHED_RR);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_sched_policy_set para 2", PASS);
    } else {
        PRINT_RESULT("krhino_sched_policy_set para 2", FAIL);
        test_case_fail++;
    }

    ret = krhino_sched_policy_set(NULL, KSCHED_RR);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_sched_policy_set para 3", PASS);
    } else {
        PRINT_RESULT("krhino_sched_policy_set para 3", FAIL);
        test_case_fail++;
    }

    ret = krhino_sched_policy_get(NULL, &policy);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_sched_policy_get para 1", PASS);
    } else {
        PRINT_RESULT("ykrhino_sched_policy_get para 1", FAIL);
        test_case_fail++;
    }

    ret = krhino_sched_policy_get(task_misc, NULL);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_sched_policy_get para 2", PASS);
    } else {
        PRINT_RESULT("krhino_sched_policy_get para 2", FAIL);
        test_case_fail++;
    }

    ret = krhino_sched_policy_get(task_misc, &policy);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_sched_policy_get para 3", PASS);
    } else {
        PRINT_RESULT("krhino_sched_policy_get para 3", FAIL);
        test_case_fail++;
    }

#define INFO_IDX (RHINO_CONFIG_TASK_INFO_NUM - 1)
    ret = krhino_task_info_set(NULL, INFO_IDX, (void *)0x111);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_task_info_set para 2", PASS);
    } else {
        PRINT_RESULT("krhino_task_info_set para 2", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_info_set(task_misc, INFO_IDX, (void *)0x111);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_task_info_set para 3", PASS);
    } else {
        PRINT_RESULT("krhino_task_info_set para 3", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_info_get(task_misc, INFO_IDX, &info);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_task_info_get para 1", PASS);
    } else {
        PRINT_RESULT("krhino_task_info_get para 1", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_info_get(NULL, INFO_IDX, &info);

    if (ret == RHINO_NULL_PTR) {
        test_case_success++;
        PRINT_RESULT("krhino_task_info_get para 2", PASS);
    } else {
        PRINT_RESULT("krhino_task_info_get para 2", FAIL);
        test_case_fail++;
    }

#undef INFO_IDX

    task_free = krhino_global_space_get();

    if (task_free > 0u) {
        test_case_success++;
        PRINT_RESULT("krhino_global_space_get", PASS);
    } else {
        PRINT_RESULT("krhino_global_space_get", FAIL);
        test_case_fail++;
    }

    tmp.blk_state = BLK_ABORT;
    pend_state_end_proc(&tmp);

    tmp.blk_state = BLK_TIMEOUT;

    pend_state_end_proc(&tmp);

#if (RHINO_CONFIG_HW_COUNT > 0)
    krhino_overhead_measure();
#endif
    krhino_sem_give(sem3);

    ret = krhino_sem_dyn_create(&sem4, "del", 0);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_sem_dyn_create", PASS);
    } else {
        PRINT_RESULT("krhino_sem_dyn_create", FAIL);
        test_case_fail++;
    }

    ret = krhino_sem_dyn_del(sem4);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_sem_dyn_del", PASS);
    } else {
        PRINT_RESULT("krhino_sem_dyn_del", FAIL);
        test_case_fail++;
    }

    ret = krhino_mutex_dyn_create(&mutex4, "mutex4");

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_mutex_dyn_create", PASS);
    } else {
        PRINT_RESULT("krhino_mutex_dyn_create", FAIL);
        test_case_fail++;
    }

    ret = krhino_mutex_dyn_del(mutex4);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_mutex_dyn_del", PASS);
    } else {
        PRINT_RESULT("krhino_mutex_dyn_del", FAIL);
        test_case_fail++;
    }

    ret = krhino_event_dyn_create(&event4, "event4", 0);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_event_dyn_create", PASS);
    } else {
        PRINT_RESULT("krhino_event_dyn_create", FAIL);
        test_case_fail++;
    }

    ret = krhino_event_dyn_del(event4);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_event_dyn_del", PASS);
    } else {
        PRINT_RESULT("krhino_event_dyn_del", FAIL);
        test_case_fail++;
    }

    ret = krhino_timer_dyn_create(&timer4, "timer4", timer_cb4, 10, 0, 0, 0);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_timer_dyn_create", PASS);
    } else {
        PRINT_RESULT("krhino_timer_dyn_create", FAIL);
        test_case_fail++;
    }

    ret = krhino_timer_dyn_del(timer4);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_timer_dyn_del", PASS);
    } else {
        PRINT_RESULT("krhino_timer_dyn_del", FAIL);
        test_case_fail++;
    }

    ret = krhino_buf_queue_dyn_create(&buf_queue4, "queue4", 100, 20);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_buf_queue_dyn_create", PASS);
    } else {
        PRINT_RESULT("krhino_buf_queue_dyn_create", FAIL);
        test_case_fail++;
    }

    ret = krhino_buf_queue_dyn_del(buf_queue4);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_buf_queue_dyn_del", PASS);
    } else {
        PRINT_RESULT("krhino_buf_queue_dyn_del", FAIL);
        test_case_fail++;
    }

    ret = krhino_queue_dyn_create(&queue4, "queue4", 10);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_queue_dyn_create", PASS);
    } else {
        PRINT_RESULT("krhino_queue_dyn_create", FAIL);
        test_case_fail++;
    }

    ret = krhino_queue_dyn_del(queue4);

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_queue_dyn_del", PASS);
    } else {
        PRINT_RESULT("krhino_queue_dyn_del", FAIL);
        test_case_fail++;
    }

    ret = krhino_task_wait_abort(krhino_cur_task_get());

    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT("krhino_task_wait_abort ready", PASS);
    } else {
        PRINT_RESULT("krhino_task_wait_abort ready", FAIL);
        test_case_fail++;
    }

    krhino_task_dyn_create(&task_misc4, "task_misc_test4", 0, 1,
                           0, TASK_TEST_STACK_SIZE,
                           task_misc_entry4, 1);

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}

void task_misc_test(void)
{
    krhino_task_dyn_create(NULL, "task_misc_test", 0, 10,
                           0, TASK_TEST_STACK_SIZE,
                           task_misc_entry, 1);

    krhino_task_dyn_create(&task_misc, "task_misc_test", 0, 10,
                           0, TASK_TEST_STACK_SIZE,
                           task_misc_entry, 1);

    krhino_task_dyn_create(&task_misc3, "task_misc_test3", 0, 5,
                           0, TASK_TEST_STACK_SIZE,
                           task_misc_entry3, 1);
}

