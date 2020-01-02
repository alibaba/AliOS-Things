/**
 * @file debug.h
 *
 * @author    yx170385@alibaba-inc.com
 * @version   V1.0
 * @date      2019-12-23
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AOS_DBG_H
#define AOS_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** @addtogroup debug debug
 *  DEBUG API: debug service for AliOS Things
 *
 *  @{
 */

/**
 * Show backtrace when called by printf
 *
 * @note: printf should be check when used in interrupt routing
 *
 * @retrun NULL
 */
void aos_debug_backtrace_now(void);

/**
 * Show task backtrace when called by printf
 *
 * @note: printf should be check when used in interrupt routing
 *
 * @param[in]  taskname  the task name which is need to be got
 *
 * @retrun NULL
 */
void aos_debug_backtrace_task(char *taskname);

/**
 * Show the overview of memory(heap and pool)
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_mm_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of tasks
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_task_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of buf_queue
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_buf_queue_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of queue
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_queue_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of sem
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_sem_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of all(task/memory/bufqueue/queue/sem)
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void aos_debug_overview(void);

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
 * @return cpuusage, the units are 1/10,000
 */
int32_t aos_debug_task_cpu_usage_get(char *taskname);

/**
 * This function will get the cpuusage for the specified CPU
 *
 * @param[in]   cpuid   the cpu id to obtain CPU utilization
 *
 * @return -1 is error, others is cpuusage, the units are 1/10,000
 */
uint32_t aos_debug_total_cpu_usage_get(uint32_t cpuid);

/**
 * This function will show the statistics for CPU utilization
 *
 * @return NULL
 */
void aos_debug_total_cpu_usage_show(void);

/**
 * Get system reboot reason
 *
 * @return reboot reason:
 * 0x01 for watchdog reset;
 * 0x02 for system panic
 * 0x03 for system repower
 * 0x04 for system fatal error
 */
unsigned int aos_debug_reboot_reason_get(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_DBG_H */
