/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "cli/cli_api.h"

#include "cli_adapt.h"
#include "debug_api.h"

#ifdef OSAL_RHINO
#include "k_api.h"

typedef struct {
    char       task_name[32];
    uint32_t   task_state;
    uint32_t   stack_size;
    size_t     free_size;
    sys_time_t time_total;
    uint32_t   task_cpu_usage;
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

#if (RHINO_CONFIG_KOBJ_LIST > 0)

uint32_t dumpsys_task_func(char *buf, uint32_t len, int32_t detail)
{
    kstat_t    rst;
    size_t     free_size  = 0;
    sys_time_t time_total = 0;
    /* consistent with "task_stat_t" */
    char *   cpu_stat[] = { "ERROR",    "RDY", "PEND",    "SUS",
                         "PEND_SUS", "SLP", "SLP_SUS", "DELETED" };
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

    int32_t tasknum = 0;
    int32_t taskindex = 0;

    dumpsys_task_info_t *taskinfo;
    dumpsys_task_info_t *taskinfoeach;

    uint32_t task_cpu_usage = 0;
#if (RHINO_CONFIG_SYS_STATS > 0)
    uint32_t total_cpu_usage[RHINO_CONFIG_CPU_NUM] = {0};
#endif

    cli_printf("-----------------------------------------------------------"
               "-------------\r\n");

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

#if (RHINO_CONFIG_SYS_STATS > 0)
#if (DEBUG_CONFIG_CPU_USAGE_PERIOD == 0)
    debug_task_cpu_usage_stats();
#endif

    for (corenum = 0; corenum < RHINO_CONFIG_CPU_NUM; corenum++) {
        total_cpu_usage[corenum] = debug_total_cpu_usage_get(corenum);
    }

#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        tasknum ++;
    }

    taskinfo = cli_malloc(tasknum * sizeof(dumpsys_task_info_t));
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
        time_total = (sys_time_t)(task->task_time_total_run / 20);
        task_cpu_usage = debug_task_cpu_usage_get(task);
#endif

        if (task->task_name != NULL) {
            task_name = task->task_name;
        } else {
            task_name = "anonym";
        }
        strncpy(taskinfoeach->task_name,task_name,sizeof(taskinfoeach->task_name) - 1);

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
        taskinfoeach->task_prio = task->prio;
        taskinfoeach->stack_size = task->stack_size * sizeof(cpu_stack_t);
        taskinfoeach->free_size = free_size * sizeof(cpu_stack_t);
        taskinfoeach->time_total = time_total;
        taskinfoeach->task_cpu_usage = task_cpu_usage;
        taskinfoeach->candidate = yes;
#if (RHINO_CONFIG_CPU_NUM > 1)
        taskinfoeach->cpu_binded = task->cpu_binded;
        taskinfoeach->cpu_num = task->cpu_num;
        taskinfoeach->cur_exc = task->cur_exc;
#endif

        /* if not support %-N.Ms,cut it manually */
        if (strlen(task_name) > 18) {
            memset(name_cut, 0, sizeof(name_cut));
            memcpy(name_cut, task->task_name, 18);
            task_name = name_cut;

            strncpy(taskinfoeach->task_name,task_name,strlen(task_name));
        }
    }

    krhino_sched_enable();

    /*print out info*/
#if (RHINO_CONFIG_SYS_STATS > 0)
#if (RHINO_CONFIG_CPU_NUM > 1)
    for (corenum = 0; corenum < RHINO_CONFIG_CPU_NUM; corenum++) {
        cli_printf("CPU%d usage : %3d.%02d                    \r\n",
                   corenum, total_cpu_usage[corenum] / 100, total_cpu_usage[corenum] % 100);
    }

#else
    cli_printf("CPU%d usage : %3d.%02d                    \r\n",
               0, total_cpu_usage[0]/100, total_cpu_usage[0]%100);
#endif
    cli_printf("------------------------------------------------------------------------\r\n");
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
    cli_printf("Name               State    Prio StackSize MinFreesize "
               "Runtime %%CPU Candidate cpu_binded cpu_num cur_exc\r\n");
#else
    cli_printf("Name               State    Prio StackSize MinFreesize "
               "Runtime %%CPU Candidate\r\n");
#endif
    cli_printf("------------------------------------------------------------------------\r\n");


    for (taskindex = 0; taskindex < tasknum; taskindex++) {
        taskinfoeach = taskinfo + taskindex;

#if (RHINO_CONFIG_CPU_NUM > 1)
        cli_printf("%-19s%-9s%-5d%-10d%-12u%-9u%3d.%02d   %-11c%-10d%-10d%-10d\r\n",
                    taskinfoeach->task_name, cpu_stat[taskinfoeach->task_state],
                    taskinfoeach->task_prio, taskinfoeach->stack_size,
                    taskinfoeach->free_size, (uint32_t)taskinfoeach->time_total,
                    taskinfoeach->task_cpu_usage/100, taskinfoeach->task_cpu_usage%100,
                    taskinfoeach->candidate, taskinfoeach->cpu_binded,
                    taskinfoeach->cpu_num, taskinfoeach->cur_exc);
#else
        cli_printf("%-19s%-9s%-5d%-10d%-12u%-9u%3d.%02d   %-11c \r\n",
                    taskinfoeach->task_name, cpu_stat[taskinfoeach->task_state],
                    taskinfoeach->task_prio, (int32_t)taskinfoeach->stack_size,
                    taskinfoeach->free_size, (uint32_t)taskinfoeach->time_total,
                    (int32_t)(taskinfoeach->task_cpu_usage / 100),
                    (int32_t)(taskinfoeach->task_cpu_usage % 100),
                    taskinfoeach->candidate);
#endif
    }

    cli_free(taskinfo);
    cli_printf("-----------------------------------------------------------"
               "-------------\r\n");

    return RHINO_SUCCESS;
}
#endif

static uint32_t dumpsys_info_func(char *buf, uint32_t len)
{
    int16_t plen = 0;

    char *esc_tag = cli_tag_get();

#if (RHINO_CONFIG_SYS_STATS > 0)
    uint32_t total_cpu_usage[RHINO_CONFIG_CPU_NUM] = {0};
#endif

    plen +=
      sprintf(buf + plen, "%s---------------------------------------------\r\n",
              esc_tag);
#if (RHINO_CONFIG_SYS_STATS > 0)
#if (DEBUG_CONFIG_CPU_USAGE_PERIOD == 0)
    debug_task_cpu_usage_stats();
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
    for (uint32_t corenum = 0; corenum < RHINO_CONFIG_CPU_NUM; corenum++) {
        total_cpu_usage[corenum] = debug_total_cpu_usage_get(corenum);
        plen += sprintf(buf + plen, "%sCPU%d usage :%3d.%02d     \r\n", esc_tag,
                        corenum, (int32_t)total_cpu_usage[corenum] / 100,
                        (int32_t)total_cpu_usage[corenum] % 100);
    }

#else
    total_cpu_usage[0] = debug_total_cpu_usage_get(0);
    plen += sprintf(buf + plen, "%sCPU usage :%3d.%02d     \r\n", esc_tag,
                    (int32_t)total_cpu_usage[0] / 100,
                    (int32_t)total_cpu_usage[0] % 100);

#endif

#endif

#if (RHINO_CONFIG_SYS_STATS > 0)
    plen += sprintf(buf + plen, "%sMax sched disable time  :%-10d\r\n", esc_tag,
                    g_sched_disable_max_time);
    plen += sprintf(buf + plen, "%sMax intrpt disable time :%-10d\r\n", esc_tag,
                    g_intrpt_disable_max_time);
#else
    plen += sprintf(buf + plen, "%sMax sched disable time  :%-10d\r\n", esc_tag, 0);
    plen += sprintf(buf + plen, "%sMax intrpt disable time :%-10d\r\n", esc_tag, 0);
#endif

    plen += sprintf(buf + plen, "%s---------------------------------------------\r\n",
                    esc_tag);

    return RHINO_SUCCESS;
}

uint32_t dumpsys_func(char *pcWriteBuffer, int32_t xWriteBufferLen, int32_t argc,
                      char **argv)
{
    uint32_t ret;

    char *esc_tag = cli_tag_get();

    if (argc >= 2 && 0 == strcmp(argv[1], "task")) {
        if (argc == 3 && (0 == strcmp(argv[2], "detail"))) {
            ret = dumpsys_task_func(pcWriteBuffer, xWriteBufferLen, 1);
        } else {
            ret = dumpsys_task_func(pcWriteBuffer, xWriteBufferLen, 0);
        }

        return ret;
    }
    else if (argc == 2 && 0 == strcmp(argv[1], "info")) {
        ret = dumpsys_info_func(pcWriteBuffer, xWriteBufferLen);
        return ret;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0)
    else if (argc == 2 && 0 == strcmp(argv[1], "mm_info")) {
        ret = dumpsys_mm_info_func(0);
        return ret;
    }
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
    else if (argc == 2 && 0 == strcmp(argv[1], "mm")) {
        debug_mm_overview(NULL);
        return RHINO_SUCCESS;
    }
#endif

    else {
        int len = 0;
        len += snprintf(pcWriteBuffer + len, xWriteBufferLen - len,
                        "%sdumpsys help:\r\n", esc_tag);
        len += snprintf(pcWriteBuffer + len, xWriteBufferLen - len,
                        "%s\tdumpsys task         : show the task info.\r\n",
                        esc_tag);
#ifndef CSP_LINUXHOST
        len += snprintf(
          pcWriteBuffer + len, xWriteBufferLen - len,
          "%s\tdumpsys task_stack   : show the task stack info.\r\n", esc_tag);
#endif
        len += snprintf(
          pcWriteBuffer + len, xWriteBufferLen - len,
          "%s\tdumpsys mm_info      : show the memory has alloced.\r\n",
          esc_tag);

        len += snprintf(
          pcWriteBuffer + len, xWriteBufferLen - len,
          "%s\tdumpsys mm           : show the system memory.\r\n",
          esc_tag);

#if (RHINO_CONFIG_SYS_STATS > 0)
        len += snprintf(pcWriteBuffer + len, xWriteBufferLen - len,
                        "%s\tdumpsys info         : show the system info\r\n",
                        esc_tag);
#endif
        return RHINO_SUCCESS;
    }
}

static void task_cmd(char *buf, int len, int argc, char **argv)
{
    dumpsys_task_func(NULL, 0, 1);
}

static void dumpsys_cmd(char *buf, int len, int argc, char **argv)
{
    dumpsys_func(buf, len, argc, argv);
}

struct cli_command_st dumpsys_cli_cmd[] = {
    { "tasklist", "list all thread info", task_cmd },
    { "dumpsys", "dump system info", dumpsys_cmd },
};


void dumpsys_cli_init(void)
{
    int32_t num = sizeof(dumpsys_cli_cmd) / sizeof(struct cli_command_st);
    cli_register_commands(&dumpsys_cli_cmd[0], num);
}


#endif
