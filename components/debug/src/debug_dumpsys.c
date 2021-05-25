/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "aos/cli.h"
#include "debug_api.h"

typedef struct {
    char       task_name[32];
    uint32_t   task_id;
    uint32_t   task_state;
    uint32_t   stack_size;
    size_t     free_size;
    uint64_t   time_total;
    uint8_t    task_prio;
    char       candidate;
#if (RHINO_CONFIG_CPU_NUM > 1)
    uint8_t    cpu_binded;
    uint8_t    cpu_num;
    uint8_t    cur_exc;
#endif
} dumpsys_task_info_t;


#define MM_LEAK_CHECK_ROUND_SCOND 10 * 1000
#define RHINO_BACKTRACE_DEPTH     10

ktimer_t g_mm_leak_check_timer;

uint32_t dumpsys_task_func(char *buf, uint32_t len, int32_t detail)
{
    kstat_t    rst;
    size_t     free_size  = 0;
    uint64_t   time_total = 0;
    /* consistent with "task_stat_t" */
    char    *cpu_stat[] = { "ERROR",    "RDY", "PEND",    "SUS",
                            "PEND_SUS", "SLP", "SLP_SUS", "DELETED"
                          };
    klist_t *taskhead   = &g_kobj_list.task_head;
    klist_t *taskend    = taskhead;
    klist_t *tmp;
    ktask_t *task;
    size_t   corenum;
    (void)corenum;

#if (RHINO_CONFIG_CPU_NUM > 1)
    ktask_t *candidate[RHINO_CONFIG_CPU_NUM];
#else
    ktask_t *candidate;
#endif
    char    yes       = 'N';
    int32_t taskstate = 0;
    int32_t tasknum   = 0;
    int32_t taskindex = 0;

    dumpsys_task_info_t *taskinfo;
    dumpsys_task_info_t *taskinfoeach;

    aos_cli_printf("-----------------------------------------------------------"
                   "---------------------\r\n");

    krhino_sched_disable();

#if (RHINO_CONFIG_CPU_NUM > 1)
    for (corenum = 0; corenum < RHINO_CONFIG_CPU_NUM; corenum++) {
        preferred_cpu_ready_task_get(&g_ready_queue, corenum);
        candidate[corenum] = g_preferred_ready_task[corenum];
    }

#else
    preferred_cpu_ready_task_get(&g_ready_queue, cpu_cur_get());
    candidate = g_preferred_ready_task[cpu_cur_get()];
#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        tasknum ++;
    }

    taskinfo = malloc(tasknum * sizeof(dumpsys_task_info_t));
    if (taskinfo ==  NULL) {
        krhino_sched_enable();
        return RHINO_NO_MEM;
    }
    memset(taskinfo, 0, tasknum * sizeof(dumpsys_task_info_t));

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {

        char name_cut[19];
        taskinfoeach = taskinfo + taskindex;
        taskindex++;

        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        const name_t *task_name;
        rst = krhino_task_stack_min_free(task, &free_size);

        if (rst != RHINO_SUCCESS) {
            free_size = 0;
        }

#if (RHINO_CONFIG_SYS_STATS > 0)
        time_total = task->task_time_total_run / 20;
#endif

        if (task->task_name != NULL) {
            task_name = task->task_name;
        } else {
            task_name = "anonym";
        }
        strncpy(taskinfoeach->task_name, task_name, sizeof(taskinfoeach->task_name) - 1);
        taskinfoeach->task_id = task->task_id;

#if (RHINO_CONFIG_CPU_NUM > 1)
        for (corenum = 0; corenum < RHINO_CONFIG_CPU_NUM; corenum++) {
            if (candidate[corenum] == task) {
                yes = 'Y';
            } else {
                yes = 'N';
            }
        }

#else
        if (candidate == task) {
            yes = 'Y';
        } else {
            yes = 'N';
        }

#endif
        taskstate = task->task_state >= sizeof(cpu_stat) / sizeof(cpu_stat[0])
                    ? 0
                    : task->task_state;
        taskinfoeach->task_state = taskstate;
        taskinfoeach->task_prio  = task->prio;
        taskinfoeach->stack_size = task->stack_size * sizeof(cpu_stack_t);
        taskinfoeach->free_size  = free_size * sizeof(cpu_stack_t);
        taskinfoeach->time_total = time_total;
        taskinfoeach->candidate  = yes;
#if (RHINO_CONFIG_CPU_NUM > 1)
        taskinfoeach->cpu_binded = task->cpu_binded;
        taskinfoeach->cpu_num    = task->cpu_num;
        taskinfoeach->cur_exc    = task->cur_exc;
#endif

        /* if not support %-N.Ms,cut it manually */
        if (strlen(task_name) > 18) {
            memset(name_cut, 0, sizeof(name_cut));
            memcpy(name_cut, task->task_name, 18);
            task_name = name_cut;

            strncpy(taskinfoeach->task_name, task_name, strlen(task_name));
        }
    }

    krhino_sched_enable();

#if (RHINO_CONFIG_CPU_NUM > 1)
    aos_cli_printf("Name               ID    State    Prio StackSize MinFreesize "
               "Runtime  Candidate cpu_binded cpu_num cur_exc\r\n");
#else
    aos_cli_printf("Name               ID    State    Prio StackSize MinFreesize "
               "Runtime  Candidate\r\n");
#endif

    aos_cli_printf("-----------------------------------------------------------"
                   "---------------------\r\n");

    for (taskindex = 0; taskindex < tasknum; taskindex++) {
        taskinfoeach = taskinfo + taskindex;

#if (RHINO_CONFIG_CPU_NUM > 1)
        aos_cli_printf("%-19s%-6d%-9s%-5d%-10d%-12u%-9u   %-11c%-10d%-10d%-10d\r\n",
                   taskinfoeach->task_name, taskinfoeach->task_id, cpu_stat[taskinfoeach->task_state],
                   taskinfoeach->task_prio, taskinfoeach->stack_size,
                   taskinfoeach->free_size, (uint32_t)taskinfoeach->time_total,
                   taskinfoeach->candidate, taskinfoeach->cpu_binded,
                   taskinfoeach->cpu_num, taskinfoeach->cur_exc);
#else
        aos_cli_printf("%-19s%-6d%-9s%-5d%-10d%-12u%-9u   %-11c \r\n",
                   taskinfoeach->task_name, taskinfoeach->task_id, cpu_stat[taskinfoeach->task_state],
                   taskinfoeach->task_prio, (int32_t)taskinfoeach->stack_size,
                   taskinfoeach->free_size, (uint32_t)taskinfoeach->time_total,
                   taskinfoeach->candidate);
#endif
    }

    free(taskinfo);
    aos_cli_printf("-----------------------------------------------------------"
                   "---------------------\r\n");

    return RHINO_SUCCESS;
}

static void task_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    dumpsys_task_func(NULL, 0, 1);
}

static void dumpsys_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 2) {
        aos_cli_printf("dumpsys help:\r\n");
        aos_cli_printf("dumpsys mm      : show kernel memory status.\r\n");
#if (RHINO_CONFIG_MM_DEBUG > 0)
        aos_cli_printf("dumpsys mm_info : show kernel memory has alloced.\r\n");
#endif
        return;
    }

    if (0 == strcmp(argv[1], "mm")) {
        debug_mm_overview(aos_cli_printf);
    }
#if (RHINO_CONFIG_MM_DEBUG > 0)
    else if (0 == strcmp(argv[1], "mm_info")) {
        dumpsys_mm_info_func(0);
    }
#endif
}


static void task_bt(char *buf, int32_t len, int32_t argc, char **argv)
{
    uint32_t task_id;
    ktask_t *task;

    switch (argc) {
        case 2:
            task_id = (uint32_t)strtoul(argv[1], 0, 10);
            break;
        default:
            aos_cli_printf("task_bt <taskid>\r\n"
                           "taskid : task id\r\n");
            return;
    }

    task = debug_task_find_by_id(task_id);
    if (task == NULL) {
        aos_cli_printf("taskid %d not exist\r\n", task_id);
        return;
    }

    if (((ktask_t *)task)->task_name) {
        aos_cli_printf("task name : %s\r\n", ((ktask_t *)task)->task_name);
        debug_backtrace_task((char *)(((ktask_t *)task)->task_name), aos_cli_printf);
    }
}


static void task_btn(char *buf, int32_t len, int32_t argc, char **argv)
{
    if (argc == 2) {
        debug_backtrace_task(argv[1], aos_cli_printf);
    } else {
        aos_cli_printf("task_btn <taskname>\r\n");
    }
}

#if (RHINO_CONFIG_MM_DEBUG > 0)
static void mem_leak(char *buf, int32_t len, int32_t argc, char **argv)
{
    static int call_cnt = 0;
    int query_index = -1;

    if (call_cnt == 0) {
        aos_cli_printf("memory leak check start.\r\n");
    }
    if (argc == 2) {
        query_index = strtoul(argv[1], NULL, 0);
        if (query_index > call_cnt) {
            query_index = -1;
        }
    } else {
        call_cnt++;
        printf("\r\nAdd tag %d when malloc\r\n", call_cnt);
    }

    dumpsys_mm_leakcheck(call_cnt, query_index);
}
#endif

static const struct cli_command dumpsys_cli_cmd[] = {
    { "tasklist", "list all thread info", task_cmd },
    { "dumpsys", "dump system info", dumpsys_cmd },
    { "taskbt", "list thread backtrace", task_bt },
    { "taskbtn", "list thread backtrace by name", task_btn },
#if (RHINO_CONFIG_MM_DEBUG > 0)
    { "mmlk", "memory leak info", mem_leak },
#endif
};


void debug_dumpsys_cmds_register(void)
{
    int32_t ret;

    ret = aos_cli_register_commands(dumpsys_cli_cmd, sizeof(dumpsys_cli_cmd) / sizeof(struct cli_command));
    if (ret) {
        printf("%s %d failed, ret = %d\r\n", __func__, __LINE__, ret);
    }
}
#endif /* AOS_COMP_CLI */
