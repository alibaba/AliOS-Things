/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <time.h>

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include "rhino_p.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

static unsigned int used_bitmap;

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

aos_status_t aos_task_new(const char *name, void (*fn)(void *), void *arg, size_t stack_size)
{
    int ret;

    ktask_t *task_handle = NULL;

#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret = (int)krhino_cfs_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI,
                                          stack_size / sizeof(cpu_stack_t), fn, 1u);
#else
    ret = (int)krhino_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI,
                                      0, stack_size / sizeof(cpu_stack_t), fn, 1u);
#endif

    return rhino2stderrno(ret);
}

aos_status_t aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, size_t stack_size, int32_t prio)
{
    int ret;
    if  (task == NULL) {
        return -EINVAL;
    }
#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret = (int)krhino_cfs_task_dyn_create((ktask_t **)task, name, arg, AOS_DEFAULT_APP_PRI,
                                          stack_size / sizeof(cpu_stack_t), fn, 1u);
#else
    ret = (int)krhino_task_dyn_create((ktask_t **)task, name, arg, prio,
                                      0, stack_size / sizeof(cpu_stack_t), fn, 1u);
#endif
    return rhino2stderrno(ret);
}

aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, void *stack, size_t stack_size, int32_t prio, uint32_t options)
{
    int       ret;
    cpu_stack_t  *task_stack = NULL;
    ktask_t      *task_obj;

    if (task == NULL) {
        return -EINVAL;
    }

    if (stack == NULL) {
        task_stack = aos_malloc(stack_size * sizeof(cpu_stack_t));
        if (task_stack == NULL) {
            return -ENOMEM;
        }
        stack = task_stack;
    }

    task_obj = aos_malloc(sizeof(ktask_t));
    if (task_obj == NULL) {
        aos_free(task_stack);
        return -ENOMEM;
    }

    *task = task_obj;
#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret = (int)krhino_cfs_task_create(task_obj, name, arg, prio, (cpu_stack_t *)stack,
                                      stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#else
    ret = (int)krhino_task_create(task_obj, name, arg, prio, 0, (cpu_stack_t *)stack,
                                  stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#endif
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        aos_free(task_stack);
        aos_free(task_obj);
        *task = NULL;
    }

    return rhino2stderrno(ret);
}

void aos_task_exit(int32_t code)
{
    (void)code;

    krhino_task_dyn_del(NULL);
}

aos_status_t aos_task_delete(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);

    ktask = (ktask_t *)(*task);

    ret = (int)krhino_task_dyn_del(ktask);

    return rhino2stderrno(ret);
}
#endif /* RHINO_CONFIG_KOBJ_DYN_ALLOC */

aos_status_t aos_task_resume(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);

    ktask = (ktask_t *)(*task);
    ret = krhino_task_resume(ktask);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_suspend(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);
    ktask = (ktask_t *)(*task);
    ret = krhino_task_suspend(ktask);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_yield()
{
    int ret;
    ret = krhino_task_yield();
    return rhino2stderrno(ret);
}

aos_task_t aos_task_self()
{
    return (aos_task_t)krhino_cur_task_get();

}

aos_status_t aos_task_name_get(aos_task_t *task, char *buf, size_t buf_size)
{
    const char *name;
    int str_len, copy_len;

    CHECK_HANDLE(task);

    if (buf == NULL || buf_size == 0) {
        return -EINVAL;
    }

    name = ((ktask_t *)*task)->task_name;

    str_len = strlen(name);

    copy_len = (str_len > (buf_size - 1)) ? buf_size - 1 : str_len;
    memcpy(buf, name, copy_len);
    return 0;

}

aos_status_t aos_task_key_create(aos_task_key_t *key)
{
    int i;
    if (key == NULL) {
        return -EINVAL;
    }
    for (i = RHINO_CONFIG_TASK_INFO_NUM - 1; i >= 0; i--) {
        if (!((1 << i) & used_bitmap)) {
            used_bitmap |= 1 << i;
            *key = i;

            return 0;
        }
    }

    return -EINVAL;
}

void aos_task_key_delete(aos_task_key_t key)
{
    if (key >= RHINO_CONFIG_TASK_INFO_NUM) {
        return;
    }

    used_bitmap &= ~(1 << key);
}

#if (RHINO_CONFIG_TASK_INFO > 0)
aos_status_t aos_task_setspecific(aos_task_key_t key, void *vp)
{
    int ret;
    ret = krhino_task_info_set(krhino_cur_task_get(), key, vp);
    return rhino2stderrno(ret);
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;

    krhino_task_info_get(krhino_cur_task_get(), key, &vp);

    return vp;
}

void aos_msleep(uint32_t ms)
{
    krhino_task_sleep(MS2TICK(ms));
}

uint32_t aos_sched_get_priority_max(uint32_t policy)
{
    return RHINO_CONFIG_PRI_MAX;
}

aos_status_t aos_task_ptcb_get(aos_task_t *task, void **ptcb)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (ptcb == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    *(ptcb) = ((ktask_t *)*task)->ptcb;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_ptcb_set(aos_task_t *task, void *ptcb)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);

    RHINO_CRITICAL_ENTER();
    ((ktask_t *)*task)->ptcb = ptcb;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri)
{
    kstat_t ret;

    CHECK_HANDLE(task);

    ret = krhino_task_pri_change((ktask_t *)*task, pri, old_pri);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_pri_get(aos_task_t *task, uint8_t *priority)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (priority == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    *priority = ((ktask_t *)*task)->b_prio;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri)
{
    kstat_t ret;

    CHECK_HANDLE(task);

    if (policy != KSCHED_FIFO && policy != KSCHED_RR && policy != KSCHED_CFS) {
        return -EINVAL;
    }

    ret = krhino_sched_param_set((ktask_t *)*task, policy, pri);
    return rhino2stderrno(ret);
}

aos_status_t aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy)
{
    kstat_t ret = 0;

    CHECK_HANDLE(task);
    if (policy == NULL) {
        return -EINVAL;
    }

    ret = krhino_sched_policy_get((ktask_t *)*task, policy);
    return rhino2stderrno(ret);
}

uint32_t aos_task_sched_policy_get_default()
{
#if (RHINO_CONFIG_SCHED_CFS > 0)
    return KSCHED_CFS;
#else
    return KSCHED_RR;
#endif
}

aos_status_t aos_task_time_slice_set(aos_task_t *task, uint32_t slice)
{
    kstat_t ret;
    CHECK_HANDLE(task);

    ret = krhino_task_time_slice_set((ktask_t *)*task, MS2TICK(slice));
    return rhino2stderrno(ret);
}

aos_status_t aos_task_time_slice_get(aos_task_t *task, uint32_t *slice)
{
    uint32_t time_slice;
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (slice == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    time_slice = ((ktask_t *)*task)->time_slice;
    RHINO_CRITICAL_EXIT();
    *slice = (uint32_t)krhino_ticks_to_ms(time_slice);

    return 0;
}

#endif

