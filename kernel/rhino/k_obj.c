/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

kstat_t      g_sys_stat;
uint8_t      g_idle_task_spawned[RHINO_CONFIG_CPU_NUM];

runqueue_t   g_ready_queue;

/* schedule lock counter */
uint8_t      g_sched_lock[RHINO_CONFIG_CPU_NUM];
uint8_t      g_intrpt_nested_level[RHINO_CONFIG_CPU_NUM];

/* highest pri task in ready queue */
ktask_t     *g_preferred_ready_task[RHINO_CONFIG_CPU_NUM];

/* current active task */
ktask_t     *g_active_task[RHINO_CONFIG_CPU_NUM];

/* idle task attribute */
ktask_t      g_idle_task[RHINO_CONFIG_CPU_NUM];
idle_count_t g_idle_count[RHINO_CONFIG_CPU_NUM];
cpu_stack_t  g_idle_task_stack[RHINO_CONFIG_CPU_NUM][RHINO_CONFIG_IDLE_TASK_STACK_SIZE];

/* tick attribute */
tick_t       g_tick_count;
klist_t      g_tick_head;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
kobj_list_t  g_kobj_list;
#endif

#if (RHINO_CONFIG_TIMER > 0)
klist_t          g_timer_head;
sys_time_t       g_timer_count;
ktask_t          g_timer_task;
cpu_stack_t      g_timer_task_stack[RHINO_CONFIG_TIMER_TASK_STACK_SIZE];
kbuf_queue_t     g_timer_queue;
k_timer_queue_cb timer_queue_cb[RHINO_CONFIG_TIMER_MSG_NUM];
#endif

#if (RHINO_CONFIG_SCHED_STATS > 0)
hr_timer_t   g_sched_disable_time_start;
hr_timer_t   g_sched_disable_max_time;
hr_timer_t   g_cur_sched_disable_max_time;
#endif

#if (RHINO_CONFIG_INTRPT_STATS > 0)
uint16_t     g_intrpt_disable_times;
hr_timer_t   g_intrpt_disable_time_start;
hr_timer_t   g_intrpt_disable_max_time;
hr_timer_t   g_cur_intrpt_disable_max_time;
#endif

#if (RHINO_CONFIG_HW_COUNT > 0)
hr_timer_t   g_sys_measure_waste;
#endif

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
ctx_switch_t g_sys_ctx_switch_times;
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
ksem_t       g_res_sem;
klist_t      g_res_list;
ktask_t      g_dyn_task;
cpu_stack_t  g_dyn_task_stack[RHINO_CONFIG_K_DYN_TASK_STACK];
#endif

#if (RHINO_CONFIG_WORKQUEUE > 0)
klist_t       g_workqueue_list_head;
kmutex_t      g_workqueue_mutex;
kworkqueue_t  g_workqueue_default;
cpu_stack_t   g_workqueue_stack[RHINO_CONFIG_WORKQUEUE_STACK_SIZE];
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
k_mm_head    *g_kmm_head;
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
kspinlock_t   g_sys_lock;
#endif
