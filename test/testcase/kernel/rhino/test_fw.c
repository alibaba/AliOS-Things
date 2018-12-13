/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

uint32_t            test_case_success;
uint32_t            test_case_fail;
uint32_t            test_case_check_err;
static ksem_t       test_case_sem;
static kmutex_t     test_case_mutex;
static test_case_t *case_func_runner = NULL;

extern void task_test(void);
extern void mm_test(void);
extern void mm_blk_test(void);
extern void time_test(void);
extern void sys_test(void);
extern void event_test(void);
extern void sem_test(void);
extern void tasksem_test(void);
extern void mutex_test(void);
extern void timer_test(void);
extern void queue_test(void);
extern void buf_queue_test(void);
extern void comb_test(void);
extern void workqueue_test(void);
extern void ysh_cmd_test(void);
extern void mm_region_test(void);
extern void ringbuf_test(void);

test_case_map_t test_fw_map[] = {
    {"task_test", task_test},
#if (RHINO_CONFIG_MM_TLF > 0)
    {"mm_test",   mm_test},
#endif
    {"mm_blk_test", mm_blk_test},
    {"time_test", time_test},
    {"sys_test", sys_test},
    {"event_test", event_test},
    {"sem_test", sem_test},
    {"tasksem_test", tasksem_test},
    {"mutex_test", mutex_test},
    {"timer_test", timer_test},
    {"ringbuf_test", ringbuf_test},
    {"queue_test", queue_test},
#if (RHINO_CONFIG_WORKQUEUE > 0)
    {"workqueue_test", workqueue_test},
#endif
    {"buf_queue_test", buf_queue_test},
    {"comb_test", comb_test},
    /* last must be NULL! */
    {NULL, NULL},
};

uint8_t test_case_register(test_case_t *case_runner)
{
    if (case_func_runner == NULL) {
        case_func_runner = case_runner;
        return 0;
    }

    return 1;
}

void test_case_unregister(void)
{
    if (case_func_runner != NULL) {
        case_func_runner = NULL;
    }
}

void test_case_run(void)
{
    test_case_t *runner;

    runner = (test_case_t *)case_func_runner;

    while (1) {
        if (*runner == NULL) {
            break;
        }

        (*runner)();
        runner++;
        next_test_case_wait();
    }
}

void test_case_init(void)
{
    test_case_success = 0;
    test_case_fail = 0;
    krhino_sem_create(&test_case_sem, "test_case_sem", 0);
    krhino_mutex_create(&test_case_mutex, "test_case_mutex");
}

void test_case_cleanup(void)
{
    krhino_sem_del(&test_case_sem);
    krhino_mutex_del(&test_case_mutex);
}

void test_case_critical_enter(void)
{
    krhino_mutex_lock(&test_case_mutex, RHINO_WAIT_FOREVER);
}

void test_case_critical_exit(void)
{
    krhino_mutex_unlock(&test_case_mutex);
}

void next_test_case_notify(void)
{
    if (krhino_sem_give(&test_case_sem) != RHINO_SUCCESS) {
        printf("next_test_case_notify failed!!!\n");
    }
}

void next_test_case_wait(void)
{
    if (krhino_sem_take(&test_case_sem, RHINO_WAIT_FOREVER) != RHINO_SUCCESS) {
        printf("next_test_case_wait failed!!!\n");
    }

    krhino_task_sleep(10);
}
