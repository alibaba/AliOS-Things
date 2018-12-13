/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "time_test.h"

#define MODULE_NAME    "time_opr"
#define MODULE_NAME_CO "time_coopr"

void gettaskinfo(void)
{
    klist_t *taskhead = &g_kobj_list.task_head;
    klist_t *taskend  = taskhead;
    klist_t *tmp;
    ktask_t *task;

    printf("\n");
    printf("---------------------------------------------\n");
    printf("Name                 State          StackSize\n");
    printf("---------------------------------------------\n");
    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        printf("%s\t\t %d\t\t\%d\n", task->task_name, (int)task->task_state,
               (int)task->stack_size);
    }
    printf("---------------------------------------------\n");
}

static uint8_t time_opr_case(void)
{
    tick_t     ticks;
    sys_time_t ms;
    sys_time_t start;
    sys_time_t end;

    ticks = krhino_ms_to_ticks(1000);
    MYASSERT(ticks == RHINO_CONFIG_TICKS_PER_SECOND);

    ms = krhino_ticks_to_ms(RHINO_CONFIG_TICKS_PER_SECOND);
    MYASSERT(ms == 1000);

    start = krhino_sys_tick_get();
    krhino_task_sleep(10);
    end   = krhino_sys_tick_get();

    MYASSERT((end - start) < 15);

    start = krhino_sys_time_get();
    krhino_task_sleep(16);
    end   = krhino_sys_time_get();

    MYASSERT((end - start) < (20 * 1000 / RHINO_CONFIG_TICKS_PER_SECOND));

    return 0;
}

static const test_func_t time_func_runner[] = {
    time_opr_case,
    NULL
};

void time_opr_test(void)
{
    kstat_t ret;

    task_time_entry_register(MODULE_NAME, (test_func_t *)time_func_runner,
                             sizeof(time_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_time, MODULE_NAME, 0, TASK_TIME_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_time_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

