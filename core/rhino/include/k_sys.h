/**
 * @file k_sys.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_SYS_H
#define K_SYS_H

/** @addtogroup aos_rhino sys
 *  OS system functions
 *
 *  @{
 */

#define RHINO_VERSION  12000
#define RHINO_IDLE_PRI (RHINO_CONFIG_PRI_MAX - 1)
#define RHINO_NO_WAIT      0u
#define RHINO_WAIT_FOREVER ((uint64_t)-1)
#define MAX_TIMER_TICKS ((tick_t)-1 >> 1)

typedef enum
{
    RHINO_FALSE = 0u,
    RHINO_TRUE  = 1u
} RHINO_BOOL;

typedef char     name_t;
typedef uint8_t  suspend_nested_t;
typedef uint32_t sem_count_t;
typedef uint32_t mutex_nested_t;
typedef uint64_t sys_time_t;
typedef uint64_t tick_t;
typedef int64_t  tick_i_t;
typedef uint64_t idle_count_t;
typedef uint64_t ctx_switch_t;

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
extern cpu_stack_t *g_intrpt_stack_bottom;
#else
extern cpu_stack_t *g_intrpt_stack_top;
#endif
#endif /* RHINO_CONFIG_INTRPT_STACK_OVF_CHECK */

/**
 * Init krhino.
 *
 * @param[in]  NULL
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 *
 */
kstat_t krhino_init(void);

/**
 * Start krhino.
 *
 * @param[in]  NULL
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_start(void);

/**
 * Interrupt handler starts, called when enter interrupt.
 *
 * @param[in]  NULL
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_intrpt_enter(void);

/**
 * Interrupt handler ends, called when exit interrupt.
 *
 * @param[in]  NULL
 *
 * @return  NULL
 */
void krhino_intrpt_exit(void);

/**
 * Check system stack (used by interrupt) overflow.
 *
 * @param[in]  NULL
 *
 * @return  NULL
 */
void krhino_intrpt_stack_ovf_check(void);

/**
 * Get the number of ticks before next os tick event.
 *
 * @param[in]  NULL
 *
 * @return  RHINO_WAIT_FOREVER or the number of ticks
 */
tick_t krhino_next_sleep_ticks_get(void);

/**
 * Get the whole ram space used by krhino global variable.
 *
 * @param[in]  NULL
 *
 * @return  the whole ram space used by kernel
 */
size_t krhino_global_space_get(void);

/**
 * Get kernel version.
 *
 * @param[in]  NULL
 *
 * @return the kernel version
 */
uint32_t krhino_version_get(void);

/** @} */

#endif /* K_SYS_H */

