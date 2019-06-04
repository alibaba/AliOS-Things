/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "k_api.h"
#include "utask.h"
#include "task_group.h"
#include "callback.h"

static kstat_t task_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *ustack_buf,
                           size_t ustack_size, cpu_stack_t *kstack_buf, size_t kstack_size,
                           task_entry_t entry, uint8_t autorun,uint8_t mm_alloc_flag,
                           uint8_t cpu_num, uint8_t cpu_binded, uint32_t pid, uint8_t is_proc)
{
    CPSR_ALLOC();

    cpu_stack_t *tmp;
    ktask_t     *cur_task;
    uint8_t      i = 0;

    NULL_PARA_CHK(task);
    NULL_PARA_CHK(name);
    NULL_PARA_CHK(entry);
    NULL_PARA_CHK(ustack_buf);
    NULL_PARA_CHK(kstack_buf);

    if (kstack_size == 0u) {
        return RHINO_TASK_INV_STACK_SIZE;
    }

    if (prio >= RHINO_CONFIG_PRI_MAX) {
        return RHINO_BEYOND_MAX_PRI;
    }

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    /* idle task is only allowed to create once */
    if (prio == RHINO_IDLE_PRI) {
        if (g_idle_task_spawned[cpu_num] > 0u) {
            RHINO_CRITICAL_EXIT();
            return RHINO_IDLE_TASK_EXIST;
        }

        g_idle_task_spawned[cpu_num] = 1u;
    }

    RHINO_CRITICAL_EXIT();

    memset(task, 0, sizeof(ktask_t));

#if (RHINO_CONFIG_SCHED_RR > 0)
    if (ticks > 0u) {
        task->time_total = ticks;
    } else {
        task->time_total = RHINO_CONFIG_TIME_SLICE_DEFAULT;
    }

    task->time_slice   = task->time_total;
    task->sched_policy = KSCHED_RR;
#endif

    if (autorun > 0u) {
        task->task_state    = K_RDY;
    } else {
        task->task_state    = K_SUSPENDED;
        task->suspend_count = 1u;
    }

    /* init all the stack element to 0 */
    task->task_stack_base = kstack_buf;
    tmp = kstack_buf;

    memset(tmp, 0, kstack_size * sizeof(cpu_stack_t));

    task->task_name          = name;
    task->prio               = prio;
    task->b_prio             = prio;
    task->stack_size         = kstack_size;
    task->mm_alloc_flag      = mm_alloc_flag;
    task->cpu_num            = cpu_num;
    task->ustack_size        = ustack_size;
    task->task_ustack_base   = ustack_buf;
    task->mode               = 0x3;
    task->pid                = pid;
    task->is_proc            = is_proc;

    cpu_binded = cpu_binded;
    i          = i;

    if (!is_proc) {
        cur_task = krhino_cur_task_get();
        task->task_group = cur_task->task_group;
        task_group_add_res_obj(cur_task->task_group, RES_OBJ_TASK, task);
    }

    cpu_binded = cpu_binded;
    i          = i;

#if (RHINO_CONFIG_CPU_NUM > 1)
    task->cpu_binded    = cpu_binded;
#endif

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    /* fill magic for kernel stack */
    tmp  = task->task_stack_base;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
    /* fill magic for user stack */
    tmp  = task->task_ustack_base;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#else
    /* fill magic for kernel stack */
    tmp  = (cpu_stack_t *)(task->task_stack_base) + task->stack_size - RHINO_CONFIG_STK_CHK_WORDS;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
    /* fill magic for user stack */
    tmp  = (cpu_stack_t *)(task->task_ustack_base) + task->ustack_size - RHINO_CONFIG_STK_CHK_WORDS;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#endif
#endif

    cpu_utask_stack_init(task, kstack_buf, kstack_size,
                         ustack_buf, ustack_size,
                         arg, entry);

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_task_create_hook(task);
#endif

    TRACE_TASK_CREATE(task);

    RHINO_CRITICAL_ENTER();

#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_insert(&(g_kobj_list.task_head), &task->task_stats_item);
#endif

    if (autorun > 0u) {
        ready_list_add_tail(&g_ready_queue, task);
        /* if system is not start,not call core_sched */
        if (g_sys_stat == RHINO_RUNNING) {
            RHINO_CRITICAL_EXIT_SCHED();
            return RHINO_SUCCESS;
        }
    }

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

static kstat_t utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry,
                            uint8_t cpu_num, uint8_t cpu_binded, uint32_t pid,
                            uint8_t autorun, uint8_t is_proc)
{
    kstat_t      ret;
    cpu_stack_t *ktask_stack;
    ktask_t     *task_obj;

    NULL_PARA_CHK(task);

    if (kstack == 0) {
        return RHINO_INV_PARAM;
    }

    ktask_stack = krhino_mm_alloc(kstack * sizeof(cpu_stack_t));
    if (ktask_stack == NULL) {
        return RHINO_NO_MEM;
    }

    task_obj = krhino_mm_alloc(sizeof(ktask_t));
    if (task_obj == NULL) {
        krhino_mm_free(ktask_stack);
        return RHINO_NO_MEM;
    }

   *task = task_obj;

    ret = task_create(task_obj, name, arg, pri, ticks, ustack_buf, ustack,
                      ktask_stack, kstack, entry, autorun, K_OBJ_DYN_ALLOC,
                      cpu_num, cpu_binded, pid, is_proc);

    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        krhino_mm_free(ktask_stack);
        krhino_mm_free(task_obj);

        return ret;
    }

    return ret;
}

kstat_t krhino_utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry, uint8_t autorun)
{
    ktask_t *cur_task;
    kstat_t  ret;

    cur_task = krhino_cur_task_get();

    ret = utask_create(task, name, arg, pri, ticks, ustack_buf,
                        ustack, kstack, entry, 0, 0, cur_task->pid, autorun, 0);

    return ret;
}

kstat_t krhino_utask_del(ktask_t *task)
{
    return krhino_task_dyn_del(task);
}

kstat_t krhino_uprocess_create(const name_t *process_name, ktask_t **task,
                               const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry,
                               uint32_t pid, uint8_t autorun)
{
    ktask_t      *task_tmp;
    task_group_t *group;
    kstat_t       ret;

    ret = utask_create(task, name, arg, pri, ticks, ustack_buf,
                        ustack, kstack, entry, 0, 0, pid, 0, 1);
    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    task_tmp = *task;

    group = (task_group_t*)krhino_mm_alloc(sizeof(task_group_t));
    if (group == NULL) {
        ret = RHINO_NO_MEM;
        goto delete_task;
    }

    if (task_group_init(group, process_name, pid) != 0) {
        krhino_mm_free(group);
        //TODO: set the ret code propoerly
        ret = RHINO_NO_MEM;
        goto delete_task;
    }

    task_tmp->task_group = group;
    task_group_add_res_obj(group, RES_OBJ_TASK, task_tmp);

    if (autorun > 0) {
        krhino_task_resume(task_tmp);
    }

    return RHINO_SUCCESS;

delete_task:
    krhino_utask_del(task_tmp);

    return ret;
}

void krhino_utask_cancel_check(void)
{
    CPSR_ALLOC();
    ktask_t *cur_task;

    cur_task= krhino_cur_task_get();

    RHINO_CRITICAL_ENTER();

    if (cur_task->cancel > 0) {
        krhino_task_dyn_del(cur_task);
    }

    RHINO_CRITICAL_EXIT();
}

static void uprocess_exit(int pid, uint8_t del_me)
{
    CPSR_ALLOC();

    task_group_t *group;
    klist_t      *head;
    klist_t      *iter;
    ktask_t      *cur_task;
    ktask_t      *task;

    cur_task = krhino_cur_task_get();
    group = task_group_get_by_pid(pid);

    if (NULL == group)
        return;

    RHINO_CRITICAL_ENTER();
    group->state = TGS_EXITING;
    head = &group->kobj_list.task_head;
    iter = head->next;

    /* wakeup all tasks of the process */
    while (iter != head) {
        task = krhino_list_entry(iter, ktask_t, task_user);
        task->cancel = 1;
        iter = iter->next;
        /* don't delete myself here */
        if ((del_me == 1) && (task == cur_task))
            continue;
        krhino_task_wait_abort(task);
    }

    RHINO_CRITICAL_EXIT();

    if (del_me == 1) {
        krhino_task_dyn_del(cur_task);
    }
}

void krhino_uprocess_exit(void)
{
    ktask_t *cur_task;
    uint8_t  old;
    uint8_t  new;

    /* Note: there is BUG if kill process at high priority
     * change the current task's prio to the lowest, needn't
     * to restore as it will nerver return.
     */
    new = RHINO_CONFIG_USER_PRI_MAX;
    krhino_task_pri_change(krhino_cur_task_get(), new, &old);

    cur_task = krhino_cur_task_get();
    uprocess_exit(cur_task->pid, 1);
}

int krhino_uprocess_kill(int pid)
{
    uint8_t old;
    uint8_t new;

    /* Note: there is BUG if kill process at high priority
     * change the current task's prio to the lowest,
     * and restore to its original prio after killing
     * the process
     */
    new = RHINO_CONFIG_USER_PRI_MAX;
    krhino_task_pri_change(krhino_cur_task_get(), new, &old);
    uprocess_exit(pid, 0);
    krhino_task_pri_change(krhino_cur_task_get(), old, &old);

    return 0;
}

void krhino_uprocess_res_get(int32_t id, void **res)
{
    CPSR_ALLOC();

    ktask_t      *cur_task;
    task_group_t *group;

    cur_task = krhino_cur_task_get();

    RHINO_CRITICAL_ENTER();
    group    = cur_task->task_group;

    switch(id) {
#if (RHINO_CONFIG_URES > 0)
        case USER_RES_REQUEST_BUF_QUEUE:
            *res = group->res_request_q;
            break;
        case USER_RES_REPLY_BUF_QUEUE:
            *res = group->res_reply_q;
            break;
#endif
#if (RHINO_CONFIG_UCLI > 0)
        case USER_CLI_BUF_QUEUE:
            *res = group->cli_q;
            break;
#endif
#if (RHINO_CONFIG_UCALLBACK > 0)
        case USER_CB_CALL_BUF_QUEUE:
            *res = group->cb_call_buf_queue;
            break;
        case USER_CB_RET_BUF_QUEUE:
            *res = group->cb_ret_buf_queue;
            break;
#endif
        default:
            printf("%s: Unknown request, id 0x%x\r\n", __func__, id);
            break;
    }
    RHINO_CRITICAL_EXIT();
}

kstat_t krhino_utask_stack_min_free(ktask_t *task, size_t *free)
{
    cpu_stack_t *task_stack;
    size_t free_stk = 0;

    NULL_PARA_CHK(task);
    NULL_PARA_CHK(free);

    if (task->task_state == K_DELETED) {
        return RHINO_INV_TASK_STATE;
    }

#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    task_stack = task->task_ustack_base + RHINO_CONFIG_STK_CHK_WORDS;
    while (*task_stack++ == 0u) {
        free_stk++;
    }
#else
    task_stack = (cpu_stack_t *)(task->task_ustack_base) + task->ustack_size
                 -  RHINO_CONFIG_STK_CHK_WORDS - 1u;
    while (*task_stack-- == 0u) {
        free_stk++;
    }
#endif

    *free = free_stk;

    return RHINO_SUCCESS;
}
