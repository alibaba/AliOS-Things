/**
 * @file debug.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_DBG_H
#define AOS_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup debug_aos_api debug
 * @{
 */

#include <stdint.h>
#include <k_api.h>

/**
 * Show current contex backtrace
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_backtrace_now(int32_t (*print_func)(const char *fmt, ...));

/**
 * Show task backtrace
 *
 * @param[in]  taskname  the task name which is need to be got
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_backtrace_task(char *taskname, int32_t (*print_func)(const char *fmt, ...));

/**
 * Show the overview of memory(heap and pool)
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_mm_overview(int32_t (*print_func)(const char *fmt, ...));

/**
 * Show the overview of tasks
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_task_overview(int32_t (*print_func)(const char *fmt, ...));

/**
 * Show the overview of buf_queue
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_buf_queue_overview(int32_t (*print_func)(const char *fmt, ...));

/**
 * Show the overview of queue
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_queue_overview(int32_t (*print_func)(const char *fmt, ...));

/**
 * Show the overview of sem
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_sem_overview(int32_t (*print_func)(const char *fmt, ...));

/**
 * Show the overview of mutex
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_mutex_overview(int32_t (*print_func)(const char *fmt, ...));

/**
 * Show the overview of all(task/memory/bufqueue/queue/sem)
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_overview(int32_t (*print_func)(const char *fmt, ...));

/**
 * This function will statistics the task run time in the previous statistics cycle
 *
 * @return NULL
 */
void aos_debug_task_cpu_usage_stats(void);

/**
 * This function will get the cpuusage for the specified task
 *
 * @param[in]  taskname  the task name which is need to be got
 *
 * @return -1 is error, others is cpuusage, the units are 1/10,000
 */
int32_t aos_debug_task_cpu_usage_get(char *taskname);

/**
 * This function will get the cpuusage for the specified CPU
 *
 * @param[in]   cpuid   the cpu id to obtain CPU utilization
 *
 * @return cpuusage, the units are 1/10,000
 */
uint32_t aos_debug_total_cpu_usage_get(uint32_t cpuid);

/**
 * This function will show the statistics for CPU utilization
 *
 * @return NULL
 */
void aos_debug_total_cpu_usage_show(void);

/**
 * system assert, called by k_err_proc
 *
 * @param[in]  err  the kernel err status
 * @param[in]  file  same as __FILE__
 * @param[in]  file  same as __line__
 * @return NULL
 */
void aos_debug_fatal_error(kstat_t err, char *file, int32_t line);

/**
 * This function support debug print same as printf
 *
 * @return 0 is ok, others err
 */
int32_t aos_debug_printf(const char *fmt, ...);
#define printk aos_debug_printf

/**
 * This function init debug module
 *
 * @return NULL
 */
void aos_debug_init(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* AOS_DBG_H */
