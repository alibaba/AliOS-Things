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
 * Get task handle by its name
 *
 * @param[in]  name  the task name which is need to be got.
 *
 * @return task handle when found, otherwise return NULL.
 */
ktask_t *debug_task_find(char *name);

/**
 * Get task running status by task handle
 *
 * @param[in]  task  the task handle which is need to be check
 *
 * @return  1 when task is running, otherwise return 0;
 */
int debug_task_is_running(ktask_t *task);

/**
 * Get the task stack base
 *
 * @note: get the current active task stack base if param[in] is NULL
 *
 * @param[in]  task  the task handle which is need to be got
 *
 * @return  task stack base
 */
void *debug_task_stack_bottom(ktask_t *task);

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
 * Get system reboot reason
 *
 * @return reboot reason:
 * 0x01 for watchdog reset;
 * 0x02 for system panic
 * 0x03 for system repower
 * 0x04 for system fatal error
 */
unsigned int debug_reboot_reason_get(void);

/**
 * convert int to ascii(HEX)
 * @note:  while using format % in libc, malloc/free is involved.
 * this function avoid using malloc/free. so it works when heap corrupt.
 *
 * @param[in]   num      number
 * @param[in]   str      fix 8 character str
 *
 * @return  str
 */
char *k_int2str(int num, char *str);


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DBG_API_H */
