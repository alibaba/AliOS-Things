/**
 * @file debug_api.h
 *
 * @author    yx170385@alibaba-inc.com
 * @version   V1.0
 * @date      2019-12-23
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef DBG_API_H
#define DBG_API_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup debug debug
 *  DEBUG API: debug service for AliOS Things
 *
 *  @{
 */

#include <stdio.h>
#include "k_api.h"

/* system reboot reason description */
#define DEBUG_REBOOT_REASON_WD_RST	  0x01 /**< Watchdog reset */
#define DEBUG_REBOOT_REASON_PANIC	  0x02 /**< System panic */
#define DEBUG_REBOOT_REASON_REPOWER   0x03 /**< System repower */
#define DEBUG_REBOOT_REASON_FATAL_ERR 0x04 /**< System fatal error */

/**
 * convert int to ascii(HEX)
 * while using format % in libc, malloc/free is involved.
 * this function avoid using malloc/free. so it works when heap corrupt.
 * @param[in]   num      number
 * @param[in]   str      fix 8 character str
 * @return  str
 */
char *k_int2str(int num, char *str);

/**
 * Show backtrace when called by printf
 *
 * @note: printf should be check when used in interrupt routing
 *
 * @retrun NULL
 */
void debug_backtrace_now(void);

/**
 * Show task backtrace when called by printf
 *
 * @note: printf should be check when used in interrupt routing
 *
 * @param[in]  taskname  the task name which is need to be got
 *
 * @retrun NULL
 */
void debug_backtrace_task(char *taskname);

/**
 * Show the overview of memory(heap and pool)
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void debug_mm_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of tasks
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void debug_task_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of buf_queue
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void debug_buf_queue_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of queue
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void debug_queue_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of sem
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void debug_sem_overview(int (*print_func)(const char *fmt, ...));

/**
 * Show the overview of all(task/memory/bufqueue/queue/sem)
 *
 * @param[in]  print_func  function to output information, NULL for "printf"
 *
 * @retrun NULL
 */
void debug_overview(void);

/**
 * This function will statistics the task run time in the previous statistics cycle
 * @return NULL
 */
void debug_task_cpu_usage_stats(void);

/**
 * This function will get the cpuusage for the specified task
 * @param[in]   task   the task to obtain CPU utilization
 * @return cpuusage, the units are 1/10,000
 */
uint32_t debug_task_cpu_usage_get(ktask_t *task);

/**
 * This function will get the cpuusage for the specified CPU
 * @param[in]   cpuid   the cpu id to obtain CPU utilization
 * @return cpuusage, the units are 1/10,000
 */
uint32_t debug_total_cpu_usage_get(uint32_t cpuid);

/**
 * This function will show the statistics for CPU utilization
 * @return NULL
 */
void debug_total_cpu_usage_show(void);

void panicHandler(void *context);
void debug_fatal_error(kstat_t err, char *file, int line);
void debug_reboot_reason_update(unsigned int reason);

/**
 * Get system reboot reason
 *
 * @return reboot reason:
 * DEBUG_REBOOT_REASON_WD_RST
 * DEBUG_REBOOT_REASON_PANIC
 * DEBUG_REBOOT_REASON_REPOWER
 * DEBUG_REBOOT_REASON_FATAL_ERR
 */
unsigned int debug_reboot_reason_get(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DBG_API_H */
