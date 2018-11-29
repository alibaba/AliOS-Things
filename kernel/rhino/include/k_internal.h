/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_INTERNAL_H
#define K_INTERNAL_H

extern kstat_t g_sys_stat;
extern uint8_t g_idle_task_spawned[RHINO_CONFIG_CPU_NUM];

extern runqueue_t g_ready_queue;

/* System lock */
extern uint8_t g_sched_lock[RHINO_CONFIG_CPU_NUM];
extern uint8_t g_intrpt_nested_level[RHINO_CONFIG_CPU_NUM];

/* highest pri ready task object */
extern ktask_t *g_preferred_ready_task[RHINO_CONFIG_CPU_NUM];

/* current active task */
extern ktask_t *g_active_task[RHINO_CONFIG_CPU_NUM];

/* idle attribute */
extern ktask_t      g_idle_task[RHINO_CONFIG_CPU_NUM];
extern idle_count_t g_idle_count[RHINO_CONFIG_CPU_NUM];
extern cpu_stack_t  g_idle_task_stack[RHINO_CONFIG_CPU_NUM]
                                    [RHINO_CONFIG_IDLE_TASK_STACK_SIZE];

/* tick attribute */
extern tick_t  g_tick_count;
extern klist_t g_tick_head;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
extern kobj_list_t g_kobj_list;
#endif

#if (RHINO_CONFIG_TIMER > 0)
extern klist_t          g_timer_head;
extern sys_time_t       g_timer_count;
extern ktask_t          g_timer_task;
extern cpu_stack_t      g_timer_task_stack[RHINO_CONFIG_TIMER_TASK_STACK_SIZE];
extern kbuf_queue_t     g_timer_queue;
extern k_timer_queue_cb timer_queue_cb[RHINO_CONFIG_TIMER_MSG_NUM];
#endif

#if (RHINO_CONFIG_SCHED_STATS > 0)
extern hr_timer_t g_sched_disable_time_start;
extern hr_timer_t g_sched_disable_max_time;
extern hr_timer_t g_cur_sched_disable_max_time;
#endif

#if (RHINO_CONFIG_INTRPT_STATS > 0)
extern uint16_t   g_intrpt_disable_times;
extern hr_timer_t g_intrpt_disable_time_start;
extern hr_timer_t g_intrpt_disable_max_time;
extern hr_timer_t g_cur_intrpt_disable_max_time;
#endif

#if (RHINO_CONFIG_HW_COUNT > 0)
extern hr_timer_t g_sys_measure_waste;
#endif

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
extern ctx_switch_t g_sys_ctx_switch_times;
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
extern ksem_t      g_res_sem;
extern klist_t     g_res_list;
extern ktask_t     g_dyn_task;
extern cpu_stack_t g_dyn_task_stack[RHINO_CONFIG_K_DYN_TASK_STACK];
#endif

#if (RHINO_CONFIG_WORKQUEUE > 0)
extern klist_t      g_workqueue_list_head;
extern kmutex_t     g_workqueue_mutex;
extern kworkqueue_t g_workqueue_default;
extern cpu_stack_t  g_workqueue_stack[RHINO_CONFIG_WORKQUEUE_STACK_SIZE];
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
extern k_mm_head *g_kmm_head;
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
extern kspinlock_t g_sys_lock;
#endif

#define K_OBJ_STATIC_ALLOC 1u
#define K_OBJ_DYN_ALLOC 2u

#define NULL_PARA_CHK(para)        \
    do {                           \
        if (para == NULL) {        \
            return RHINO_NULL_PTR; \
        }                          \
    } while (0)

#define INTRPT_NESTED_LEVEL_CHK()                        \
    do {                                                 \
        if (g_intrpt_nested_level[cpu_cur_get()] > 0u) { \
            RHINO_CRITICAL_EXIT();                       \
            return RHINO_NOT_CALLED_BY_INTRPT;           \
        }                                                \
    } while (0)

#define RES_FREE_NUM 4

typedef struct
{
    size_t  cnt;
    void *  res[RES_FREE_NUM];
    klist_t res_list;
} res_free_t;

ktask_t *preferred_cpu_ready_task_get(runqueue_t *rq, uint8_t cpu_num);

void core_sched(void);
void runqueue_init(runqueue_t *rq);

void ready_list_add(runqueue_t *rq, ktask_t *task);
void ready_list_add_head(runqueue_t *rq, ktask_t *task);
void ready_list_add_tail(runqueue_t *rq, ktask_t *task);
void ready_list_rm(runqueue_t *rq, ktask_t *task);
void ready_list_head_to_tail(runqueue_t *rq, ktask_t *task);

void time_slice_update(void);
void timer_task_sched(void);

void pend_list_reorder(ktask_t *task);
void pend_task_wakeup(ktask_t *task);
void pend_to_blk_obj(blk_obj_t *blk_obj, ktask_t *task, tick_t timeout);
void pend_task_rm(ktask_t *task);

kstat_t pend_state_end_proc(ktask_t *task);

void         idle_task(void *p_arg);

void tick_list_init(void);
void tick_task_start(void);
void tick_list_rm(ktask_t *task);
void tick_list_insert(ktask_t *task, tick_t time);
void tick_list_update(tick_i_t ticks);

uint8_t mutex_pri_limit(ktask_t *tcb, uint8_t pri);
void    mutex_task_pri_reset(ktask_t *tcb);
uint8_t mutex_pri_look(ktask_t *tcb, kmutex_t *mutex_rel);

kstat_t task_pri_change(ktask_t *task, uint8_t new_pri);

void ktimer_init(void);

void intrpt_disable_measure_start(void);
void intrpt_disable_measure_stop(void);
void dyn_mem_proc_task_start(void);

kstat_t ringbuf_init(k_ringbuf_t *p_ringbuf, void *buf, size_t len, size_t type,
                     size_t block_size);
kstat_t ringbuf_reset(k_ringbuf_t *p_ringbuf);
kstat_t ringbuf_push(k_ringbuf_t *p_ringbuf, void *data, size_t len);
kstat_t ringbuf_head_push(k_ringbuf_t *p_ringbuf, void *data, size_t len);
kstat_t ringbuf_pop(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen);
uint8_t ringbuf_is_full(k_ringbuf_t *p_ringbuf);
uint8_t ringbuf_is_empty(k_ringbuf_t *p_ringbuf);
void    workqueue_init(void);
void    k_mm_init(void);

#if (RHINO_CONFIG_CPU_PWR_MGMT > 0)
void cpu_pwr_down(void);
void cpu_pwr_up(void);
#endif

#endif /* K_INTERNAL_H */
