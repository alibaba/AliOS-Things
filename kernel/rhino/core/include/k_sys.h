/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_SYS_H
#define K_SYS_H

#define RHINO_VERSION      12000
#define RHINO_IDLE_PRI     (RHINO_CONFIG_PRI_MAX - 1)
#define RHINO_FALSE        0u
#define RHINO_TRUE         1u

#define RHINO_NO_WAIT      0u
#define RHINO_WAIT_FOREVER ((uint64_t)-1)

#define MAX_TIMER_TICKS    ((tick_t)-1 >> 1)

typedef uint64_t        sys_time_t;
typedef int64_t         sys_time_i_t;
typedef uint64_t        idle_count_t;
typedef uint64_t        tick_t;
typedef int64_t         tick_i_t;

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
extern cpu_stack_t *g_intrpt_stack_bottom;
#else
extern cpu_stack_t *g_intrpt_stack_top;
#endif
#endif /* RHINO_CONFIG_INTRPT_STACK_OVF_CHECK */

/**
 * This function will init AliOS
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t       krhino_init(void);

/**
 * This function will start AliOS
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t       krhino_start(void);

/**
 * This function will enter interrupt
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t       krhino_intrpt_enter(void);

/**
 * This function will exit interrupt
 */
void          krhino_intrpt_exit(void);

/**
 * This function will check intrpt-stack overflow
 */
void          krhino_intrpt_stack_ovf_check(void);

/**
 * This function get the system next sleep ticks
 */
tick_t krhino_next_sleep_ticks_get(void);

/**
 * This function will get the whole ram space used by kernel
 * @return  the whole ram space used by kernel
 */
size_t        krhino_global_space_get(void);

/**
 * This function will get kernel version
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
uint32_t      krhino_version_get(void);

#endif /* K_SYS_H */

