/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    #include <reent.h>
#endif
#include "k_api.h"
#include "k_proc.h"
#include "utask.h"
#include "task_group.h"
#include "callback.h"
#include "res.h"

#if (AOS_KERNEL_OBJECT > 0)
    #include "object_api.h"
#endif

static kstat_t task_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *ustack_buf,
                           size_t ustack_size, cpu_stack_t *kstack_buf, size_t kstack_size,
                           task_entry_t entry, void *ret_entry, uint8_t autorun,
                           uint8_t mm_alloc_flag, uint8_t cpu_num, uint8_t cpu_binded,
                           uint32_t pid, uint8_t is_proc)
{
    CPSR_ALLOC();

    cpu_stack_t *tmp;
    ktask_t     *cur_task;
    uint8_t      i = 0;
    uint32_t new_task_id = 0;

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
    new_task_id = alloc_tid();
    if (new_task_id == 0) {
        RHINO_CRITICAL_EXIT();
        return RHINO_TASK_NO_MORE_ID;
    }

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
    memset(ustack_buf, 0, ustack_size * sizeof(cpu_stack_t));

    klist_init(&task->tick_list);
    task->task_name        = name;
#if (RHINO_CONFIG_SCHED_CFS > 0)
    task->sched_policy     = KSCHED_CFS;
    task->prio             =  CFS_TASK_PRI;
    task->b_prio           =  CFS_TASK_PRI;
#else
    task->prio             = prio;
    task->b_prio           = prio;
#endif
    task->stack_size       = kstack_size;
    task->mm_alloc_flag    = mm_alloc_flag;
    task->cpu_num          = cpu_num;
    task->ustack_size      = ustack_size;
    task->task_ustack_base = ustack_buf;
    task->mode             = 0x3;
    task->pid              = pid;
    task->is_proc          = is_proc;
    task->task_id          = new_task_id;
#if (RHINO_CONFIG_MM_DEBUG > 0)
    task->task_alloc_size  = 0;
#endif

    cpu_binded = cpu_binded;
    i          = i;

    if (!is_proc) {
        cur_task = krhino_cur_task_get();
        task->task_group = cur_task->task_group;
        klist_init(&task->task_user);
        klist_insert(&((task_group_t *)task->task_group)->kobj_list.task_head, &task->task_user);
        ((task_group_t *)task->task_group)->task_cnt++;
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

    cpu_utask_stack_init(task, kstack_buf, kstack_size, ustack_buf, ustack_size,
                         arg, entry, ret_entry);

#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    struct utask_couple_data *utask_data;

    utask_data = (cpu_stack_t *)(task->task_ustack_base) + task->ustack_size;
    /* use newlibc reent */
    _REENT_INIT_PTR(&(utask_data->newlibc_reent));
    /* This structure is used when a thread enters kernel mode */
    _REENT_INIT_PTR((&(task->newlibc_reent)));
#endif

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
                            void *ret_entry, uint8_t cpu_num, uint8_t cpu_binded,
                            uint32_t pid, uint8_t flag, uint8_t is_proc)
{
    kstat_t      ret;
    cpu_stack_t *ktask_stack;
    ktask_t     *task_obj;
    uint8_t      mm_alloc_flag;
    uint8_t      autorun;

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

    mm_alloc_flag = K_OBJ_DYN_ALLOC;
    if (flag & USTACK_DYN_ALLOC) {
        mm_alloc_flag |=  USTACK_DYN_ALLOC;
    }

    autorun = (flag & UTASK_AUTORUN) ? 1 : 0;

    ret = task_create(task_obj, name, arg, pri, ticks, ustack_buf, ustack,
                      ktask_stack, kstack, entry, ret_entry, autorun, mm_alloc_flag,
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
                            size_t ustack, size_t kstack, task_entry_t entry, uint8_t flag)
{
    task_group_t *group;
    ktask_t      *cur_task;
    void         *ret_entry;
    kstat_t       ret;

    cur_task = krhino_cur_task_get();
    group    = (task_group_t *)cur_task->task_group;

    if (group->state == TGS_EXITING) {
        return  RHINO_TASK_CANCELED;
    }

    ret_entry = group->ret_entry;

    ret = utask_create(task, name, arg, pri, ticks, ustack_buf,
                       ustack, kstack, entry, ret_entry, 0, 0,
                       cur_task->pid, flag, 0);

    return ret;
}

kstat_t krhino_utask_del(ktask_t *task)
{
    return krhino_task_dyn_del(task);
}

RHINO_WEAK int32_t aos_get_pm_pid(void)
{
    return -1;
}

static void *g_pm_exit_fun = NULL;
void krhino_set_pm_exit_fun(void * exit_fun)
{
    int32_t pm_pid = aos_get_pm_pid();
    if(pm_pid == k_asid_get()){
        if(NULL == g_pm_exit_fun){
            g_pm_exit_fun = exit_fun;
        }
    }
}

void krhino_pm_callback_free_pid(uint32_t pid)
{
#ifndef AOS_COMPILE_APP
    int32_t pm_pid = aos_get_pm_pid();
    if(pm_pid <= 0){
        return;
    }

    cb_call_msg_t call_msg;
    task_group_t* group = task_group_get_by_pid(pm_pid);
    if(group == NULL) {
        return;
    }

    call_msg.func_ptr     = g_pm_exit_fun;
    call_msg.arg_cnt      = 1;
    call_msg.has_ret      = 0;
    call_msg.arg_value[0] = (void*)pid;
    cb_call_buf_queue_push(group->cb_call_buf_queue, &call_msg);
#endif
}

void krhino_utask_free_res(ktask_t *task)
{
    task_group_t *group;
    res_free_t   *ures_free;
    int           i;
    uint32_t     pid = task->pid;

    CPSR_ALLOC();

    group = (task_group_t *)task->task_group;
    if (NULL == group) {
        return;
    }

    RHINO_CRITICAL_ENTER();
    klist_rm(&task->task_user);
    RHINO_CRITICAL_EXIT();

    group->task_cnt--;

    if (group->task_cnt > 0) {
        if (group->state == TGS_NORMAL && task->mm_alloc_flag & USTACK_DYN_ALLOC) {
#if (RHINO_CONFIG_URES > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
            ures_free = (res_free_t *)(task->task_ustack_base + RHINO_CONFIG_STK_CHK_WORDS);
#else
            ures_free = (res_free_t *)(task->task_ustack_base + task->ustack_size -
                                       (sizeof(res_free_t) / sizeof(cpu_stack_t)) - RHINO_CONFIG_STK_CHK_WORDS);
#endif
            if (ures_free->cnt > 0) {
                for (i = 0; i < ures_free->cnt; i++) {
                    res_free(group->pid, ures_free->res[i]);
                }
            }
            res_free(group->pid, task->task_ustack_base);
#endif
        }
    } else {
        task_group_proc_free(group);
        krhino_pm_callback_free_pid(pid);
    }
}

kstat_t krhino_uprocess_create(const name_t *process_name, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry,
                               void *ret_entry, uint32_t pid, uint8_t flag)
{
    ktask_t      *task;
    task_group_t *group;
    kstat_t       ret;
    uint8_t       temp_flag;

    CPSR_ALLOC();

    /* Don't run the task immediately, resume it later */
    temp_flag = flag & (~UTASK_AUTORUN);

    ret = utask_create(&task, name, arg, pri, ticks, ustack_buf,
                       ustack, kstack, entry, ret_entry, 0, 0,
                       pid, temp_flag, 1);
    if (ret != RHINO_SUCCESS) {
        return ret;
    }

#if (AOS_KERNEL_OBJECT > 0)
    group = (task_group_t *) obj_alloc(proc_class_id);
    if (group == NULL) {
        obj_errno = ENOSPC;
        ret = RHINO_OBJ_INIT_FAIL;
        goto delete_task;
    }

    obj_core_init(&group->obj_core, proc_class_id);

    if (obj_handle_tbl_create(group) == ERROR) {
        ret = RHINO_OBJ_INIT_FAIL;
        goto delete_task;
    }
#else
    group = (task_group_t *)krhino_mm_alloc(sizeof(task_group_t));
    if (group == NULL) {
        ret = RHINO_NO_MEM;
        goto delete_task;
    }
    memset(group, 0, sizeof(task_group_t));
#endif

    if (task_group_init(group, process_name, pid) != 0) {
        krhino_mm_free(group);
        /* TODO: set the ret code propoerly */
        ret = RHINO_NO_MEM;
        goto delete_task;
    }

    group->ret_entry = ret_entry;

    task->task_group = group;
    klist_init(&task->task_user);

    RHINO_CRITICAL_ENTER();
    klist_insert(&((task_group_t *)task->task_group)->kobj_list.task_head, &task->task_user);
    ((task_group_t *)task->task_group)->task_cnt++;
    RHINO_CRITICAL_EXIT();

    if (flag & UTASK_AUTORUN) {
        krhino_task_resume(task);
    }

    return RHINO_SUCCESS;

delete_task:
    krhino_utask_del(task);

    return ret;
}

void krhino_utask_cancel_check(void)
{
    ktask_t *cur_task;

    cur_task = krhino_cur_task_get();
    if (cur_task->cancel > 0) {
        krhino_task_dyn_del(cur_task);
    }
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

    if (NULL == group) {
        return;
    }
#if (AOS_KERNEL_OBJECT > 0)
#if 0
    /* clean up handle table */
    obj_handle_tbl_terminate(group);
    /* invalid the obj */
    obj_core_invalidate((obj_core_t *) group);
    /* destroy resources */
    obj_core_deinit((obj_core_t *) group);
#endif
#endif
    krhino_sched_disable();
    RHINO_CRITICAL_ENTER();

    group->state = TGS_EXITING;
    head = &group->kobj_list.task_head;
    iter = head->next;

    /* wakeup all tasks of the process */
    while (iter != head) {
        task = krhino_list_entry(iter, ktask_t, task_user);
        iter = iter->next;
        /* don't delete myself here */
        if ((del_me == 1) && (task == cur_task)) {
            continue;
        }

        krhino_task_cancel(task);
    }

    RHINO_CRITICAL_EXIT();
    krhino_sched_enable();

    if (del_me == 1) {
        krhino_task_dyn_del(cur_task);
    }
}

void krhino_uprocess_exit(void)
{
    ktask_t *cur_task;

    cur_task = krhino_cur_task_get();
    uprocess_exit(cur_task->pid, 1);
}

int krhino_uprocess_kill(int pid)
{
    uprocess_exit(pid, 0);

    return 0;
}

uint32_t krhino_uprocess_id_get(void)
{
    ktask_t      *cur_task;

    cur_task = krhino_cur_task_get();

    return cur_task->pid;
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

    if (task->task_ustack_base == NULL) {
        return RHINO_NULL_PTR;
    }

    if (task->ustack_size == 0u) {
        return RHINO_TASK_INV_STACK_SIZE;
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

int32_t krhino_uprocess_load(const char *file, const char *name)
{
    if (k_proc_load(file, name) == 0) {
        return RHINO_SUCCESS;
    } else {
        return RHINO_SYS_FATAL_ERR;
    }
}

