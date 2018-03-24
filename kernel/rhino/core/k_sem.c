/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_SEM > 0)
static kstat_t sem_create(ksem_t *sem, const name_t *name, sem_count_t count,
                          uint8_t mm_alloc_flag)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(sem);
    NULL_PARA_CHK(name);

    /* init the list */
    klist_init(&sem->blk_obj.blk_list);

    /* init resource */
    sem->count              = count;
    sem->peak_count         = count;
    sem->blk_obj.name       = name;
    sem->blk_obj.blk_policy = BLK_POLICY_PRI;
    sem->mm_alloc_flag      = mm_alloc_flag;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    RHINO_CRITICAL_ENTER();
    klist_insert(&(g_kobj_list.sem_head), &sem->sem_item);
    RHINO_CRITICAL_EXIT();
#endif

    sem->blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;

    TRACE_SEM_CREATE(krhino_cur_task_get(), sem);

    return RHINO_SUCCESS;
}

kstat_t krhino_sem_create(ksem_t *sem, const name_t *name, sem_count_t count)
{
    return sem_create(sem, name, count, K_OBJ_STATIC_ALLOC);
}

kstat_t krhino_sem_del(ksem_t *sem)
{
    CPSR_ALLOC();

    klist_t *blk_list_head;

    NULL_PARA_CHK(sem);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (sem->mm_alloc_flag != K_OBJ_STATIC_ALLOC) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_DEL_ERR;
    }

    blk_list_head = &sem->blk_obj.blk_list;
    sem->blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;

    /* all task blocked on this queue is waken up */
    while (!is_klist_empty(blk_list_head)) {
        pend_task_rm(krhino_list_entry(blk_list_head->next, ktask_t, task_list));
    }

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&sem->sem_item);
#endif

    TRACE_SEM_DEL(g_active_task[cpu_cur_get()], sem);
    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
kstat_t krhino_sem_dyn_create(ksem_t **sem, const name_t *name,
                              sem_count_t count)
{
    kstat_t stat;
    ksem_t  *sem_obj;

    NULL_PARA_CHK(sem);

    sem_obj = krhino_mm_alloc(sizeof(ksem_t));

    if (sem_obj == NULL) {
        return RHINO_NO_MEM;
    }

    stat = sem_create(sem_obj, name, count, K_OBJ_DYN_ALLOC);

    if (stat != RHINO_SUCCESS) {
        krhino_mm_free(sem_obj);
        return stat;
    }

    *sem = sem_obj;

    return stat;
}

kstat_t krhino_sem_dyn_del(ksem_t *sem)
{
    CPSR_ALLOC();

    klist_t *blk_list_head;

    NULL_PARA_CHK(sem);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (sem->mm_alloc_flag != K_OBJ_DYN_ALLOC) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_DEL_ERR;
    }

    blk_list_head = &sem->blk_obj.blk_list;
    sem->blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;

    /* all task blocked on this queue is waken up */
    while (!is_klist_empty(blk_list_head)) {
        pend_task_rm(krhino_list_entry(blk_list_head->next, ktask_t, task_list));
    }

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&sem->sem_item);
#endif

    TRACE_SEM_DEL(g_active_task[cpu_cur_get()], sem);
    RHINO_CRITICAL_EXIT_SCHED();

    krhino_mm_free(sem);

    return RHINO_SUCCESS;
}

#endif

static kstat_t sem_give(ksem_t *sem, uint8_t opt_wake_all)
{
    CPSR_ALLOC();

    uint8_t  cur_cpu_num;
    klist_t *blk_list_head;

    /* this is only needed when system zero interrupt feature is enabled */
#if (RHINO_CONFIG_INTRPT_GUARD > 0)
    soc_intrpt_guard();
#endif

    RHINO_CRITICAL_ENTER();

    if (sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    cur_cpu_num = cpu_cur_get();
    (void)cur_cpu_num;

    blk_list_head = &sem->blk_obj.blk_list;

    if (is_klist_empty(blk_list_head)) {
        if (sem->count == (sem_count_t)-1) {

            TRACE_SEM_OVERFLOW(g_active_task[cur_cpu_num], sem);
            RHINO_CRITICAL_EXIT();

            return RHINO_SEM_OVF;
        }

        /* increase resource */
        sem->count++;

        if (sem->count > sem->peak_count) {
            sem->peak_count = sem->count;
        }

        TRACE_SEM_CNT_INCREASE(g_active_task[cur_cpu_num], sem);
        RHINO_CRITICAL_EXIT();
        return RHINO_SUCCESS;
    }

    /* wake all the task blocked on this semaphore */
    if (opt_wake_all) {
        while (!is_klist_empty(blk_list_head)) {
            TRACE_SEM_TASK_WAKE(g_active_task[cur_cpu_num], krhino_list_entry(blk_list_head->next,
                                                                              ktask_t, task_list),
                                sem, opt_wake_all);

            pend_task_wakeup(krhino_list_entry(blk_list_head->next, ktask_t, task_list));
        }

    } else {
        TRACE_SEM_TASK_WAKE(g_active_task[cur_cpu_num], krhino_list_entry(blk_list_head->next,
                                                                          ktask_t, task_list),
                            sem, opt_wake_all);

        /* wake up the highest prio task block on the semaphore */
        pend_task_wakeup(krhino_list_entry(blk_list_head->next, ktask_t, task_list));
    }

    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

kstat_t krhino_sem_give(ksem_t *sem)
{
    NULL_PARA_CHK(sem);

    return sem_give(sem, WAKE_ONE_SEM);
}

kstat_t krhino_sem_give_all(ksem_t *sem)
{
    NULL_PARA_CHK(sem);

    return sem_give(sem, WAKE_ALL_SEM);
}

kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks)
{
    CPSR_ALLOC();

    uint8_t  cur_cpu_num;
    kstat_t  stat;

    NULL_PARA_CHK(sem);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    cur_cpu_num = cpu_cur_get();

    if (sem->count > 0u) {
        sem->count--;

        TRACE_SEM_GET_SUCCESS(g_active_task[cur_cpu_num], sem);
        RHINO_CRITICAL_EXIT();

        return RHINO_SUCCESS;
    }

    /* can't get semphore, and return immediately if wait_option is  RHINO_NO_WAIT */
    if (ticks == RHINO_NO_WAIT) {
        RHINO_CRITICAL_EXIT();
        return RHINO_NO_PEND_WAIT;
    }

    if (g_sched_lock[cur_cpu_num] > 0u) {
        RHINO_CRITICAL_EXIT();
        return RHINO_SCHED_DISABLE;
    }

    pend_to_blk_obj((blk_obj_t *)sem, g_active_task[cur_cpu_num], ticks);

    TRACE_SEM_GET_BLK(g_active_task[cur_cpu_num], sem, ticks);

    RHINO_CRITICAL_EXIT_SCHED();

    RHINO_CPU_INTRPT_DISABLE();

    stat = pend_state_end_proc(g_active_task[cpu_cur_get()]);

    RHINO_CPU_INTRPT_ENABLE();

    return stat;
}

kstat_t krhino_sem_count_set(ksem_t *sem, sem_count_t sem_count)
{
    CPSR_ALLOC();

    klist_t *blk_list_head;

    NULL_PARA_CHK(sem);

    blk_list_head = &sem->blk_obj.blk_list;

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    /* set new count */
    if (sem->count > 0u) {
        sem->count = sem_count;
    } else {
        if (is_klist_empty(blk_list_head)) {
            sem->count = sem_count;
        } else {
            RHINO_CRITICAL_EXIT();
            return RHINO_SEM_TASK_WAITING;
        }
    }

    /* update sem peak count if need */
    if (sem->count > sem->peak_count) {
        sem->peak_count = sem->count;
    }

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

kstat_t krhino_sem_count_get(ksem_t *sem, sem_count_t *count)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(sem);
    NULL_PARA_CHK(count);

    RHINO_CRITICAL_ENTER();
   *count = sem->count;
    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

#endif /* RHINO_CONFIG_SEM */

