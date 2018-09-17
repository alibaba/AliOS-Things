/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_USER_SPACE > 0)

static void start_utask(void *arg)
{
    ktask_t *task;
    volatile cpu_stack_t     *kstack;

    task = krhino_cur_task_get();

    kstack = task->task_stack_base + task->stack_size;

    __asm__(
            "mov r0, %0\t\n"
            "svc #0x01\t\n"
            :: "r"(kstack)
            : "r0"
            );

    if (task->entry) {
        task->entry(arg);
    }
}

static kstat_t task_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *ustack_buf,
                           size_t ustack_size, cpu_stack_t *kstack_buf, size_t kstack_size,
                           task_entry_t entry, uint8_t autorun,uint8_t mm_alloc_flag,
                           uint8_t cpu_num, uint8_t cpu_binded)
{
    CPSR_ALLOC();

    cpu_stack_t *tmp;
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

    task->task_name     = name;
    task->prio          = prio;
    task->b_prio        = prio;
    task->stack_size    = kstack_size;
    task->mm_alloc_flag = mm_alloc_flag;
    task->cpu_num       = cpu_num;

    task->utask_stack   = ustack_buf;
    task->ustack_size   = ustack_size;
    task->entry         = entry;
    task->mode          = 0x2;
    cpu_binded          = cpu_binded;
    i                   = i;

#if (RHINO_CONFIG_CPU_NUM > 1)
    task->cpu_binded    = cpu_binded;
#endif

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    tmp  = task->task_stack_base;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#else
    tmp  = (cpu_stack_t *)(task->task_stack_base) + task->stack_size - RHINO_CONFIG_STK_CHK_WORDS;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#endif
#endif

    task->task_stack  = cpu_task_stack_init(kstack_buf, kstack_size, arg, start_utask);
    task->utask_stack = cpu_task_ustack_init(ustack_buf, ustack_size);

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_task_create_hook(task);
#endif

    TRACE_TASK_CREATE(task);

    RHINO_CRITICAL_ENTER();

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
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

static kstat_t utask_create(ktask_t *task, const name_t *name, void *arg,
                        uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                        size_t ustack, size_t kstack, task_entry_t entry,
                        uint8_t cpu_num, uint8_t cpu_binded, uint8_t autorun)
{
    kstat_t      ret;
    cpu_stack_t *ktask_stack;

    NULL_PARA_CHK(task);

    if (kstack == 0) {
        return RHINO_INV_PARAM;
    }

    ktask_stack = krhino_mm_alloc(kstack * sizeof(cpu_stack_t));
    if (ktask_stack == NULL) {
        return RHINO_NO_MEM;
    }

    ret = task_create(task, name, arg, pri, ticks, ustack_buf, ustack,
                      ktask_stack, kstack, entry, autorun, K_OBJ_DYN_ALLOC,
                      cpu_num, cpu_binded);

    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        krhino_mm_free(ktask_stack);
        return ret;
    }

    return ret;
}

kstat_t krhino_utask_create(ktask_t *task, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry, uint8_t autorun)
{
    return utask_create(task, name, arg, pri, ticks, ustack_buf,
                        ustack, kstack, entry, 0, 0, autorun);
}

#endif // RHINO_CONFIG_USER_SPACE

