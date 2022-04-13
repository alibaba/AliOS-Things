/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "k_api.h"
#include "debug_api.h"
#include "aos/cli.h"

#if (RHINO_CONFIG_SYS_STATS > 0)

#define CPU_USAGE_DEFAULT_PRI      7
#define CPU_USAGE_STACK            4048

ktask_t *cpuusage_task;
int      sync_mode   = 0;
ksem_t   sync_sem;

struct statistics_param {
    uint32_t period;
    uint32_t total;
    uint32_t record_to_file;
};


typedef struct {
    char     task_name[32];
    uint32_t task_cpu_usage;
} task_cpuusage_info;

static uint32_t task_cpu_usage_period = 0;

void debug_task_cpu_usage_stats(void)
{
    uint32_t   i;
    klist_t    *taskhead = &g_kobj_list.task_head;
    klist_t    *taskend  = taskhead;
    klist_t    *tmp;
    ktask_t    *task;
    lr_timer_t cur_time;
    lr_timer_t exec_time;

    static lr_timer_t stats_start = 0;
    lr_timer_t        stats_end;

    (void)i; /* to avoid compiler warning */

    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#if (RHINO_CONFIG_CPU_NUM > 1)
    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        cur_time  = (lr_timer_t)LR_COUNT_GET();
        exec_time = cur_time - g_active_task[i]->task_time_start;

        g_active_task[i]->task_time_total_run += (sys_time_t)exec_time;
        g_active_task[i]->task_time_start     = cur_time;
    }
#else
    cur_time  = (lr_timer_t)LR_COUNT_GET();
    exec_time = cur_time - g_active_task[0]->task_time_start;

    g_active_task[0]->task_time_total_run += (sys_time_t)exec_time;
    g_active_task[0]->task_time_start     = cur_time;
#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        task->task_exec_time = task->task_time_total_run -
                               task->task_time_total_run_prev;
        task->task_time_total_run_prev = task->task_time_total_run;
    }
    RHINO_CRITICAL_EXIT();

    stats_end             = (lr_timer_t)LR_COUNT_GET();
    task_cpu_usage_period = stats_end - stats_start;
    stats_start           = stats_end;
}

void debug_total_cpu_usage_show(struct cpuusage_data * cpuusage_record, int32_t record_len,int32_t index)
{
    uint32_t    i, j;
    klist_t    *taskhead = &g_kobj_list.task_head;
    klist_t    *taskend  = taskhead;
    klist_t    *tmp;
    ktask_t    *task;
    const char *task_name;
    lr_timer_t  task_cpu_usage;
    uint32_t    total_cpu_usage[RHINO_CONFIG_CPU_NUM];

    uint32_t            tasknum = 0;
    task_cpuusage_info *taskinfo;
    task_cpuusage_info *taskinfoeach;

    if (cpuusage_record == NULL) {
        aos_cli_printf("-----------------------\r\n");
    }

#if (RHINO_CONFIG_CPU_NUM > 1)
    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        total_cpu_usage[i] = debug_total_cpu_usage_get(i);
        if (cpuusage_record != NULL) {
            if (cpuusage_record[i].taskname[0] == 0) {
                snprintf((char *)cpuusage_record[i].taskname, CPU_USAGE_MAX_TASKNAME_LEN, "CPU%d", (int)i);
            }
            cpuusage_record[i].cpuusage[index] = (float)total_cpu_usage[i] / 100;
        } else {
            aos_cli_printf("CPU%d usage :%3d.%02d%%  \r\n", (int)i, (int)total_cpu_usage[i] / 100,
                           (int)total_cpu_usage[i] % 100);
        }
    }
#else
    total_cpu_usage[0] = debug_total_cpu_usage_get(0);
    if (cpuusage_record != NULL) {
        if (cpuusage_record[0].taskname[0] == 0) {
            snprintf(cpuusage_record[0].taskname, CPU_USAGE_MAX_TASKNAME_LEN, "CPU");
        }
        cpuusage_record[0].cpuusage[index] = (float)total_cpu_usage[0] / 100;
    } else {
        aos_cli_printf("CPU usage :%3d.%02d%%  \r\n", (int)total_cpu_usage[0] / 100,
                       (int)total_cpu_usage[0] % 100);
    }
#endif

    if (cpuusage_record == NULL) {
        aos_cli_printf("---------------------------\r\n");
        aos_cli_printf("Name               %%CPU\r\n");
        aos_cli_printf("---------------------------\r\n");
    }

    krhino_sched_disable();
    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        tasknum ++;
    }
    taskinfo = krhino_mm_alloc(tasknum * sizeof(task_cpuusage_info));
    if (taskinfo ==  NULL) {
        krhino_sched_enable();
        return;
    }
    memset(taskinfo, 0, tasknum * sizeof(task_cpuusage_info));

    taskinfoeach = taskinfo;

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);

        if (task->task_name != NULL) {
            task_name = task->task_name;
        } else {
            task_name = "anonym";
        }
        taskinfoeach->task_cpu_usage = debug_task_cpu_usage_get(task);
        strncpy(taskinfoeach->task_name, task_name, sizeof(taskinfoeach->task_name) - 1);
        taskinfoeach++;
    }
    krhino_sched_enable();

    for (i = 0; i < tasknum; i++) {
        taskinfoeach   = taskinfo + i;
        task_name      = taskinfoeach->task_name;
        task_cpu_usage = taskinfoeach->task_cpu_usage;

        if (cpuusage_record != NULL) {
            for (j = RHINO_CONFIG_CPU_NUM; j < record_len; j++) {
                if (cpuusage_record[j].taskname[0] == 0) {
                    if (strlen(taskinfoeach->task_name) > CPU_USAGE_MAX_TASKNAME_LEN) {
                        aos_cli_printf("task name is longer than CPU_USAGE_MAX_TASKNAME_LEN\r\n");
                        break;
                    }
                    memcpy(cpuusage_record[j].taskname, taskinfoeach->task_name, strlen(taskinfoeach->task_name));
                }

                if (strcmp((char *)cpuusage_record[j].taskname, taskinfoeach->task_name) != 0) {
                    continue;
                }

                cpuusage_record[j].cpuusage[index] = (float)task_cpu_usage / 100;
                break;
            }
        } else {
            aos_cli_printf("%-19s%3d.%02d\r\n", task_name, (int)task_cpu_usage / 100,
                           (int)task_cpu_usage % 100);
        }
    }

    krhino_mm_free(taskinfo);

    if (cpuusage_record == NULL) {
        aos_cli_printf("---------------------------\r\n");
    }
}

/* one in ten thousand */
uint32_t debug_task_cpu_usage_get(ktask_t *task)
{
    uint32_t task_cpu_usage = 0;

    if (task_cpu_usage_period == 0) {
        return 0;
    }

    task_cpu_usage = (uint64_t)(task->task_exec_time) * 10000 / task_cpu_usage_period;
    if (task_cpu_usage > 10000) {
        task_cpu_usage = 10000;
    }

    return task_cpu_usage;
}

/* one in ten thousand */
uint32_t debug_total_cpu_usage_get(uint32_t cpuid)
{
    uint32_t total_cpu_usage = 0;
    total_cpu_usage          = 10000 - debug_task_cpu_usage_get(&g_idle_task[cpuid]);
    return total_cpu_usage;
}

void cpuusage_statistics(void *arg)
{
    ktask_t *cur_task;
    kstat_t  status;
    uint32_t period     = 0;
    uint32_t total      = 0;
    int32_t  stat_count = 0;
    int32_t  index      = 0;
    struct statistics_param *param  = (struct statistics_param *)arg;

    /* used for record to file */
    int fd  = -1;
    int i;

#if DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE
    int j;
    int ret = -1;
    char buf[CPU_USAGE_MAX_TASKNAME_LEN + 1];
#endif
    struct  cpuusage_data cpuusage_record[DEBUG_CPUUSAGE_MAX_TASK];

    period = param->period;
    total  = param->total;
    stat_count = total / period;

#if DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE
    if (param->record_to_file == 1) {
        for (i = 0; i < DEBUG_CPUUSAGE_MAX_TASK; i++) {
            memset(cpuusage_record[i].taskname, 0, CPU_USAGE_MAX_TASKNAME_LEN);
            cpuusage_record[i].cpuusage = krhino_mm_alloc(stat_count * sizeof(float));
            memset(cpuusage_record[i].cpuusage, 0, stat_count * sizeof(float));
        }
    }
#endif

    aos_cli_printf("Start to statistics CPU utilization, period %d ms\r\n", (int)period);
    if (total != 0) {
        aos_cli_printf("Total statistical time %d ms\r\n", (int)total);
    }

    debug_task_cpu_usage_stats();
    status = krhino_task_sleep(period * RHINO_CONFIG_TICKS_PER_SECOND / 1000);
    if (status == RHINO_TASK_CANCELED) {
        //krhino_task_cancel_clr();
    }

    /* If total is 0, it will run continuously */
    while ((index < stat_count) || (total == 0)) {
        if (krhino_task_cancel_chk() == RHINO_TRUE) {
            cur_task = krhino_cur_task_get();
            krhino_task_dyn_del(cur_task);
        }

        debug_task_cpu_usage_stats();
        status = krhino_task_sleep(period * RHINO_CONFIG_TICKS_PER_SECOND / 1000);
        if (status == RHINO_TASK_CANCELED) {
            //krhino_task_cancel_clr();
            break;
        } else {
            if (param->record_to_file == 1) {
#if DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE
                debug_total_cpu_usage_show(cpuusage_record, DEBUG_CPUUSAGE_MAX_TASK, index);
#endif
            } else {
                debug_total_cpu_usage_show(NULL, 0, 0);
            }

            index++;
        }
    }

#if DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE
    if (param->record_to_file == 1) {
        fd = open(DEBUG_CPUUSAGE_FILE_NAME, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (fd < 0) {
            aos_cli_printf("file open error %s, %d\r\n", __FILE__, __LINE__);
        }

        for (i = 0; i < DEBUG_CPUUSAGE_MAX_TASK; i++) {
            if (cpuusage_record[i].taskname[0] == 0) {
                break;
            }
            if (fd >= 0) {
                ret = snprintf(buf, sizeof(buf), "%-30s", cpuusage_record[i].taskname);
                if (ret > 0) {
                    write(fd, buf, ret);
                }
            } else {
                aos_cli_printf("%-30s", cpuusage_record[i].taskname);
            }
            for (j = 0; j < stat_count; j++) {
                if (fd >= 0) {
                    ret = snprintf(buf, sizeof(buf), "%7.2f", cpuusage_record[i].cpuusage[j]);
                    if (ret > 0) {
                        write(fd, buf, ret);
                    }
                } else {
                    aos_cli_printf("%7.2f", cpuusage_record[i].cpuusage[j]);
                }
            }
            if (fd >= 0) {
                write(fd, "\r\n", sizeof("\r\n"));
            } else {
                aos_cli_printf("\r\n");
            }
        }

        if (fd >= 0) {
            close(fd);
        }

        for (i = 0; i < DEBUG_CPUUSAGE_MAX_TASK; i++) {
            krhino_mm_free(cpuusage_record[i].cpuusage);
        }
    }
#endif

    printf("====CPU utilization statistics end====\r\n");
    if (sync_mode == 1) {
        krhino_sem_give(&sync_sem);
    }
}

void cpuusage_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    static struct statistics_param param = {.period = 0, .total = 0};

    ktask_t *task_tmp   = NULL;
    int      argv_index = 1;

    memset(&param, 0, sizeof(param));

    while (argv_index < argc) {
        if (0 == strcmp(argv[argv_index], "-d")) {
            argv_index++;
            if (argv_index < argc) {
                param.period = atoi(argv[argv_index]);
                if (param.period == 0) {
                    aos_cli_printf("warning -d parma is error\r\n");
                } else {
                    argv_index++;
                }
            }
        } else if (0 == strcmp(argv[argv_index], "-t")) {
            argv_index++;
            if (argv_index < argc) {
                param.total = atoi(argv[argv_index]);
                if (param.total == 0) {
                    aos_cli_printf("warning -t parma is error\r\n");
                } else {
                    argv_index++;
                }
            }
        } else if (0 == strcmp(argv[argv_index], "-f")) {
            param.record_to_file = 1;
            argv_index++;
        } else if (0 == strcmp(argv[argv_index], "-e")) {
            if (cpuusage_task != NULL) {
                krhino_task_cancel(cpuusage_task);
            }
            cpuusage_task = NULL;
            return;
        } else if (0 == strcmp(argv[argv_index], "-s")) {
            if (sync_mode == 0) {
                if ((krhino_sem_create(&sync_sem, "cpuusage_sync", 0)) != RHINO_SUCCESS) {
                    aos_cli_printf("warning -s parma is error\r\n");
                    continue;
                }
                argv_index++;
                sync_mode = 1;
            }
        } else {
            /* unrecognized parameters */
            argv_index++;
        }
    }

    if (param.period == 0) {
        param.period = 1000;
        aos_cli_printf("use default param: period %dms\r\n", param.period);
    }

    if ((param.record_to_file == 1) && (param.total == 0)) {
        param.total = 30000;
        aos_cli_printf("use default param: total %dms\r\n", param.total);
    }

    task_tmp = debug_task_find("cpuusage");
    if (task_tmp != NULL) {
        return;
    }

    krhino_task_dyn_create(&cpuusage_task, "cpuusage", (void *)&param, (uint8_t)CPU_USAGE_DEFAULT_PRI, 0, CPU_USAGE_STACK,
                           cpuusage_statistics, 1);

    if (sync_mode == 1) {
        krhino_sem_take(&sync_sem, RHINO_WAIT_FOREVER);

        krhino_sem_del(&sync_sem);
        sync_mode = 0;
    }
}

static const struct cli_command dumpsys_cpuusage_cmd[] = {
    {"cpuusage", "show cpu usage", cpuusage_cmd},
};

void debug_cpuusage_cmds_register(void)
{
    int32_t ret;

    ret = aos_cli_register_commands(dumpsys_cpuusage_cmd, sizeof(dumpsys_cpuusage_cmd) / sizeof(struct cli_command));
    if (ret) {
        printf("%s %d failed, ret = %d\r\n", __func__, __LINE__, ret);
    }
}

#endif /* #if (RHINO_CONFIG_SYS_STATS > 0) */
#endif /* AOS_COMP_CLI */
