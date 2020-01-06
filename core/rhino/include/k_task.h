/**
 * @file k_task.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_TASK_H
#define K_TASK_H

/** @addtogroup aos_rhino task
 *  Task management
 *
 *  @{
 */


/**
 * Task status
 */
typedef enum {
    K_SEED,
    K_RDY,              /**< READY, task in ready list (g_ready_queue) */
    K_PEND,             /**< PEND, task in tick list (g_tick_head) and block list (sem/mutex/queue/...) */
    K_SUSPENDED,
    K_PEND_SUSPENDED,
    K_SLEEP,            /**< SLEEP, task tick list (g_tick_head) */
    K_SLEEP_SUSPENDED,
    K_DELETED,
} task_stat_t;

typedef void (*task_entry_t)(void *arg);

/**
 * Task control information
 */
typedef struct {
    /**<
     *  Task SP, update when task switching.
     *  Access by assemble code, so do not change position.
     */
    void            *task_stack;
    /**< access by activation, so do not change position */
    const name_t    *task_name;
#if (RHINO_CONFIG_TASK_INFO > 0)
    /**< access by assemble code, so do not change position */
    void            *user_info[RHINO_CONFIG_TASK_INFO_NUM];
#endif

#if (RHINO_CONFIG_USER_SPACE > 0)
    void            *task_ustack;
    uint32_t         ustack_size;
    uint32_t         pid;
    uint8_t          mode;
    uint8_t          is_proc;
    cpu_stack_t     *task_ustack_base;
    klist_t          task_user;
    void            *task_group;
#endif
    /**<
     *  Task stack info
     *  start 'task_stack_base', len 'stack_size * sizeof(cpu_stack_t)'
     */
    cpu_stack_t     *task_stack_base;
    uint32_t         stack_size;
    /**<
     *  Put task into different linked lists according to the status:
     *  1. ready queue. The list hade is g_ready_queue->cur_list_item[prio]
     *  2. The pending queue. The list hade is blk_obj in the blocking object (sem / mutex / queue, etc.).
     *     The final blocking result is recorded in blk_state
     */
    klist_t          task_list;
    /**< Count of entering the K_SUSPENDED state */
    suspend_nested_t suspend_count;
    /**< Mutex owned by this task */
    struct mutex_s  *mutex_list;
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    /**< Link all task for statistics */
    klist_t          task_stats_item;
#endif
    /**< Put task into the timeout list of the tick, list head is 'g_tick_head' */
    klist_t          tick_list;
    /**< When 'g_tick_count' reaches tick_match, task's PEND or SLEEP state expires. */
    tick_t           tick_match;
    /**< Countdown of the PEND state */
    tick_t           tick_remain;

    /**< Passing massage, for 'queue' and 'buf_queue' */
    void            *msg;
#if (RHINO_CONFIG_BUF_QUEUE > 0)
    /**< Record the msg length, for 'buf_queue'. */
    size_t           bq_msg_size;
#endif
    /**<  */
    /**< Task status */
    task_stat_t      task_state;
    /**< Reasons for the end of the blocking state */
    blk_state_t      blk_state;

    /* Task block on mutex, queue, semphore, event */
    blk_obj_t       *blk_obj;

#if (RHINO_CONFIG_TASK_SEM > 0)
    /**< Task semaphore  */
    struct sem_s    *task_sem_obj;
#endif

#if (RHINO_CONFIG_SYS_STATS > 0)
    size_t           task_free_stack_size;
    ctx_switch_t     task_ctx_switch_times;
    uint64_t         task_time_total_run;
    uint64_t         task_time_total_run_prev;
    lr_timer_t       task_time_this_run;
    lr_timer_t       task_exec_time;
    lr_timer_t       task_time_start;
    hr_timer_t       task_intrpt_disable_time_max;
    hr_timer_t       task_sched_disable_time_max;
#endif

#if (RHINO_CONFIG_SCHED_RR > 0)
    /**< During this round of scheduling, tasks can execute 'time_slice' ticks */
    uint32_t         time_slice;
    /**< Once RR scheduling, tasks can execute a total of 'time_total' ticks */
    uint32_t         time_total;
#endif

#if (RHINO_CONFIG_EVENT_FLAG > 0)
    uint32_t         pend_flags;
    void            *pend_info;
    uint8_t          pend_option;
#endif
    /**< KSCHED_FIFO / KSCHED_RR / KSCHED_CFS */
    uint8_t          sched_policy;

    cfs_node         node;
    /**< On which CPU the task runs */
    uint8_t          cpu_num;

#if (RHINO_CONFIG_CPU_NUM > 1)
    /**< Whether the task is binded to the cpu, 0 no, 1 yes */
    uint8_t          cpu_binded;
    /**< Whether the task is ready to execute, 0 no, 1 yes */
    uint8_t          cur_exc;
    klist_t          task_del_item;
#endif

#if (RHINO_CONFIG_TASK_DEL > 0)
    uint8_t          cancel;
#endif

    /**< current prio */
    uint8_t          prio;
    /**< base prio */
    uint8_t          b_prio;
    /**< buffer from internal malloc or caller input */
    uint8_t          mm_alloc_flag;
} ktask_t;

/**
 * Create a task and provide the stack space.
 *
 * @param[in]  task       the task to be created (the space is provided outside, by user)
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  pri        the priority of task
 * @param[in]  ticks      the time slice
 * @param[in]  stack_buf  the start address of task stack
 * @param[in]  stack_size task stack size = stack_size * sizeof(cpu_stack_t)
 * @param[in]  entry      the entry function of task
 * @param[in]  autorun    the autorunning flag of task
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *stack_buf,
                           size_t stack_size, task_entry_t entry, uint8_t autorun);

kstat_t krhino_cfs_task_create(ktask_t *task, const name_t *name, void *arg,
                               uint8_t prio, cpu_stack_t *stack_buf, size_t stack_size,
                               task_entry_t entry, uint8_t autorun);

#if (RHINO_CONFIG_CPU_NUM > 1)
/**
 * Create a task and provide the stack space, bind the task to specific cpu.
 *
 * @param[in]  task       the task to be created (the space is provided outside, by user)
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  pri        the priority of task
 * @param[in]  ticks      the time slice
 * @param[in]  stack_buf  the start address of task stack
 * @param[in]  stack_size task stack size = stack_size * sizeof(cpu_stack_t)
 * @param[in]  entry      the entry function of task
 * @param[in]  cpu_num    the cpu that task bind to
 * @param[in]  autorun    the autorunning flag of task
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_cpu_create(ktask_t *task, const name_t *name, void *arg,
                               uint8_t prio, tick_t ticks, cpu_stack_t *stack_buf,
                               size_t stack_size, task_entry_t entry, uint8_t cpu_num,
                               uint8_t autorun);

kstat_t krhino_cfs_task_cpu_create(ktask_t *task, const name_t *name, void *arg,
                                   uint8_t prio, cpu_stack_t *stack_buf, size_t stack_size,
                                   task_entry_t entry, uint8_t cpu_num, uint8_t autorun);

/**
 * Bind a task to specific cpu.
 *
 * @param[in]  task       the task to be binded
 * @param[in]  cpu_num    the cpu that task bind to
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_cpu_bind(ktask_t *task, uint8_t cpu_num);

/**
 * Unbind a task.
 *
 * @param[in]  task       the task to be binded
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
 kstat_t krhino_task_cpu_unbind(ktask_t *task);
#endif


#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Create a task and malloc the stack space.
 *
 * @param[out] task     the task to be created (the space is provided inside, from heap)
 * @param[in]  name     the name of task
 * @param[in]  arg      the parameter of task enter function
 * @param[in]  pri      the priority of task
 * @param[in]  ticks    the time slice
 * @param[in]  stack    task stack size = stack * sizeof(cpu_stack_t)
 * @param[in]  entry    the entry function of task
 * @param[in]  autorun  the autorunning flag of task
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_dyn_create(ktask_t **task, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, size_t stack,
                               task_entry_t entry, uint8_t autorun);

kstat_t krhino_cfs_task_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                   uint8_t pri, size_t stack, task_entry_t entry,
                                   uint8_t autorun);

#if (RHINO_CONFIG_CPU_NUM > 1)
/**
 * Create a task and malloc the stack space, bind the task to specific cpu.
 *
 * @param[in]  task       the task to be created (the space is provided inside, from heap)
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  pri        the priority of task
 * @param[in]  ticks      the time slice
 * @param[in]  stack      task stack size = stack * sizeof(cpu_stack_t)
 * @param[in]  entry      the entry function of task
 * @param[in]  cpu_num    the cpu that task bind to
 * @param[in]  autorun    the autorunning flag of task
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_cpu_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                   uint8_t pri, tick_t ticks, size_t stack,
                                   task_entry_t entry, uint8_t cpu_num, uint8_t autorun);

kstat_t krhino_cfs_task_cpu_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                       uint8_t pri, size_t stack, task_entry_t entry,
                                       uint8_t cpu_num, uint8_t autorun);
#endif
#endif

#if (RHINO_CONFIG_TASK_DEL > 0)
/**
 * Delete a task, free the task stack.
 *
 * @param[in]  task  the task to be deleted.
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_del(ktask_t *task);
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Delete a task, free the task stack, free ktask_t.
 *
 * @param[in]  task  the task to be deleted.
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_dyn_del(ktask_t *task);
#endif

/**
 * Cancel a task.
 *
 * @param[in]  task  the task to be killed.
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_cancel(ktask_t *task);

/**
 * Check the task self whether is canceled or not.
 *
 * @param[in]  NULL
 *
 * @return  false or true
 */
RHINO_BOOL krhino_task_cancel_chk(void);
#endif

/**
 * Task sleep for some ticks.
 *
 * @param[in]  ticks  the ticks to sleep
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_sleep(tick_t ticks);

/**
 * Yield a task.
 *
 * @param[in]  NULL
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_yield(void);

/**
 * Get the current task for this cpu.
 *
 * @param[in]  NULL
 *
 * @return the current task
 */
ktask_t *krhino_cur_task_get(void);

/**
 * Suspend a task.
 *
 * @param[in]  task  the task to be suspended
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_suspend(ktask_t *task);

/**
 * Resume a task from SUSPEND.
 *
 * @param[in]  task  the task to be resumed
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_resume(ktask_t *task);

/**
 * Get min free stack size in the total runtime.
 *
 * @param[in]  task  the task where get free stack size.
 * @param[out] free  the free task stack size
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_stack_min_free(ktask_t *task, size_t *free);

/**
 * Change the priority of task.
 *
 * @param[in]   task     the task to be changed priority
 * @param[in]   pri      the priority to be changed.
 * @param[out]  old_pri  the old priority
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_pri_change(ktask_t *task, uint8_t pri, uint8_t *old_pri);

/**
 * Wakup the task, SUSPENDED/SLEEP/PEND -> READY.
 *
 * @param[in]  task  the task to be Wakup
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_wait_abort(ktask_t *task);

#if (RHINO_CONFIG_SCHED_RR > 0)
/**
 * Set task timeslice when KSCHED_RR.
 *
 * @param[in]  task   the task to be set timeslice
 * @param[in]  slice  the task time slice
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_time_slice_set(ktask_t *task, size_t slice);

/**
 * Set task sched policy.
 *
 * @param[in]  task    the task to be set
 * @param[in]  policy  KSCHED_FIFO / KSCHED_RR / KSCHED_CFS
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sched_policy_set(ktask_t *task, uint8_t policy);


/**
 * Set task sched policy and priority.
 *
 * @param[in]  task    the task to be set timeslice
 * @param[in]  policy  KSCHED_FIFO / KSCHED_RR / KSCHED_CFS
 * @param[in]  pri     the prio of task
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sched_param_set(ktask_t *task, uint8_t policy, uint8_t pri);

/**
 * Get task sched policy.
 *
 * @param[in]   task    the task to be get timeslice
 * @param[out]  policy  KSCHED_FIFO / KSCHED_RR / KSCHED_CFS
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sched_policy_get(ktask_t *task, uint8_t *policy);
#endif

#if (RHINO_CONFIG_TASK_INFO > 0)
/**
 * Set task private infomation.
 *
 * @param[in]   task  the task to be set private infomation
 * @param[in]   idx   set the info[idx], < RHINO_CONFIG_TASK_INFO_NUM
 * @param[in]   info  the private information
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_info_set(ktask_t *task, size_t idx, void *info);

/**
 * Get task private infomation.
 *
 * @param[in]   task  the task to be get private infomation
 * @param[in]   idx   get the info[idx], < RHINO_CONFIG_TASK_INFO_NUM
 * @param[out]  info  to save private infomation
 *
 * @return  the task private information
 */
kstat_t krhino_task_info_get(ktask_t *task, size_t idx, void **info);
#endif

/**
 * Task self delete, run when task ends.
 *
 * @param[in]   NULL
 *
 * @return  NULL
 */
void krhino_task_deathbed(void);

/**
 * Get the task by name.
 *
 * @param[in]   name    task name
 *
 * @return  the task
 */
ktask_t *krhino_task_find(name_t *name);

/** @} */

#endif /* K_TASK_H */

