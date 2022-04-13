/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TASK_H
#define K_TASK_H

typedef enum {
    K_SEED,
    K_RDY,
    K_PEND,
    K_SUSPENDED,
    K_PEND_SUSPENDED,
    K_SLEEP,
    K_SLEEP_SUSPENDED,
    K_DELETED,
} task_stat_t;

typedef void (*task_entry_t)(void *arg);

/* task control information */
typedef struct {
    /* update while task switching
       access by assemble code, so do not change position */
    void            *task_stack;
    /* access by activation, so do not change position */
    const name_t    *task_name;
#if (RHINO_CONFIG_TASK_INFO > 0)
    /* access by assemble code, so do not change position */
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

    cpu_stack_t     *task_stack_base;
    uint32_t         stack_size;
    klist_t          task_list;

    suspend_nested_t suspend_count;

    struct mutex_s  *mutex_list;
    struct k_futex  *futex;

#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t          task_stats_item;
#endif

    klist_t          tick_list;
    tick_t           tick_match;
    tick_t           tick_remain;
    klist_t         *tick_head;

    void            *msg;

#if (RHINO_CONFIG_BUF_QUEUE > 0)
    size_t           bq_msg_size;
#endif

    task_stat_t      task_state;
    blk_state_t      blk_state;

    /* Task block on mutex, queue, semphore, event */
    blk_obj_t       *blk_obj;

    uint32_t         task_id;
#if (RHINO_CONFIG_MM_DEBUG > 0)
    uint32_t         task_alloc_size;
#endif

#if (RHINO_CONFIG_TASK_SEM > 0)
    struct sem_s    *task_sem_obj;
#endif

#if (RHINO_CONFIG_SYS_STATS > 0)
    size_t           task_free_stack_size;
    ctx_switch_t     task_ctx_switch_times;
    sys_time_t       task_time_total_run;
    sys_time_t       task_time_total_run_prev;
    lr_timer_t       task_exec_time;
    lr_timer_t       task_time_start;
    lr_timer_t       task_time_this_run;
    lr_timer_t       task_vtime_start;
    lr_timer_t       task_vtime_this_run;
    hr_timer_t       task_intrpt_disable_time_max;
    hr_timer_t       task_sched_disable_time_max;
#endif

#if (RHINO_CONFIG_SCHED_RR > 0)
    /* for task time slice*/
    uint32_t         time_slice;
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
    uint8_t          cpu_binded;
    uint8_t          cur_exc;
    klist_t          task_del_item;
#endif

#if (RHINO_CONFIG_TASK_DEL > 0)
    uint8_t          cancel;
#endif

    /* current prio */
    uint8_t          prio;
    /* base prio */
    uint8_t          b_prio;
    uint8_t          mm_alloc_flag;

#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    /* use newlibc reent */
    struct  _reent   newlibc_reent;
#endif

    /* The posix thread control block. */
    void            *ptcb;
#if (RHINO_CONFIG_USIGNAL > 0)
    /* The user space task signal control block. */
    sigptcb_t       usignal;
#endif
} ktask_t;

/**
 * This function will initialize a task
 * @param[in]  task       the task to be created
 * @param[in]  name       the name of task, which shall be unique
 * @param[in]  arg        the parameter of task enter function
 * @param[in]  pri        the prio of task
 * @param[in]  ticks      the time slice if there are same prio task
 * @param[in]  stack_buf  the start address of task stack
 * @param[in]  stack      the size of thread stack
 * @param[in]  entry      the entry function of task
 * @param[in]  autorun    the autorunning flag of task
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *stack_buf,
                           size_t stack_size, task_entry_t entry, uint8_t autorun);

kstat_t krhino_cfs_task_create(ktask_t *task, const name_t *name, void *arg,
                               uint8_t prio, cpu_stack_t *stack_buf, size_t stack_size,
                               task_entry_t entry, uint8_t autorun);

#if (RHINO_CONFIG_CPU_NUM > 1)
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
kstat_t krhino_task_cpu_unbind(ktask_t *task);
#endif


#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * This function will initialize a task
 * @param[in]  task     the task to be created
 * @param[in]  name     the name of task, which shall be unique
 * @param[in]  arg      the parameter of task enter function
 * @param[in]  pri      the prio of task
 * @param[in]  ticks    the time slice if there are same prio task
 * @param[in]  stack    the size of thread stack
 * @param[in]  entry    the entry function of task
 * @param[in]  autorun  the autorunning flag of task
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
    * This function will delete a task
    * @param[in]  task  the task to be deleted.
    * @return  the operation status, RHINO_SUCCESS is OK, others is error
    */
    kstat_t krhino_task_del(ktask_t *task);
    #if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
        /**
        * This function will delete a dyn-task
        * @param[in]  task  the task to be deleted.
        * @return  the operation status, RHINO_SUCCESS is OK, others is error
        */
        kstat_t krhino_task_dyn_del(ktask_t *task);
    #endif

    /**
    * This function will cancel a task
    * @param[in]  task  the task to be killed.
    * @return  the operation status, RHINO_SUCCESS is OK, others is error
    */
    kstat_t krhino_task_cancel(ktask_t *task);


    /**
    * This function will clr the task cancel bit
    *
    */
    void krhino_task_cancel_clr(void);

    /**
    * This function will check the task whether is canceled or not.
    * @param[in]  task  the task to be checked.
    * @return  the operation status, RHINO_TRUE is TRUE, RHINO_FALSE is FALSE
    */
    RHINO_BOOL krhino_task_cancel_chk(void);
#endif

/**
 * This function will cause a task to sleep for some ticks
 * @param[in]  ticks  the ticks to sleep
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */

kstat_t krhino_task_sleep(tick_t dly);

/**
 * This function will yield a task
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_yield(void);

/**
 * This function will get the current task for this cpu
 * @return the current task
 */
ktask_t *krhino_cur_task_get(void);

/**
 * This function will suspend a task
 * @param[in]  task  the task to be suspended
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_suspend(ktask_t *task);

/**
 * This function will resume a task
 * @param[in]  task  the task to be resumed
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_resume(ktask_t *task);

/**
 * This function will get min free stack size in the total runtime
 * @param[in]  task  the task where get free stack size.
 * @param[in]  free  the free task stack size to be filled with.
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_stack_min_free(ktask_t *task, size_t *free);

/**
 * This function will change the prio of task
 * @param[in]   task     the task to be changed prio
 * @param[in]   pri      the prio to be changed.
 * @param[out]  old_pri  the old task prio to be filled with
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_task_pri_change(ktask_t *task, uint8_t pri, uint8_t *old_pri);

#if (RHINO_CONFIG_TASK_WAIT_ABORT > 0)
    /**
    * This function will abort a task and wakup the task
    * @param[in]  task  the task to be aborted
    * @return  the operation status, RHINO_SUCCESS is OK, others is error
    */
    kstat_t krhino_task_wait_abort(ktask_t *task);
#endif

#if (RHINO_CONFIG_SCHED_RR > 0)
    /**
    * This function will set task timeslice
    * @param[in]  task   the task to be set timeslice
    * @param[in]  slice  the task time slice
    * @return  the operation status, RHINO_SUCCESS is OK, others is error
    */
    kstat_t krhino_task_time_slice_set(ktask_t *task, uint32_t slice);

    /**
    * This function will get task timeslice
    * @param[in]  task   the task to be get timeslice
    * @param[out] slice  the task time slice
    * @return  the operation status, RHINO_SUCCESS is OK, others is error
    */
    kstat_t krhino_task_time_slice_get(ktask_t *task, uint32_t *slice);

    /**
    * Set task sched policy.
    *
    * @param[in]  task    the task to be set
    * @param[in]  policy  KSCHED_FIFO / KSCHED_RR / KSCHED_CFS
    *
    * @return  the operation status, RHINO_SUCCESS is OK, others is error
    */
    kstat_t krhino_sched_policy_set(ktask_t *task, uint8_t policy, uint8_t pri);


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
    * This function will set task private infomation
    * @param[in]   task  the task to be set private infomation
    * @param[out]  info  the private information
    * @return  the operation status, RHINO_SUCCESS is OK, others is error
    */
    kstat_t krhino_task_info_set(ktask_t *task, size_t idx, void *info);

    /**
    * This function will get task private infomation
    * @param[in]   task  the task to be get private infomation
    * @param[out]  info  to save private infomation
    * @return  the task private information
    */
    kstat_t krhino_task_info_get(ktask_t *task, size_t idx, void **info);
#endif

/**
 * This function will be set in cpu_task_stack_init,set LR reg with
 * this funtion pointer
 */
void  krhino_task_deathbed(void);

/**
 * Get the task by name.
 *
 * @param[in]   name    task name
 *
 * @return  the task
 */
ktask_t *krhino_task_find(name_t *name);

/**
* This function will set task posix tcb
* @param[in]   task  the task to be set posix tcb
* @param[out]  ptcb  the posix  tcb
* @return  the operation status, RHINO_SUCCESS is OK, others is error
*/
kstat_t krhino_task_ptcb_set(ktask_t *task, void *ptcb);

/**
* This function will get task posix tcb
* @param[in]   task  the task to be get posix tcb
* @param[out]  ptcb  the posix  tcb
* @return  the operation status, RHINO_SUCCESS is OK, others is error
*/
kstat_t krhino_task_ptcb_get(ktask_t *task, void **ptcb);

/**
* Wake up the task
* @param[in]   task  the task to wakeup
* @return  the operation status, RHINO_SUCCESS is OK, others is error
*/
kstat_t krhino_task_wakeup(ktask_t *task);

/** @} */

#endif /* K_TASK_H */

