/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <aos/aos.h>

#ifdef OSAL_RHINO
#include "k_api.h"

#define MM_LEAK_CHECK_ROUND_SCOND 10 * 1000
#define RHINO_BACKTRACE_DEPTH 10

extern char esc_tag[64];

#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
extern uint32_t dump_mmleak(void);
#endif

ktimer_t g_mm_leak_check_timer;

int dump_task_stack_byname(char *taskname);

#if (RHINO_CONFIG_SYSTEM_STATS > 0)

uint32_t dumpsys_task_func(char *buf, uint32_t len, int detail)
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

#if (RHINO_CONFIG_CPU_NUM > 1)
    ktask_t *candidate[RHINO_CONFIG_CPU_NUM];
    size_t   corenum;
#else
    ktask_t *candidate;
#endif
    char yes       = 'N';
    int  taskstate = 0;

    uint32_t task_cpu_usage = 0;
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    uint32_t total_cpu_usage = 0;
#endif

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

    aos_cli_printf("-----------------------------------------------------------"
                   "-------------\r\n");

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)

#if (RHINO_CONFIG_CPU_USAGE_PERIOD == 0)
    krhino_task_cpu_usage_stats();
#endif

    total_cpu_usage = krhino_total_cpu_usage_get();

    aos_cli_printf("CPU usage : %3d.%02d                    \r\n",
                   total_cpu_usage/100, total_cpu_usage%100);
    aos_cli_printf("-----------------------------------------------------------"
                   "-------------\r\n");

#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
    aos_cli_printf("Name               State    Prio StackSize MinFreesize "
                   "Runtime Candidate cpu_binded cpu_num cur_exc\r\n");
#else
    aos_cli_printf("Name               State    Prio StackSize MinFreesize "
                   "Runtime %%CPU Candidate\r\n");
#endif

    aos_cli_printf("-----------------------------------------------------------"
                   "-------------\r\n");

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        const name_t *task_name;
        rst = krhino_task_stack_min_free(task, &free_size);

        if (rst != RHINO_SUCCESS) {
            free_size = 0;
        }

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
        time_total = (sys_time_t)(task->task_time_total_run / 20);
        task_cpu_usage = krhino_task_cpu_usage_get(task);
#endif

        if (task->task_name != NULL) {
            task_name = task->task_name;
        } else {
            task_name = "anonym";
        }


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

#ifndef HAVE_NOT_ADVANCED_FORMATE
#if (RHINO_CONFIG_CPU_NUM > 1)
        aos_cli_printf(
          "%-19.18s%-9s%-5d%-10d%-12zu%-9llu%3d.%02d   %-11c%-10d%-10d%-10d\r\n",
          task_name, cpu_stat[taskstate], task->prio, task->stack_size,
          free_size, (unsigned long long)time_total, task_cpu_usage/100, task_cpu_usage%100, yes,
          task->cpu_binded, task->cpu_num, task->cur_exc);
#else
        aos_cli_printf("%-19.18s%-9s%-5d%-10d%-12zu%-9llu%3d.%02d   %-11c\r\n",
                       task_name, cpu_stat[taskstate], task->prio,
                       task->stack_size, free_size,
                       (unsigned long long)time_total, task_cpu_usage/100, task_cpu_usage%100, yes);
#endif

#else
        char name_cut[19];
        /* if not support %-N.Ms,cut it manually */
        if (strlen(task_name) > 18) {
            memset(name_cut, 0, sizeof(name_cut));
            memcpy(name_cut, task->task_name, 18);
            task_name = name_cut;
        }

#if (RHINO_CONFIG_CPU_NUM > 1)
        aos_cli_printf(
          "%-19s%-9s%-5d%-10d%-12u%-9u%3d.%02d   %-11c%-10d%-10d%-10d\r\n",
          task_name, cpu_stat[taskstate], task->prio, task->stack_size,
          free_size, (unsigned int)time_total, task_cpu_usage/100, task_cpu_usage%100, yes,
          task->cpu_binded, task->cpu_num, task->cur_exc);
#else
        aos_cli_printf("%-19s%-9s%-5d%-10d%-12u%-9u%3d.%02d   %-11c\r\n", task_name,
                       cpu_stat[taskstate], task->prio, task->stack_size,
                       free_size, (unsigned int)time_total, task_cpu_usage/100, task_cpu_usage%100,
                       yes);
#endif
#endif

#if 0
        /* for chip not support stack frame interface,do nothing*/
        if (detail == true && task != krhino_cur_task_get() && soc_get_first_frame_info &&
            soc_get_subs_frame_info) {
            depth = RHINO_BACKTRACE_DEPTH;
            snprintf(tmpbuf, 255, "%sTask %s Call Stack Dump:\r\n", esc_tag, task_name);
            safesprintf(printbuf, totallen, offset, tmpbuf);
            c_frame = (size_t)task->task_stack;
            soc_get_first_frame_info(c_frame, &n_frame, &pc);

            for (; (n_frame != 0) && (pc != 0) && (depth >= 0); --depth) {

                snprintf(tmpbuf, 255, "%sPC:0x%-12xSP:0x%-12x\r\n", esc_tag, c_frame, pc);
                safesprintf(printbuf, totallen, offset, tmpbuf);
                c_frame = n_frame;
                soc_get_subs_frame_info(c_frame, &n_frame, &pc);
            }
        }
#endif
    }


    aos_cli_printf("-----------------------------------------------------------"
                   "-------------\r\n");
    krhino_sched_enable();

    return RHINO_SUCCESS;
}
#endif

static uint32_t dumpsys_info_func(char *buf, uint32_t len)
{
    int16_t plen = 0;

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    uint32_t total_cpu_usage = 0;
#endif

    plen +=
      sprintf(buf + plen, "%s---------------------------------------------\r\n",
              esc_tag);
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
#if (RHINO_CONFIG_CPU_USAGE_PERIOD == 0)
        krhino_task_cpu_usage_stats();
#endif
    total_cpu_usage = krhino_total_cpu_usage_get();

    plen += sprintf(buf + plen, "%sCPU usage :%3d.%02d     \r\n", esc_tag,
                    total_cpu_usage/100, total_cpu_usage%100);
#endif

#if (RHINO_CONFIG_SCHED_STATS > 0)
    plen += sprintf(buf + plen, "%sMax sched disable time  :%-10d\r\n", esc_tag,
                    g_sched_disable_max_time);
#else
    plen +=
      sprintf(buf + plen, "%sMax sched disable time  :%-10d\r\n", esc_tag, 0);
#endif

#if (RHINO_CONFIG_INTRPT_STATS > 0)
    plen += sprintf(buf + plen, "%sMax intrpt disable time :%-10d\r\n", esc_tag,
                    g_intrpt_disable_max_time);
#else
    plen +=
      sprintf(buf + plen, "%sMax intrpt disable time :%-10d\r\n", esc_tag, 0);
#endif

    plen +=
      sprintf(buf + plen, "%s---------------------------------------------\r\n",
              esc_tag);

    return RHINO_SUCCESS;
}

#if (RHINO_CONFIG_MM_LEAKCHECK > 0)

uint32_t dumpsys_mm_leak_func(char *buf, uint32_t len)
{
    dump_mmleak();
    return RHINO_SUCCESS;
}

uint8_t mm_leak_timer_cb(void *timer, void *arg)
{
    dumpsys_mm_info_func(0);
    return 0;
}

uint32_t dumpsys_mm_leak_check_func(char *pcWriteBuffer, int xWriteBufferLen,
                                    int argc, char **argv)
{
    static uint32_t run_flag  = 0;
    sys_time_t      round_sec = MM_LEAK_CHECK_ROUND_SCOND;

    if (argc > 2 && 0 == strcmp(argv[2], "start")) {
        if (0 == run_flag) {
            if (argc > 3 && NULL != argv[3]) {
                round_sec = atoi(argv[3]) * 1000;
            }

            krhino_timer_create(&g_mm_leak_check_timer, "mm_leak_check_timer",
                                (timer_cb_t)mm_leak_timer_cb, 10,
                                krhino_ms_to_ticks(round_sec), NULL, 0);
        } else {
            if (NULL != argv[3]) {
                round_sec = atoi(argv[3]) * 1000;

                if (1 == run_flag) {
                    krhino_timer_stop(&g_mm_leak_check_timer);
                }

                krhino_timer_change(&g_mm_leak_check_timer, 10,
                                    krhino_ms_to_ticks(round_sec));
            }
        }

        run_flag = 1;
        krhino_timer_start(&g_mm_leak_check_timer);
    }

    if (argc > 2 && 0 == strcmp(argv[2], "stop")) {
        krhino_timer_stop(&g_mm_leak_check_timer);
        run_flag = 2;
    }

    return RHINO_SUCCESS;
}
#endif

uint32_t dumpsys_func(char *pcWriteBuffer, int xWriteBufferLen, int argc,
                      char **argv)
{
    uint32_t ret;

    if (argc >= 2 && 0 == strcmp(argv[1], "task")) {
        if (argc == 3 && (0 == strcmp(argv[2], "detail"))) {
            ret = dumpsys_task_func(pcWriteBuffer, xWriteBufferLen, true);
        } else {
            ret = dumpsys_task_func(pcWriteBuffer, xWriteBufferLen, false);
        }

        return ret;
    }
#ifndef CSP_LINUXHOST
    else if (argc >= 2 && 0 == strcmp(argv[1], "task_stack")) {
        if (argc == 3) {
            ret = dump_task_stack_byname(argv[2]);
        } else {
            ret =
              dump_task_stack_byname((char *)krhino_cur_task_get()->task_name);
        }

        return ret;
    }
#endif
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

#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
    else if (argc == 2 && 0 == strcmp(argv[1], "mm_leak")) {
        ret = dumpsys_mm_leak_func(NULL, 0);
        return ret;
    } else if (argc > 2 && 0 == strcmp(argv[1], "mm_monitor")) {
        ret = dumpsys_mm_leak_check_func(pcWriteBuffer, xWriteBufferLen, argc,
                                         argv);
        return ret;
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
#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
        len +=
          snprintf(pcWriteBuffer + len, xWriteBufferLen - len,
                   "%s\tdumpsys mm_leak      : show the memory maybe leak.\r\n",
                   esc_tag);
        len += snprintf(
          pcWriteBuffer + len, xWriteBufferLen - len,
          "%s\tdumpsys mm_monitor   : [start/stop] [round time] fire a timer"
          "to monitor mm, default 10s.\r\n",
          esc_tag);
#endif
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
        len += snprintf(pcWriteBuffer + len, xWriteBufferLen - len,
                        "%s\tdumpsys info         : show the system info\r\n",
                        esc_tag);
#endif
        return RHINO_SUCCESS;
    }
}

int dump_task_stack(ktask_t *task)
{
    uint32_t offset = 0;
    kstat_t  rst    = RHINO_SUCCESS;
    void *   cur, *end;
    int      i = 0;
    int *    p;

    krhino_sched_disable();

    end = task->task_stack_base + task->stack_size;

    rst = krhino_task_stack_cur_free(task, &offset);
    if (rst == RHINO_SUCCESS) {
        cur = task->task_stack_base + task->stack_size - offset;
    } else {
        k_err_proc(RHINO_SYS_SP_ERR);
        krhino_sched_enable();
        return 1;
    }
    p = (int *)cur;
    while (p < (int *)end) {
        if (i % 4 == 0) {
            aos_cli_printf("\r\n%08x:", (uint32_t)p);
        }
        aos_cli_printf("%08x ", *p);
        i++;
        p++;
    }
    aos_cli_printf("\r\n-----------------end----------------\r\n\r\n");
    krhino_sched_enable();

    return 0;
}

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
int dump_task_stack_byname(char *taskname)
{
    klist_t *taskhead = &g_kobj_list.task_head;
    klist_t *taskend  = taskhead;
    klist_t *tmp;
    ktask_t *task;
    int      printall = 0;

    if (strcmp(taskname, "all") == 0) {
        printall = 1;
    }

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        if (printall == 1 || strcmp(taskname, task->task_name) == 0) {
            aos_cli_printf("------task %s stack -------", task->task_name);
            dump_task_stack(task);
        }
    }

    return 0;
}
#endif

static void task_cmd(char *buf, int len, int argc, char **argv)
{
    dumpsys_task_func(NULL, 0, 1);
}

static void dumpsys_cmd(char *buf, int len, int argc, char **argv)
{
    dumpsys_func(buf, len, argc, argv);
}

struct cli_command dumpsys_cli_cmd[] = {
    { "tasklist", "list all thread info", task_cmd },
    { "dumpsys", "dump system info", dumpsys_cmd },
};


void dumpsys_cli_init(void)
{
    aos_cli_register_commands(&dumpsys_cli_cmd[0],
                              sizeof(dumpsys_cli_cmd) /
                                sizeof(struct cli_command));
}


#endif
